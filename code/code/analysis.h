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
char tokType;		// ��� ���������� �������
char errType;		// ��� ������
bool err;			// ���� �� ������ � ���������

bool calculate(char *pStr, unsigned long *Result);
unsigned long expression(void);
unsigned long summand(void);
unsigned long multiplier(void);
void getToken(void);
void setErr(char Type);
char *errMessage(void);

/* 
����� ����� �����������. 
*/
bool calculate(char *pStr, unsigned long *result) {
	err = false;	// ���������� ���� ������
	pExpr = pStr;	// ����� ��������� �������� ����������� ���������
	getToken();		// �������� ������ �������
	*result = expression();	// ���������� �������� ���������
		
	if(*pExpr && !err) setErr(SYNTAX_ERR); // ���� ����� ���������� ��������� �� � ����� ������, ������ ��������� ������

	return !err;
}

/*
������� ���������� �������� ���������
*/
unsigned long expression(void) {
	unsigned long result = 1;
	char operat;
		
	if (!err) {	// ���� ������ ������
		result = summand();	// ������� �������� ������� ����������
		while ((operat = *token) == '+' || operat == '-'){
			getToken();
			switch(operat) {
				case '-':
					result = result - summand();
					break;
				case '+':
					result = result + summand();
					break;
			}
		}
	}
	return result;
}

/*
������� ���������� �������� ����������
*/
unsigned long summand(void) {
	unsigned long result = 1, temp;
	char operat;
	
	if (!err) {
		result = multiplier();	// �������� �������� ������� ���������
		while ((operat = *token) == '*' || operat == '/'){
			getToken();
			switch(operat) {
				case '*':
					result = result * multiplier();
					break;
				case '/':
					temp = multiplier();
					if (temp == 0) setErr(DIV_BY_ZERO);	// ���������� ������� �� 0
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
	char operat;
	
	if (!err){
		if (tokType == NUMBER) sscanf(token, "%x", &result);	// ���� ��� ���������� ������� - �����, �������� ��� ��������
		else if (*token == '(') {	// ���� ���������� ������� - ����������� ������
			getToken();
			result = expression();	// �������� �������� ��������� � �������
			if (!err && *token != ')') setErr(CLOSE_BR_EXP);	// ������� ����������� ������ ����� ���������
		}
		else if (tokType == IDENTIFIER){	// ���� ��� ������� - ������������� �������
			operat = *token;	// ���������� ������ ������ ��������� ��������
			getToken();
			if (*token == '(') {	// ����� ������� ��������� �������� ������� ������
				getToken();
				result = expression();	// �������� ��������� ������� ���������
				if (operat == 'N') result = ~result;	// ���������� �������� ��������� ��������
				else {	// ���� �������� �� NOT �������� ������ ��������
					if (*token == ',') { // ������� ����������� ����������
						getToken();
						switch(operat){
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
					else if (!err) setErr(DELIMITER_EXP); // ���� �� ���������� ������� ����� ������� ���������
				}
				if (!err && *token != ')') setErr(CLOSE_BR_EXP); // ������� ����������� ������ ����� ���������� �������
			}
			else setErr(OPEN_BR_EXP); // ���� �� ���������� ����������� ������
		}
		else setErr(SYNTAX_ERR);	// ���� ������� ��� �� ��� �� ����������
		getToken();
	}
	return result;
}

/* 
������� ���������� ��������� �������
*/
void getToken(void) {
	char *pChar;

	tokType = UNKNOWN;
	pChar = token;
	*pChar = '\0'; //����-�������������� ���������� ������

	if(!*pExpr || err) return; // ����� ��������� ��� ������
	/* ��������� ������ ���������� � ����������*/
	if(strchr("+-*/(),", *pExpr)){
		tokType = DELIMITER;
		*pChar++ = *pExpr++;	// ��������� � ���������� �������
	}
	/*��������� ��������� ��������*/
	else if (memcmp(pExpr, "OR", 2) == 0){
		for (int i = 0; i < 2; i++) *pChar++ = *pExpr++;
		tokType = IDENTIFIER;
	}
	else if (memcmp(pExpr, "AND", 3) == 0 || 
			memcmp(pExpr, "NOT", 3) == 0 ||
			memcmp(pExpr, "XOR", 3) == 0){
		for (int i = 0; i < 3; i++) *pChar++ = *pExpr++;
		tokType = IDENTIFIER;
	}
	/*��������� ����������������� �����*/
	else if(isxdigit(*pExpr)){
		while(isxdigit(*pExpr)) *pChar++ = *pExpr++;
		if (strlen(token) > 8) setErr(EXCESS_OF_THE_MAX); // ����� ��������� ����. ���. ��������
		else tokType = NUMBER;
	}
	/*��������� ������ � ���������*/
	else setErr(UNKNOWN_SYMB);

  *pChar = '\0'; //����-�������������� ���������� ������
}

/*
������� ������������� ���� � ��� ������
*/
void setErr(char Type) {
	err = true;
	errType = Type;
}

/*
������� ���������� ��������� �� ������ � ���������� �� ������
*/
char *errMessage(void) {
  static char *errStr[]= {
      "������������ ������ � ���������",
      "�������������� ������ � ���������",
      "��������� '('",
      "��������� ')'",
      "��������� ','",
      "������� �� ����!",
	  "���������� ������������� �������� �����"
  };
  return errStr[errType];
}
