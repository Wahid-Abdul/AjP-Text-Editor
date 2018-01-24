//
//  textEditor.cpp
//  Text Editor
//
//  Created by Ajith Panneerselvam on 23/01/18.
//

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <cmath>
using namespace std;


struct character{
    char data;
    int column;
    struct character* nextCharacter;
};


struct line{
    int lineNumber;
    int characterCount;
    struct character* characterHead;
    struct line* nextLine;
};


struct hashPattern{
    long hashValue;
    char pattern[16];
    int patternLength;
    struct character* nextString;
};


struct hashString{
    long hashValue;
    int stringLength;
};


typedef struct character* CHARACTER;
typedef struct line* LINE;

//Global variable Declarations
LINE lineHead = NULL;
int totalNumberOfLines = 0;

// Auxiliary function to create Character node
CHARACTER createCharacterNode(char ch){
    CHARACTER newNode = (CHARACTER)malloc(sizeof(struct character));
    if(newNode != NULL){
        newNode->data = ch;
        newNode->column = 1;
        newNode->nextCharacter = NULL;
    }
    return newNode;
}


// Auxiliary function to create Line node
LINE createLineNode(int lineN){
    LINE newNode = (LINE)malloc(sizeof(struct line));
    if(newNode != NULL){
        newNode->lineNumber = lineN;
        newNode->characterCount = 0;
        newNode->characterHead = NULL;
        newNode->nextLine = NULL;
    }
    return newNode;
}


// Auxiliary function, which the checks whether the given two strings are equal
bool checkEqual(char* string1, char* string2){
    int i = 0;
    while(string1[i] != 0){
        if(string1[i] != string2[i]) return false;
        i++;
    }
    return true;
}


// Auxiliary function to get the length of the word, where the demiliter is 'space' character
int findWordLength(char* textData, int start, int end){
    int len = 0;
    for(int i = start; i < end; i++){
        if(textData[i] == ' ') break;
        len++;
    }
    return len;
}


// Auxiliary function to find the hash value of a given string
// It can produce hash for a string with at most 15 characters. When the string size is greater than 15, it will lead to 'long int' overflow!
hashString findHashString(char * str){
    int base = 3, exponent = 0, i = 0;
    long hashValue = 0;
    hashString hash;
    
    while(str[i] != '\0'){
        hashValue += int(str[i]) * pow(base, exponent);
        exponent++;
        i++;
    }
    
    hash.hashValue = hashValue;
    hash.stringLength = i;
    return hash;
}


// Auxiliary function to find the hash value of a given pattern
// It can produce hash for a pattern with at most 15 characters. When the pattern size is greater than 15, it will lead to 'long int' overflow!
hashPattern findHashPattern(CHARACTER charNode){
    int base = 3, exponent = 0, i = 0;
    long hashValue = 0;
    hashPattern hash;
    CHARACTER currentCharacter = charNode;
    
    while(currentCharacter != NULL && currentCharacter->data != ' '){
        hashValue = hashValue + (int(currentCharacter->data) * pow(base, exponent));
        hash.pattern[i] = currentCharacter->data;
        exponent++;
        i++;
        currentCharacter = currentCharacter->nextCharacter;
    }
    
    //    cout<<"\nHash value for "<<str<<" is "<< hash;
    hash.hashValue = hashValue;
    hash.pattern[i] = '\0';
    hash.patternLength = i;

    while(currentCharacter != NULL && (currentCharacter->data == ' ' || currentCharacter->data == '.')) currentCharacter = currentCharacter->nextCharacter;
    
    hash.nextString = currentCharacter;

    return hash;
}


// Search
// Robin Karp Algorithm, which uses hash value technique to find the substring. 
// This module currently supports only single word search like "Ipsum", unlike "Lorem Ipsum"
void search(char* str){
    if(!lineHead) return;
    
    hashString hashS = findHashString(str);
    hashPattern hashP;
    LINE currentLine = lineHead;
    CHARACTER currentCharacter = NULL;
    bool isPresent = false;
    
    while(currentLine){
        currentCharacter = currentLine->characterHead;
        
        while(currentCharacter){
            hashP = findHashPattern(currentCharacter);
            if(hashP.patternLength == hashS.stringLength && hashP.hashValue == hashS.hashValue){
                if(checkEqual(str, hashP.pattern)){
                    isPresent = true;
                    cout<<"\nLine number: "<<currentLine->lineNumber<<" Column: "<<currentCharacter->column;
                }
            }
            currentCharacter = hashP.nextString;
        }
        
        currentLine = currentLine->nextLine;
    }
    
    if(!isPresent) cout<<"\nNo instances of \""<<str<<"\" was found.\n";
}


// Find and Replace
void findAndReplace(){
    // yet to implement
}


// Populate the text editor with some random paragraph
// This module can be synonymous to 'Copy-Paste'. This module will be effective when we paste a paragraph in the text editor.
void populateTextEditor(char* textData, int n){
    int i = 0, j = 0, offset = 0, wordLength = 0, charLimit = 0, debug = 0, lineNumber = 1;
    CHARACTER previousCharacter = NULL;
    CHARACTER newCharNode;
    LINE previousLine = NULL;
    
    while(textData[i] != '\0' && i < n){
        LINE currentLine = createLineNode(lineNumber);
        previousCharacter = NULL;
        totalNumberOfLines++;
        // set the lineHead to the first line node
        if(lineHead == NULL)    lineHead = currentLine;
        
        // The constant '40' is the number of character allowed in a line.
        charLimit = i + 40;
        wordLength = findWordLength(textData, i, n) - 1;
        
        // Create a sequence of words in a line
        while(i + wordLength < charLimit && i < n){
            // Create a word linked list
            for(j = i; j <= i + wordLength; j++){
                newCharNode = createCharacterNode(textData[j]);
                if(!currentLine->characterHead) currentLine->characterHead = newCharNode;
                if(!previousCharacter)  previousCharacter = newCharNode;
                else{
                    previousCharacter->nextCharacter = newCharNode;
                    previousCharacter = newCharNode;
                }
                currentLine->characterCount++;
                newCharNode->column = currentLine->characterCount;
            }
            
            // Create a 'space' node  if it fits in the line
            if(j < charLimit && textData[j] == ' '){
                newCharNode = createCharacterNode(textData[j]);
                previousCharacter->nextCharacter = newCharNode;
                previousCharacter = newCharNode;
                currentLine->characterCount++;
                newCharNode->column = currentLine->characterCount;
                j++;
            }
            
            i = j;
            
            if(i == charLimit && textData[i] == ' '){
                i++;
                break;
            }
            wordLength = findWordLength(textData, i, n) - 1;
        }
        
        // This gets exectued when first line node is created
        if(!previousLine) previousLine = currentLine;
        else{
            previousLine->nextLine = currentLine;
            previousLine = currentLine;
        }
        lineNumber++;
    }
}


// Insert a character
void insertCharacter(char ch, int lineNumber, int column){
    if(!lineHead) return;
    int i = 1;
    LINE currentLine = lineHead;
    CHARACTER currentCharacter = NULL, previousCharacter = NULL;
    
    while(currentLine != NULL && currentLine->lineNumber != lineNumber)     currentLine = currentLine->nextLine;
    currentCharacter = currentLine->characterHead;
    
    if(currentLine->characterCount + 1 <= 40){
        currentLine->characterCount++;
        while(currentCharacter != NULL && currentCharacter->column != column){
            previousCharacter = currentCharacter;
            currentCharacter = currentCharacter->nextCharacter;
            i++;
        }
        
        CHARACTER newNode = createCharacterNode(ch);
        previousCharacter->nextCharacter = newNode;
        newNode->nextCharacter = currentCharacter;
        
        if(!currentCharacter)   newNode->column = i;
        else    newNode->column = column;

        while(currentCharacter){
            currentCharacter->column++;
            currentCharacter = currentCharacter->nextCharacter;
        }
        cout<<"\nCharacter \""<<ch<<"\" is inserted at Line Number: "<< lineNumber << ", Column: " <<column <<"\n";
    }
    else{
        // yet to implement
    }
}


// Insert a word
void insertWord(){
    // yet to implement
}


// Delete a character
void deleteCharacter(){
    
}


// Delete a word
void deleteWord(){
    // yet to implement
}


// Delete a line
void deleteLine(int lineNumber){
    if(!lineHead || lineNumber > totalNumberOfLines){   cout<<"\n\nInvalid Line Number"; return;    }
    LINE currentLine = lineHead, previousLine = NULL;
    
    while(currentLine && currentLine->lineNumber != lineNumber){
        previousLine = currentLine;
        currentLine = currentLine->nextLine;
    }
    previousLine->nextLine = currentLine->nextLine;
    free(currentLine);
    cout<<"\n\nLine " <<lineNumber<<" Deleted.";
}


// Print the entire file
void printFile(){
    if(!lineHead) return;
    int i = 1;
    LINE currentLine = lineHead;
    CHARACTER currentCharacter = NULL;
    
    cout<<"\n";
    while(currentLine){
//        cout<<"\nLine Number "<<currentLine->lineNumber<< " " <<currentLine->characterCount<<"\n";
        currentCharacter = currentLine->characterHead;
        while(currentCharacter){
            cout<<currentCharacter->data;
            currentCharacter = currentCharacter->nextCharacter;
        }
        cout<<"\n";
        currentLine = currentLine->nextLine;
    }
}




//
void testcases(){
    char sampleData1[] = "Lorem Ipsum is simply dummy text of the";
    char sampleData2[] = "ZZZZZZZZZZZZZZZ";
    char sampleData3[] = "Ipsum";
    char sampleData4[] =  "evil";
    char textData[] = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
    int textDataLength = strlen(textData);
    
    //    cout<<"TextData length is "<<textDataLength<<"\n";
    //    cout<<"Sample length is "<<strlen(sampleData)<<"\n";
    
    populateTextEditor(textData, textDataLength);
    printFile();
    cout<<"\nSearch for the word \""<<sampleData3<<"\"";
    search(sampleData3);
    cout<<"\n";
    cout<<"\nSearch for the word \""<<sampleData4<<"\"";
    search(sampleData4);
    cout<<"\n";
    insertCharacter('X', 5, 39);
    printFile();
    insertCharacter('Y', 5, 2);
    printFile();
    deleteLine(3);
    printFile();
}


int main(){
    testcases();
    return 0;
}
