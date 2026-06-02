/// Key Inputs Read
#include "inputs.h"
#include <stdio.h>
#include <stdlib.h>

void keyboardDownSpecial(int key, struct KeyState *keyState) {
    if (key == GLUT_KEY_UP) { keyState->keyUp = 1; }
    if (key == GLUT_KEY_DOWN) { keyState->keyDown = 1; }
    if (key == GLUT_KEY_LEFT) { keyState->keyA = 1; }
    if (key == GLUT_KEY_RIGHT) { keyState->keyD = 1; }
}

void keyboardUpSpecial(int key, struct KeyState *keyState) {
    if (key == GLUT_KEY_UP) { keyState->keyUp = 0; }
    if (key == GLUT_KEY_DOWN) { keyState->keyDown = 0; }
    if (key == GLUT_KEY_LEFT) { keyState->keyA = 0; }
    if (key == GLUT_KEY_RIGHT) { keyState->keyD = 0; }
}

void keyboardDown(char key, struct KeyState *keyState) {
    if (key == 'w') { keyState->keyW = 1; }
    if (key == 's') { keyState->keyS = 1; }
}

void keyboardUp(char key, struct KeyState *keyState) {
    if (key == 'w') { keyState->keyW = 0; }
    if (key == 's') { keyState->keyS = 0; }
}

// Mouse Inputs Read

void getMouseAction(int button, int state, int x, int y, struct ScreenArray screenArray, uint8_t *mouseFeedback) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Record the mouse click coordinates
        *mouseFeedback = screenArray.screenClickArrayTest[x][y];
        printf("mouse hit %d\n",*mouseFeedback);
        //do stuff here?
    } // GLUT
}

void setScreenSize(struct ScreenArray *screenArray){
    screenArray->screenXSize=1024;
    screenArray->screenYSize=720;

    screenArray->screenClickArrayEdit = (uint8_t **)malloc(screenArray->screenXSize * sizeof(uint8_t *));
    screenArray->screenClickArrayTest = (uint8_t **)malloc(screenArray->screenXSize * sizeof(uint8_t *));
    for (int i = 0; i < screenArray->screenXSize; i++) {
        screenArray->screenClickArrayEdit[i] = (uint8_t *)malloc(screenArray->screenYSize * sizeof(uint8_t));
        screenArray->screenClickArrayTest[i] = (uint8_t *)malloc(screenArray->screenYSize * sizeof(uint8_t));
    }
}

void resetClickArray(struct ScreenArray *screenArray){
    for (int i = 0; i < screenArray->screenXSize; i++) {
        for (int j = 0; j < screenArray->screenYSize; j++) {
            screenArray->screenClickArrayEdit[i][j] = 255;
        }
    }
}
