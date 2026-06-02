#include "3dGraphicFunctions.h"
#include <math.h>

static float eyeX, eyeY, eyeZ;
static float lookX, lookY, lookZ;
static float upX, upY, upZ;

void calcCameraPos (struct PosOrient3D *posOrient3D, uint8_t cameraMode) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, 1.0, 1.0, 100.0);  // Set up a perspective projection

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    switch (cameraMode) {
        // First Person
        case 0: {
            eyeX = posOrient3D->positionX;
            eyeY = posOrient3D->positionY;
            eyeZ = posOrient3D->positionZ;
            lookX = posOrient3D->positionX + posOrient3D->orientationX;
            lookY = posOrient3D->positionY + posOrient3D->orientationY;
            lookZ = posOrient3D->positionZ + posOrient3D->orientationZ;
            upX = posOrient3D->positionX;
            upY = posOrient3D->positionY;
            upZ = posOrient3D->positionZ;
            break;
        }
    }
}
void positionCameraPosition() {

    gluLookAt(eyeX, eyeY, eyeZ,  // Camera position
              lookX, lookY, lookZ,  // Look-at point (center of axes) gluLookAt
              upX, upY, upZ); // Up direction gluLookAt
    /*
    gluLookAt(0, 0, -30.5f,  // Camera position
              lookX, lookY, lookZ,  // Look-at point (center of axes) gluLookAt
              0, 1, 0); // Up direction gluLookAt
    */
}
void positionCameraView() {
    gluLookAt(0, 0, 0,  // Camera position
              lookX - eyeX, lookY - eyeY, lookZ - eyeZ,  // Look-at point (center of axes) gluLookAt
              upX, upY, upZ); // Up direction gluLookAt
}

void displaySkybox(struct ScreenArray *screenArray, GLuint skyBox[6], struct PosOrient3D *posOrient3D) {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glDisable(GL_LIGHTING);
    gluPerspective(40.0, ((double)screenArray->screenXSize) / (screenArray->screenYSize - 200.0), 0.05, 100.0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    positionCameraView();
    glDepthMask(FALSE);

    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    #define skyBoxSize  20
    for (int side=0; side<6; side++) {
        glBindTexture(GL_TEXTURE_2D, skyBox[side]);
        glBegin(GL_QUADS);
        for (int vertex=0; vertex<4; vertex++){
            glTexCoord2f(squareVerts[vertex][0], squareVerts[vertex][1]);
            glVertex3f(skyBoxSize*cubeVerts[side][vertex][0], skyBoxSize*cubeVerts[side][vertex][1], skyBoxSize*cubeVerts[side][vertex][2]);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix(); // Restore modelview matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); // Restore projection matrix
    glMatrixMode(GL_MODELVIEW); // Reset to modelview mode
}

// Not used for final game
#ifdef DEBUG
void drawAxes() {
    glPushMatrix(); // Save current transformation matrix
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING); // Disable lighting for solid colors

    glBegin(GL_LINES);

    // X axis - Red
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);

    // Y axis - Green
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);

    // Z axis - Blue
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);

    glEnd();
    glPopMatrix(); // Restore previous transformation matrix
}

#endif // DEBUG

///------------------loadPlanetDrawcalls-----------------------///

// change this to use function pointers so we can have a generic draw for icosohedron's or for spheres
void loadPlanetDrawcalls(int *planetSurfaceList, struct PlanetData *planetData, int Detail) {
    //Get Information about the Planet's Surface Properties Here
    //We'll use this for determining fluid height and for coloring the planet

    #define ambientLightMult 0.5
    #define diffuseLightMult 1.0

    // Set up material properties for the surface of the planet
    GLfloat SurfaceAmbient [] = {planetData->landColor[0]*ambientLightMult, planetData->landColor[1]*ambientLightMult, planetData->landColor[2]*ambientLightMult, 1.0}; //Color in RGB of Ambient light reflections on ground
    GLfloat SurfaceDiffuse [] = {planetData->landColor[0]*diffuseLightMult, planetData->landColor[1]*diffuseLightMult, planetData->landColor[2]*diffuseLightMult, 1.0}; //Color in RGB of Diffuse light reflections on ground
    GLfloat SurfaceSpecular[] = {planetData->landColor[0], planetData->landColor[1], planetData->landColor[2], 1.0}; //Color in RGB of Specular light reflections on ground

    // Set up material properties for the surface of the planet
    GLfloat LiquidAmbient [] = {planetData->seaColor[0]*ambientLightMult, planetData->seaColor[1]*ambientLightMult, planetData->seaColor[2]*ambientLightMult, 0.0}; //Color in RGB of Ambient light reflections on liquid
    GLfloat LiquidDiffuse [] = {planetData->seaColor[0]*diffuseLightMult, planetData->seaColor[1]*diffuseLightMult, planetData->seaColor[2]*diffuseLightMult, 0.0}; //Color in RGB of Diffuse light reflections on liquid
    GLfloat LiquidSpecular[] = {1.0, 1.0, 1.0, 0.0}; //Color in RGB of Specular light reflections on liquid

    *planetSurfaceList = glGenLists(1);
    glNewList(*planetSurfaceList, GL_COMPILE);

    glBegin(GL_TRIANGLES);

    //Go through each face of the Icosahedron since an Icosahedron is approximately  a sphere
    for (int face = 0; face < 20; face++) {
        //Here majorVertA,majorVertB,and majorVertC are the vertices of a given face of the icosohedron
        double majorVertA[3] = {icosahedronVerts[icosahedronFaces[face][0]][0], icosahedronVerts[icosahedronFaces[face][0]][1], icosahedronVerts[icosahedronFaces[face][0]][2]};
        double majorVertB[3] = {icosahedronVerts[icosahedronFaces[face][1]][0], icosahedronVerts[icosahedronFaces[face][1]][1], icosahedronVerts[icosahedronFaces[face][1]][2]};
        double majorVertC[3] = {icosahedronVerts[icosahedronFaces[face][2]][0], icosahedronVerts[icosahedronFaces[face][2]][1], icosahedronVerts[icosahedronFaces[face][2]][2]};
        //a,b,and c are the vertices of a sub-face of the Icosahedron
        //sub-faces are triangles made within the triangles of the Icosahedron that allows
        //us to better approximate a sphere
        double minorVertA[3], minorVertB[3], minorVertC[3];

        //Here there is a detail argument in our original function that says how many sides we should break up a traingle of the icosohedron into
        //Each of these minisides will then be patterned to form their own triangle
        //This means that a detail of 2 will have each side length of the icosohedron broken up into 2 side lengths and there are therefore actually 4 triangles within each 1 triangle

        //since we are in 2D space, we need one loop that goes down one length of the triangle adding a smaller traingle there
        //and 1 loop inside of it that goes in the direction of the other length adding more triangles to make it fill up 2d space
        for (int i=0; i<Detail; i++) {
            for (int j=0; j<i+1; j++) {
                //There is a flip term here because otherwise our triangle of subtriangles will look just like a tiled triforce with holes in it.
                //There are upsidedown triangles that we need to fill the holes. The amount of flipped triangles is less than the amount of normal
                //triangles because the last row to be filled in does not need to be flipped since the flipped holes are entirely contained by the outside
                //normal triangles of the shape. Therefore we will have this flip variable iterate from 0 to 1 everytime there is a flip (all before the last row).
                //and just go through 0 when it is the last row and the triangle is completely filled in already
                for (int flip=0;flip < 1+(i<Detail-1);flip++) {
                    //Here coord loops through X,Y,and then Z for each vertex of the sub-triangle that we are defining.
                    for (int coord=0;coord<3;coord++) {
                        if (flip==1) {
                            //This is the case when the triangle is flipped over to an upside down triangle. Here we need to just move 1 vertex since it is flipped
                            //around 1 side of the triangle. Therefore the vertices along the side that it is flipping don't change.
                            minorVertA[coord] = majorVertA[coord] * (Detail-i-2)/Detail + majorVertB[coord]*(i+2)/Detail*(i-j+1)/(i+2)     + majorVertC[coord]*(i+2)/Detail*(j+1)/(i+2);
                        } else if (i==0) {
                            //This is a special case because our default formula for the not flipped triangle would have a divide by zero error
                            minorVertA[coord] = majorVertA[coord];
                        } else {
                            //This is the case for the not flipped triangle
                            minorVertA[coord] = majorVertA[coord] * (Detail-i)/Detail   + majorVertB[coord]*i/Detail*(i-j)/i       + majorVertC[coord]*i/Detail*j/i;
                        }
                        minorVertB[coord] = majorVertA[coord] * (Detail-i-1)/Detail + majorVertB[coord]*(i+1)/Detail*(i-j+1)/(i+1)   + majorVertC[coord]*(i+1)/Detail*j/(i+1);
                        minorVertC[coord] = majorVertA[coord] * (Detail-i-1)/Detail + majorVertB[coord]*(i+1)/Detail*(i-j)/(i+1)     + majorVertC[coord]*(i+1)/Detail*(j+1)/(i+1);
                    }
                    //Now we store an array of pointers to each of the 3 variables to more easily loop through them.
                    //Here we are going to find what surface height they should be and set their distance from the center of the planet
                    //to be based on that surface height
                    double* minorVerts[] = {minorVertA, minorVertB, minorVertC};
                    bool aboveWater=false;//This is used to keep track of what color the surface should be.
                    for (int i = 0; i < 3; i++) {
                        double* vert = minorVerts[i];
                        double phi = atan2(vert[1],vert[0]);
                        double theta = atan2(sqrt(vert[0]*vert[0]+vert[1]*vert[1]),vert[2]);
                        double surfaceHeight = getPlanetSurfaceHeight(planetData->planetHeightData, theta, phi) / 255.0;

                        if (surfaceHeight < planetData->seaLevel) {
                            surfaceHeight = planetData->seaLevel;
                        } else {
                            aboveWater=true;
                        }

                        norm3DVec(vert,8.0+2.0*surfaceHeight);
                    }

                    //if our triangle has any vertex above the liquid then what we have it a surface color triangle
                    //if not it is a liquid color triangle
                    if (aboveWater) {
                        glMaterialfv(GL_FRONT, GL_AMBIENT, SurfaceAmbient);
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, SurfaceDiffuse);
                        glMaterialfv(GL_FRONT, GL_SPECULAR, SurfaceSpecular);
                    } else {
                        glMaterialfv(GL_FRONT, GL_AMBIENT, LiquidAmbient);
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, LiquidDiffuse);
                        glMaterialfv(GL_FRONT, GL_SPECULAR, LiquidSpecular);
                    }
                    //the shininess is based on the refractive index of the surface (just using 0.3 because why not)
                    glMaterialf(GL_FRONT, GL_SHININESS, 0.5);

                    //this defines edge vectors that describe the direction of each edge of the triangle
                    //this is used for calculating the normal vectors
                    double edge1[3] = {minorVertB[0]-minorVertA[0], minorVertB[1]-minorVertA[1], minorVertB[2]-minorVertA[2]};
                    double edge2[3] = {minorVertC[0]-minorVertA[0], minorVertC[1]-minorVertA[1], minorVertC[2]-minorVertA[2]};

                    //here we actually calculate the normal vector
                    double normal[3];

#ifdef DEBUG
                    calculateNormal(edge1, edge2, normal, minorVertA);
#else // DEBUG
                    calculateNormal(edge1, edge2, normal);
#endif // DEBUG

                    //here we add the newly found positions and normal vector to the glList of vertices and move on
                    glNormal3dv(normal);
                    glVertex3dv(minorVertA);glVertex3dv(minorVertB);glVertex3dv(minorVertC);
                }
            }
        }
    }
    glEnd();
    glEndList();
}

void drawPlanetSurface (int *planetSurfaceList, struct ScreenArray *screenArray) {
    glPushMatrix();
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glEnable(GL_DEPTH_TEST);glEnable(GL_LIGHTING);glEnable(GL_LIGHT0);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    glDisable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glDepthMask(TRUE);

    gluPerspective(40.0, ((double)screenArray->screenXSize) / (screenArray->screenYSize - 200.0), 0.05, 100.0);

    positionCameraPosition();

    GLfloat lightPosition[] = {0.0f, -3.0f, -10.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    GLfloat ambientLight[] = {0.7f, 0.7f, 0.7f, 1.0f};  // Set the ambient light color (e.g., a soft white light)
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    glColor3f(1.0, 1.0, 1.0);
    glCallList(*planetSurfaceList);

    glPopMatrix();
}

void loadLandOnPlanetGraphics(struct CallRenderSave *callRenderSave, struct PosOrient3D *posOrient3D, struct PlanetData planetData) {
    loadPlanetDrawcalls(&(callRenderSave->planetSurfaceDrawCalls), &planetData, 12);
}

void drawPlanetMap(struct PosOrient3D *posOrient3D, struct PlanetData *planetData, struct CallRenderSave *callRenderSave) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 400, 0, 200, -1, 1); // Match coordinate system to viewport
    glMatrixMode(GL_MODELVIEW);glEnable(GL_COLOR_MATERIAL);glDisable(GL_LIGHTING);
    glLoadIdentity();
    glViewport(0, 0, 400, 200); // Ensure viewport matches intended area

    glBegin(GL_POINTS);
    for (int y = 0; y < 200; y++) {
        for (int x = 0; x < 400; x++) {
            double Phi = x / 200.0 * aPi;
            double Theta = y / 200.0 * aPi + aPi/2;

            double surfaceHeight = getPlanetSurfaceHeight(planetData->planetHeightData, Theta, Phi) / 200.0f;
            glColor3f(surfaceHeight, surfaceHeight, surfaceHeight);
            glVertex2f(x, y);
        }
    }
    glEnd();
}

void drawPlanetTravelGameplay (struct CallRenderSave *callRenderSave, struct PosOrient3D *posOrient3D, struct PlanetData planetData) {
    glViewport(0,200,callRenderSave->screenArray.screenXSize,callRenderSave->screenArray.screenYSize-200);

    calcCameraPos(posOrient3D, 0);

    displaySkybox (&(callRenderSave->screenArray), callRenderSave->skybox, posOrient3D);
    drawPlanetSurface(&(callRenderSave->planetSurfaceDrawCalls), &(callRenderSave->screenArray));
    //drawAxes();

    GLfloat lightPosition[] = {0.0f, 20.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    drawPlanetMap(posOrient3D, &planetData, callRenderSave);

    /*
    //The screensize is currently set to just be 200 pixels tall but span the whole width of the screen to display the map
    gluOrtho2D(0, callRenderSave->screenArray.screenXSize, 0, 200);

    //I don't know why this exists. Maybe delete later
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0,0,callRenderSave->screenArray.screenXSize,200);


    // drawPlanetMap(CAM, MapOffset,currentSolarSystem,planetNum);

    //I don't know why this exists, but it needs to or this breaks
    glMatrixMode(GL_PROJECTION);glPopMatrix();
    glMatrixMode(GL_MODELVIEW);glPopMatrix();

    //This forces the window to stay at a pre-sized resolution so someone can't drag it and arbitrarily change it.
    glutReshapeWindow(callRenderSave->screenArray.screenXSize, callRenderSave->screenArray.screenYSize);
    */
}
