#ifndef SOLARSYSTEMDATA_H_INCLUDED
#define SOLARSYSTEMDATA_H_INCLUDED
#include <assert.h>
// For type FILE
#include <stdio.h>
// For assert
#include <stdlib.h>

#include "planetData.h"

struct SpaceTransfer {
    int body1ID;
    int body2ID;
    double startTime;
    double transferTime;
    double energyStart;
    double energyStop;
    double startTheta;
};

struct StarData {
    char starType[2];
    int starSize;
    int starColor;
    int mass;
};

struct SolarSystemData {
    struct PlanetData planetData[MaxPlanetCount];
    struct StarData starData;
};

void loadSolarSystem(struct SolarSystemData *solarSystemData, int solarSystemID);

struct SpaceTransfer calcHohmannTransfer(struct SolarSystemData solarSystemData, int planet1, int planet2, double spaceShipMass);

#endif // SOLARSYSTEMDATA_H_INCLUDED
