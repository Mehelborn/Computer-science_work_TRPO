/*ћодуль значений констант дл€ анализатора*/

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

/*Error codes*/
#define UNKNOWN_SUMB			0
#define SUNTAX_ERR    		1
#define OPEN_BR_EXP    		2
#define CLOSE_BR_EXP			3
#define DELIMITER_EXP			4
#define DIV_BY_ZERO			5
#define EXCESS_OF_THE_MAX	6

/*Token types*/
#define UNKNOWN			7
#define DELIMITER		8
#define NUMBER				9
#define IDENTIFIER		10

/*Buffer lengths*/
#define MAX_S				255
#define SMALL_S			11

#endif
