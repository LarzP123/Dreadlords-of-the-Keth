#ifndef PLANETDATA
#define PLANETDATA

// For assert
#include <assert.h>
#include <stdlib.h>

#include <stdbool.h>
#include "larzMath.h"
#include "stb_image.h"

// For type FILE
#include <stdio.h>
#include <stdint.h>

#define MaxPlanetCount 8

#define PlanetHeightSizeWidth 2000
#define PlanetHeightSizeHeight 2000
#define PlanetHeightSize 4000000

struct PlanetData {
    double planetNSurfaces;
    double yearAngleStarting;
    double *orbitRadius; // This is in Atomic Mass Units
    double *orbitRadiusDraw; // This is pixel count
    double *planetRadius;

    double landColor[3];
    double seaLevel;
    double seaColor[3];

    uint8_t *planetHeightData;
};

void loadPlanetData(struct PlanetData *planetData, int planetOrder, int planetIds[8], FILE *file);
double getPlanetSurfaceHeight(uint8_t planetHeightData[PlanetHeightSize], double theta, double phi);

#endif // PLANETDATA
