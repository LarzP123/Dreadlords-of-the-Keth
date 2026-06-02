#ifndef INPUTS
#define INPUTS

#include <stdbool.h>
#include <GL/glut.h>
#include <stdint.h>

struct ScreenArray {
    int screenXSize;//1024
    int screenYSize;//720

    uint8_t** screenClickArrayTest;
    uint8_t** screenClickArrayEdit;
};

struct KeyState {
    unsigned int keyUp : 1;
    unsigned int keyDown : 1;

    unsigned int keyW : 1;
    unsigned int keyA : 1;
    unsigned int keyS : 1;
    unsigned int keyD : 1;
};

void keyboardDown(char key, struct KeyState *keyState);
void keyboardUp(char key, struct KeyState *keyState);
void keyboardDownSpecial(int key, struct KeyState *keyState);
void keyboardUpSpecial(int key, struct KeyState *keyState);
void getMouseAction(int button, int state, int x, int y, struct ScreenArray screenArray, uint8_t *mouseFeedback);
void setScreenSize(struct ScreenArray *screenArray);
void resetClickArray(struct ScreenArray *screenArray);

#endif // INPUTS
