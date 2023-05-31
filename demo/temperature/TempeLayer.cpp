#include "TempeLayer"
#include <osgEarth/Shaders>
#include <osgEarth/Registry>
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/MapNode>
#include <icecream.hpp>

#define LC "[Temperature] "

using namespace osgEarth;

//........................................................................

Config
TemperatureLayer::Options::getMetadata()
{
    return Config::readJSON(R"(
        { "name" : "TempeLayer",
          "properties" : [
          ]
        }
    )");
}

Config
TemperatureLayer::Options::getConfig() const
{
    Config conf = VisibleLayer::Options::getConfig();
    Config stopsConf("stops");
    for (auto& stop : stops())
    {
        Config s("stop");
        s.set("elevation", stop.elevation);
        s.set("color", stop.color);
        stopsConf.add(s);
    }
    if (!stopsConf.empty())
        conf.add(stopsConf);
    return conf;
}

void
TemperatureLayer::Options::fromConfig(const Config& conf)
{
    stops().clear();
    const ConfigSet& stopsConf = conf.child("stops").children();
    for (auto& stop : stopsConf)
    {
        Stop s;
        if (stop.get("elevation", s.elevation) && stop.get("color", s.color))
            stops().emplace_back(s);
    }
}

//........................................................................

REGISTER_OSGEARTH_LAYER(temperature, TemperatureLayer);

void
TemperatureLayer::setTransferFunction(osg::TransferFunction1D* xfer)
{
    _xfer = xfer;
    _xferTexture->setImage(_xfer->getImage());
    _xferMin->set(_xfer->getMinimum());
    _xferRange->set(_xfer->getMaximum() - _xfer->getMinimum());
    _xferMin->set(-49.2966f);
    _xferRange->set(32.8033f + 49.2966f);

    //OE_WARN << "min=" << _xfer->getMinimum() << ", range=" << (_xfer->getMaximum()-_xfer->getMinimum()) << std::endl;
}

void
TemperatureLayer::init()
{
    VisibleLayer::init();

    setRenderType(RENDERTYPE_TERRAIN_SURFACE);

    osg::StateSet* stateset = getOrCreateStateSet();

    // uniforms we'll need:
    _xferMin = new osg::Uniform(osg::Uniform::FLOAT, "oe_temp_min");
    stateset->addUniform(_xferMin.get());

    _xferRange = new osg::Uniform(osg::Uniform::FLOAT, "oe_temp_range");
    stateset->addUniform(_xferRange.get());

    _xferSampler = new osg::Uniform(osg::Uniform::SAMPLER_1D, "oe_temp_xfer");
    stateset->addUniform(_xferSampler.get());

    _xferSampler = new osg::Uniform(osg::Uniform::SAMPLER_1D, "oe_temp_xfer");
    stateset->addUniform(_xferSampler.get());

    _tempSampler = new osg::Uniform(osg::Uniform::SAMPLER_2D, "oe_temp_src");
    stateset->addUniform(_tempSampler.get());

    // Create a 1D texture from the transfer function's image.
    _xferTexture = new TextureType();
    _xferTexture->setResizeNonPowerOfTwoHint(false);
    _xferTexture->setUseHardwareMipMapGeneration(false);
    _xferTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    _xferTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    _xferTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    // defer installing the texture until we get a unit

    // shaders:
    VirtualProgram* vp = VirtualProgram::getOrCreate(stateset);
    vp->setName(typeid(*this).name());
    Shaders pkg;
    pkg.load(vp, "temp.glsl");

    // build a transfer function.
    osg::TransferFunction1D* xfer = new osg::TransferFunction1D();

    if (options().stops().empty() == false)
    {
        for (auto& stop : options().stops())
        {
            xfer->setColor(stop.elevation, stop.color, false);
        }
    }
    else
    {
        float s = 50.0f;
        xfer->setColor(-1.0000 * s, osg::Vec4f(0.0, 0.0, 1.0, 1.0), false);
        xfer->setColor(-0.6666 * s, osg::Vec4f(0.0, 0.4392, 1.0, 1.0), false);
        xfer->setColor(-0.3333 * s, osg::Vec4f(0.0, 0.6902, 1.0, 1.0), false);
        xfer->setColor(0.0000 * s,  osg::Vec4f(0,0, 1.0, 1.0), false);
        xfer->setColor(0.2000 * s,  osg::Vec4f(0.0, 1.0, 0.0, 1.0), false);
        xfer->setColor(0.4000 * s,  osg::Vec4f(0.0, 0.5, 0.0, 1.0), false);
        xfer->setColor(0.6000 * s,  osg::Vec4f(1.0, 1.0, 0.0, 1.0), false);
        xfer->setColor(0.8000 * s,  osg::Vec4f(1.0, .5, 0.0, 1.0), false);
        xfer->setColor(1.0000 * s,  osg::Vec4f(1.0, 0.0, 0.0, 1.0), false);
    }

    xfer->updateImage();
    osg::Vec4f c = xfer->getColor(25.0f);
    IC(c[0], c[1], c[2], c[3]);
    c = xfer->getColor(-35.0f);
    IC(c[0], c[1], c[2], c[3]);
    this->setTransferFunction(xfer);
}

void
TemperatureLayer::prepareForRendering(TerrainEngine* engine)
{
    VisibleLayer::prepareForRendering(engine);

    auto* res = engine->getResources();
    bool ok = res->reserveTextureImageUnitForLayer(_reservation, this, "Temp value");
    ok &= res->reserveTextureImageUnitForLayer(_resTemp, this, "Temp Source");
    if (!ok)
    {
        setStatus(Status::ResourceUnavailable, "No texture image units available");
        return;
    }

    osg::StateSet* stateset = getOrCreateStateSet();
    stateset->setTextureAttributeAndModes(_reservation.unit(), _xferTexture.get(), osg::StateAttribute::ON);
    _xferSampler->set(_reservation.unit());
    IC(_reservation.unit());
    IC(_resTemp.unit());
    stateset->setTextureAttributeAndModes(_resTemp.unit(), _tempSrc, osg::StateAttribute::ON);
    _tempSampler->set(_resTemp.unit());
}

Status
TemperatureLayer::closeImplementation()
{
    _reservation.release();

    return VisibleLayer::closeImplementation();
}
void TemperatureLayer::applyTempSrc(osg::ref_ptr<osg::Texture> temp)
{
    _tempSrc = temp;
}
