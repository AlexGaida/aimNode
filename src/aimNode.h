#pragma once
/*

@info:
aimNode: This is an aim rotation node plugin.
@author: Alexei Gaidachev
@date: February 15, 2017
@versions:
1.0.0 - February 15, 2017:
    initial commit.
1.1.0 - April 01, 2020:
    updated the code from Autodesks' reference site.

*/

// classify header
#ifndef aimNode_H
#define aimNode_H

// import modules
#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// define constructor
class aimNode : public MPxNode
{
public:
	static void* creator();
	aimNode();
	static MStatus initialize();
	virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock);

public:
	static MTypeId typeId;
	// define translate values
	static MObject translateX;
	static MObject translateY;
	static MObject translateZ;
	static MObject translate;
	// define forward values
	static MObject forwardX;
	static MObject forwardY;
	static MObject forwardZ;
	static MObject forward;
	// define rotates values
	static MObject outputRotateX;
	static MObject outputRotateY;
	static MObject outputRotateZ;
	static MObject outputRotate;
	// define aim node up vector
	static  MObject     up;
    static  MObject     upX;
    static  MObject     upY;
    static  MObject     upZ;
};

#endif