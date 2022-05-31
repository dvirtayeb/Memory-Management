#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "q1.h"
#include <ctype.h>
#include <string.h>

// MAIN:
int main()
{
	q1();
}

// FUNCTIONS:
void q1()
{
	char letters[LETTER_SIZE + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int sizeOfLetters[LETTER_SIZE + 1] = { 8, 16, 16, 32, 8, 16, 8, 16, 8, 16,
		16, 8, 48, 8, 8, 16, 8, 16, 72, 24, 32, 8, 16, 32, 8, 20 };
	Letter** ch;
	ch = (Letter**)malloc(LETTER_SIZE * sizeof(Letter*));
	if (!ch)
	{
		return;
	}
	for (int i = 0; i < LETTER_SIZE; i++)
	{
		ch[i] = (Letter*)malloc(LETTER_SIZE * sizeof(Letter));
		if (ch[i] == NULL)
			return;
		ch[i]->c = letters[i];
		ch[i]->size = sizeOfLetters[i];
	}
	FILE* fpWrite = fopen("mem simple out.txt", "w");
	if (fpWrite == NULL)
	{
		return;
	}
	FILE* fp = fopen("mem simple in.txt", "r");
	if (fp == NULL)
	{
		return;
	}
	char* vector = (char*)calloc(SIZE_VECTOR, sizeof(char));
	if (!vector)
	{
		return;
	}
	int counterLocation = 0;
	int* freelist = (int*)calloc(SIZE_VECTOR, sizeof(int));
	if (!freelist)
	{
		return;
	}

	for (int i = 0; i < SIZE_VECTOR; i++)
	{
		freelist[i] = -1;
	}

	int* freeSpace = (int*)calloc(SIZE_VECTOR, sizeof(int));
	if (!freeSpace)
	{
		return;
	}
	int speacial;
	char buffer[MAX];
	HoleMemory* holeMemory8;
	HoleMemory* holeMemory16;
	// FIRST LINE (FROM FILE):
	int s = fscanf(fp, "%d", &speacial);
	switch (speacial)
	{
	case 0:// NO SPECIALS
		freeSpace[0] = SIZE_VECTOR;
		fprintf(fpWrite, "%d\n", speacial);
		while (fscanf(fp, "%s", &buffer[0]) > 0)
		{
			Letter* letter = findLetter(ch, &buffer[0]);
			if (buffer[1] == '+') {
				int address1 = insertVector(vector, freelist, freeSpace, letter);
				vectorUpdateInFile(fpWrite, vector, letters, sizeOfLetters, buffer, address1);
			}
			else
			{
				int address2 = deleteFromVector(vector, freelist, freeSpace, ch, buffer[0]);
				vectorUpdateInFile(fpWrite, vector, letters, sizeOfLetters, buffer, address2);
			}
		}

		break;

	case 1: // WITH SPECIALS
		fprintf(fpWrite, "%d\n", speacial);
		holeMemory8 = (HoleMemory*)malloc(sizeof(HoleMemory));
		if (holeMemory8 == NULL) {
			return;
		}
		//init 8 memory special
		initHole(holeMemory8, SIZE_EIGHT);

		holeMemory16 = (HoleMemory*)malloc(sizeof(HoleMemory));
		if (holeMemory16 == NULL) {
			return;
		}
		//init 16 memory special
		initHole(holeMemory16, SIZE_SIXTEEN);
		while (fscanf(fp, "%s", &buffer[0]) > 0)
		{
			if (buffer[1] == '+')
			{
				int address1 = insertVectorWithSpecials(vector, &buffer[0], freelist, freeSpace, ch, &counterLocation, holeMemory8, holeMemory16);
				vectorUpdateInFile(fpWrite, vector, letters, sizeOfLetters, buffer, address1);
			}
			else
			{
				int address = deleteWithSpecials(vector, &buffer[0], freelist, freeSpace, ch, &counterLocation, holeMemory8, holeMemory16);
				vectorUpdateInFile(fpWrite, vector, letters, sizeOfLetters, buffer, address);
			}
		}
		break;

	default:
		break;
	}

	fclose(fp);
}

int deleteWithSpecials(char* vector, char* buffer, int* freelist, int* freeSpace, Letter** ch, int* counterLocation, HoleMemory* holeMemory8, HoleMemory* holeMemory16)
{
	int index = -1;
	if (*counterLocation <= SIZE_VECTOR)
	{
		for (int i = 0; i < LETTER_SIZE; i++)
		{
			if (ch[i]->c == buffer[0])
			{
				if (ch[i]->size == 16 && holeMemory16->n < 3)
				{

					int index = findIndexInVector(vector, buffer[0]);
					for (int j = index; j < ch[i]->size + index; j++)
					{
						vector[j] = 0;
						freeSpace[j] = 0;
					}
					holeMemory16->pointer[holeMemory16->n] = index;
					holeMemory16->n += 1;
					return 0;
				}
				else if (ch[i]->size == 8 && holeMemory8->n < 3)
				{
					int index = findIndexInVector(vector, buffer[0]);
					for (int j = index; j < ch[i]->size + index; j++)
					{
						vector[j] = 0;
						freeSpace[j] = 0;
					}
					holeMemory8->pointer[holeMemory8->n] = index;
					holeMemory8->n += 1;
					return 0;
				}
				else
				{
					(freeSpace[index]) = ch[i]->size;
					updateMemoryBetween(vector, freeSpace, freelist);
					updateMemoryEnd(vector, freeSpace, freelist, index);
					return 0;
				}
			}
		}
	}
	return 0;
}

int insertVectorWithSpecials(char* vector, char* buffer, int* freelist, int* freeSpace, Letter** ch, int* counterLocation, HoleMemory* holeMemory8, HoleMemory* holeMemory16)
{
	int index = -1;
	if (*counterLocation <= SIZE_VECTOR)
	{
		for (int i = 0; i < LETTER_SIZE; i++)
		{
			if (ch[i]->c == buffer[0])
			{
				if (ch[i]->size == 16 && holeMemory16->n > 0)
				{
					vector[holeMemory16->pointer[holeMemory16->n - 1]] = buffer[0];

					for (int j = holeMemory16->pointer[holeMemory16->n - 1]; j < ch[i]->size + (holeMemory16->pointer[holeMemory16->n - 1]); j++)
					{
						vector[j] = ch[i]->c;
					}
					holeMemory16->n -= 1;
					return holeMemory16->pointer[holeMemory16->n];
				}
				else if (ch[i]->size == 8 && holeMemory8->n > 0)
				{
					vector[holeMemory8->pointer[holeMemory8->n - 1]] = buffer[0];
					for (int j = holeMemory8->pointer[holeMemory8->n - 1]; j < ch[i]->size + (holeMemory8->pointer[holeMemory8->n - 1]); j++)
					{
						vector[j] = ch[i]->c;
					}
					holeMemory8->n -= 1;
					return holeMemory8->pointer[holeMemory8->n];
				}
				else
				{
					if (ch[i]->size == (freeSpace[i]))
					{
						if (ch[i]->c == buffer[0]) {
							int fit = findFitFreeSpace(ch[i]->size, freeSpace);
							vector[freelist[fit]] = ch[i]->c;
							freelist[fit] = freelist[fit] + 1;
						}
					}
					else if (buffer[0] == ch[i]->c)
					{
						int fitFree = findFitFreeSpace(ch[i]->size, freeSpace);
						for (int j = 0; j < ch[i]->size; j++)
						{
							if (fitFree == -1) {
								vector[*counterLocation] = ch[i]->c;
								freeSpace[*counterLocation] = -1;
								(*counterLocation)++;
							}
							else {
								vector[freelist[fitFree]] = ch[i]->c;
								freelist[fitFree] = freelist[fitFree] + 1;
								freeSpace[fitFree] -= 1;
							}
						}
						return *counterLocation - (ch[i]->size);
					}
				}
			}
		}
	}
	return -1;
}

int insertVector(char* vector, int* freelist, int* freeSpace, Letter* letter)
{
	int index = -1;
	int indexLetter = 0;
	int count= 0;
	index = findFreeSpaceIndex(freeSpace);
	if (index != -1) // has space
	{
		count = countFreeSpace(freeSpace, index);
		if (letter->size < count) // has extra space
		{
			updateMemoryInsert(freeSpace,freelist,vector, index, letter);
			return index;
		}
		else if (letter->size == count) // in the size
		{
			int i = 0;
			for (i = index; i < index + letter->size; i++)
			{
				vector[i] = letter->c;
				freelist[index] = index;
				freeSpace[i] = -1;
			}
			return index;
		}
		else // not fit, go to next place if exist
		{
			int tempIndex = index;
			for (int i = tempIndex; i < tempIndex+freeSpace[tempIndex]; i++)
			{
				index++;
			}
			count = 0;
			while (count == 0 || letter->size > count)
			{
				while (freeSpace[index] == -1)
				{
					index++;
					if (index == SIZE_VECTOR - 1)
					{
						count = -1;
						return count; // fail to insert there is no space
					}
				}
				count = countFreeSpace(freeSpace, index);
				if (letter->size < count) // has extra space
				{
					updateMemoryInsert(freeSpace, freelist, vector, index, letter);
					return index;
				}
				else if (letter->size == count) // in the size
				{
					int i = 0;
					for ( i = index; i < index + letter->size; i++)
					{
						vector[i] = letter->c;
						freelist[index] = index;
						freeSpace[i] = -1;
					}
					return index;
				}
				else
					index++;
			}
		}
	}
	// There is no space for this letter
	return -1;
}

void updateMemoryInsert(int* freeSpace, int* freelist, char* vector, int index, Letter* letter)
{
	int tempSpace = freeSpace[index];
	int i = 0;
	for (i = index; i < index+ letter->size; i++)
	{
		vector[i] = letter->c;
		freeSpace[i] = -1;
	}
	if (i < SIZE_VECTOR)
	{
		freeSpace[i] = tempSpace - letter->size;
		updateFreelistInsert(freeSpace, freelist);
	}
}

void updateFreelistInsert(int* freeSpace, int* freelist)
{
	for (int i = 0; i < SIZE_VECTOR; i++)
	{
		freelist[i] = -1;
	}
	int tempIndex = 0;
	for (int i = tempIndex; i < SIZE_VECTOR; i++)
	{
		while (freeSpace[tempIndex] == -1) // term if exist number in the start
		{
			tempIndex++;
			i++;
		}
		int j = 0;
		for (j = tempIndex; j < SIZE_VECTOR; j++) // loop until we got the next number in the memory.
		{
			if (freeSpace[j] != -1)
			{
				if (freeSpace[j] != 0)
					freelist[tempIndex] = j;
			}
			else
				break;
		}
		tempIndex = j;
		if (tempIndex == SIZE_VECTOR)
			break;
	}
}

int findFitFreeSpace(int charSize, int* freeSpace)
{
	for (int i = 0; i < SIZE_VECTOR; i++)
	{
		if (charSize <= freeSpace[i] && freeSpace[i] >= 0)
		{
			return i;
		}
	}
	//not found enough space
	return -1;
}

int findIndexInVector(char* vector, char buffer)
{
	for (int i = 0; i < SIZE_VECTOR; i++)
	{
		if (vector[i] == buffer)
		{
			return i;
			break;
		}
	}
	//indicates not found in vector
	return -1;
}

int countFreeSpace(int* freeSpace, int index)
{
	int counter = 0;
	for (int i = index; i < SIZE_VECTOR; i++)
	{
		if (freeSpace[i] != -1)
		{
			counter++;
			if (freeSpace[index] == counter)
				break;
		}
	}
	return counter;
}

int findFreeSpaceIndex(int* freeSpace)
{
	for (int i = 0; i < SIZE_VECTOR; i++)
	{
		if (freeSpace[i] != -1)
			return i;
	}
	return -1;
}


int deleteFromVector(char* vector, int* freelist, int* freeSpace, Letter** ch, char buffer)
{
	for (int i = 0; i < LETTER_SIZE; i++)
	{
		if (buffer == ch[i]->c)
		{
			//moved to here before we delete :)
			int index = findIndexInVector(vector, buffer);
			//save index of first empty space
			freelist[index] = index;
			//j runs from index found until the end of the space of letter which means startPlace+size
			for (int j = index; j < (freelist[index]) + ch[i]->size; j++)
			{
				vector[j] = 0;
				freeSpace[j] = 0;
			}
			(freeSpace[index]) = ch[i]->size;
			updateMemoryBetween(vector, freeSpace, freelist);
			updateMemoryEnd(vector, freeSpace, freelist, index);
			updateFreelistDelete(freeSpace, freelist);
			return 0;
		}
	}
	return 0;
}

void updateFreelistDelete(int* freeSpace, int* freelist)
{
	int tempIndex = 0;
	for (int i = tempIndex; i < SIZE_VECTOR; i++)
	{
		while (freeSpace[tempIndex] == -1) // term if exist number in the start
		{
			tempIndex++;
			i++;
		}
		int j = 0;
		for (j = tempIndex; j < SIZE_VECTOR; j++) // loop until we got the next number in the memory.
		{
			if (freeSpace[j] != -1)
			{
				if (freeSpace[j] != 0)
					freelist[j] = j;
			}
		}
		tempIndex = j;
		if (tempIndex == SIZE_VECTOR)
			break;
	}
}

void updateMemoryBetween(char* vector, int* freeSpace, int* freelist)
{
	int tempIndex = 0;
	for (int i = tempIndex; i < SIZE_VECTOR; i++) // loop until end
	{
		while (freeSpace[tempIndex] == -1) // term if exist number in the start
		{
			tempIndex++;
			i++;
		}
		int counter = -1;
		int j = 0;
		countFreeSpaceBetween(freeSpace,freelist, &j, tempIndex, &counter, i);
		if (counter >= 0)
			freeSpace[tempIndex] = counter; // update
		tempIndex = j;
		if (tempIndex == SIZE_VECTOR)
			break;
	}
}

void countFreeSpaceBetween(int* freeSpace,int* freelist, int* j, int tempIndex, int* counter, int i)
{
	for (*j = tempIndex; *j < SIZE_VECTOR; (*j)++) // loop until we got the next number in the memory.
	{
		if (freeSpace[*j] != -1)
		{
			if (freeSpace[*j] != 0)
			{
				if (*counter == -1)
					(*counter)++;
				int checker = 0;
				if (*j + freeSpace[*j] != SIZE_VECTOR)
					checker = freeSpace[*j] - freelist[*j + freeSpace[*j]];
				else
					break;
				if (checker <= 0)
				{
					int tempValue = freelist[*j];
					int k = *j;
					while(freeSpace[k] != -1)
					{
						freelist[k] = -1;
						k++;
						if (k == SIZE_VECTOR)
							break;
					}
					freelist[*j] = tempValue;
				}
				else
				{
					freelist[i] = freelist[i];
				}
				*counter += freeSpace[*j];
				freeSpace[*j] = 0;
			}
		}
		else
			break;
	}
}

void updateMemoryEnd(char* vector, int* freeSpace, int* freelist, int index)
{
	int tempIndex = 0;
	for (int i = tempIndex; i < SIZE_VECTOR; i++) // loop until end
	{
		while (freeSpace[tempIndex] == -1) // term if exist number in the start
		{
			tempIndex++;
			i++;
		}
		int counter = -1;
		int j = 0;
		countFreeSpaceEnd( freeSpace, &j, tempIndex, &counter);
		if (counter >= 0 && (SIZE_VECTOR == tempIndex + counter))
			freeSpace[tempIndex] = (SIZE_VECTOR - tempIndex); // update
		tempIndex = j;
		index = j;
		if (tempIndex == SIZE_VECTOR)
			break;
	}
}

void countFreeSpaceEnd(int* freeSpace, int* j, int tempIndex, int* counter)
{
	for (*j = tempIndex; *j < SIZE_VECTOR; (*j)++) // loop until we got the next number in the memory.
	{
		if (freeSpace[*j] != -1)
		{
			if (freeSpace[*j] != 0)
			{
				if (*counter == -1)
					(*counter)++;
				*counter += freeSpace[*j];
			}
			else if (*j == SIZE_VECTOR - 1)
			{
				*counter = SIZE_VECTOR - tempIndex;
			}
		}
		else
			break;
	}
}

Letter* findLetter(Letter** letters, char* buffer)
{
	for (int i = 0; i < LETTER_SIZE; i++)
	{
		if (letters[i]->c == buffer[0])
			return letters[i];
	}
	return NULL;
}

void initHole(HoleMemory* hole, int size)
{
	hole->size = size;
	hole->n = 0;
	hole->limit = 3;
	//three int for 3 indexs of size -> for trceback in freeSpace
	hole->pointer = (int*)malloc((hole->limit) * sizeof(int));
	if (hole->pointer == NULL)
		return;
}

void vectorUpdateInFile(FILE* fpWrite, char* vector, char* letters, int* sizeOfLetters, char* buffer, int address)
{
	if (address == 0)
	{
		if (buffer[1] == '-')
			fprintf(fpWrite, "%c %s %d\n", buffer[0], "-", address);
		else
			fprintf(fpWrite, "%c %s %d\n", buffer[0], "+", address);
	}

	else
	{
		if (buffer[1] == '-')
			fprintf(fpWrite, "%d %s %d\n", buffer[0], "-", address);
		else
			fprintf(fpWrite, "%c %s %d\n", buffer[0], "+", address);

	}
}
