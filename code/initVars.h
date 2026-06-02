#ifndef INITVARS_H_INCLUDED
#define INITVARS_H_INCLUDED

#include "calcGameLogic.h"
#include "solarSystemData.h"
#include "planetData.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>

void initVars (struct GameState *gameState, void *keyState, int sizeOfKeyState);

#endif // INITVARS_H_INCLUDED
