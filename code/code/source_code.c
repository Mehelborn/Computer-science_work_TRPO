/*
*	ѕрограмма шестнадцатеричный калькул€тор с инфиксной формой записи
*	считывает и вычисл€ет выражени€ из входного текстового файла, вида:
*	Х	беззнакова€ шестнадцатерична€ константа;
*	Х	(выражение)
*	Х	выражение + выражение
*	Х	выражение - выражение
*	Х	выражение * выражение
*	Х	выражение / выражение
*	Х	not(выражение) Ц операци€ побитовой инверсии
*	Х	and(выражение, выражение) Ц операци€ побитового »
*	Х	or(выражение, выражение) Ц операци€ побитового »Ћ»
*	Х	xor(выражение, выражение) Ц операци€ исключающего побитового »Ћ»
* 
*	–езультат сохран€ет в новом текстовом файле с окончанием "_res.txt".
* 
*	јвтор „ульдум —а€н јндреевич, 2021г.
*/

#include <stdio.h>
#include <conio.h>

#include "analysis.h" //подключаем модуль который выполн€ет синтаксический и лексический анализы


void getExpr(FILE *fp, char *pStr);

int main(void) {
	FILE *sourceFile, *resultFile;
	char fileName[GREAT_S], expr[GREAT_S];
	unsigned long result;

	//открываем исходный файл
	printf("Enter the file name: ");
	scanf("%s", fileName);
	if ((sourceFile = fopen(fileName,"r")) == NULL) {
		printf("ERROR. Cannot open the file.");
		getchar();
		return 0;
	}
	
	strcat(fileName, "_res.txt"); // им€ файла результата
	
	//открываем файл дл€ записи результата
	if ((resultFile = fopen(fileName,"w")) == NULL) {
		printf("ERROR. Cannot open/create the result file.");
		getchar();
		fclose(sourceFile);
		return 0;
	}

	while (!feof(sourceFile)) {
		getExpr(sourceFile, expr);	// получаем строку с выражением
		if (!*expr) continue;		// если строка пуста€ - не выполн€ем никаких действий
		fprintf(resultFile, "%s = ", expr); // печатаем строку с выражением в файл
		if (calculate(expr, &result)) fprintf(resultFile, "%X\n", result); // если вычисление прошло без ошибок записываем ответ
		else fprintf(resultFile, "ERROR! %s\n", errorMessage()); // иначе записываем сообщение об ошибке
	}

	fclose(sourceFile);
	fclose(resultFile);
	printf("The calculation results are written to file %s", fileName);
	
	getchar();
	return 1;
}

/*
‘ункци€ пропускает лишние пробелы, 
переводит буквы в верхний регистр 
и присваивает видоизмененную строку 
переменной expr в main
*/
void getExpr(FILE *fp, char *pStr) {
	int ch; //текущий символ
	char *pChar;
	
	pChar = pStr;
	while ((ch = getc(fp)) != '\n' && ch != EOF) { // читаем символы пока не встретим перевод строки, или конец файла
		if (isspace(ch)) continue;	// пропускаем пробельные символы
		*pChar = toupper(ch);	// переводим буквенные символы в верхний регистр
		pChar++;
	}
	*pChar = '\0'; //нуль-терминирование символьной строки
}
