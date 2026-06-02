#ifndef TWODGRAPHICFUNCTIONS
#define TWODGRAPHICFUNCTIONS

//Need to include inputs because that has screen array, so we can say what pixel is clicking on a thing
#include "inputs.h"
#include "solarSystemData.h"
#include "graphicOverhead.h"
#include "planetData.h"
#include <GL/glut.h>
#include <math.h>

#include "stb_image.h"

void drawMainMenu(struct CallRenderSave callRenderSave, int innerMenuLocation, int languageCount);
GLuint loadTexture(const char* filename, char Language[4]);

void drawSpaceTravelMenu (struct CallRenderSave callRenderSave, struct SolarSystemData solarSystemData, int innerMenuLocation, struct SpaceTimePos spaceTimePos);

#endif // TWODGRAPHICFUNCTIONS
