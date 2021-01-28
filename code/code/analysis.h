/*
*	������ ��������� ����������� � �������������� ������ ��������� 
*	� ����������������� ������� ����������, ����:
*
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
*	�� ������� ��������� pExpr � ���������� ����������.
* 
*	����� ������� ���� ���������, 2021�.
*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "consts.h" //������ ���������� �������

char *pExpr;		// ��������� �� ������������� ���������
char token[GREAT_S];	// ����� ��� ���������� �������
char tokenType;		// ��� ���������� �������
char errorType;		// ��� ������
bool error;			// ���� �� ������ � ���������

bool calculate(char *pStr, unsigned long *Result);
unsigned long expression(void);
unsigned long summation(void);
unsigned long multiplier(void);
void getToken(void);
void seterror(char Type);
char *errorMessage(void);

/* 
����� ����� �����������. 
*/
bool calculate(char *pStr, unsigned long *result) {
	error = false;	// ���������� ���� ������
	pExpr = pStr;	// ����� ��������� �������� ����������� ���������
	getToken();		// �������� ������ �������
	*result = expression();	// ���������� �������� ���������
		
	if(*pExpr && !error) seterror(SYNTAX_ERR); // ���� ����� ���������� ��������� �� � ����� ������, ������ ��������� ������

	return !error;
}

/*
������� ���������� �������� ���������
*/
unsigned long expression(void) {
	unsigned long result = 1;
	char operation;
		
	if (!error) {	// ���� ������ ������
		result = summation();	// ������� �������� ������� ����������
		while ((operation = *token) == '+' || operation == '-'){
			getToken();
			switch(operation) {
				case '-':
					result = result - summation();
					break;
				case '+':
					result = result + summation();
					break;
			}
		}
	}
	return result;
}

/*
������� ���������� �������� ����������
*/
unsigned long summation(void) {
	unsigned long result = 1, temp;
	char operation;
	
	if (!error) {
		result = multiplier();	// �������� �������� ������� ���������
		while ((operation = *token) == '*' || operation == '/'){
			getToken();
			switch(operation) {
				case '*':
					result = result * multiplier();
					break;
				case '/':
					temp = multiplier();
					if (temp == 0) seterror(DIV_BY_ZERO);	// ���������� ������� �� 0
					else result = result / temp;
					break;
			}
		}
	}
	return result;
}

/*
������� ���������� �������� ���������
*/
unsigned long multiplier(void) {
	unsigned long result = 1;
	char operation;
	
	if (!error){
		if (tokenType == NUMBER) sscanf(token, "%x", &result);	// ���� ��� ���������� ������� - �����, �������� ��� ��������
		else if (*token == '(') {	// ���� ���������� ������� - ����������� ������
			getToken();
			result = expression();	// �������� �������� ��������� � �������
			if (!error && *token != ')') seterror(CLOSE_BR_EXP);	// ������� ����������� ������ ����� ���������
		}
		else if (tokenType == IDENTIFIER){	// ���� ��� ������� - ������������� �������
			operation = *token;	// ���������� ������ ������ ��������� ��������
			getToken();
			if (*token == '(') {	// ����� ������� ��������� �������� ������� ������
				getToken();
				result = expression();	// �������� ��������� ������� ���������
				if (operation == 'N') result = ~result;	// ���������� �������� ��������� ��������
				else {	// ���� �������� �� NOT �������� ������ ��������
					if (*token == ',') { // ������� ����������� ����������
						getToken();
						switch(operation){
							case 'A':	// ��������� �
								result = result & expression();
								break;
							case 'O':	// ��������� ���
								result = result | expression();
								break;
							case 'X':	// ��������� ����������� ���
								result = result ^ expression();
								break;
						}
					}	
					else if (!error) seterror(DELIMITER_EXP); // ���� �� ���������� ������� ����� ������� ���������
				}
				if (!error && *token != ')') seterror(CLOSE_BR_EXP); // ������� ����������� ������ ����� ���������� �������
			}
			else seterror(OPEN_BR_EXP); // ���� �� ���������� ����������� ������
		}
		else seterror(SYNTAX_ERR);	// ���� ������� ��� �� ��� �� ����������
		getToken();
	}
	return result;
}

/* 
������� ���������� ��������� �������
*/
void getToken(void) {
	char *pChar;

	tokenType = UNKNOWN;
	pChar = token;
	*pChar = '\0'; //����-�������������� ���������� ������

	if(!*pExpr || error) return; // ����� ��������� ��� ������
	/* ��������� ������ ���������� � ����������*/
	if(strchr("+-*/(),", *pExpr)){
		tokenType = DELIMITER;
		*pChar++ = *pExpr++;	// ��������� � ���������� �������
	}
	/*��������� ��������� ��������*/
	else if (memcmp(pExpr, "OR", 2) == 0){
		for (int i = 0; i < 2; i++) *pChar++ = *pExpr++;
		tokenType = IDENTIFIER;
	}
	else if (memcmp(pExpr, "AND", 3) == 0 || 
			memcmp(pExpr, "NOT", 3) == 0 ||
			memcmp(pExpr, "XOR", 3) == 0){
		for (int i = 0; i < 3; i++) *pChar++ = *pExpr++;
		tokenType = IDENTIFIER;
	}
	/*��������� ����������������� �����*/
	else if(isxdigit(*pExpr)){
		while(isxdigit(*pExpr)) *pChar++ = *pExpr++;
		if (strlen(token) > 8) seterror(EXCESS_OF_THE_MAX); // ����� ��������� ����. ���. ��������
		else tokenType = NUMBER;
	}
	/*��������� ������ � ���������*/
	else seterror(UNKNOWN_SYMB);

  *pChar = '\0'; //����-�������������� ���������� ������
}

/*
������� ������������� ���� � ��� ������
*/
void seterror(char Type) {
	error = true;
	errorType = Type;
}

/*
������� ���������� ��������� �� ������ � ���������� �� ������
*/
char *errorMessage(void) {
  static char *errorStr[]= {
      "������������ ������ � ���������",
      "�������������� ������ � ���������",
      "��������� '('",
      "��������� ')'",
      "��������� ','",
      "������� �� ����!",
	  "���������� ������������� �������� �����"
  };
  return errorStr[errorType];
}
