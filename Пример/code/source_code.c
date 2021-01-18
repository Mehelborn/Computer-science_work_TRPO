/*��������� ����������������� ����������� � ��������� ������ ������
��������� � ��������� ��������� �� �������� ���������� �����, ����:
�	����������� ����������������� ���������;
�	(���������)
�	��������� + ���������
�	��������� - ���������
�	��������� * ���������
�	��������� / ���������
�	not(���������) � �������� ��������� ��������
�	and(���������, ���������) � �������� ���������� �
�	or(���������, ���������) � �������� ���������� ���
�	xor(���������, ���������) � �������� ������������ ���������� ���
��������� ��������� � ����� ��������� �����.

����� ��������� �������, 2013�.*/

#include <stdio.h>
#include <conio.h>
#include "analys.h"


void GetExpr(FILE *fp, char *pStr);

int main(void)
{
	FILE *FileIn, *FileOut;
	char FileName[MAX_S], Expr[MAX_S];
	unsigned long Result;

	system("cls");
	//��������� �������� ����
	puts("Enter the file name of the form c:\\\\example.txt:");
	gets(FileName);
	if ((FileIn = fopen(FileName,"r")) == NULL) {
		puts("Opening file to read error.");
		getchar();
		return 0;
	}
	
	strcat(FileName, "_res.txt"); // ��� ����� ����������
	
	//��������� ���� ��� ������ ����������
	if ((FileOut = fopen(FileName,"w")) == NULL) {
		puts("Opening file to write error.");
		getchar();
		fclose(FileIn);
		return 0;
	}

	while (!feof(FileIn)) {
		GetExpr(FileIn, Expr);	// �������� ������ � ����������
		if (!*Expr) continue;		// ���� ������ ������ - �� ��������� ������� ��������
		fprintf(FileOut, "%s = ", Expr); // �������� ������ � ���������� � ����
		if (Calculate(Expr, &Result)) fprintf(FileOut, "%X\n", Result); // ���� ���������� ������ ��� ������ ���������� �����
		else fprintf(FileOut, "������! %s\n", ErrMessage()); // ����� ���������� ��������� �� ������
	}

	fclose(FileIn);
	fclose(FileOut);
	printf("The calculation results are written to file %s", FileName);
	getchar();

  return 1;
}

/*������� ���������� ��������� �� ������, ����������� �� �����, �� �������
������� ���������� ������� � ����� ��������� � �������� ��������*/
void GetExpr(FILE *fp, char *pStr)
{
	int ch;
	char *pChar;
	
	pChar = pStr;
	while ((ch = getc(fp)) != '\n' && ch != EOF) { // ������ ������� ���� �� �������� ������� ������, ��� ����� �����
		if (isspace(ch)) continue;	// ���������� ���������� �������
		*pChar = toupper(ch);	// ��������� ��������� ������� � ������� �������
		pChar++;
	}
	*pChar = '\0';
}
