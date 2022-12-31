#pragma once
#include <osg/NodeVisitor>
class InfoVisitor : public osg::NodeVisitor
{
public:
	InfoVisitor() : _level(0)
	{
		setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
	}

	std::string space()
	{
		return std::string(_level * 2, ' ');
	}

	virtual void apply(osg::Node& node);
	virtual void apply(osg::Geode& geode);
	void showAttribute(osg::Node& node);

private:
	unsigned int _level;
};
