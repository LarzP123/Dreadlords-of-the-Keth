#ifndef CALCGAMELOGIC_H_INCLUDED
#define CALCGAMELOGIC_H_INCLUDED
#include <stdio.h>
#include <time.h>

#include "solarSystemData.h"
#include "inputs.h"
#include <stdint.h>

struct SpaceTimePos {
    double gameDate; // Years
    double gameSpeed; // Years per Year

    uint8_t currentStar;
    uint8_t currentPlanet;
    // Your position in Space when traveling between planets/stars
    struct SpaceTransfer spaceTransfer;
};

struct GameState {
    clock_t previousTime;

    int planetSurfaceList;

    uint8_t mouseFeedback;

    //1 is main menu
    //2 is star traveling view
    uint8_t gameMenu;

    char language[4];
    uint8_t innerMenuLocation;

    char **languageNames;
    int languageCount;
    struct SolarSystemData solarSystemData;

    struct SpaceTimePos spaceTimePos; // when you're in space
    struct PosOrient3D posOrient3D; // when you're on a planet
};

void startNewGameSetVars (struct GameState* gameState);
void calcGameLogic (struct GameState* gameState, struct KeyState* keyState);

#endif // CALCGAMELOGIC_H_INCLUDED
