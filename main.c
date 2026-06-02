#include <stdio.h>
#include "code/inputs.h"
#include "code/initVars.h"
#include "code/graphicOverhead.h"
#include "code/calcGameLogic.h"
#include "code/jsonParser.h"
#include <stdint.h>

static struct GameState gameState;
static struct CallRenderSave callRenderSave;
static struct KeyState keyState;

void timer(int value) {
    calcGameLogic (&gameState, &keyState);
    glutTimerFunc(16, timer, 0);
}

// Have to do this so that we can get the values in
void keyboardUpSpecialInit(int key, int x, int y) {
    keyboardUpSpecial(key, &keyState);
}
void keyboardDownSpecialInit(int key, int x, int y) {
    keyboardDownSpecial(key, &keyState);
}

void keyboardUpInit(unsigned char key, int x, int y) {
    keyboardUp(key, &keyState);
}
void keyboardDownInit(unsigned char key, int x, int y) {
    keyboardDown(key, &keyState);
}


// Define the XOR swap macro
#define XOR_SWAP(pointerA, pointerB) \
    pointerA = (uint8_t**)((uintptr_t)pointerA ^ (uintptr_t)pointerB); \
    pointerB = (uint8_t**)((uintptr_t)pointerA ^ (uintptr_t)pointerB); \
    pointerA = (uint8_t**)((uintptr_t)pointerA ^ (uintptr_t)pointerB);

void display() {
    resetClickArray(&callRenderSave.screenArray);
    drawScreen(&callRenderSave, gameState);

    // Swap the click arrays, so we push the edited click array to testing mouse clicks
    XOR_SWAP(callRenderSave.screenArray.screenClickArrayEdit, callRenderSave.screenArray.screenClickArrayTest);
}

void mouse (int button, int state, int x, int y) {
    getMouseAction(button, state, x, y, callRenderSave.screenArray, &gameState.mouseFeedback);
}

int main (int argc, char** argv) {
    TestLoadingJson();

#ifdef DEBUG
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd");
    }
#endif // DEBUG

    // Yes this order is screwed up. Yes I hate opengl
    initVars(&gameState, (void *)&keyState, sizeof(struct KeyState));
    setScreenSize(&callRenderSave.screenArray);
    initScreen(callRenderSave);
    loadUI(&callRenderSave,gameState.language,gameState.languageNames,gameState.languageCount);

    // Sets function used for everytime display update is called
    glutDisplayFunc(display);

    glutKeyboardFunc(keyboardDownInit);
    glutKeyboardUpFunc(keyboardUpInit);

    glutSpecialFunc(keyboardDownSpecialInit);
    glutSpecialUpFunc(keyboardUpSpecialInit);
    glutMouseFunc(mouse);

    // Sets function used for everytime a new update of game runtime is performed
    glutTimerFunc(0, timer, 0); // Start the timer
    glutMainLoop();
    return 0;
}
