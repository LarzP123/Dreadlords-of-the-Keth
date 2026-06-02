#ifndef THREEDGRAPHICFUNCTIONS
#define THREEDGRAPHICFUNCTIONS

#include "graphicOverhead.h"
#include <GL/glut.h>
#include "larzMath.h"
#include <math.h>
#include <stdbool.h>

/// Look at the markdown file 3dBallGraphicsExplained.md to see what any of this stuff actual represents

#define P 1.618033988749895 // Golden Ratio (Phi)

// Quaternary numeral system
//      b4   b8  b10
#define Q000 000 //0
#define Q001 001 //1
#define Q002 002 //2
#define Q003 003 //3
#define Q010 004 //4
#define Q011 005 //5
#define Q012 006 //6
#define Q013 007 //7
#define Q020 010 //8
#define Q021 011 //9
#define Q022 012 //10
#define Q023 013 //11
#define Q030 014 //12
#define Q031 015 //13
#define Q032 016 //14
#define Q033 017 //15

static const double icosahedronVerts[Q030][3] = {
    { +P,  1,  0 }, //Q000
    { +P, -1,  0 }, //Q001
    { -P, -1,  0 }, //Q002
    { -P,  1,  0 }, //Q003
    { +1,  0, +P }, //Q010
    { -1,  0, +P }, //Q011
    { -1,  0, -P }, //Q012
    { +1,  0, -P }, //Q013
    { +0, +P, +1 }, //Q020
    { +0, +P, -1 }, //Q021
    { +0, -P, -1 }, //Q022
    { +0, -P, +1 }, //Q023
};

// This defines the faces of an Icosahedron. Here each set of 3 numbers references 3 vertices that make up 1 face of the icosohedron
// notice the patterns going down the columns (e.g. first column alternates between 0 and 2 and incrementing the second digit every 4)
static const int icosahedronFaces[20][3] = {
    //make the 2 +X facing ones
    { Q000, Q001, Q010 },
    { Q000, Q013, Q001 },
    //make the 2 -X facing ones
    { Q002, Q003, Q011 },
    { Q002, Q012, Q003 },
    //make the 2 +Z facing ones
    { Q010, Q011, Q020 },
    { Q010, Q023, Q011 },
    //make the 2 -Z facing ones
    { Q012, Q013, Q021 },
    { Q012, Q022, Q013 },
    //make the 2 +Y facing ones
    { Q020, Q021, Q000 },
    { Q020, Q003, Q021 },
    //make the 2 -Y facing ones
    { Q022, Q023, Q001 },
    { Q022, Q002, Q023 },
    //Corner Fill in +Z
    { Q000, Q010, Q020 }, // +phi fill in triangle
    { Q001, Q010, Q023 }, // +phi fill in triangle Y flipped
    { Q002, Q011, Q023 }, // +phi fill in triangle X,Y flipped
    { Q003, Q011, Q020 }, // +phi fill in triangle X flipped
    //Corner Fill in -Z
    { Q002, Q012, Q022 }, // -phi fill in triangle
    { Q003, Q012, Q021 }, // -phi fill in triangle Y Flipped
    { Q000, Q013, Q021 }, // -phi fill in triangle X,Y Flipped
    { Q001, Q013, Q022 }  // -phi fill in triangle X Flipped
};

static const int squareVerts[4][2] = {
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },
    { 0, 1 }
};

static const int cubeVerts[6][4][3] = {
    // +X face
    {
        { +1, +1, +1 },
        { +1, -1, +1 },
        { +1, -1, -1 },
        { +1, +1, -1 }
    },
    // -X face
    {
        { -1, -1, +1 },
        { -1, +1, +1 },
        { -1, +1, -1 },
        { -1, -1, -1 }

    },
    // +Z face
    {   { -1, +1, +1 },
        { -1, -1, +1 },
        { +1, -1, +1 },
        { +1, +1, +1 }
    },
    // -Z face
    {
        { +1, +1, -1 },
        { +1, -1, -1 },
        { -1, -1, -1 },
        { -1, +1, -1 }
    },
    // +Y face
    {
        { -1, +1, +1 },
        { +1, +1, +1 },
        { +1, +1, -1 },
        { -1, +1, -1 }

    },
    // -Y face
    {
        { +1, -1, +1 },
        { -1, -1, +1 },
        { -1, -1, -1 },
        { +1, -1, -1 }
    }
};

void loadLandOnPlanetGraphics (struct CallRenderSave *callRenderSave, struct PosOrient3D *posOrient3D, struct PlanetData planetData);
void drawPlanetTravelGameplay (struct CallRenderSave *callRenderSave, struct PosOrient3D *posOrient3D, struct PlanetData planetData);
void calculateCameraPosition (struct PosOrient3D *posOrient3D);

#endif // SOLARSYSTEMDATA_H_INCLUDED
