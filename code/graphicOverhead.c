#include "graphicOverhead.h"
#include <string.h>

#define MAX_NAME_LENGTH 60  // Define the maximum size (including null terminator)

void loadUI(struct CallRenderSave* callRenderSave, char language[4], char **languageNames, int languageCount) {
    callRenderSave->planetSurfaceDrawCalls = -1;
    // glutInit takes parameters that I don't care about. First 1 is a pointer to 1, net one is an empty string.
    callRenderSave->gameLogo = loadTexture("images\\buttons\\gameLogo", language);

    callRenderSave->singeplayerButton = loadTexture("images\\buttons\\singleplayer", language);
    callRenderSave->multiplayerButton = loadTexture("images\\buttons\\multiplayer", language);
    callRenderSave->optionsButton = loadTexture("images\\buttons\\options", language);
    callRenderSave->languageButton = loadTexture("images\\buttons\\languageSelect.png", NULL);

    callRenderSave->travelToPlanetButton = loadTexture("images\\buttons\\travelPlanet", language);
    callRenderSave->landOnPlanetButton = loadTexture("images\\buttons\\landOnPlanet", language);

    callRenderSave->saveSlot1 = loadTexture("images\\buttons\\gameState1", language);
    callRenderSave->saveSlot2 = loadTexture("images\\buttons\\gameState2", language);
    callRenderSave->saveSlot3 = loadTexture("images\\buttons\\gameState3", language);
    callRenderSave->trash = loadTexture("images\\buttons\\trash.png", NULL);
    callRenderSave->backButton = loadTexture("images\\buttons\\back", language);

    // load the skyboxes
    callRenderSave->skybox[0] = loadTexture("images\\spaceSkyboxes\\back.png", NULL);
    callRenderSave->skybox[1] = loadTexture("images\\spaceSkyboxes\\bottom.png", NULL);
    callRenderSave->skybox[2] = loadTexture("images\\spaceSkyboxes\\front.png", NULL);
    callRenderSave->skybox[3] = loadTexture("images\\spaceSkyboxes\\left.png", NULL);
    callRenderSave->skybox[4] = loadTexture("images\\spaceSkyboxes\\right.png", NULL);
    callRenderSave->skybox[5] = loadTexture("images\\spaceSkyboxes\\top.png", NULL);

    // replace the X with the actual digit so the digit 1 is at images\\sidePanels\\1
    char digitLocation[] = "images\\sidePanels\\X";
    for (int i=0;i<10;i++) {
        digitLocation[18] = '0' + i;
        callRenderSave->digits[i] = loadTexture(digitLocation, language);
    }

    char speedLocation[] = "images\\buttons\\speedX.png";
    for (int i=0;i<4;i++) {
        speedLocation[20] = '1' + i;
        callRenderSave->speeds[i] = loadTexture(speedLocation, NULL);
    }

    callRenderSave->slash = loadTexture("images\\sidePanels\\slash.png", NULL);
    callRenderSave->colon = loadTexture("images\\sidePanels\\colon.png", NULL);

    callRenderSave->year = loadTexture("images\\buttons\\year", language);
    callRenderSave->day = loadTexture("images\\buttons\\day", language);
    callRenderSave->hour = loadTexture("images\\buttons\\hour", language);

    // This is because we are going to store a 2d version of the space ship at multiple rotations relative to light source for images
    callRenderSave->spaceShip[0] = loadTexture("images\\objects\\spaceShipStarter.png", NULL);

    callRenderSave->panelTexture = loadTexture("images\\sidePanels\\panel.png", NULL);

    callRenderSave->languageOptions = malloc(languageCount * sizeof(int));
    char *languageFolderDirectory = malloc(26 * sizeof(char));
    strcpy(languageFolderDirectory, "images\\languages\\XXX.png"); // NOLINT(clang-analyzer-security.insecureAPI.strcpy). This is a pain to work around, so I don't care
    for (int i=0; i<languageCount; i++) {
        for (int letter=0; letter<3; letter++) {
            languageFolderDirectory[17+letter] = languageNames[i][letter];
        }

        callRenderSave->languageOptions[i] = loadTexture(languageFolderDirectory, NULL);
    }
    free(languageFolderDirectory);
}

// PLEASE CHANGE THIS TO USE A POINTER IN THE FUTURE FOR CALL RENDER SAVE TO SAVE STACK SPACE AND SPEED
void drawScreen(struct CallRenderSave* callRenderSave, struct GameState gameState) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //planetViewDrawcalls;
    switch (gameState.gameMenu) {
        case 1: {
            drawMainMenu(*callRenderSave, gameState.innerMenuLocation, gameState.languageCount);
            break;
        }
        case 2: {
            drawSpaceTravelMenu(*callRenderSave, gameState.solarSystemData, gameState.innerMenuLocation, gameState.spaceTimePos);
            break;
        }
        case 3: {
            struct PlanetData thisPlanetData = gameState.solarSystemData.planetData[gameState.spaceTimePos.currentPlanet];
            if (callRenderSave->planetSurfaceDrawCalls == -1) {
                loadLandOnPlanetGraphics(callRenderSave, &gameState.posOrient3D, thisPlanetData);
            }

            drawPlanetTravelGameplay(callRenderSave, &gameState.posOrient3D, thisPlanetData);
            break;
        }
    }

    glutSwapBuffers();
    glutReshapeWindow(callRenderSave->screenArray.screenXSize, callRenderSave->screenArray.screenYSize);
}

// PLEASE CHANGE THIS TO USE A POINTER IN THE FUTURE FOR CALL RENDER SAVE TO SAVE STACK SPACE AND SPEED
void initScreen(struct CallRenderSave callRenderSave) {
    glutInit((int[]){1}, (char*[]){(char*)""});
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(callRenderSave.screenArray.screenXSize, callRenderSave.screenArray.screenYSize);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Circle Drawing");
}
