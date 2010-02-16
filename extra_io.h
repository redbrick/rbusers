#ifndef _extra_io_h_
#define _extra_io_h_


/*
** File: utmp_lib.h
** Desc: a small library to provide some frequently required
**       io functions.
** Auth: Colm Mac Carthaigh, colmmacc@redbrick.dcu.ie
*/


/*
** PLEASE NOTE:
** the escpape code colours represented in this file
** work for most terminals, however telnet clients
** can map these escapes to different colours, bear this
** in mind when using this library.
*/

#include <stdlib.h>

/* 
** macros to start and end a colourmode 
** once you start colour mode all output is in that 
** colour until you start another colourmode or call
** ENDCOL();
*/

/* returned by termios_getch if there is a fatal error */
#define FATAL_ERROR 999

#define COLOUR(x);	printf("[");printf("%dm",x);
#define ENDCOL();	printf("[0m");


/* regular colours (send me more)*/
#define GREEN	32
#define RED	31
#define WHITE	1
#define BLACK	30
#define GREY	4
#define YELLOW 	33
#define BLUE	34
#define PURPLE	35
#define CYAN	36

/* there reversed counterparts */
#define REV_GREEN   42
#define REV_RED     41
#define REV_WHITE   7
#define REV_BLACK   1
#define REV_GREY    7
#define REV_YELLOW  43
#define REV_BLUE    44
#define REV_PURPLE  45
#define REV_CYAN    46 

/* default colour for terminal */
#define DEFAULTCOL  0


/* are we in a pipe, if so, return 0,
 if not return 1 */
#define use_colour()  (isatty(STDOUT_FILENO))



/*
** returns a colour escape sequence string for a given
** colour so you can: 
** printf("%s test%s\n",col_str(GREEN),col_str(DEFAULT))
*/
char *col_str(int);


/* 
** function a mangled version of stuff in:
** Beginning Linux Programming Second Edition, Neil Matthew
** and Richard Stones, ISBN: 1861002971
**
** bobb@redbrick.dcu.ie suggested I use it
** clever bobb  
**
** It acts like getc exept it doesnt echo input and
** doesnt need to wait for a new line 
*/
int termios_getch();

#endif 
