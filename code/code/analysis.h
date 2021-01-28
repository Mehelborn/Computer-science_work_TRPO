/*
*	Модуль выполняет лексический и синтаксический анализ выражения 
*	в шестнадцатеричной системе исчисления, вида:
*
*	•	беззнаковая шестнадцатеричная константа;
*	•	(выражение)
*	•	выражение + выражение
*	•	выражение - выражение
*	•	выражение * выражение
*	•	выражение / выражение
*	•	not(выражение) – операция побитовой инверсии
*	•	and(выражение, выражение) – операция побитового И
*	•	or(выражение, выражение) – операция побитового ИЛИ
*	•	xor(выражение, выражение) – операция исключающего побитового ИЛИ
*
*	на которое указывает pExpr и производит вычисление.
* 
*	Автор Чульдум Саян Андреевич, 2021г.
*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "consts.h" //модуль постоянных величин

char *pExpr;		// указатель на анализируемое выражение
char token[GREAT_S];	// буфер для выделенной лексемы
char tokenType;		// тип выделенной лексемы
char errorType;		// тип ошибки
bool error;			// флаг об ошибке в выражении

bool calculate(char *pStr, unsigned long *Result);
unsigned long expression(void);
unsigned long summation(void);
unsigned long multiplier(void);
void getToken(void);
void seterror(char Type);
char *errorMessage(void);

/* 
Точка входа анализатора. 
*/
bool calculate(char *pStr, unsigned long *result) {
	error = false;	// сбрасываем флаг ошибки
	pExpr = pStr;	// адрес выражения передаем глобальному указателю
	getToken();		// получаем первую лексему
	*result = expression();	// определяем значение выражения
		
	if(*pExpr && !error) seterror(SYNTAX_ERR); // если после вычисления указатель не в конце строки, значит произошла ошибка

	return !error;
}

/*
Функция возвращает значение выражения
*/
unsigned long expression(void) {
	unsigned long result = 1;
	char operation;
		
	if (!error) {	// если небыло ошибки
		result = summation();	// находим значение первого слагаемого
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
Функция возвращает значение слагаемого
*/
unsigned long summation(void) {
	unsigned long result = 1, temp;
	char operation;
	
	if (!error) {
		result = multiplier();	// получаем значение первого множителя
		while ((operation = *token) == '*' || operation == '/'){
			getToken();
			switch(operation) {
				case '*':
					result = result * multiplier();
					break;
				case '/':
					temp = multiplier();
					if (temp == 0) seterror(DIV_BY_ZERO);	// исключение деления на 0
					else result = result / temp;
					break;
			}
		}
	}
	return result;
}

/*
Функция возвращает значение множителя
*/
unsigned long multiplier(void) {
	unsigned long result = 1;
	char operation;
	
	if (!error){
		if (tokenType == NUMBER) sscanf(token, "%x", &result);	// если тип полученной лексемы - число, получаем его значение
		else if (*token == '(') {	// если полученная лексема - открывающая скобка
			getToken();
			result = expression();	// получаем значение выражения в скобках
			if (!error && *token != ')') seterror(CLOSE_BR_EXP);	// ожидаем закрывающую скобку после выражения
		}
		else if (tokenType == IDENTIFIER){	// если тип лексемы - идентификатор функции
			operation = *token;	// запоминаем первый символ побитовой операции
			getToken();
			if (*token == '(') {	// после лексемы побитовой операции ожидаем скобку
				getToken();
				result = expression();	// получаем результат первого аргумента
				if (operation == 'N') result = ~result;	// выполнение операции побитовой инверсии
				else {	// если операция не NOT получаем второй аргумент
					if (*token == ',') { // ожидаем разделитель аргументов
						getToken();
						switch(operation){
							case 'A':	// побитовое И
								result = result & expression();
								break;
							case 'O':	// побитовое ИЛИ
								result = result | expression();
								break;
							case 'X':	// побитовое исключающее ИЛИ
								result = result ^ expression();
								break;
						}
					}	
					else if (!error) seterror(DELIMITER_EXP); // если не обнаружили запятую после первого аргумента
				}
				if (!error && *token != ')') seterror(CLOSE_BR_EXP); // ожидаем закрывающую скобку после аргументов функции
			}
			else seterror(OPEN_BR_EXP); // если не обнаружили открывающую скобку
		}
		else seterror(SYNTAX_ERR);	// если лексема или ее тип не определена
		getToken();
	}
	return result;
}

/* 
Функция возвращает очередную лексему
*/
void getToken(void) {
	char *pChar;

	tokenType = UNKNOWN;
	pChar = token;
	*pChar = '\0'; //нуль-терминирование символьной строки

	if(!*pExpr || error) return; // конец выражения или ошибка
	/* выделение знаков пунктуации и операторов*/
	if(strchr("+-*/(),", *pExpr)){
		tokenType = DELIMITER;
		*pChar++ = *pExpr++;	// переходим к следующему символу
	}
	/*выделение побитовых операций*/
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
	/*выделение шестнадцатеричных чисел*/
	else if(isxdigit(*pExpr)){
		while(isxdigit(*pExpr)) *pChar++ = *pExpr++;
		if (strlen(token) > 8) seterror(EXCESS_OF_THE_MAX); // число превышает макс. доп. значение
		else tokenType = NUMBER;
	}
	/*ошибочный символ в выражении*/
	else seterror(UNKNOWN_SYMB);

  *pChar = '\0'; //нуль-терминирование символьной строки
}

/*
Функция устанавливает флаг и тип ошибки
*/
void seterror(char Type) {
	error = true;
	errorType = Type;
}

/*
Функция возвращает указатель на строку с сообщением об ошибке
*/
char *errorMessage(void) {
  static char *errorStr[]= {
      "Недопустимый символ в выражении",
      "Синтаксическая ошибка в выражении",
      "Ожидается '('",
      "Ожидается ')'",
      "Ожидается ','",
      "Деление на ноль!",
	  "Превышение максимального значения числа"
  };
  return errorStr[errorType];
}
