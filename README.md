# aimNode
----
# Information:
  Basic Aim Node C++ Plugin using CMake

# Purpose:
  To save the configuration and build settings of CMake files.

## The Plugin:
  Basically an aim constraint. Using normalized Vectors.

### the math:
  _MVector_ aimVecV = driverMatrix - inputTranslate;

  _MVector_ upVecV  = upVector - inputTranslate;

  upVecV.normalize();
  
  aimVecV.normalize();

  _MVector_ cross = aimVecV ^ upVecV;

  _MVector_ upVec = cross ^ aimVecV;

### building the matrix
  _double_ matrix[4][4] = {

    {aimVecV.x, aimVecV.y, aimVecV.z, 0},
    {upVecV.x, upVecV.y, upVecV.z, 0},
    {cross.x, cross.y, cross.z, 0},
    {inputTranslate[0], inputTranslate[1], inputTranslate[2], 1},

  }
