#include "initVars.h"
#include <string.h>

void getLanguages (char ***languageNames,int* languageCount) {
    struct dirent *entry;
    DIR *dir = opendir("./images/languages"); // Replace with your folder path

    assert("unable to open language directory './images/languages'"&&dir);

    *languageCount = 0;
    // each file pointer has a subobject 'dir' for directory even though 'dir' includes directories and files and its only files in our case
    while ((entry = readdir(dir)) != NULL) {
        //each file name is 3 characters long like english is 'eng' so the file is actually 7 characters because 'eng.png'
        //what
        if (strlen(entry->d_name) == 7) {
            *languageNames = realloc(*languageNames, (*languageCount + 1) * sizeof(char *));
            (*languageNames)[*languageCount] = malloc(4 * sizeof(char));
            strncpy((*languageNames)[*languageCount], entry->d_name, 3); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling). This is a pain to work around, so I don't care
            (*languageNames)[*languageCount][3]='\0';
            (*languageCount)++;
        }
    }

    closedir(dir);
}

void initVars (struct GameState *gameState, void *keyState, int sizeOfKeyState) {
    // Set the Key Values to 0
    memset(keyState, 0, sizeOfKeyState); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling). This is a pain to work around, so I don't care
    // Set starting menu to main menu
    gameState->gameMenu = 1;
    gameState->mouseFeedback = 255;
    gameState->innerMenuLocation = 0;
    strcpy(gameState->language, "eng");
    getLanguages(&(gameState->languageNames),&(gameState->languageCount));

    // Okay so there is some witchcraft here. orbitRadius and planetRadius are stored as pointer in each planet, this is so that all of the orbitRadius/planetRadius values can
    // be right next to each other in memory for easy sorting/adjusting later on since you only need to reference the first orbit radius pointer to have something know the whole list
    double* orbitRadiusPointerList = (double *)malloc(sizeof(double) * MaxPlanetCount);
    double* orbitRadiusDrawPointerList = (double *)malloc(sizeof(double) * MaxPlanetCount);
    double* planetRadiusPointerList = (double *)malloc(sizeof(double) * MaxPlanetCount);
    for (int i=0; i<MaxPlanetCount; i++) {
        gameState->solarSystemData.planetData[i].orbitRadius = orbitRadiusPointerList + i;
        gameState->solarSystemData.planetData[i].orbitRadiusDraw = orbitRadiusDrawPointerList + i;
        gameState->solarSystemData.planetData[i].planetRadius = planetRadiusPointerList + i;
        // We need to malloc the height data because if we just leave it in the struct of planet data everything crashes since there is too much data
        gameState->solarSystemData.planetData[i].planetHeightData = (uint8_t *)malloc(sizeof(uint8_t) * PlanetHeightSize);
    }

    gameState->spaceTimePos.gameSpeed = 1;
    gameState->spaceTimePos.gameDate = 0405.232131;
}
