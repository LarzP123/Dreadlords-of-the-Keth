# Struct PositionAndOrientation3D

A large portion of this game involves traveling on a planet in 3d. In order to keep track of spacial coordinates in 3D, there is a struct in
3dGraphicFunctions.h called PositionAndOrientation3D. This contains the necessassary variables for efficent calcualtions of any object's position and where
it is facing/travelling too.

This struct is then used in 3dGraphicFunctions.c to properly draw where objects are 
and calcGameLogic.c to properly handle movement and updating of these variables as you move

## Inner Variables of PositionAndOrientation3D

Each variable in this struct either helps describe where an object is, or where it is facing. This can usually be described with something like quaternions, but
since all traveling in this game is done on a sphere, we can have some efficiency in calculations by storing things like position or orientation in multiple different
ways for different coordinate systems

### positionTheta, positionPhi, positionRad

<img src="./images/documentation/positionSphericalCoordinates.png" alt="image info" width="400">

### orientationTheta, orientationPhi

<img src="./images/documentation/orientationCylindricalCoordinates.png" alt="image info" width="400">

### semiMajor, semiMinor, moveT

<img src="./images/documentation/orbitPlanet.png" alt="image info" width="400">

<img src="./images/documentation/semiMajorSemiMinorCoordinates.png" alt="image info" width="400">

### positionX, positionY, positionZ

<img src="./images/documentation/positionCartesianCoordinates.png" alt="image info" width="400">

### recalculateMovement

### isMoving

### recalculateLookDirection

### orientationX, orientationY, orientationZ

<img src="./images/documentation/positionCartesianCoordinates.png" alt="image info" width="400">