#include "jsonParser.h"

bool isNumeric (char character) {
    return isdigit(character) || character=='.';
}

FILE* jsonSectionToNodeRecurse (FILE *filePtr, struct Node* NodeAllocateLocation, void** MemoryShiftSave) {
    struct Node newNode;

    bool inString = false;
    bool inNum = false;
    bool stringContainsDecimal = false;

    long saveStartString;
    char* tempStringSave;
    char* tempStringMover;
    bool isCategory = true;
    void* nextSaveLocation = (void*)NodeAllocateLocation + sizeof(struct Node);
    while (true) {
        char character = fgetc(filePtr);
#ifdef DEBUG
        if (character == EOF) {
            // WRITE SOME ERROR, THIS SHOULDN't HAPPEN
            break;
        }
#endif // DEBUG
        if (character == '"' || inNum != isNumeric(character)) {
            if (inString) {
                // future thing
                if (isCategory) {
                    ///SAVE NAME OF NODE
                    // the name of the node is stored immediately after the node in memory
                    strcpy(nextSaveLocation, tempStringSave);
                    nextSaveLocation += tempStringMover - tempStringSave + 1;
                    isCategory = false;
                } else if (inNum) {
                    if (stringContainsDecimal) {
                        /// Save A FLOAT VALUE
                        newNode.nodeValue.f = (float)atof(tempStringSave);
                    } else {
                        /// Save AN INT VALUE
                        newNode.nodeValue.i = atoi(tempStringSave);
                    }
                    fseek(filePtr, -1, SEEK_CUR);
                } else {
                    /// SAVE A STRING VALUE
                    newNode.nodeValue.s = nextSaveLocation;
                    strcpy(nextSaveLocation, tempStringSave);
                    nextSaveLocation += tempStringMover - tempStringSave;
                }
                free(tempStringSave);
                inNum = false;
            } else {
                if (character != '"') { inNum = true; stringContainsDecimal = false; }
                // Figure out the size of the string
                saveStartString = ftell(filePtr);
                int sizeOfThisString = 0;
                character = fgetc(filePtr);
                if (inNum) {
                    while (isNumeric(character)){
                        character = fgetc(filePtr);
                        sizeOfThisString ++;
                    }
                    sizeOfThisString++;
                    saveStartString--;
                } else {
                    while (character != '"'){
                        character = fgetc(filePtr);
                        sizeOfThisString ++;
                    }
                }
                tempStringSave = (char*)malloc(sizeof(char) * (sizeOfThisString + 1));
                tempStringMover = tempStringSave;
                *(tempStringSave + sizeOfThisString) = '\0';
                fseek(filePtr, saveStartString, SEEK_SET);
            }
            inString = !inString;
            continue;
        }

        if (inString) {
            if (character == '.') {
                stringContainsDecimal = true;
            }
            *tempStringMover = character;
            tempStringMover++;
        } else if (character == 't') {
            /// SAVE THE BOOL 'TRUE'
            newNode.nodeValue.b = true;
        } else if (character == 'f') {
            /// SAVE THE BOOL 'FALSE'
            newNode.nodeValue.b = false;
        } else if (character == ',') {
            /// GET NEXT LINKED NODE AND END
            fgetc(filePtr);
            jsonSectionToNodeRecurse (filePtr, (struct Node*)nextSaveLocation, NULL);
            newNode.nextNode = nextSaveLocation;
            break;
        } else if (character == '}') {
            /// END OF NODE
            newNode.nextNode = NULL;
            break;
        } else if (character == '{') {
            newNode.nodeValue.subNode = nextSaveLocation;
            filePtr = jsonSectionToNodeRecurse (filePtr, nextSaveLocation, &nextSaveLocation);
        }
    }
    /// SAVE STRUCT TO MEMORY
    memcpy(NodeAllocateLocation, &newNode, sizeof(struct Node));
    if (MemoryShiftSave != NULL) {
        *MemoryShiftSave += (nextSaveLocation-(void*)NodeAllocateLocation);
    }
    return (filePtr);
}

union NodeValue getJsonValue (struct Node* jsonNode, char* pathToValue) {
    int charsTraversedInPathSoFar = 0;
    int lenOfPath = strlen(pathToValue);
    int i; int j = 0;
    struct Node* testJsonNode = jsonNode;
    char* testNodeName = (void*)testJsonNode + sizeof(struct Node);
    char* nextPathToTraverse = malloc(lenOfPath * sizeof(char));

    while (true) {
        /// Get the next name of the target node
        for (i = 0; i < lenOfPath && (nextPathToTraverse[i] = pathToValue[j]) != '/' && pathToValue[j] != '\0'; i++, j++);
        j++;
        nextPathToTraverse[i] = '\0';
        /// Go through the nodes until you find the target
        while (true) {
            if (strcmp(testNodeName, nextPathToTraverse) == 0) {
                // node matches
                if (j > lenOfPath) {
                    return testJsonNode->nodeValue;
                }
                testJsonNode = testJsonNode->nodeValue.subNode;
                testNodeName = (void*)testJsonNode + sizeof(struct Node);
                break;
            } else {
                // node doesn't match, try the next node
                testJsonNode = testJsonNode->nextNode;
                if (testJsonNode == NULL) {
                    fprintf(stderr, "Path in JSON does not exist: %c", pathToValue);
                }
                testNodeName = (void*)testJsonNode + sizeof(struct Node);
            }
        }
    }
};

struct Node* jsonFileToParentNode (FILE *filePtr) {
    /// Get basic vars to figure out what size this needs to be
    int numOfNodes = 0;
    int totalStringLengths = 0;
    bool inString = false;

    long saveStartFile = ftell(filePtr);
    char character;
    while ((character = fgetc(filePtr)) != EOF) {
        if (inString) {
            totalStringLengths ++;
        } else if (character == ':') {
            numOfNodes++;
        }
        if (character == '"') {
            inString = !inString;
            totalStringLengths--;
        }
    }

    /// Calculate size of node to store the json
    int sizeAssignToParentNode = (numOfNodes * sizeof(struct Node)) + (totalStringLengths * sizeof(char));
#ifdef DEBUG
    printf("Num of Nodes in Json %d\n", numOfNodes);
    printf("Num Chars in Str %d\n", totalStringLengths);
    printf("Size of Node %d\n", sizeof(struct Node));
    printf("Size being assigned to Json %d\n", sizeAssignToParentNode);
#endif // DEBUG

    /// Actually assign vars for the json thing
    struct Node* jsonParentPointer = (struct Node*)malloc(sizeAssignToParentNode);
    fseek(filePtr, saveStartFile, SEEK_SET);

    while (fgetc(filePtr) != '{') {} // get past the starting '{'

    jsonSectionToNodeRecurse(filePtr, jsonParentPointer, NULL);

    return jsonParentPointer;
}

void TestLoadingJson () {
    FILE *file = fopen("./config/testJson.json", "r");
    assert("Error Opening Solar System Data File"&&file!=NULL);

    struct Node* testJson = jsonFileToParentNode (file);
    getJsonValue(testJson, "SurfaceColor/R");
    fclose(file);
}
