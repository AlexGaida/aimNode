#include "aimNode.h"
#include <maya/MFnPlugin.h>

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin fnPlugin(obj, "Alexei Gaidachev", "1.0", "Any");

	status = fnPlugin.registerNode(
		"aimNode",
		aimNode::typeId,
		&aimNode::creator,
		&aimNode::initialize
	);

	if (status != MS::kSuccess)
		status.perror("Could not register the stretchyIk node");

	return status;
}

MStatus uninitializePlugin(MObject object)
{
	MFnPlugin pluginFn;
	pluginFn.deregisterNode(aimNode::typeId);
	return MS::kSuccess;
}