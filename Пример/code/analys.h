/*Модуль выполняет лексический и синтаксический анализ выражения в шестнадцатеричной системе исчисления, вида:

•	беззнаковая шестнадцатеричная константа;
•	(выражение)
•	выражение + выражение
•	выражение - выражение
•	выражение * выражение
•	выражение / выражение
•	not(выражение) – операция побитовой инверсии
•	and(выражение, выражение) – операция побитового И
•	or(выражение, выражение) – операция побитового ИЛИ
•	xor(выражение, выражение) – операция исключающего побитового ИЛИ

на которое указывает pExpr и производит вычисление.
Автор Ермошенко Евгений, 2013г.*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "constants.h"

char *pExpr;		// указатель на анализируемое выражение
char Token[SMALL_S];	// буфер для выделенной лексемы
char TokType;		// тип выделенной лексемы
char ErrType;		// тип ошибки
bool Err;			// флаг об ошибке в выражении

bool Calculate(char *pStr, unsigned long *Result);
unsigned long Expression(void);
unsigned long Summand(void);
unsigned long Multiplier(void);
void GetToken(void);
void SetErr(char Type);
char *ErrMessage(void);

/* Точка входа анализатора. */
bool Calculate(char *pStr, unsigned long *Result)
{
	Err = false;	// сбрасываем флаг ошибки
	pExpr = pStr;	// адрес выражения передаем глобальному указателю
	GetToken();		// получаем первую лексему
	*Result = Expression();	// определяем значение выражения
		
	if(*pExpr && !Err) SetErr(SUNTAX_ERR); // если после вычисления указатель не в конце строки, значит произошла ошибка

	return !Err;
}

/*Функция возвращает значение выражения*/
unsigned long Expression(void)
{
	unsigned long Result = 1;
	char Operat;
		
	if (!Err) {	// если небыло ошибки
		Result = Summand();	// находим значение первого слагаемого
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

/*Функция возвращает значение слагаемого*/
unsigned long Summand(void)
{
	unsigned long Result = 1, Temp;
	char Operat;
	
	if (!Err) {
		Result = Multiplier();	// получаем значение первого множителя
		while ((Operat = *Token) == '*' || Operat == '/'){
			GetToken();
			switch(Operat) {
				case '*':
					Result = Result * Multiplier();
					break;
				case '/':
					Temp = Multiplier();
					if (Temp == 0) SetErr(DIV_BY_ZERO);	// исключение деления на 0
					else Result = Result / Temp;
					break;
			}
		}
	}
	return Result;
}

/*Функция возвращает значение множителя*/
unsigned long Multiplier(void)
{
	unsigned long Result = 1;
	char Operat;
	
	if (!Err){
		if (TokType == NUMBER) scanf(Token, "%x", &Result);	// если тип полученной лексемы - число, получаем его значение
		else if (*Token == '(') {	// если полученная лексема - открывающая скобка
			GetToken();
			Result = Expression();	// получаем значение выражения в скобках
			if (!Err && *Token != ')') SetErr(CLOSE_BR_EXP);	// ожидаем закрывающую скобку после выражения
		}
		else if (TokType == IDENTIFIER){	// если тип лексемы - идентификатор функции
			Operat = *Token;	// запоминаем первый символ побитовой операции
			GetToken();
			if (*Token == '(') {	// после лексемы побитовой операции ожидаем скобку
				GetToken();
				Result = Expression();	// получаем результат первого аргумента
				if (Operat == 'N') Result = ~Result;	// выполнение операции побитовой инверсии
				else {	// если операция не NOT получаем второй аргумент
					if (*Token == ',') { // ожидаем разделитель аргументов
						GetToken();
						switch(Operat){
							case 'A':	// побитовое И
								Result = Result & Expression();
								break;
							case 'O':	// побитовое ИЛИ
								Result = Result | Expression();
								break;
							case 'X':	// побитовое исключающее ИЛИ
								Result = Result ^ Expression();
								break;
						}
					}	
					else if (!Err) SetErr(DELIMITER_EXP); // если не обнаружили запятую после первого аргумента
				}
				if (!Err && *Token != ')') SetErr(CLOSE_BR_EXP); // ожидаем закрывающую скобку после аргументов функции
			}
			else SetErr(OPEN_BR_EXP); // если не обнаружили открывающую скобку
		}
		else SetErr(SUNTAX_ERR);	// если лексема или ее тип не определена
		GetToken();
	}
	return Result;
}

/* Функция возвращает очередную лексему */
void GetToken(void)
{
	char *pChar;

	TokType = UNKNOWN;
	pChar = Token;
	*pChar = '\0';

	if(!*pExpr || Err) return; // конец выражения или ошибка
	/* выделение знаков пунктуации и операторов*/
	if(strchr("+-*/(),", *pExpr)){
		TokType = DELIMITER;
		*pChar++ = *pExpr++;	// переходим к следующему символу
	}
	/*выделение побитовых операций*/
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
	/*выделение шестнадцатеричных чисел*/
	else if(isxdigit(*pExpr)){
		while(isxdigit(*pExpr)) *pChar++ = *pExpr++;
		if (strlen(Token) > 8) SetErr(EXCESS_OF_THE_MAX); // число превышает макс. доп. значение
		else TokType = NUMBER;
	}
	/*ошибочный символ в выражении*/
	else SetErr(UNKNOWN_SUMB);

  *pChar = '\0';
}

/*Функция устанавливает флаг и тип ошибки*/
void SetErr(char Type)
{
	Err = true;
	ErrType = Type;
}

/*Функция возвращает указатель на строку с сообщением об ошибке*/
char *ErrMessage(void)
{
  static char *ErrStr[]= {
      "Недопустимый символ в выражении",
      "Синтаксическая ошибка в выражении",
      "Ожидается '('",
      "Ожидается ')'",
      "Ожидается ','",
      "Деление на ноль!",
	  "Превышение максимального значения числа"
  };
  return ErrStr[ErrType];
}
