/*
** File: utmp_lib.c
** Desc: a small library to get as much info on a logged-in user
**	 as possible
** Auth: Colm Mac Carthaigh, colmmacc@redbrick.dcu.ie
*/

/* With: */

/*
** File: utent.c
** Desc: A replacement for the *ut* functions not provided by FreeBSD
** Auth: Cian Synnott, pooka@redbrick.dcu.ie
** Date: Thu May  6 15:43:54 IST 1999
*/

#include "utmp_lib.h"


#ifndef HAVE_GETUTENT 

static int utfile;

void setutent(void) {
	if ((utfile = open(_PATH_UTMP, O_RDONLY)) < 0) {	
		printf("Argh. Couldn't open %s, for some reason.\n", _PATH_UTMP);
		exit(1);
	}
}

struct utmp *getutent(void) {
	static struct utmp u;
	int ret;

	ret = read(utfile, (void *)&u, sizeof(struct utmp));
	switch(ret) {
		case -1: 
			printf("Argh. Error reading from %s, for some reason.\n", _PATH_UTMP);
			break;
		case 0:
			return NULL;
			break;
	}
	
	return &u;

}

void endutent(void) {
	close(utfile);
}

#endif

/*
** cleanup host field
*/
static void chomp_host(char *string);

struct utmp_list *get_utmp_username(char *username) {
struct utmp_list *u;	
struct utmp *utmp;
char filename[FILENAME_MAX];
struct stat s;

	/* allocate the space for the structure */
	if(!(u=(struct utmp_list *)malloc(sizeof(struct utmp_list)))) {
		fprintf(stderr,"not enough memory, exiting!\n");
		exit(1);
	}

	/* set pointers to NULL so we can use realloc */
	u->user_name=NULL;
	u->user_ttys=NULL;
	u->ttys_mesg=NULL;
	u->ttys_active=NULL;
	u->ttys_login=NULL;
	
	/* no tty's yet */
	u->tty_num=0;

	/* fill the username field */
	u->user_name=(char *)malloc((strlen(username)+1)*sizeof(char));
	strncpy(u->user_name,username,strlen(username)+1);	

	/* open the utmp file */
	setutent();
	
	/* now search through the logged in users */
	while((utmp = getutent())!=NULL) {
		if(!strcmp(utmp->ut_name,username) && utmp->ut_type==USER_PROCESS ) {
			/* we have a new session for username */
			u->tty_num++;

			/* realloc the arrays */
			if(!(u->user_ttys=(char **)realloc(u->user_ttys,u->tty_num*sizeof(char *)))||
			   !(u->ttys_mesg=(int *)realloc(u->ttys_mesg,u->tty_num*sizeof(int)))||
			   !(u->ttys_active=(time_t*)realloc(u->ttys_active,u->tty_num*sizeof(time_t)))||
			   !(u->ttys_login=(time_t*)realloc(u->ttys_login,u->tty_num*sizeof(time_t)))) {
				fprintf(stderr,"not enough memory, exiting!\n");
				exit(1);
			}

			/* allocate space for values */
			if(!(u->user_ttys[u->tty_num-1]=(char *)malloc((strlen(utmp->ut_line)+1)*sizeof(char)))) {
				fprintf(stderr,"not enough memory,exiting\n");
				exit(1);
			}

			/* resolve those values */
			strcpy(u->user_ttys[u->tty_num-1],utmp->ut_line);

                        /* get login time */
                        u->ttys_login[u->tty_num-1]=utmp->ut_time;   

			/* resolve the mesg status */
			
			/* get the tty filename */
			snprintf(filename,FILENAME_MAX,"/dev/%s",utmp->ut_line);

			/* can we write to tty */
			if (stat(filename, &s) == -1) {
				fprintf(stderr,"Woah. Error statting %s.\n",filename);
 			}
			if (s.st_mode & S_IWGRP) {
				u->ttys_mesg[u->tty_num-1] = 1;
			}
			else {
				u->ttys_mesg[u->tty_num-1] = 0;
			}   
                        /* get the idle time */
			u->ttys_active[u->tty_num-1]=s.st_atime;   
		}
	}
	
	/* we're finished with the utmp file */
	endutent();

	/* we're not part of a list */
	u->prev=NULL;
	u->next=NULL;

return u;
}

struct utmp_list *get_utmp_list() {
struct utmp_list *current=NULL,*prev=NULL,*next=NULL,*tempprev=NULL,*ret=NULL;
struct utmp *utmp;
char filename[FILENAME_MAX];
struct stat s;

	/* open the utmp file */
	setutent();
	while((utmp=getutent())!=NULL) {
		/* check to see if it's a user  */
		if(strcmp(utmp->ut_name,"") && utmp->ut_type == USER_PROCESS) {
			/* check to see if we already encountered that user */
			current=NULL;
			tempprev=prev;
			while(tempprev!=NULL) {
				if(!strncmp(utmp->ut_name,tempprev->user_name,8)) {
					current=tempprev;
				}
				tempprev=tempprev->prev;
			}
			if(current==NULL) {
			        /* allocate the space for the entry */
				if(!(current=(struct utmp_list *)malloc(sizeof(struct utmp_list)))) {
					fprintf(stderr,"Not enough memory to run");
					exit(1);
				}
				/* set pointers to NULL so we can use realloc */
				current->user_name=NULL;
				current->user_ttys=NULL;
				current->ttys_mesg=NULL;
				current->ttys_active=NULL;
				current->ttys_login=NULL;

				/* tell it where prev was */
				current->prev=prev;
				
				/* tell the previous one where we are */
				if(prev!=NULL) {
					prev->next=current;
				}

				/* decide if current is the return */			
				if(prev==NULL) {
					ret=current;
				}

				/* We're the last one for now  */
				current->next=NULL;

				/* specify username */
				if(!(current->user_name=(char*)malloc((strlen(utmp->ut_name)+1)*sizeof(char)))) {
					fprintf(stderr,"not enough memory to run\n");
					exit(1);
				}
				strcpy(current->user_name,utmp->ut_name);
				
				/* no tty's yet */
				current->tty_num=0;

				/* reset prev */
				prev=current;
			}

			/* we have a new session for username */
			current->tty_num++;

			/* realloc the arrays */
			if(!(current->user_ttys=(char**)realloc(current->user_ttys,current->tty_num*sizeof(char*)))||
			   !(current->ttys_mesg=(int *)realloc(current->ttys_mesg,current->tty_num*sizeof(int)))||
 			   !(current->ttys_login=(time_t *)realloc(current->ttys_login,current->tty_num*sizeof(time_t)))||
			   !(current->ttys_active=(time_t *)realloc(current->ttys_active,current->tty_num*sizeof(time_t)))){
				fprintf(stderr,"not enough memory, exiting!\n");
				exit(1);
			}

			/* allocate space for values */
			if(!(current->user_ttys[current->tty_num-1]=(char *)malloc((strlen(utmp->ut_line)+1)*sizeof(char)))) {
				fprintf(stderr,"not enough memory,exiting\n");
				exit(1);
			}

			/* resolve those values */
			strcpy(current->user_ttys[current->tty_num-1],utmp->ut_line);

			/* get login time */			
			current->ttys_login[current->tty_num-1]=utmp->ut_time;

			/* resolve the mesg status */
			
			/* get the tty filename */
			snprintf(filename,FILENAME_MAX,"/dev/%s",utmp->ut_line);

			/* can we write to tty */
			if (stat(filename, &s) == -1) {
				fprintf(stderr,"Woah. Error statting %s.\n",filename);
 			}
			if (s.st_mode & S_IWGRP) {
				current->ttys_mesg[current->tty_num-1] = 1;
			}
			else {
				current->ttys_mesg[current->tty_num-1] = 0;
			}   

			/* get the idle time */	
			current->ttys_active[current->tty_num-1]=s.st_atime;
		}
	}
	endutent();

		
return ret;
}


/*
 * sometimes getutent isnt fully clean, this function
 * helps get rid of stuff that shouldnt be in the host 
 * field
 */
void chomp_host(char *string) {
int ret,i;

	for(i=0;i<strlen(string);i++) {
		/* this makes sure we have no controls or other
		   nasty characters */
		if(string[i]<'!'||string[i]>'~') {
			string[i]='\0';
			break;
		}
		
		/* now get rid of stuff that should be there! */
		if(string[i]==':') {
			string[i]='\0';
			break;
		}
	}
}


/*
** Frees a utmp_list(or entry)
*/
void free_utmp(struct utmp_list *user) {
int i;

	/* o.k. see if we really are the first user, if not navigate 
	   all the way back to the first */
	if(user->prev!=NULL) {
		while(user->prev!=NULL) {
			user=user->prev;
		}
	}


	/* cycle thru the users */
	while(user!=NULL) {
		
		/* free all the 2d arrays */
		for(i=0;i<user->tty_num;i++) {
			free(user->user_ttys[i]);
		}
		free(user->user_ttys);

		/* free the rest */
		free(user->ttys_mesg);
		free(user->ttys_active);
		free(user->ttys_login);
		free(user->user_name);
	
		/* get next pointer */
		user=user->next;

		/* free the list pointers */
		free(user);

	}

}


/*
** moves src to before dest
*/
void move_utmp_list_entry_b(struct utmp_list *src, struct utmp_list *dest) {

	/* tell src's neighbours where they are */
	if(src->prev) {
		src->prev->next=src->next;
	}
	if(src->next) {
		src->next->prev=src->prev;
	}

	/* tell src where it's new neighbours are */
	src->next=dest;
	src->prev=dest->prev;

	/* tell dests prev neighbour where we are  */
	if(dest->prev) {
		dest->prev->next=src;
	}

	/* tell dest where src is */
	dest->prev=src;
}

/*
** moves src to after dest
*/
void move_utmp_list_entry_a(struct utmp_list *src, struct utmp_list *dest) {

	/* tell src's neighbours where they are */
	if(src->prev) {
		src->prev->next=src->next;
	}
	if(src->next) {
		src->next->prev=src->prev;
	}

	/* tell src where it's new neighbours are */
	src->prev=dest;
	src->next=dest->next;

	/* tell dests prev neighbour where we are  */
	if(dest->next) {
		dest->next->prev=src;
	}

	/* tell dest where src is */
	dest->next=src;
}



/*
** deletes an entry
*/
void delete_utmp_list_entry(struct utmp_list *user) {

	/* tell user's neighbours where they are */
	if(user->prev) {
		user->prev->next=user->next;
	}
	if(user->next) {
		user->next->prev=user->prev;
	}

	/* Make users pointers NULL so we can free it */	
	user->prev=NULL;
	user->next=NULL;

	/* free the memory */
	free_utmp(user);	

}


/*
** sorting function for the list. Pass it a pointer
** to the list and a pointer to the comparison
** function, read the header for a better description
*/
struct utmp_list *utmp_list_sort(struct utmp_list *start, int (*compare)(struct utmp_list *,struct utmp_list*)) {
struct utmp_list *insertpoint,*insert,*test,*first;

	/* get the unordered list */
	insertpoint=start;
	first=start;

	while(insertpoint!=NULL) {

		/*
		** get the lowest in list between insertpoint
                ** and end of list 
		*/
        	test=insertpoint;
		insert=insertpoint;
	        while(test!=NULL) {
        	        if(compare(insert,test)>0) {
                        	insert=test;
                	}
                	test=test->next;
        	}                         
		/* isert is now the lowest */

		/* now move it before the insertpoint */
		if(insert!=insertpoint) {
			move_utmp_list_entry_b(insert,insertpoint);
		}

		/* check if insert is now the start of the list */
		if(insert->prev==NULL) {
			first=insert;		
		}
		insertpoint=insert->next;
	}

return first;
}

