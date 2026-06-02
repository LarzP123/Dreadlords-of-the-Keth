#include "calcGameLogic.h"
#include <stdint.h>
#include <GL/glut.h>

double calcElpasedTime(struct GameState *gameState) {
	clock_t currentTime = clock();
    double elapsedTime = (double)(currentTime - gameState->previousTime) / CLOCKS_PER_SEC;
	gameState->previousTime = currentTime;
	gameState->spaceTimePos.gameDate += elapsedTime/31536000 * gameState->spaceTimePos.gameSpeed;
	return elapsedTime;
}

void startNewGameSetVars (struct GameState* gameState) {
    gameState->spaceTimePos.currentPlanet = 2; // the Earth
    gameState->spaceTimePos.currentStar = 0; // the Sun
    loadSolarSystem(&(gameState->solarSystemData), 1);
}

void mainMenuLogic(struct GameState* gameState) {
    switch (gameState->mouseFeedback) {
        case 0: {// hit some go back to main menu main screen button
            gameState->innerMenuLocation = 0;
            break;
        }
        case 1: {// hit singleplayer button
            gameState->innerMenuLocation = 1;
            break;
        }
        case 2: {// hit multiplayer button
            gameState->innerMenuLocation = 2;
            break;
        }
        case 3: {// hit options button
            gameState->innerMenuLocation = 3;
            break;
        }
        case 4: {// hit language button
            gameState->innerMenuLocation = 4;
            break;
        }
        // Temporary, replace when saving works
        case 11: {// start game
            gameState->innerMenuLocation = 0;
            startNewGameSetVars(gameState);
            //Go to planet fly around screen
            gameState->gameMenu = 2;
            break;
        }
    }
}

void calcCartesianOrientationFromSpherical (struct PosOrient3D *posOrient3D, float orientationThetaSpherical, float orientationPhiSpherical, float orientationRadpherical) {
    posOrient3D->orientationX = orientationRadpherical     * sin(posOrient3D->positionTheta) * cos(posOrient3D->positionPhi)
                                +orientationThetaSpherical * cos(posOrient3D->positionTheta) * cos(posOrient3D->positionPhi)
                                -orientationPhiSpherical   * sin(posOrient3D->positionPhi);
    posOrient3D->orientationY = orientationRadpherical     * sin(posOrient3D->positionTheta) * sin(posOrient3D->positionPhi)
                                +orientationThetaSpherical * cos(posOrient3D->positionTheta) * sin(posOrient3D->positionPhi)
                                +orientationPhiSpherical   * cos(posOrient3D->positionPhi);
    posOrient3D->orientationZ = orientationRadpherical     * cos(posOrient3D->positionTheta)
                                -orientationThetaSpherical * sin(posOrient3D->positionTheta);
}

void calc3DEntityMovement (struct PosOrient3D *posOrient3D) {
    // This was not taken from the E&M Fields textbook but from my brain
    // This uses the orientationTheta and orientationPhi coordinates which describe the way that you are looking on the planet relative to earth being flat logic
    // (Theta is looking left to right and Phi is up and down)
    // and transforms it into a vector describing the way you are facing in terms of spherical global coordinates
    // These coordinates are defined the same way thetaHat, phiHat, and RHat are defined in Figure 3-18 of the E&M Fields textbook on page 149
    if (!(posOrient3D->recalculateMovement || posOrient3D->recalculateLookDirection || posOrient3D->isMoving)) {
        return;
    }

    float orientationThetaSpherical = cos(posOrient3D->orientationTheta)*cos(posOrient3D->orientationPhi);
    float orientationPhiSpherical   = sin(posOrient3D->orientationTheta)*cos(posOrient3D->orientationPhi);
    float orientationRadpherical    = sin(posOrient3D->orientationPhi);

    if (posOrient3D->recalculateLookDirection){
        calcCartesianOrientationFromSpherical (posOrient3D, orientationThetaSpherical, orientationPhiSpherical, orientationRadpherical);
        posOrient3D->recalculateLookDirection = false;
    }
    //recalculate movement is pulled true when the camera is turning or has started or stopped moving.
    //It sets the v1 and v2 semi-major and semi-minor axis which say the path around the globe you would
    //take if you moved either forwards or backwards.
    if (posOrient3D->recalculateMovement){
        //initially v1 the first point along the path is set to your current position.
        //your current position must be a possible point along the path you will take along the sphere.
        posOrient3D->semiMajor[0] = posOrient3D->positionX;
        posOrient3D->semiMajor[1] = posOrient3D->positionY;
        posOrient3D->semiMajor[2] = posOrient3D->positionZ;
        //v2 is set to a place that you are looking at. Since xHat,yHat,and zHat describe the direction you are looking.
        //the position at which you are looking at is your current position plus the direction you are looking at
        posOrient3D->semiMinor[0] = posOrient3D->positionX + posOrient3D->orientationX;
        posOrient3D->semiMinor[1] = posOrient3D->positionY + posOrient3D->orientationY;
        posOrient3D->semiMinor[2] = posOrient3D->positionZ + posOrient3D->orientationY;

        //now we need v2 to be 90 degrees of from v1 to give us a semi-major and semi-minor axis so that we can move at a constant speed
        //around the circle. We will therefore define v2 as the position that is 90 degrees in forward movement along the path
        //To do this we find the normal vector to the circular path by taking the cross product of v1 and v2. This vector is perependicular to v1 and v2.
        //Then we find find the new v2 by taking the cross product of v1 and the normal vector.
        //This ensures that the new v2 is in the plane of motion (because it is perpendicular to the normal vector from properties of cross product).
        //This also ensures that it is 90 degrees of from v1 (because it is perpendicular to v1 from properties of cross product).
        double normal[3];
        crossProd(posOrient3D->semiMajor, posOrient3D->semiMinor, normal);
        crossProd(normal, posOrient3D->semiMajor, posOrient3D->semiMinor);

        //The v1 and v2 vectors are then normalized to have a radius of the camera's radius on the sphere.
        norm3DVec(posOrient3D->semiMajor, posOrient3D->positionRad); // wtf, do I even need this line
        norm3DVec(posOrient3D->semiMinor, posOrient3D->positionRad);

        //movement has been recalculated so we should reset recalculate movement and we should also set MoveT to a small number so if we start moving forward
        //we can use the change in theta and change in phi for more math without have a divide by 0 error
        posOrient3D->recalculateMovement = false;
        posOrient3D->moveT = 0.0001; // I shouldn't need this to be here
    }
    if (posOrient3D->isMoving){
        //This is going to track the change in Theta and change in Phi as you travel across the surface of the planet
        //This is going to be done by first tracking the previous camTheta and previous camPhi positions on the globe
        //then it will update it with the parametric equations
        double positionThetaPrev = posOrient3D->positionTheta;
        double positionPhiPrev   = posOrient3D->positionPhi;

        //link bellow shows formula for parametric equation of circle in 3d space
        //https://math.stackexchange.com/questions/73237/parametric-equation-of-a-circle-in-3d-space
        //here the angle of the specific point on the circle changes with time.
        //This allows us to pinpoint where we are by using the parametric equation
        double *newCartesianPosition[3] = {&posOrient3D->positionX, &posOrient3D->positionY, &posOrient3D->positionZ};
        getPosFromPerpAxes(posOrient3D->semiMajor, posOrient3D->semiMinor, newCartesianPosition, posOrient3D->moveT);

        //Theta and Phi are then found based on the Cartesian coordiantes just previously defined
        //This is on page 148 of the E&M Fields book on the "Coordinate Variables" column of the "Cartesian to Spherical" row
        posOrient3D->positionTheta = atan2(sqrt(posOrient3D->positionX*posOrient3D->positionX + posOrient3D->positionY*posOrient3D->positionY), posOrient3D->positionZ);
        posOrient3D->positionPhi   = atan2(posOrient3D->positionY, posOrient3D->positionX);

        //PhiDif tracks the change in phi since last time and ThetaDif change in theta for position on planet.
        //This will be used to know which way you are going to figure out which direction to have the camera face
        //in order to keep it facing forward.
        double positionThetaDif = correctAngleDiff(posOrient3D->positionTheta-positionThetaPrev);
        double positionPhiDif   = correctAngleDiff(posOrient3D->positionPhi-positionPhiPrev);

        //This adjusts the new way the camera faces
        //I do not know why this works
        //this took way too much trial and error
        posOrient3D->orientationTheta = atan2(sin(posOrient3D->positionTheta) * positionPhiDif, positionThetaDif);

        //The direction that the camera is facing in Cartesian coordinates is updated
        //This is found on page 148 on the "Unit Vectors" column of the "Spherical to Cartesian" row of table 3-2.
        calcCartesianOrientationFromSpherical (posOrient3D, orientationThetaSpherical, orientationPhiSpherical, orientationRadpherical);
    }
}

void calcCartesianPositionFromSpherical (struct PosOrient3D *posOrient3D) {
    posOrient3D->positionX = posOrient3D->positionRad * sin(posOrient3D->orientationPhi) * cos(posOrient3D->orientationTheta);
    posOrient3D->positionY = posOrient3D->positionRad * sin(posOrient3D->orientationPhi) * sin(posOrient3D->orientationTheta);
    posOrient3D->positionZ = posOrient3D->positionRad * cos(posOrient3D->orientationPhi);
}

// (Land on Planet)
void initiate3DPosData (struct PosOrient3D *posOrient3D) {
    posOrient3D->recalculateMovement = true;
    posOrient3D->recalculateLookDirection = true;
    posOrient3D->isMoving = false;
    // Looking straight on
    posOrient3D->orientationTheta = aPi;
    posOrient3D->orientationPhi = 12;
    // random position
    posOrient3D->positionRad = 12.0;
    posOrient3D->positionPhi = 0.231231;
    posOrient3D->positionTheta = 0.232;

    calcCartesianPositionFromSpherical (posOrient3D);
    // calculate Cartesian coordiantes and stuff
    calc3DEntityMovement(posOrient3D);
}

void spaceTravelMenuLogic (struct GameState* gameState) {
    // check if traveling between planets
    if (gameState->spaceTimePos.currentPlanet == 255) {
        // check if the transfer should be done
        if (gameState->spaceTimePos.spaceTransfer.transferTime + gameState->spaceTimePos.spaceTransfer.startTime <= gameState->spaceTimePos.gameDate) {
            gameState->spaceTimePos.currentPlanet = gameState->spaceTimePos.spaceTransfer.body2ID;
        }
    }

    // You clicked on a planet
    if (gameState->mouseFeedback == 255) {
        return;
    } else if (gameState->mouseFeedback < MaxPlanetCount) {
        gameState->innerMenuLocation = gameState->mouseFeedback;
    } else if (gameState->mouseFeedback == 11) { // travel to planet
        // Check here that we are traveling to a planet besides the one we are currently on. That should never happen though
        #ifdef DEBUG
        if (gameState->spaceTimePos.currentPlanet == gameState->mouseFeedback) {
            fprintf(stderr, "Traveling to planet that you are already on. This should not happen");
            exit(EXIT_FAILURE);
        }
        if (gameState->innerMenuLocation >= MaxPlanetCount) {
            fprintf(stderr, "Traveling to planet id that is larger than max amount of planets. This should not happen");
            exit(EXIT_FAILURE);
        }
        if (gameState->spaceTimePos.currentPlanet >= MaxPlanetCount) {
            fprintf(stderr, "Current planet is larger than max amount of planets. This should not happen");
            exit(EXIT_FAILURE);
        }
        #endif // DEBUG
        gameState->spaceTimePos.spaceTransfer = calcHohmannTransfer(gameState->solarSystemData, gameState->spaceTimePos.currentPlanet, gameState->innerMenuLocation, 2);
        gameState->spaceTimePos.spaceTransfer.startTime = gameState->spaceTimePos.gameDate;
        gameState->spaceTimePos.currentPlanet = 255;
    } else if (gameState->mouseFeedback == 12) {
        // change the game mode to being on the planet
        initiate3DPosData(&(gameState->posOrient3D));
        gameState->gameMenu = 3;
    } else if (gameState->mouseFeedback == 20) {
        gameState->spaceTimePos.gameSpeed = 1;
    } else if (gameState->mouseFeedback == 21) {
        gameState->spaceTimePos.gameSpeed = 1000;
    } else if (gameState->mouseFeedback == 22) {
        gameState->spaceTimePos.gameSpeed = 10000000;
    } else if (gameState->mouseFeedback == 23) {
        gameState->spaceTimePos.gameSpeed = 150000000;
    }
}

void onplanetGameplayLogic (struct GameState* gameState, struct KeyState* keyState, double deltaTime) {
    // reposition the camera if keys are pressed to do so
    if (keyState->keyUp == 1) {
        gameState->posOrient3D.orientationPhi += deltaTime;
        gameState->posOrient3D.recalculateLookDirection = true;
    }
    if (keyState->keyDown == 1) {
        gameState->posOrient3D.orientationPhi -= deltaTime;
        gameState->posOrient3D.recalculateLookDirection = true;
    }
    if (keyState->keyA == 1) {
        gameState->posOrient3D.orientationTheta -= deltaTime;
        gameState->posOrient3D.recalculateLookDirection = true;
        gameState->posOrient3D.recalculateMovement = true;
    }
    if (keyState->keyD == 1) {
        gameState->posOrient3D.orientationTheta += deltaTime;
        gameState->posOrient3D.recalculateLookDirection = true;
        gameState->posOrient3D.recalculateMovement = true;
    }

    if (keyState->keyW == 1) {
        gameState->posOrient3D.moveT += deltaTime / 2;
        gameState->posOrient3D.isMoving = true;
    } else if (keyState->keyS == 1) {
        gameState->posOrient3D.moveT -= deltaTime / 2;
        gameState->posOrient3D.isMoving = true;
    } else {
        gameState->posOrient3D.isMoving = false;
    }

    // Recalculate where your player should be
    calc3DEntityMovement(&(gameState->posOrient3D));
}

void calcGameLogic (struct GameState* gameState, struct KeyState* keyState) {
    double deltaTime = calcElpasedTime (gameState);
    switch (gameState->gameMenu) {
        case 1: {
            mainMenuLogic(gameState);
            break;
        }
        case 2: {
            // time menu and planets moving have to be updated
            glutPostRedisplay();
            spaceTravelMenuLogic(gameState);
            break;
        }
        case 3: {
            glutPostRedisplay();
            onplanetGameplayLogic(gameState, keyState, deltaTime);
            break;
        }
    }

    // reset mouse having clicked something
    gameState->mouseFeedback = 255;
}
