#pragma once
/*

@info:
stretchyIk: This is an aim node plugin.
@author: Alexei Gaidachev
@date: February 15, 2017
@versions:
1.0.0 - February 15, 2017:
initial commit.

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
	// define translates values
	static MObject inputTranslateX;
	static MObject inputTranslateY;
	static MObject inputTranslateZ;
	static MObject inputTranslate;
	// define rotates values
	static MObject outputRotateX;
	static MObject outputRotateY;
	static MObject outputRotateZ;
	static MObject outputRotate;
	// define matrixes
	static MObject driverMatrix;
	static MObject upVectorMatrix;
};

#endif