#pragma once
#define MAX 255
#define SIZE_VECTOR 128
#define SIZE_EIGHT 8
#define SIZE_SIXTEEN 16
#define LETTER_SIZE 26

typedef struct {
	int size;
	int* pointer;
	int n;
	int limit;
} HoleMemory;

typedef struct {
	char c;
	int size;
} Letter;
void vectorUpdateInFile(FILE* fpWrite, char* vector, char* letters, int* sizeOfLetters, char* buffer, int address);
void q1();
int insertVectorWithSpecials(char* vector, char* buffer, int* freelist, int* freeSpace, Letter** ch, int* counterLocation, HoleMemory* holeMemory8, HoleMemory* holeMemory16);
int deleteWithSpecials(char* vector, char* buffer, int* freelist, int* freeSpace, Letter** ch, int* counterLocation, HoleMemory* holeMemory8, HoleMemory* holeMemory16);
int findFreeSpaceIndex(int* freeSpace);
int findFitFreeSpace(int charSize, int* freeSpace);
Letter* findLetter(Letter** letters, char* buffer);
void initHole(HoleMemory* hole, int size);
int insertVector(char* vector, int* freelist, int* freeSpace, Letter* letter);
int deleteFromVector(char* vector, int* freelist, int* freeSpace, Letter** ch, char buffer);
int findIndexInVector(char* vector, char buffer);
int countFreeSpace(int* freeSpace, int index);
void updateMemoryEnd(char* vector, int* freeSpace, int* freelist, int index);
void updateFreelistInsert(int* freeSpace, int* freelist);
void updateFreelistDelete(int* freeSpace, int* freelist);
void countFreeSpaceEnd(int* freeSpace, int* j, int tempIndex, int* counter);
void updateMemoryBetween(char* vector, int* freeSpace, int* freelist);
void countFreeSpaceBetween(int* freeSpace,int* freelist, int* j, int tempIndex, int* counter, int i);
void updateMemoryInsert(int* freeSpace, int* freelist, char* vector, int index, Letter* letter);