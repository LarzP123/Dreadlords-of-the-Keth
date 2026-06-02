#ifndef JSONPARSER_H_INCLUDED
#define JSONPARSER_H_INCLUDED
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

union NodeValue {
    int i;
    float f;
    char* s;
    struct Node* subNode;
    bool b;
};

struct Node {
    // char nodeValueType; // Whenever you query for the node, you have to provide the type, so we don't have to say the type
    union NodeValue nodeValue;
    struct Node* nextNode;
    // Immediately after the node in memory is the node's name
};

struct Node* jsonFileToParentNode (FILE *filePtr);
char* nodeToJson (struct Node* node);
void freeNodeChain (struct Node* node);
void TestLoadingJson ();

#endif // JSONPARSER_H_INCLUDED
