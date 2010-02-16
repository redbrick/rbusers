#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* max length of ctrl sequence */
#define RETLEN 6
#include "extra_io.h"





char *col_str(int colour) {
char *ret;

	if(!use_colour()) {
		return "";
	}

	/* allocate the return string */		
	if(!(ret=malloc(RETLEN))) {
		fprintf(stderr,"Not enough memory\n");
		exit(1);
	}
	
	/* write the return */
	snprintf(ret,RETLEN,"[%dm",colour);

return ret;
}


/* function a mangled version of stuff in:
   Beginning Linux Programming Second Edition, Neil Matthew
   and Richard Stones, ISBN: 1861002971

   bobb@redbrick.dcu.ie suggested I use it
   clever bobb  */      
int termios_getch() {
    int choice = 0;
    FILE *input;
    struct termios initial_settings, new_settings;

    /* open the tty  */
    input = fopen("/dev/tty", "r");
    if(!input) {
        fprintf(stderr, "Unable to open /dev/tty\n");
        exit(FATAL_ERROR);
    }

    /* remember current settings */
    tcgetattr(fileno(input),&initial_settings);

    /* change the settings */
    new_settings = initial_settings;

    /* I have *no idea how this works */
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_cc[VMIN] = 1;          
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_lflag &= ~ISIG;

    /* set these new settings */    
    if(tcsetattr(fileno(input), TCSANOW, &new_settings) != 0) {
        fprintf(stderr,"could not set attributes\n");
    }

    /* get the user input */
    choice=fgetc(input);

    /* bring back the original setting */
    tcsetattr(fileno(input),TCSANOW,&initial_settings);


    return choice;            
}
