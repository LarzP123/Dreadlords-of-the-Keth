#include "solarSystemData.h"
///test
void loadSolarSystem(struct SolarSystemData *solarSystemData, int solarSystemID) {
    /// FIND THE IDS OF THE PLANETS WHICH PLANETS ARE IN THE GIVEN SOLAR SYSTEM ///
    // First open up the solarSystemData file. This  contains data about each of the center stars and the IDS of the planets that order them

    int planetIds[MaxPlanetCount];//Here Planet ID is the PL number in planetData.txt
    // the Planet ID references the absolute reference of the planet within the file containing all planet info

    #define charsPerLineSolar 114 // two additional then what you think here for the return character '\n' char
    #define DataPointsBeforePlanetIds 6 // This is the amount of leading characters within a line before information is given about planetIDs
    #define CharsInDataBufferSolar 6 // This is the amount of characters within a databuffer
    #define CharsSeperateDataSolar (CharsInDataBufferSolar + 2)

    // Here we define an array of character to store the line entry for the solar system within the SolarSystem Data file
    // We then seek the starting character which that line starts on and read the line
    FILE *file = fopen("SolarSystemList.txt", "r");
    assert("Error Opening Solar System Data File"&&file!=NULL);
    char lineSolar[charsPerLineSolar] = {'\0'};
    int fseekTest = fseek(file, charsPerLineSolar * solarSystemID, SEEK_SET);
    assert("Finding character in file of Solar System Data Failed. Out of Bounds"&&fseekTest== 0);
    int freadTest = fread(lineSolar, 1, charsPerLineSolar-1, file);
    printf("Fread test please %d\n",freadTest);
    assert("Reading line in file of Solar System Data Failed. Number of Chars transfered successful not equal to number requested\n"&&freadTest==charsPerLineSolar-1);
    fclose(file);

    /// Later, this code needs to get bombed and we need to loop through all of the CharsInDataBufferSolar and then go ahead and load all of them in one big thing, but I only need mass now
    char dataBuffer[CharsInDataBufferSolar+1] = {'\0'}; // Buffer to hold extracted double string
    // load solar mass
    for (int charOfDataBufferNum = 0; charOfDataBufferNum < CharsInDataBufferSolar; charOfDataBufferNum++) {
        dataBuffer[charOfDataBufferNum] = lineSolar[charOfDataBufferNum + 5*CharsSeperateDataSolar];
    }
    solarSystemData->starData.mass = atoi(dataBuffer);
    /// End of future code bombing

    // We then define a data buffer to store in characters 1 item of data. This will later be converted to a double and saved to the solarSystem container
    // We then loop through all of the planets in order and save the characters stating the planet ID to the buffer. Then we convert the char buffer to a double and save it
    // to the solar system container
    for (int planetOrder = 0; planetOrder < 8; planetOrder++) {
        // Here planet order references the planet number within the solar system
        // 0 is the nearest planet and 7 is the furthest
        for (int charOfDataBufferNum = 0; charOfDataBufferNum < CharsInDataBufferSolar; charOfDataBufferNum++) {
            dataBuffer[charOfDataBufferNum] = lineSolar[charOfDataBufferNum + (planetOrder+DataPointsBeforePlanetIds)*CharsSeperateDataSolar];
        }
        planetIds[planetOrder] = atoi(dataBuffer);
    }
    fclose(file);

    /// LOAD DATA ABOUT THE PLANETS ///
    // First open up the planetData file. This  contains data about each of the planet's names, colors, sea level, orbit, etc...
    file = fopen("planetList.txt", "r");
    assert("Error Opening Planet Data File"&&file!=NULL);

    // loop through all of the planets in order from closest to the star to furthest from the star and save the data to the solar system container
    for (int planetOrder = 0; planetOrder < 8; planetOrder++) {
        loadPlanetData(&(solarSystemData->planetData[planetOrder]), planetOrder, planetIds, file);
    }

    /// scale planet radius
    double scaleSlope, scaleOffset;

    /// --- This code is filled with too many magic numbers but it works for now at current screen size so magic numbers are nice pixel values to make it look good
    // The reason we can just pass in the first value of planetRadius and give it the size is because we did magic memory allocation in initVars
    scaleMaxMin(solarSystemData->planetData[0].planetRadius, MaxPlanetCount, 20, 100, &scaleSlope, &scaleOffset);

    for (int i = 0; i < MaxPlanetCount; i++) {
        *(solarSystemData->planetData[i].planetRadius) = scaleSlope * *(solarSystemData->planetData[i].planetRadius) + scaleOffset;
    }
    /// end scale planet radius

    /// scale planet orbit
    for (int i = 1; i < MaxPlanetCount; i++) {
        *(solarSystemData->planetData[i].orbitRadiusDraw) = *(solarSystemData->planetData[i].orbitRadius);
    }

    scaleMaxMin(solarSystemData->planetData[0].orbitRadiusDraw, MaxPlanetCount, 100, 900, &scaleSlope, &scaleOffset);
    // scale the first object
    *(solarSystemData->planetData[0].orbitRadiusDraw) = scaleSlope * *(solarSystemData->planetData[0].orbitRadiusDraw) + scaleOffset;
    // start list here at 1 since we are scaling too allow enough distance between planets and need to fix for initial value

    for (int i = 1; i < MaxPlanetCount; i++) {
        *(solarSystemData->planetData[i].orbitRadiusDraw) = scaleSlope * *(solarSystemData->planetData[i].orbitRadiusDraw) + scaleOffset;
        double spaceBetweenOrbits = *(solarSystemData->planetData[i].orbitRadiusDraw) - *(solarSystemData->planetData[i-1].orbitRadiusDraw);
        double sizeOfPlanetsTouching = *(solarSystemData->planetData[i].planetRadius) + *(solarSystemData->planetData[i-1].planetRadius);
        if (spaceBetweenOrbits < sizeOfPlanetsTouching){
            *(solarSystemData->planetData[i].orbitRadiusDraw) = sizeOfPlanetsTouching + *(solarSystemData->planetData[i-1].orbitRadiusDraw);
        }
    }

    /// end scale planet orbit
}

#define DaysInAYear 365.242374

struct SpaceTransfer calcHohmannTransfer(struct SolarSystemData solarSystemData, int planet1, int planet2, double spaceShipMass) {
    struct SpaceTransfer spaceTransfer;
    // https://www.instructables.com/Calculating-a-Hohmann-Transfer/
    // Planet Radius's are by default stored in AU
    // Star Mass is by Default stored in solar mass
    // Space Ship Mass is in 10 million kilograms

    /// Initial setup of struct, don't need major calculations here
    spaceTransfer.body1ID = planet1;
    spaceTransfer.body2ID = planet2;

    spaceTransfer.startTheta = solarSystemData.planetData[planet1].yearAngleStarting;

    /// Calculate Flight Time
    double planet1OrbitalRadius = *(solarSystemData.planetData[planet1].orbitRadius);
    double planet2OrbitalRadius = *(solarSystemData.planetData[planet2].orbitRadius);
    double starMass = solarSystemData.starData.mass;

    double semiMajorAxis = (planet1OrbitalRadius + planet2OrbitalRadius) / 2; // (Au)
    // In order to get out Days as a value here, there had to be some unit conversion done from AU and solar masses to get out days, for some reason the constant
    // that we get out to make unit conversion work is the number of days in a year (DaysInAYear). I don't know why, but that would be fun to look into
    double periodOfTransfer = pow(starMass * pow(semiMajorAxis,3), 0.5); // Kepler's 3rd law (Days)

    spaceTransfer.transferTime = 0.5 * periodOfTransfer;

    /// Calculate Flight Energy
    /*
    double startingTransferVelocity = pow(CONSTANT * starMass / planet1OrbitalRadius, 0.5);
    double endingTransferVelocity = pow(CONSTANT * starMass / planet2OrbitalRadius, 0.5);

    double velocityTransferAtPeriphelion = (2*pi*semiMajorAxis/periodOfTransfer) * pow((2*semiMajorAxis/planet1OrbitalRadius)-1, 0.5);
    *outEnergyInitial = 0.5 * spaceShipMass * (pow(velocityTransferAtPeriphelion,2) - pow(startingTransferVelocity,2));

    double velocityTransferAtAphelion = (2*pi*semiMajorAxis/periodOfTransfer) * pow((2*semiMajorAxis/planet2OrbitalRadius)-1, 0.5);
    *outEnergyFinal = 0.5 * spaceShipMass * (pow(velocityTransferAtAphelion,2) - pow(startingTransferVelocity,2));
    */

    return spaceTransfer;
}
