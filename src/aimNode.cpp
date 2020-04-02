/*

@info:
     aimNode rotation calculation. 
	 Basically works.

*/
#include "aimNode.h"
#include <maya/MGlobal.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MTransformationMatrix.h>
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

// the typeId is a valid ID from Autodesk
MTypeId aimNode::typeId(0x00134e05);

#define EPSILON             0.001

// declare objects
MObject aimNode::translate;
MObject aimNode::translateX;
MObject aimNode::translateY;
MObject aimNode::translateZ;
MObject aimNode::forward;
MObject aimNode::forwardX;
MObject aimNode::forwardY;
MObject aimNode::forwardZ;
MObject aimNode::up;
MObject aimNode::upX;
MObject aimNode::upY;
MObject aimNode::upZ;
MObject aimNode::outputRotate;
MObject aimNode::outputRotateX;
MObject aimNode::outputRotateY;
MObject aimNode::outputRotateZ;

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
	MFnCompoundAttribute compound;
	MFnUnitAttribute uAttr;

	// define output rotates
	outputRotateX = uAttr.create("outputRotateX", "orx", MFnUnitAttribute::kAngle, 0.0);
		uAttr.setStorable(false);
	addAttribute(outputRotateX);

	outputRotateY = uAttr.create("outputRotateY", "ory", MFnUnitAttribute::kAngle, 0.0);
		uAttr.setStorable(false);
	addAttribute(outputRotateY);

	outputRotateZ = uAttr.create("outputRotateZ", "orz", MFnUnitAttribute::kAngle, 0.0);
		uAttr.setStorable(false);
	addAttribute(outputRotateZ);

	// output rotate 
    outputRotate = numFn.create( "outputRotate", "or", outputRotateX, outputRotateY, outputRotateZ );
        numFn.setStorable(false);
	addAttribute(outputRotate);

	// define input translates
	translateX = numFn.create("translateX", "tx", MFnNumericData::kDouble, 0.0);
		numFn.setStorable(false);
	addAttribute(translateX);

	translateY = numFn.create("translateY", "ty", MFnNumericData::kDouble, 0.0);
		numFn.setStorable(false);
	addAttribute(translateY);

	translateZ = numFn.create("translateZ", "tz", MFnNumericData::kDouble, 0.0);
		numFn.setStorable(false);
	addAttribute(translateZ);

	// output rotate 
    translate = numFn.create( "inputTranslate", "int", translateX, translateY, translateZ );
		numFn.setKeyable(true);
		numFn.setStorable(false);
	addAttribute(translate);

	// define input translates
	forwardX = numFn.create("forwardX", "fwx", MFnNumericData::kDouble, 0.0);
		numFn.setStorable(false);
	addAttribute(forwardX);

	forwardY = numFn.create("forwardY", "fwy", MFnNumericData::kDouble, 0.0);
		numFn.setStorable(false);
	addAttribute(forwardY);

	forwardZ = numFn.create("forwardZ", "fwz", MFnNumericData::kDouble, 0.0);
		numFn.setStorable(false);
	addAttribute(forwardZ);

	// input translate
    forward = numFn.create( "forward", "fwd", forwardX, forwardY, forwardZ );
		numFn.setDefault( 0.0, 0.0, 1.0 );
        numFn.setStorable(false);
		numFn.setKeyable(true);
	addAttribute(forward);

    // the up vector
    upX = numFn.create( "upX", "ux", MFnNumericData::kDouble, 0.0 );
        numFn.setStorable(false);
	addAttribute(upX);
    upY = numFn.create( "upY", "uy", MFnNumericData::kDouble, 0.0 );
        numFn.setStorable(false);
	addAttribute(upY);
    upZ = numFn.create( "upZ", "uz", MFnNumericData::kDouble, 0.0 );
        numFn.setStorable(false);
	addAttribute(upZ);
    up = numFn.create( "upVector", "up", upX, upY, upZ );
        numFn.setDefault( 0.0, 1.0, 0.0 );
		numFn.setKeyable(true);
        numFn.setStorable(false);
	addAttribute(up);

	// make attribute connections
	attributeAffects(forward, outputRotate);
	attributeAffects(up, outputRotate);
	attributeAffects(translate, outputRotate);

	return MS::kSuccess;
}


MStatus aimNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	// get data
	if ((plug == outputRotate) || (plug == forward) || (plug == up) || (plug == translate))
	{
		// extract input data
		MDataHandle forwardData = dataBlock.inputValue( forward );
		MDataHandle upData = dataBlock.inputValue( up );
		MDataHandle translateData = dataBlock.inputValue( translate );
		MVector translation = translateData.asVector();
		// in XYZ z vector is the forward vector
		MVector z_vec = forwardData.asVector();
		MVector upVector = upData.asVector();

		// normalize the upVector
		z_vec *= -1;
		upVector.normalize();
		z_vec.normalize();

		// extrace output data
        MDataHandle outputRot = dataBlock.outputValue( outputRotate );
		MVector x_vec = z_vec ^ upVector;
		x_vec.normalize();

		// get the y vector
		MVector y_vec = x_vec ^ z_vec;
		y_vec.normalize();

		// build rotation matrix
		double MyMatrix[4][4] = { {x_vec.x, x_vec.y, x_vec.z, 0},
								  {y_vec.x, y_vec.y, y_vec.z, 0},
								  {z_vec.x, z_vec.y, z_vec.z, 0},
								  {0.0, 0.0, 0.0, 1.0} 
		};

		// XYZ rotation order
		MMatrix matrix = MyMatrix;
		MTransformationMatrix result(matrix);
		MTransformationMatrix::RotationOrder rotOrder;
		rotOrder = MTransformationMatrix::kXYZ;
        result.reorderRotation(rotOrder);

        double rotation[3];
        result.getRotation( rotation, rotOrder);

		// set output values
        outputRot.set( rotation[0], rotation[1], rotation[2] );
        outputRot.setClean();
	}

	return MS::kSuccess;
}