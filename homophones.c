/* make -f hom.mk */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

#include "mvm.h"

char g_strKey[135000][50];
char g_strPhonemes[135000][128];

char* SplitKey(char *strLine, char *strKey)
{
	int i;

	for (i = 0; i < (int)strlen(strLine); i++)
	{
		if (strLine[i] != '#')
			strKey[i] = strLine[i];
		else
			break;
	}
	strKey[i] = NULL;

	return strKey;
}

char* SplitPhonemes(char *strLine, int iNum, char *strPhonemes)
{
	int i, j;
	int iTokenCount = 0;

	for (i = (int)strlen(strLine); i >= 0; i--)
	{
		if (strLine[i] == '#')
		{
			iTokenCount++;
			break;
		}

		if (strLine[i] == ' ')
			iTokenCount++;

		if (iTokenCount == iNum)
			break;
	}

	for (i = i + 1, j = 0; i < (int)strlen(strLine); i++, j++)
		strPhonemes[j] = strLine[i];

	strPhonemes[j] = NULL;

	return strPhonemes;
}

int main(int argc, char *argv[])
{
	mvm *m1, *m2;

	int iPhonemesNum;
	int iIndex;
	int i, j;
	int mi;

	FILE * fp;
	char line[256];

	char strBuf[255];

	char strWord[50], *strPhonemes;

	char** av;

	/* check parameter */
	if (argc < 4)
	{
		printf("Too less parameters.\n\n");
		return -1;
	}

	if (strcmp(argv[1], "-n") != 0)
	{
		printf("Parameter 1 must be '-n'.\n\n");
		return -1;
	}

	iPhonemesNum = atoi(argv[2]);

	/* read cmudict.txt */
	fp = fopen("cmudict.txt", "r");
	if (fp == NULL)
	{
		printf("File cmudict.txt not found.\n\n");
		return(-1);
	}

	/* Set up empty array */
	m1 = mvm_init();
	m2 = mvm_init();

	iIndex = 0;
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		line[strlen(line) - 1] = 0;

		SplitKey(line, strBuf);
		strcpy(g_strKey[iIndex], strBuf);
		SplitPhonemes(line, iPhonemesNum, strBuf);
		strcpy(g_strPhonemes[iIndex], strBuf);

		mvm_insert(m1, g_strKey[iIndex], g_strPhonemes[iIndex]);
		mvm_insert(m2, g_strPhonemes[iIndex], g_strKey[iIndex]);

		iIndex++;
	}

	fclose(fp);

	/* start find */
	for (i = 3; i < argc; i++)
	{
		strcpy(strWord, argv[i]);

		strPhonemes = mvm_search(m1, strWord);
		if (strPhonemes != NULL)
		{
			printf("%s (%s) : ", strWord, strPhonemes);

			av = mvm_multisearch(m2, strPhonemes, &mi);
			for (j = 1; j <= mi; j++)
				printf("%s ", av[j-1]);

			printf("\n");
		}
		else
		{
			printf("%s cannot be found.\n", strWord);
		}
	}

	printf("\n");

	/* Freeing */
	mvm_free(&m1);
	mvm_free(&m2);

	return 0;
}
