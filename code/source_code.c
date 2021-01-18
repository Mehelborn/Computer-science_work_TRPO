#include <stdio.h>

#include "analyse.h"

void getExpr(FILE *fp, char* currentExpr);

int main(void) {
	FILE* sourceFile, * resultFile;
	char FileName[GREAT_S], Expr[GREAT_S];
	unsigned long result;

	printf("Enter the local file name: ");
	scanf("%s", FileName);
	if ((sourceFile = fopen(FileName, "r")) == NULL) {
		printf("ERROR. Cannot open the file.");
		getchar();
		return 0;
	}

	strcat(FileName, "_results.txt");


}

void getExpr(FILE* fp, char* currentExpr) {
	int ch;
	int *currentCh;

	currentCh = currentExpr;
	while ((ch = getc(fp)) != '\n' && ch != EOF) {
		if (isspace(ch)) continue;
		*currentCh = toupper(ch);
		currentCh++;
	}
}