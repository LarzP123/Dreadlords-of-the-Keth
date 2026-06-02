
#include "2dGraphicFunctions.h"

// This goes in the .c file not the header file
// Don't really get why, but see here: https://stackoverflow.com/questions/43348798/double-inclusion-and-headers-only-library-stbi-image
#include "stb_image.h"
#include <string.h>


// Initialize textures for each face of the cube
GLuint loadTexture(const char* filename, char language[4]) {
    int width, height, channels;
    // Get the language
    unsigned char* texDat;
    if (language != NULL && strlen(language) > 0) { // NOLINT(clang-analyzer-core.NullDereference). Clang warns that language may be null here
        char *filenameLang = malloc(strlen(filename) + 9);
        strcpy(filenameLang, filename); // NOLINT(clang-analyzer-security.insecureAPI.strcpy). This is a pain to work around, so I don't care
        strcat(filenameLang, "_"); // NOLINT(clang-analyzer-security.insecureAPI.strcpy). This is a pain to work around, so I don't care
        strcat(filenameLang, language); // NOLINT(clang-analyzer-security.insecureAPI.strcpy). This is a pain to work around, so I don't care
        strcat(filenameLang, ".png"); // NOLINT(clang-analyzer-security.insecureAPI.strcpy). This is a pain to work around, so I don't care
        texDat = stbi_load(filenameLang, &width, &height, &channels, STBI_rgb_alpha);
#ifdef DEBUG
        if (!texDat) {
            fprintf(stderr, "Failed to load texture: %s\n", filenameLang);
            return 0;
        }
#endif // DEBUG
        free(filenameLang);
    } else {
        texDat = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
        // Error handling
        if (!texDat) {
            fprintf(stderr, "Failed to load texture: %s\n", filename);
            return 0;
        }
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Since you're loading the image with STBI_rgb_alpha, use GL_RGBA here
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDat);

    stbi_image_free(texDat);  // Use stbi_image_free to free the image data
    return tex;
}

void draw2DTexture(GLuint textureID, float x1, float x2, float y1, float y2, struct ScreenArray* screenArray, uint8_t clickStore, bool intPixelExact) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBegin(GL_QUADS);
    if (intPixelExact) {
        glTexCoord2i(0, 1); glVertex2i(x1, y1);
        glTexCoord2i(0, 0); glVertex2i(x1, y2);
        glTexCoord2i(1, 0); glVertex2i(x2, y2);
        glTexCoord2i(1, 1); glVertex2i(x2, y1);
    } else {
        glTexCoord2i(0, 1); glVertex2i(x1*screenArray->screenXSize, y1*screenArray->screenYSize);
        glTexCoord2i(0, 0); glVertex2i(x1*screenArray->screenXSize, y2*screenArray->screenYSize);
        glTexCoord2i(1, 0); glVertex2i(x2*screenArray->screenXSize, y2*screenArray->screenYSize);
        glTexCoord2i(1, 1); glVertex2i(x2*screenArray->screenXSize, y1*screenArray->screenYSize);
    }
    glEnd();

    if (clickStore != 255) {
        for (int i=x1*screenArray->screenXSize; i<x2*screenArray->screenXSize; i++) {
            for (int j=screenArray->screenYSize-y2*screenArray->screenYSize; j<screenArray->screenYSize-y1*screenArray->screenYSize; j++) {
                screenArray->screenClickArrayEdit[i][j] = clickStore;
            }
        }
    }
}

void draw2DRotatedTexture(GLuint textureID, float x1, float x2, float y1, float y2, struct ScreenArray* screenArray, uint8_t clickStore, float theta, bool intPixelExact) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBegin(GL_QUADS);

    // This offset is needed in order to make sure that theta=0 has the object facing how it normally would if you just call draw2DTexture
    float theta2 = theta - aPi/4;

    float xAve = (x1+x2) / 2;
    float yAve = (y1+y2) / 2;
    float xDif = (x2-xAve); // we don't need absolute value checks here because we can assume x2>x1 for efficency
    float yDif = (y2-yAve); // we don't need absolute value checks here because we can assume y2>y1 for efficency

    // here the alternating of sin and cos are equivalent to doing cos(x), cos(x+pi/2), cos(x+pi), cos(x+3pi/2) as as the different corners of a square shoot out at different angles
    if (intPixelExact) {
        glTexCoord2i(0, 1); glVertex2i(xAve+xDif*+cos(theta2), yAve+yDif*+sin(theta2));
        glTexCoord2i(0, 0); glVertex2i(xAve+xDif*-sin(theta2), yAve+yDif*+cos(theta2));
        glTexCoord2i(1, 0); glVertex2i(xAve+xDif*-cos(theta2), yAve+yDif*-sin(theta2));
        glTexCoord2i(1, 1); glVertex2i(xAve+xDif*+sin(theta2), yAve+yDif*-cos(theta2));
    } else {
        glTexCoord2i(0, 1); glVertex2i((xAve+xDif*+cos(theta2))*screenArray->screenXSize, (yAve+yDif*+sin(theta2))*screenArray->screenYSize);
        glTexCoord2i(0, 0); glVertex2i((xAve+xDif*-sin(theta2))*screenArray->screenXSize, (yAve+yDif*+cos(theta2))*screenArray->screenYSize);
        glTexCoord2i(1, 0); glVertex2i((xAve+xDif*-cos(theta2))*screenArray->screenXSize, (yAve+yDif*-sin(theta2))*screenArray->screenYSize);
        glTexCoord2i(1, 1); glVertex2i((xAve+xDif*+sin(theta2))*screenArray->screenXSize, (yAve+yDif*-cos(theta2))*screenArray->screenYSize);
    }
    glEnd();

    // Probably update this later since this is like still a square hit box, but I don't really know if this matters
    if (clickStore != 255) {
        for (int i=x1*screenArray->screenXSize; i<x2*screenArray->screenXSize; i++) {
            for (int j=screenArray->screenYSize-y2*screenArray->screenYSize; j<screenArray->screenYSize-y1*screenArray->screenYSize; j++) {
                screenArray->screenClickArrayEdit[i][j] = clickStore;
            }
        }
    }
}

static void drawOrbitRing(int xAve, int yAve, int orbitRadius, int orbitRadiusScaler, float circleHalfThickness, bool slashed, float color[4], float angleChangeScaler) {
    glEnable(GL_BLEND);  // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_STRIP); // Use a strip to render the ring with thickness
    bool drawSegment = true;
    float angleChange = orbitRadiusScaler/orbitRadius * 0.01 * angleChangeScaler;

    for (float angle = 0.0f; angle <= 2*aPi + angleChange; angle += angleChange) { // Small steps for smoothness
        if (slashed) { drawSegment = !drawSegment; }
        float outerX = xAve + (orbitRadius + circleHalfThickness) * cos(angle);
        float outerY = yAve + (orbitRadius + circleHalfThickness) * sin(angle);
        float innerX = xAve + (orbitRadius - circleHalfThickness) * cos(angle);
        float innerY = yAve + (orbitRadius - circleHalfThickness) * sin(angle);

        glColor4f(color[0], color[1], color[2], drawSegment * color[3]); // Adjust transparency for off segments
        glVertex2f(outerX, outerY);
        glVertex2f(innerX, innerY);
    }

    glEnd();
}

// PLEASE CHANGE THIS TO USE A POINTER IN THE FUTURE FOR CALL RENDER SAVE TO SAVE STACK SPACE AND SPEED
void drawMainMenu(struct CallRenderSave callRenderSave, int innerMenuLocation, int languageCount) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, callRenderSave.screenArray.screenXSize, 0.0, callRenderSave.screenArray.screenYSize, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // Bind the texture
    glEnable(GL_TEXTURE_2D);

    // Front screen main menu
    if (innerMenuLocation == 0){
        draw2DTexture(callRenderSave.gameLogo, 0.25, 0.75, 0.75, 0.925, &callRenderSave.screenArray, 255, false);
        draw2DTexture(callRenderSave.singeplayerButton, 0.25, 0.75, 0.45, 0.55, &callRenderSave.screenArray, 1, false);
        draw2DTexture(callRenderSave.multiplayerButton, 0.25, 0.75, 0.3, 0.4, &callRenderSave.screenArray, 2, false);
        draw2DTexture(callRenderSave.optionsButton, 0.40, 0.75, 0.15, 0.25, &callRenderSave.screenArray, 3, false);
        draw2DTexture(callRenderSave.languageButton, 0.25, 0.35, 0.15, 0.25, &callRenderSave.screenArray, 4, false);
    } else if (innerMenuLocation == 1) {
        draw2DTexture(callRenderSave.gameLogo, 0.25, 0.75, 0.75, 0.925, &callRenderSave.screenArray, 255, false);

        //Select Game Slot
        draw2DTexture(callRenderSave.saveSlot1, 0.3, 0.7, 0.45, 0.55, &callRenderSave.screenArray, 11, false);
        draw2DTexture(callRenderSave.trash, 0.6, 0.69, 0.46, 0.54, &callRenderSave.screenArray, 21, false);
        draw2DTexture(callRenderSave.saveSlot2, 0.3, 0.7, 0.3, 0.4, &callRenderSave.screenArray, 12, false);
        draw2DTexture(callRenderSave.trash, 0.6, 0.69, 0.31, 0.39, &callRenderSave.screenArray, 21, false);
        draw2DTexture(callRenderSave.saveSlot3, 0.3, 0.7, 0.15, 0.25, &callRenderSave.screenArray, 13, false);
        draw2DTexture(callRenderSave.trash, 0.6, 0.69, 0.16, 0.24, &callRenderSave.screenArray, 21, false);
        draw2DTexture(callRenderSave.backButton, 0.75, 0.85, 0.15, 0.25, &callRenderSave.screenArray, 0, false);
    } else if (innerMenuLocation == 2) {
        draw2DTexture(callRenderSave.backButton, 0.75, 0.85, 0.15, 0.25, &callRenderSave.screenArray, 0, false);
    } else if (innerMenuLocation == 3) {
        draw2DTexture(callRenderSave.backButton, 0.75, 0.85, 0.15, 0.25, &callRenderSave.screenArray, 0, false);
    } else if (innerMenuLocation == 4) {
        draw2DTexture(callRenderSave.backButton, 0.75, 0.85, 0.15, 0.25, &callRenderSave.screenArray, 0, false);

        for (int i=0; i<languageCount; i++) {
            draw2DTexture(callRenderSave.languageOptions[i], 0.4, 0.6, 0.8-0.12*i, 0.9-0.12*i, &callRenderSave.screenArray, 20+i, false);
        }
    }
    // Start Game, continue or delete save file

    // Multiplayer Menu

    // Disable textures after drawing
    glDisable(GL_TEXTURE_2D);
}

// MAKE CALL RENDER SAVE A POINTER PLEASE!!!!!!!!!!!!!!!!! and planetData too please
static void drawPlanet (struct CallRenderSave callRenderSave, bool zoomedInSideView, struct PlanetData planetData, double InGameTime, int clickSave, float x1, float x2, float y1, float y2, bool drawHaloView, bool drawHaloOn) {
    int xAve = (x1 + x2) * callRenderSave.screenArray.screenXSize / 2;
    int yAve = (y1 + y2) * callRenderSave.screenArray.screenYSize / 2;

    int xDif = (x2 - x1) * callRenderSave.screenArray.screenXSize;
    int yDif = (y2 - y1) * callRenderSave.screenArray.screenYSize;
    int planetDif = ((xDif < yDif) ? xDif : yDif);

    int planetRadius = *(planetData.planetRadius);

    // This needs some math later on to account for time
    double yearAngle = planetData.yearAngleStarting;

    double n_surface = planetData.planetNSurfaces;
    double fluidLevel = planetData.seaLevel;
    int posX, posY;

    #define max_Illum  0.8
    #define min_Illum 0.3

    if (zoomedInSideView){
        posX = xAve; posY = yAve;// Circle position
        planetRadius = planetDif / 2;
    } else {
        int orbitRadius = *(planetData.orbitRadiusDraw) * planetDif / 2000;
        planetRadius = planetRadius * planetDif / 2000;
        // the Planet is a circle. This is the position of it's center
        posX = orbitRadius*cos(yearAngle);
        posY = orbitRadius*sin(yearAngle);

        posX += xAve;
        posY += yAve;

        float planetOrbitColor[4] = {1.0f, 1.0f, 1.0f, 0.2f};
        drawOrbitRing(xAve, yAve, orbitRadius, planetDif, 1, true, planetOrbitColor, 1.0);
    }

    if (drawHaloView) {
        float haloColor[4] = {0.8f, 1.0f, 1.0f, 0.8f};
        drawOrbitRing(posX, posY, planetRadius*1.1, planetDif, 1, false, haloColor, 1.0);
    } else if (drawHaloOn) {
        float haloColor[4] = {0.7f, 0.6f, 1.0f, 0.8f};
        drawOrbitRing(posX, posY, planetRadius*1.1, planetDif, 1, false, haloColor, 1.0);
    }

    // Spherical coordinates for finding spot on sphere for mapping height coords
    double phi_offset = 0;
    double theta_offset = 0;

    double heightScale = 1.0f;
    double invHeightScale = 1.0f / (1.0f + heightScale);

    double invRadius = 1.0f / planetRadius;
    double pi2 = 2 * aPi;

    glBegin(GL_POINTS);
    for (int x = -planetRadius; x < planetRadius; x++) {
        double phiSet = asinf(x * invRadius) + phi_offset;
        int circleLength = sqrtf(planetRadius * planetRadius - x * x);

        for (int y = -circleLength; y < circleLength; y++) {
            double theta = asinf(y * invRadius) + theta_offset;

            double phi = phiSet;

            double surfaceHeight = getPlanetSurfaceHeight(planetData.planetHeightData, theta, phi)/255.0f;

            if (theta >= pi2) {
                theta = pi2 - theta;
                phi -= aPi;
            } else if (theta <= 0) {
                theta = pi2 - theta;
                phi -= aPi;
            }

            double incAngle = sqrtf(theta * theta + phi * phi);
            double transAngle = asinf(sinf(incAngle) / n_surface);

            // Fresnel's Equations
            // https://en.wikipedia.org/wiki/Fresnel_equations
            double cosIncAngle = cosf(incAngle);
            double cosTransAngle = cosf(transAngle);
            double reflectanceS = (cosIncAngle - n_surface * cosTransAngle) / (cosIncAngle + n_surface * cosTransAngle);
            reflectanceS = reflectanceS * reflectanceS;
            double reflectanceP = (cosTransAngle - n_surface * cosIncAngle) / (cosTransAngle + n_surface * cosIncAngle);
            reflectanceP = reflectanceP * reflectanceP;

            double illuminance = 1.0f - 0.5*(reflectanceS + reflectanceP);

            if (illuminance < 0.0f) {
                illuminance = 0.0f;
            }

            illuminance += surfaceHeight * heightScale;
            illuminance = min_Illum + (max_Illum - min_Illum) * illuminance * invHeightScale;
            if (fluidLevel<0.02){
                glColor3f(illuminance * (planetData.seaColor[0]*(1-surfaceHeight) + surfaceHeight*planetData.landColor[0]),
                          illuminance * (planetData.seaColor[1]*(1-surfaceHeight) + surfaceHeight*planetData.landColor[1]), //glColor3f
                          illuminance * (planetData.seaColor[2]*(1-surfaceHeight) + surfaceHeight*planetData.landColor[2])); //glColor3f
            } else if (surfaceHeight < fluidLevel) {
                glColor3f(illuminance * planetData.seaColor[0], illuminance * planetData.seaColor[1], illuminance * planetData.seaColor[2]);
            } else {
                glColor3f(illuminance * planetData.landColor[0], illuminance * planetData.landColor[1], illuminance * planetData.landColor[2]);
            }
            //glColor3f(0.6f, 0.6f, 0.6f);

            glVertex2f(x + posX, y + posY);
            callRenderSave.screenArray.screenClickArrayEdit[x + posX][callRenderSave.screenArray.screenYSize - (y + posY)] = clickSave;
        }
    }
    glEnd();
}

#define X1ViewPort 0.185
#define X2ViewPort 0.835
#define Y1ViewPort 0.055
#define Y2ViewPort 0.985

static void drawSolarSystem (struct CallRenderSave callRenderSave, struct SolarSystemData solarSystemData, int planetView, int currentPlanet) {
    // hardcoded in this is the x limits and y limits of where the farthest planets should reach as the last 4 numbers
    for (int i=0;i<MaxPlanetCount;i++){
        #define drawHaloView planetView==i
        #define drawHaloOn currentPlanet==i
        drawPlanet(callRenderSave, false, solarSystemData.planetData[i], 0.0f, i, X1ViewPort, X2ViewPort, Y1ViewPort, Y2ViewPort, drawHaloView, drawHaloOn);
    }
    if (planetView>=0){
        drawPlanet(callRenderSave, true, solarSystemData.planetData[planetView], 0.0f, -1, 0.031, 0.235, 0.055, 0.32, false, false);
    }
}

static void drawSpaceShipSolar (struct CallRenderSave callRenderSave, struct SolarSystemData solarSystemData, struct SpaceTimePos spaceTimePos, float x1, float x2, float y1, float y2) {
    if (spaceTimePos.currentPlanet == 255) {
        // the math here is all fucked up
        // for simplicity I made it that the ship increases in distance away from the star at constant velocity
        // which is completely not true and makes it look derpy
        float currentProgress = (spaceTimePos.gameDate - spaceTimePos.spaceTransfer.startTime) / spaceTimePos.spaceTransfer.transferTime;

        float thetaShipPos = spaceTimePos.spaceTransfer.startTheta + aPi * currentProgress;

        int xAve = (x1 + x2) * callRenderSave.screenArray.screenXSize / 2;
        int yAve = (y1 + y2) * callRenderSave.screenArray.screenYSize / 2;

        int xDif = (x2 - x1) * callRenderSave.screenArray.screenXSize;
        int yDif = (y2 - y1) * callRenderSave.screenArray.screenYSize;
        int planetDif = ((xDif < yDif) ? xDif : yDif);

        double orbitRadius1 = *(solarSystemData.planetData[spaceTimePos.spaceTransfer.body1ID].orbitRadiusDraw) / 2000* planetDif;
        double orbitRadius2 = *(solarSystemData.planetData[spaceTimePos.spaceTransfer.body2ID].orbitRadiusDraw) / 2000* planetDif;
        double orbitRadius = orbitRadius1*(1-currentProgress) + orbitRadius2*(currentProgress);

        // Draw a ring to show where we are going to
        float goldTravelColor[4] = {0.78f, 0.66f, 0.f, 0.8f};
        drawOrbitRing(xAve, yAve, orbitRadius2, planetDif, 1, true, goldTravelColor, 3);

        int xShip = round(orbitRadius * cos(thetaShipPos)) + xAve;
        int yShip = round(orbitRadius * sin(thetaShipPos)) + yAve;

        int shipSize = round(planetDif / 42);

        // Something may be wrong here
        // This is page 148 of Fundmanetals of Applied Eleectrodynamics Table 3-2. Cylindrical to Cartesian
        #define thetaHatShipPos aPi // change in theta of ShipPos with respect to currrentProgress
        float orbitRadiusHat = orbitRadius2 - orbitRadius1;
        float xHatShip = orbitRadiusHat * cos(thetaShipPos) - thetaHatShipPos * sin(thetaShipPos);
        float yHatShip = orbitRadiusHat * sin(thetaShipPos) + thetaHatShipPos * cos(thetaShipPos);
        float thetaShipDirection = atan2(yHatShip, xHatShip); // the direction the ship is traveling in over time and the direction it is pointing

        glEnable(GL_TEXTURE_2D);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // turn on drawing with alpha
        glColor3f(1.0, 1.0, 1.0); // reset color so nothing is tinted
        // Draw the space ship in transit
        draw2DRotatedTexture(callRenderSave.spaceShip[0], xShip-shipSize, xShip+shipSize, yShip-shipSize, yShip+shipSize, &callRenderSave.screenArray, -1, thetaShipDirection, true); // draw SpaceShip
    }
}

static void drawNumbers (struct CallRenderSave callRenderSave, int number, float x1, float xSize, float y1, float ySize, int leadingZeros) {
    float xMem = x1;
    float y2 = y1 + ySize;
    while (number > 0 || leadingZeros > 0) {
        draw2DTexture(callRenderSave.digits[number%10], xMem - xSize, xMem, y1, y2, &callRenderSave.screenArray, -1, false);
        xMem -= xSize;
        number /= 10;
        leadingZeros -= 1;
    }
}

#define DigitsDateString 19
static void drawDateTime (struct CallRenderSave callRenderSave, char* dateString, float x1, float xSize, float y1, float ySize) {
    float y2 = y1 + ySize;
    float xMem = x1;
    for (uint8_t i=0; i<DigitsDateString; i++) {
        uint8_t digit = *(dateString + i) - '0';
        GLuint digitTexture;
        xMem += xSize;
        switch (digit) {
            case 10 : { digitTexture = callRenderSave.colon; break; }
            case 255 : { digitTexture = callRenderSave.slash; break; }
            case 240 : { continue; } // Space
            default : {
                #ifdef DEBUG
                if (digit > 9) { fprintf(stderr, "draw date is trying to print a digit greater than 9 which doesn't exist. This should not happen."); }
                #endif // DEBUG
                digitTexture = callRenderSave.digits[digit];
            }
        }
        draw2DTexture(digitTexture, xMem - xSize, xMem, y1, y2, &callRenderSave.screenArray, -1, false);
    }
}

void drawSpaceTravelMenu (struct CallRenderSave callRenderSave, struct SolarSystemData solarSystemData, int innerMenuLocation, struct SpaceTimePos spaceTimePos) {
    // 2D drawing settings
    static char dateString[DigitsDateString];
    glEnable(GL_TEXTURE_2D);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // turn on drawing with alpha
    glColor3f(1.0, 1.0, 1.0); // reset color so nothing is tinted

    // Draw main background panel
    draw2DTexture(callRenderSave.panelTexture, 0.0, 1.0, 0.0, 1.0, &callRenderSave.screenArray, -1, false); // draw Menu Background

    // Draw the inner menu of planet select screen
    if (innerMenuLocation == spaceTimePos.currentPlanet) { // curent planet is selected, can't travel there
        draw2DTexture(callRenderSave.landOnPlanetButton, 0.2, 0.4, 0.2, 0.25, &callRenderSave.screenArray, 12, false); // draw Land to Planet Button
    } else {
        draw2DTexture(callRenderSave.travelToPlanetButton, 0.2, 0.4, 0.2, 0.25, &callRenderSave.screenArray, 11, false); // draw Travel to Planet Button
    }

    // Draw current spaceship traveling info
    if (spaceTimePos.currentPlanet == 255) {
        struct TimeDisplay timeToTravel = convertDaysToConvenient(spaceTimePos.spaceTransfer.transferTime);
        switch (timeToTravel.Unit) {
            case 'y': { draw2DTexture (callRenderSave.year, 0.6, 0.8, 0.2, 0.8, &callRenderSave.screenArray, -1, false); break; }
            case 'd': { draw2DTexture (callRenderSave.day, 0.6, 0.8, 0.2, 0.8, &callRenderSave.screenArray, -1, false); break; }
            case 'h': { draw2DTexture (callRenderSave.hour, 0.6, 0.8, 0.2, 0.8, &callRenderSave.screenArray, -1, false); break; }
            #ifdef DEBUG
            default: { fprintf(stderr, "The time travel unit was not matching the existing listed. This should not happen"); }
            #endif // DEBUG
        }
        drawNumbers (callRenderSave, round(timeToTravel.Value), 0.7, 0.015, 0.8, 0.025, 5);
    }

    // Draw the different speed icons to change game speed
    for (int i=0; i<4; i++) {
        draw2DTexture(callRenderSave.speeds[i], 0.7+0.041*i, 0.74+0.042*i, 0.8, 0.842, &callRenderSave.screenArray, i+20, false); // draw Menu Background
    }

    drawNumbers (callRenderSave, round(spaceTimePos.gameSpeed), 0.7, 0.025, 0.85, 0.05, 1);

    // Show current time and time speed menu
    convertDateToString(dateString, spaceTimePos.gameDate, 0);
    drawDateTime (callRenderSave, dateString, 0.4, 0.015, 0.3, 0.025);

    // Test Draw numbers
    drawNumbers (callRenderSave, spaceTimePos.currentPlanet, 0.5, 0.05, 0.5, 0.05, 0);

    drawSpaceShipSolar(callRenderSave, solarSystemData, spaceTimePos, X1ViewPort, X2ViewPort, Y1ViewPort, Y2ViewPort);

    glDisable(GL_TEXTURE_2D);glDisable(GL_BLEND);
    // Draw Planets
    drawSolarSystem (callRenderSave, solarSystemData, innerMenuLocation, spaceTimePos.currentPlanet);

    drawSpaceShipSolar(callRenderSave, solarSystemData, spaceTimePos, X1ViewPort, X2ViewPort, Y1ViewPort, Y2ViewPort);
}

/// will uncomment this and deal with this crap later

/*
void drawEntityPath(struct roverCam *CAM) {
    // this draws the path of the rover over the planet if it goes in a straight line
    glBegin(GL_POINTS);
    glColor3f(0, 0, 1);
    // we then loop through possible times to see what the position is at each of them and plot it
    for (double time=0;time<2*aPi;time+=aPi/100) {
        //here we then calculate the future x,y,and z positions based on the semi-major and semi-minor
        //axis's given in the camera object
        double Theta, Phi;
        getFutPlayerPos(CAM,time,&Theta,&Phi);

        int xPlot=(int)(Phi*200/pi);int yPlot=200-(int)(200*Theta/pi);
        xPlot=100+mod(xPlot,400);yPlot=mod(yPlot,200);
        glVertex2f(xPlot, yPlot);     // Specify each point
    }
    glEnd();
}
*/
