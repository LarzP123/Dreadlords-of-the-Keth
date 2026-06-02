#ifndef LARZMATH
#define LARZMATH
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#define aPi 3.141592

bool getPosFromPerpAxes(double ax1[3],double ax2[3],double *posReturn[3],double angle);
int mod(int a, int b);
bool norm3DVec(double vec[3],double newLength);
bool scaleMaxMin(double *inputData, int inputSize,double targetMin,double targetMax, double *returnSlope, double *returnOffset);
double correctAngleDiff(double angleDif);
bool crossProd(double v1[3],double v2[3],double vOut[3]);
struct TimeDisplay convertDaysToConvenient (double days);
void convertDateToString (char* dateDisplay, double gameDate, int displaySystem);

struct TimeDisplay {
    double Value;
    char Unit;
};

struct PosOrient3D {
    double positionTheta; double positionPhi; double positionRad;
    double orientationTheta; double orientationPhi;
    double semiMajor[3]; double semiMinor[3]; double moveT;
    double positionX, positionY, positionZ;
    bool recalculateMovement; bool isMoving; bool recalculateLookDirection;
    double orientationX; double orientationY; double orientationZ;
};

#ifdef DEBUG
void calculateNormal(double edge1[3], double edge2[3], double normal[3], double refPos[3]);
#else // DEBUG
void calculateNormal(double edge1[3], double edge2[3], double normal[3]);
#endif // DEBUG

#endif // SOLARSYSTEMDATA_H_INCLUDED
