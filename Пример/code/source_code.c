/*ѕрограмма шестнадцатеричный калькул€тор с инфиксной формой записи
считывает и вычисл€ет выражени€ из входного текстового файла, вида:
Х	беззнакова€ шестнадцатерична€ константа;
Х	(выражение)
Х	выражение + выражение
Х	выражение - выражение
Х	выражение * выражение
Х	выражение / выражение
Х	not(выражение) Ц операци€ побитовой инверсии
Х	and(выражение, выражение) Ц операци€ побитового »
Х	or(выражение, выражение) Ц операци€ побитового »Ћ»
Х	xor(выражение, выражение) Ц операци€ исключающего побитового »Ћ»
–езультат сохран€ет в новом текстовом файле.

јвтор ≈рмошенко ≈вгений, 2013г.*/

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
	//открываем исходный файл
	puts("Enter the file name of the form c:\\\\example.txt:");
	gets(FileName);
	if ((FileIn = fopen(FileName,"r")) == NULL) {
		puts("Opening file to read error.");
		getchar();
		return 0;
	}
	
	strcat(FileName, "_res.txt"); // им€ файла результата
	
	//открываем файл дл€ записи результата
	if ((FileOut = fopen(FileName,"w")) == NULL) {
		puts("Opening file to write error.");
		getchar();
		fclose(FileIn);
		return 0;
	}

	while (!feof(FileIn)) {
		GetExpr(FileIn, Expr);	// получаем строку с выражением
		if (!*Expr) continue;		// если строка пуста€ - не выполн€ем никаких действий
		fprintf(FileOut, "%s = ", Expr); // печатаем строку с выражением в файл
		if (Calculate(Expr, &Result)) fprintf(FileOut, "%X\n", Result); // если вычисление прошло без ошибок записываем ответ
		else fprintf(FileOut, "ќшибка! %s\n", ErrMessage()); // иначе записываем сообщение об ошибке
	}

	fclose(FileIn);
	fclose(FileOut);
	printf("The calculation results are written to file %s", FileName);
	getchar();

  return 1;
}

/*‘ункци€ возвращает указатель на строку, прочитанную из файла, из которой
удалены пробельные символы и буквы приведены к верхнему регистру*/
void GetExpr(FILE *fp, char *pStr)
{
	int ch;
	char *pChar;
	
	pChar = pStr;
	while ((ch = getc(fp)) != '\n' && ch != EOF) { // читаем символы пока не встретим перевод строки, или конец файла
		if (isspace(ch)) continue;	// пропускаем пробельные символы
		*pChar = toupper(ch);	// переводим буквенные символы в верхний регистр
		pChar++;
	}
	*pChar = '\0';
}
