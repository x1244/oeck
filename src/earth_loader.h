/*\brief 改造地球加载器
*/
#pragma once

#include <osgEarth/Common>
#include <osgEarth/Controls>
#include <osgEarth/EarthManipulator>
#include <osgEarth/Sky>
#include <osgEarth/SimpleOceanLayer>
#include <osgEarth/Viewpoint>

namespace osgEarth {
    class MapNode;
}
namespace osg {
    class ArgumentParser;
}
namespace osgViewer {
    class View;
    class ViewerBase;
};

/**
 * This is a collection of resources used by the osgEarth example applications.
 */
namespace osgEarth { namespace Util
{
    using namespace osgEarth;
    using namespace osgEarth::Util::Controls;

    /**
     * Parses a set of built-in example arguments. Any Controls created by parsing
     * command-line parameters will appear in the lower-left corner of the display.
     */
    class EarthLoader
    {
    public:
        /**
         * Loads a map file and processes all the built-in example command line
         * arguemnts and XML externals.
         */
        osg::Group* load(
            osg::ArgumentParser&   args,
            osgViewer::ViewerBase* viewer,
            Container*             userContainer = 0L) const
        {
            return load(args, viewer, userContainer, 0L);
        }

        osg::Group* load(
            osg::ArgumentParser&   args,
            osgViewer::ViewerBase* viewer,
            Container*             userContainer,
            const osgDB::Options*  readOptions) const;

        /**
         * Takes an existing map node and processes all the built-in example command
         * line arguments and mapNode externals.
         */
        void parse(
            MapNode*             mapNode,
            osg::ArgumentParser& args,
            osgViewer::View*     view,
            osg::Group*          parentGroup,
            Container*           userContainer) const;

        void parse(
            MapNode*             mapNode,
            osg::ArgumentParser& args,
            osgViewer::View*     view,
            osg::Group*          parentGroup,
            LabelControl*        userLabel) const;

        /**
         * Configures a view with a stock set of event handlers that are useful
         * for demos, and performs some other common view configuration for osgEarth.
         */
        void configureView( osgViewer::View* view ) const;

        /**
         * Returns a usage string
         */
        std::string usage() const;
    };


} } // namespace osgEarth::Util
