/*
*	��������� ����������������� ����������� � ��������� ������ ������
*	��������� � ��������� ��������� �� �������� ���������� �����, ����:
*	�	����������� ����������������� ���������;
*	�	(���������)
*	�	��������� + ���������
*	�	��������� - ���������
*	�	��������� * ���������
*	�	��������� / ���������
*	�	not(���������) � �������� ��������� ��������
*	�	and(���������, ���������) � �������� ���������� �
*	�	or(���������, ���������) � �������� ���������� ���
*	�	xor(���������, ���������) � �������� ������������ ���������� ���
* 
*	��������� ��������� � ����� ��������� ����� � ���������� "_res.txt".
* 
*	����� ������� ���� ���������, 2021�.
*/

#include <stdio.h>
#include <conio.h>

#include "analysis.h" //���������� ������ ������� ��������� �������������� � ����������� �������


void getExpr(FILE *fp, char *pStr);

int main(void) {
	FILE *sourceFile, *resultFile;
	char fileName[GREAT_S], expr[GREAT_S];
	unsigned long result;

	//��������� �������� ����
	printf("Enter the file name: ");
	scanf("%s", fileName);
	if ((sourceFile = fopen(fileName,"r")) == NULL) {
		printf("ERROR. Cannot open the file.");
		getchar();
		return 0;
	}
	
	strcat(fileName, "_res.txt"); // ��� ����� ����������
	
	//��������� ���� ��� ������ ����������
	if ((resultFile = fopen(fileName,"w")) == NULL) {
		printf("ERROR. Cannot open/create the result file.");
		getchar();
		fclose(sourceFile);
		return 0;
	}

	while (!feof(sourceFile)) {
		getExpr(sourceFile, expr);	// �������� ������ � ����������
		if (!*expr) continue;		// ���� ������ ������ - �� ��������� ������� ��������
		fprintf(resultFile, "%s = ", expr); // �������� ������ � ���������� � ����
		if (calculate(expr, &result)) fprintf(resultFile, "%X\n", result); // ���� ���������� ������ ��� ������ ���������� �����
		else fprintf(resultFile, "ERROR! %s\n", errMessage()); // ����� ���������� ��������� �� ������
	}

	fclose(sourceFile);
	fclose(resultFile);
	printf("The calculation results are written to file %s", fileName);
	
	getchar();
	return 1;
}

/*
������� ���������� ������ �������, 
��������� ����� � ������� ������� 
� ����������� �������������� ������ 
���������� expr � main
*/
void getExpr(FILE *fp, char *pStr) {
	int ch; //������� ������
	char *pChar;
	
	pChar = pStr;
	while ((ch = getc(fp)) != '\n' && ch != EOF) { // ������ ������� ���� �� �������� ������� ������, ��� ����� �����
		if (isspace(ch)) continue;	// ���������� ���������� �������
		*pChar = toupper(ch);	// ��������� ��������� ������� � ������� �������
		pChar++;
	}
	*pChar = '\0'; //����-�������������� ���������� ������
}
