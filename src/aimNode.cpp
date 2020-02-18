/*

@info:
A part of rigging dojo plugin series.

*/
#include "aimNode.h"
#include <maya/MGlobal.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFloatVector.h>
#include <maya/MMatrix.h>
#include <maya/MVector.h>
#include <maya/MDataHandle.h>
#include <maya/MPoint.h>
#include <math.h>

using namespace std;

MTypeId aimNode::typeId(0x80001);

// declare objects
MObject aimNode::inputTranslate;
MObject aimNode::inputTranslateX;
MObject aimNode::inputTranslateY;
MObject aimNode::inputTranslateZ;
MObject aimNode::outputRotate;
MObject aimNode::outputRotateX;
MObject aimNode::outputRotateY;
MObject aimNode::outputRotateZ;
MObject aimNode::upVectorMatrix;
MObject aimNode::driverMatrix;

// class constructor
// useful when you want add variables and initialize them
aimNode::aimNode()
{
}

// change the name of the class we are creating
void* aimNode::creator()
{
	return new aimNode();
}


MStatus aimNode::initialize()
{
	// initialize Maya attributes
	MFnNumericAttribute numFn;
	MFnMatrixAttribute matrixFn;
	MFnTypedAttribute typedFn;
	MFnCompoundAttribute compound;
	MFnUnitAttribute uAttr;

	// define matrixes
	driverMatrix = matrixFn.create("driverMatrix", "dvm");
	addAttribute(driverMatrix);

	upVectorMatrix = matrixFn.create("upVectorMatrix", "uvm");
	addAttribute(upVectorMatrix);

	// define input translates
	inputTranslateX = numFn.create("inputTranslateX", "itx", MFnNumericData::kDouble, 0.0);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setWritable(true);
	addAttribute(inputTranslateX);

	inputTranslateY = numFn.create("inputTranslateY", "ity", MFnNumericData::kDouble, 0.0);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setWritable(true);
	addAttribute(inputTranslateY);

	inputTranslateZ = numFn.create("inputTranslateZ", "ity", MFnNumericData::kDouble, 0.0);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setWritable(true);
	addAttribute(inputTranslateZ);

	inputTranslate = compound.create("inputTranslate", "intr");
	compound.addChild(inputTranslateX);
	compound.addChild(inputTranslateY);
	compound.addChild(inputTranslateZ);
	compound.setStorable(true);
	compound.setKeyable(true);
	compound.setWritable(true);
	addAttribute(inputTranslate);

	// define output rotates
	outputRotateX = uAttr.create("outputRotateX", "orx", MFnUnitAttribute::kAngle, 0.0);
	uAttr.setStorable(true);
	uAttr.setKeyable(true);
	uAttr.setWritable(true);
	addAttribute(outputRotateX);

	outputRotateY = uAttr.create("outputRotateY", "ory", MFnUnitAttribute::kAngle, 0.0);
	uAttr.setStorable(true);
	uAttr.setKeyable(true);
	uAttr.setWritable(true);
	addAttribute(outputRotateY);

	outputRotateZ = uAttr.create("outputRotateZ", "orz", MFnUnitAttribute::kAngle, 0.0);
	uAttr.setStorable(true);
	uAttr.setKeyable(true);
	uAttr.setWritable(true);
	addAttribute(outputRotateZ);

	outputRotate = compound.create("outputRotate", "outr");
	compound.addChild(outputRotateX);
	compound.addChild(outputRotateY);
	compound.addChild(outputRotateZ);
	compound.setStorable(true);
	compound.setKeyable(true);
	compound.setWritable(true);
	addAttribute(outputRotate);

	// make attribute connections
	attributeAffects(driverMatrix, outputRotate);
	attributeAffects(upVectorMatrix, outputRotate);
	attributeAffects(inputTranslate, outputRotate);

	return MS::kSuccess;
}


MStatus aimNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	// get data
	if ((plug == outputRotate) || (plug == outputRotateX) || (plug == outputRotateY) || (plug == outputRotateZ))
	{
		// extract input data
		MMatrix driverMatrixV = dataBlock.inputValue(driverMatrix).asMatrix();
		MMatrix upVectorMatrixV = dataBlock.inputValue(upVectorMatrix).asMatrix();
		MVector inputTranslateV = dataBlock.inputValue(inputTranslate).asVector();

		// extract positions
		MVector driverMatrixPos (driverMatrixV[3][0], driverMatrixV[3][1], driverMatrixV[3][2]);
		MVector upVectorMatrixPos(upVectorMatrixV[3][0], upVectorMatrixV[3][1], upVectorMatrixV[3][2]);

		// compute needed vectors and normalize
		MVector upVecV = upVectorMatrixPos - inputTranslateV;
		MVector aimVecV = driverMatrixPos - inputTranslateV;
		upVecV.normalize();
		aimVecV.normalize();

		// compute cross product to get perpendicular vector
		MVector cross = aimVecV ^ upVecV;
		MVector upVec = cross ^ aimVecV;

		// build rotation matrix
		double MyMatrix[4][4] = { { aimVecV.x, aimVecV.y, aimVecV.z, 0},
								  {upVec.x, upVec.y, upVec.z, 0},
								  {cross.x, cross.y, cross.z, 0},
								  {inputTranslateV[0], inputTranslateV[1], inputTranslateV[2], 1} };

		// extract euler rotation
		MMatrix rotMatrix(MyMatrix);
		MTransformationMatrix matrixFn(rotMatrix);
		MEulerRotation euler = matrixFn.eulerRotation();

		// set output values
		dataBlock.outputValue(outputRotate).set(euler.x, euler.y, euler.z);
		dataBlock.outputValue(outputRotate).setClean();

	}

	return MS::kSuccess;
}