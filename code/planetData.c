#include "planetData.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool loadPlanetHeightMap(uint8_t *PlanetHeightMap, char planetName[10]) {
    int imageWidth, imageHeight, imageChannels;
    char fileName[37];
    snprintf(fileName, 37, "images/planetMaps/%s.png", planetName);
    unsigned char *image = stbi_load(fileName, &imageWidth, &imageHeight, &imageChannels, 0); //NOLINT(clang-analyzer-core.uninitialized.Assign) I expect the file to exist

    assert("planet image returned null on loading"&&image);
    assert("planet image width and height do not match standard of 2000 pixels"&&(imageWidth == 2000 && imageHeight == 2000));

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            int index = y * imageWidth + x;
            // Assuming we use the red channel for height map so we don't need to get more info per pixel
            PlanetHeightMap[index] = image[index * imageChannels]; //NOLINT(clang-analyzer-core.uninitialized.Assign). If the asserts fail image is null
        }
    }

    stbi_image_free(image);
    return true;
}

/// MAKE PLANET HEIGHT DATA A POINTER FOR PERFORMANCE PLEASEEEEEEEEEEEEEEEEEEEEEEEEEEE
double getPlanetSurfaceHeight(uint8_t planetHeightData[PlanetHeightSize], double theta, double phi) {
    theta = aPi/2 - theta;
    //Diagram showing what theta and phi is on page 149 of the E&M Fields Book
    int yRef = mod(theta*PlanetHeightSizeHeight/aPi, PlanetHeightSizeHeight);
    int xRef = mod(phi*PlanetHeightSizeWidth/2/aPi, PlanetHeightSizeWidth);
    return planetHeightData[yRef*PlanetHeightSizeWidth+xRef];
}

void loadPlanetData(struct PlanetData *planetData, int planetOrder, int planetIds[8], FILE *file) {
    #define doubleDataPointsPlanet 11

	//here we will make an array of pointers of the data stored in doubles that we want to store.
    //There are a lot of data-points at the end of the file that store information about the planet in some decimal form
	//and we'll just handle that here by getting the locations of where all of the data should actually be stored in the solar system container
	//and then just looping through each data-point given and assigning it to the pointer within the array
	double *dataPointers[doubleDataPointsPlanet] = {
		&(planetData->planetNSurfaces),
		&(planetData->yearAngleStarting),
		(planetData->orbitRadius),
		(planetData->planetRadius),
		&(planetData->landColor[0]),
		&(planetData->landColor[1]),
		&(planetData->landColor[2]),
		&(planetData->seaColor[0]),
		&(planetData->seaColor[1]),
		&(planetData->seaColor[2]),
		&(planetData->seaLevel)
	};

	//we should get the universal planet id from the given line in the file.
	//the universal planet ids in order of planet number from the center star was already found in the SolarSystemData.txt file
	int planetID = planetIds[planetOrder];

	//Here we define an array of character to store the line entry for the solar system within the SolarSystem Data file
	//We then seek the starting character which that line starts on and read the line

	#define charsPerLinePlanet 114 //two additional then what you think here for the return character '\n' char
    #define doubleDataPointsPlanet 11
    #define dataPointsBeforePlanetProperties 3
    #define CharsInDataBufferPlanet 6 //This is the amount of characters within a databuffer
    #define CharsSeperateDataPlanet (CharsInDataBufferPlanet + 2)

	char linePlanet[charsPerLinePlanet] = {'\0'};
	int fseekTest = fseek(file, charsPerLinePlanet * planetID, SEEK_SET);
	assert("Finding character in file of Planet Data Failed. Out of Bounds"&&fseekTest== 0);
	int freadTest = fread(linePlanet, 1, charsPerLinePlanet-1, file);
	assert("Reading line in file of Solar System Data Failed. Out of Bounds"&&freadTest==charsPerLinePlanet-1);

	//We use a data buffer to store in characters 1 item of data. This will later be converted to a double and saved to the solarSystem container
	//We then loop through all of the planets in order and save the characters stating the planet ID to the buffer. Then we convert the char buffer
	//to a double and save it to the solar system container
	char dataBuffer[CharsInDataBufferPlanet+1] = {'\0'}; // Buffer to hold extracted double string
	for (int dataPointID = 0; dataPointID < doubleDataPointsPlanet; dataPointID++) {
		for (int charOfDataBufferNum = 0; charOfDataBufferNum < CharsInDataBufferPlanet; charOfDataBufferNum++) {
			dataBuffer[charOfDataBufferNum] = linePlanet[charOfDataBufferNum + (dataPointID+dataPointsBeforePlanetProperties)*CharsSeperateDataPlanet];
		}
		*(dataPointers[dataPointID]) = atof(dataBuffer);
	}

	//The planet name is a string and is stored with the character count of 2 data points.
	//This means it has size of 2 data points and the data separator marker
	#define planetNameLength (CharsSeperateDataPlanet + CharsInDataBufferPlanet)
	#define dataPointsBeforePlanetName 1
	char planetName [planetNameLength+1]= {'\0'};
	for (int charID = 0; charID < planetNameLength; charID++) {
		planetName[charID] = linePlanet[charID + dataPointsBeforePlanetName * CharsSeperateDataPlanet];
	}

	loadPlanetHeightMap(&(planetData->planetHeightData[0]), planetName);
}
