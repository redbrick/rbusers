#include "extra_io.h"
#include "utmp_lib.h"
#include "alpha.h"
#include <stdlib.h>
#include <pwd.h>

void print_username(struct utmp_list *,char **,int);

int main(int argc, char **argv) {
struct utmp_list *u,*list_entry,*first_entry;
int users=0,i,j,h,fr_num=0;
char **friends=NULL,friendsfile[128]="",userbuffer[10]="";
char tty[16];
FILE *in;
time_t t;

	
	snprintf(friendsfile,128,"%s/.friends",getenv("HOME"));
	if((in=fopen(friendsfile,"r"))) {
		while(!feof(in)) {
			fgets(userbuffer,10,in);
			fr_num++;
			if(!(friends=realloc(friends,fr_num*sizeof(char *)))||
			   !(friends[fr_num-1]=malloc(9*sizeof(char)))) {
				fprintf(stderr,"not enough memory, exiting\n");
				exit(1);
			}

			/* get the locatation of the endline(if any) */
			for(i=0;i<10;i++) {
				if(userbuffer[i]=='\n'||userbuffer[i]==' ') {
					userbuffer[i]='\0';
				}
			} 
			userbuffer[9]='\0';
			
			strncpy(friends[fr_num-1],userbuffer,i);
			memset(userbuffer,'\0',9);
		}
	}


	/* get the alphabetical list */
	first_entry=order_a2z_utmp_list();

	/* count how many users are on */
	list_entry=first_entry;
	while(list_entry!=NULL) {
		users++;
		list_entry=list_entry->next;
	}	

	/* print number of users online */
	printf(	"                         "
		"%sTotal %sNumber %sof %sUsers %sonline%s:%d\n\n",
		col_str(RED),col_str(GREEN),col_str(YELLOW),
		col_str(BLUE),col_str(PURPLE),col_str(DEFAULTCOL),users);


	/* print the legend */
	printf(	"                   "
		"%s %s = %sfriends%s                %s %s = %scommittee members%s\n\n",
		col_str(REV_WHITE),col_str(DEFAULTCOL),col_str(WHITE),col_str(DEFAULTCOL),
		col_str(REV_RED),col_str(DEFAULTCOL),col_str(RED),col_str(DEFAULTCOL)); 

	/* print the users */
	list_entry=first_entry;
	i=0;
	printf("    ");
	while(list_entry!=NULL) {
		i++;
		print_username(list_entry,friends,fr_num);
		if((i%5)==0&&list_entry->next!=NULL) {
			printf("\n    ");
		}
		list_entry=list_entry->next;
	}
	printf("\n\n");

return 0;
}

/*
** prints out a 14 character username and session number string
*/
void print_username(struct utmp_list *user,char **friends,int fr_num) {
char sessions[3],username[9],spacing[12]="            ";
int i,fr_col,j;
struct passwd *p;

	/* get the username */
	snprintf(username,9,"%s",user->user_name);

	/* get the number of sessions */
	snprintf(sessions,3,"%d",user->tty_num);

	/* get the spacing required */
	spacing[10-(strlen(username)+strlen(sessions))]='\0';

	fr_col=CYAN;
	for(i=0;i<fr_num;i++) {
		if(!strncmp(friends[i],user->user_name,8)) {
			fr_col=DEFAULTCOL;
			break;
		}
	}

	if((p=getpwnam(username))) {
               	if(strstr(p->pw_dir,"committe")) {
                     fr_col=RED;
                }
	}


	/* print the string */
	printf("  %s%s%s%s(%s%s%s)%s",col_str(fr_col),username,col_str(GREEN),spacing,
		col_str(YELLOW),sessions,col_str(GREEN),col_str(DEFAULTCOL));

}

