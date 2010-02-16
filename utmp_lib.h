#ifndef _utmp_lib_h_
#define _utmp_lib_h_



/*
** File: utmp_lib.h
** Desc: a small library to get as much info on a logged-in user
**       as possible, now contains some utility functions
** Auth: Colm Mac Carthaigh, colmmacc@redbrick.dcu.ie
*/  

/* with: */

/*
** File: utent.h
** Desc: A replacement for the *ut* functions not provided in some
**       benighted backwaters. ;o) Also sorts out the use of getuxent()
** Auth: Cian Synnott
** Date: Thu May  6 15:43:54 IST 1999
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>




/* Include the right flavour of utmp */
#ifndef HAVE_UTMPX_H

#include <utmp.h>

#else

#include <utmpx.h>

#endif

#define HAVE_GETUTENT

/* If the system isn't providing getutent() and friends, we'll have to */
#ifndef HAVE_GETUTENT

void setutent();
struct utmp *getutent();
void endutent();

#endif

/*
** double-linked list structure for logins 
**
** NOTE: ttys_host is DNS dependant and can
** 	 be a domain or an IP
*/
struct utmp_list {
        char ** user_ttys;              /* array of ttys  */
        int  *ttys_mesg;           	/* array of mesg status */
        time_t *ttys_active;       	/* time struct of last user activity */
        time_t *ttys_login;   		/* time of login */                  
        int tty_num;                    /* length of previous arrays */
        char * user_name;               /* username */ 
	struct utmp_list *next;		/* pointer to next in list */
	struct utmp_list *prev;		/* pointer to previous in list */
};


/*
 * Return a single list entry for the username you specify,
 * next and prev will point to NULL, if the user is not loggged
 * in all the arrays be NULL, check if tty_num > 0. 
 */
struct utmp_list *get_utmp_username(char *);


/*
 * returns the first entry in a doubly linked list of all
 * the users currently logged in. In the first entry the 
 * prev pointer will be NULL, in the last entry next will
 * be NULL
 */
struct utmp_list *get_utmp_list();


/*
** Frees a whole utmp list or an entry. You can pass any
** member of the list and it will delete the ENTIRE list
** however passing the first entry in the list is much
** more acceptable
*/
void free_utmp(struct utmp_list *);


/*
** these two little functions move the first pointer to before
** (_b) or after (_a) the second pointer in the linked list.
*/
void move_utmp_list_entry_b(struct utmp_list *, struct utmp_list *);
void move_utmp_list_entry_a(struct utmp_list *, struct utmp_list *);

/*
** deletes a list member, frees up it's memory too
*/
void delete_utmp_list_entry(struct utmp_list *);


/*
** Sorts the list, pass it the starting point and pointer to the
** comparison function(should take two utmp_list pointers as args
** and return an integer comparison)
**
** list will be sorted lowest to highest, equal comparisons
** will not retain intial order.
**
** See alpha.c for a demonstration based on the alphabetical order
** of the username member.
*/
struct utmp_list *utmp_list_sort(struct utmp_list *, int (*)(struct utmp_list *,struct utmp_list *));


#ifdef USE_PROC_STUFF

#include <kvm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/sysctl.h>    
#include <sys/user.h>



struct tty_proc {
	char **tty_proc_argv;  	/* array of argv strings(not actual argv arrays) */
	char **tty_proc_names;	/* array of process names */
	int  n_entries;		/* the number of entries in the arrays */
};

/* get a tty_proc returns NULL on error */
struct tty_proc *get_proc_tty(char *);

/* free a tty_proc */
void free_tty_proc(struct tty_proc *);

#endif

#endif
