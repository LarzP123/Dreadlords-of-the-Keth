#ifndef GRAPHICOVERHEAD
#define GRAPHICOVERHEAD

#include "inputs.h"
#include "calcGameLogic.h"

struct CallRenderSave{
    // Basic Stuff
    struct ScreenArray screenArray;

    // Draw Calls Store
    GLuint planetSurfaceDrawCalls;

    // Main Menu Screen
    GLuint singeplayerButton;
    GLuint multiplayerButton;
    GLuint optionsButton;
    GLuint languageButton;

    GLuint gameLogo;

    GLuint saveSlot1;
    GLuint saveSlot2;
    GLuint saveSlot3;
    GLuint trash;
    GLuint backButton;

    GLuint* languageOptions;

    GLuint digits[10];
    GLuint speeds[4];

    GLuint panelTexture;

    // 3D environment
    GLuint skybox[6];

    GLuint travelToPlanetButton;
    GLuint landOnPlanetButton;

    GLuint spaceShip[16];

    GLuint year;
    GLuint day;
    GLuint hour;

    GLuint slash;
    GLuint colon;
};

#include <GL/glut.h>
#include "2dGraphicFunctions.h"
#include "3dGraphicFunctions.h"

void drawScreen(struct CallRenderSave* callRenderSave, struct GameState gameState);
void initScreen(struct CallRenderSave callRenderSave);
void loadUI(struct CallRenderSave* callRenderSave, char language[4], char **languageNames, int languageCount);
bool loadPlanetHeightMap(uint8_t *PlanetHeightMap, char planetName[10]);

#endif // SOLARSYSTEMDATA_H_INCLUDED
