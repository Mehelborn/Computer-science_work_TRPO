/*������ ��������� ����������� � �������������� ������ ��������� � ����������������� ������� ����������, ����:

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

�� ������� ��������� pExpr � ���������� ����������.
����� ��������� �������, 2013�.*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "constants.h"

char *pExpr;		// ��������� �� ������������� ���������
char Token[SMALL_S];	// ����� ��� ���������� �������
char TokType;		// ��� ���������� �������
char ErrType;		// ��� ������
bool Err;			// ���� �� ������ � ���������

bool Calculate(char *pStr, unsigned long *Result);
unsigned long Expression(void);
unsigned long Summand(void);
unsigned long Multiplier(void);
void GetToken(void);
void SetErr(char Type);
char *ErrMessage(void);

/* ����� ����� �����������. */
bool Calculate(char *pStr, unsigned long *Result)
{
	Err = false;	// ���������� ���� ������
	pExpr = pStr;	// ����� ��������� �������� ����������� ���������
	GetToken();		// �������� ������ �������
	*Result = Expression();	// ���������� �������� ���������
		
	if(*pExpr && !Err) SetErr(SUNTAX_ERR); // ���� ����� ���������� ��������� �� � ����� ������, ������ ��������� ������

	return !Err;
}

/*������� ���������� �������� ���������*/
unsigned long Expression(void)
{
	unsigned long Result = 1;
	char Operat;
		
	if (!Err) {	// ���� ������ ������
		Result = Summand();	// ������� �������� ������� ����������
		while ((Operat = *Token) == '+' || Operat == '-'){
			GetToken();
			switch(Operat) {
				case '-':
					Result = Result - Summand();
					break;
				case '+':
					Result = Result + Summand();
					break;
			}
		}
	}
	return Result;
}

/*������� ���������� �������� ����������*/
unsigned long Summand(void)
{
	unsigned long Result = 1, Temp;
	char Operat;
	
	if (!Err) {
		Result = Multiplier();	// �������� �������� ������� ���������
		while ((Operat = *Token) == '*' || Operat == '/'){
			GetToken();
			switch(Operat) {
				case '*':
					Result = Result * Multiplier();
					break;
				case '/':
					Temp = Multiplier();
					if (Temp == 0) SetErr(DIV_BY_ZERO);	// ���������� ������� �� 0
					else Result = Result / Temp;
					break;
			}
		}
	}
	return Result;
}

/*������� ���������� �������� ���������*/
unsigned long Multiplier(void)
{
	unsigned long Result = 1;
	char Operat;
	
	if (!Err){
		if (TokType == NUMBER) scanf(Token, "%x", &Result);	// ���� ��� ���������� ������� - �����, �������� ��� ��������
		else if (*Token == '(') {	// ���� ���������� ������� - ����������� ������
			GetToken();
			Result = Expression();	// �������� �������� ��������� � �������
			if (!Err && *Token != ')') SetErr(CLOSE_BR_EXP);	// ������� ����������� ������ ����� ���������
		}
		else if (TokType == IDENTIFIER){	// ���� ��� ������� - ������������� �������
			Operat = *Token;	// ���������� ������ ������ ��������� ��������
			GetToken();
			if (*Token == '(') {	// ����� ������� ��������� �������� ������� ������
				GetToken();
				Result = Expression();	// �������� ��������� ������� ���������
				if (Operat == 'N') Result = ~Result;	// ���������� �������� ��������� ��������
				else {	// ���� �������� �� NOT �������� ������ ��������
					if (*Token == ',') { // ������� ����������� ����������
						GetToken();
						switch(Operat){
							case 'A':	// ��������� �
								Result = Result & Expression();
								break;
							case 'O':	// ��������� ���
								Result = Result | Expression();
								break;
							case 'X':	// ��������� ����������� ���
								Result = Result ^ Expression();
								break;
						}
					}	
					else if (!Err) SetErr(DELIMITER_EXP); // ���� �� ���������� ������� ����� ������� ���������
				}
				if (!Err && *Token != ')') SetErr(CLOSE_BR_EXP); // ������� ����������� ������ ����� ���������� �������
			}
			else SetErr(OPEN_BR_EXP); // ���� �� ���������� ����������� ������
		}
		else SetErr(SUNTAX_ERR);	// ���� ������� ��� �� ��� �� ����������
		GetToken();
	}
	return Result;
}

/* ������� ���������� ��������� ������� */
void GetToken(void)
{
	char *pChar;

	TokType = UNKNOWN;
	pChar = Token;
	*pChar = '\0';

	if(!*pExpr || Err) return; // ����� ��������� ��� ������
	/* ��������� ������ ���������� � ����������*/
	if(strchr("+-*/(),", *pExpr)){
		TokType = DELIMITER;
		*pChar++ = *pExpr++;	// ��������� � ���������� �������
	}
	/*��������� ��������� ��������*/
	else if (memcmp(pExpr, "OR", 2) == 0){
		for (int i = 0; i < 2; i++) *pChar++ = *pExpr++;
		TokType = IDENTIFIER;
	}
	else if (memcmp(pExpr, "AND", 3) == 0 || 
			memcmp(pExpr, "NOT", 3) == 0 ||
			memcmp(pExpr, "XOR", 3) == 0){
		for (int i = 0; i < 3; i++) *pChar++ = *pExpr++;
		TokType = IDENTIFIER;
	}
	/*��������� ����������������� �����*/
	else if(isxdigit(*pExpr)){
		while(isxdigit(*pExpr)) *pChar++ = *pExpr++;
		if (strlen(Token) > 8) SetErr(EXCESS_OF_THE_MAX); // ����� ��������� ����. ���. ��������
		else TokType = NUMBER;
	}
	/*��������� ������ � ���������*/
	else SetErr(UNKNOWN_SUMB);

  *pChar = '\0';
}

/*������� ������������� ���� � ��� ������*/
void SetErr(char Type)
{
	Err = true;
	ErrType = Type;
}

/*������� ���������� ��������� �� ������ � ���������� �� ������*/
char *ErrMessage(void)
{
  static char *ErrStr[]= {
      "������������ ������ � ���������",
      "�������������� ������ � ���������",
      "��������� '('",
      "��������� ')'",
      "��������� ','",
      "������� �� ����!",
	  "���������� ������������� �������� �����"
  };
  return ErrStr[ErrType];
}
