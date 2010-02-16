#include "utmp_lib.h"
#include "alpha.h"

/*
** File: alpha.c
** Desc: just an assitor function to add order to
**       utmp_list list
** Auth: Colm Mac Carthaigh, colmmacc@redbrick.dcu.ie
*/ 

/*
** This code probably looks a little silly now that
** I've written the utmp_list_sort function to do
** all the stuff that used to be here, but prove
** usefull for other stuff.
*/


/*
** very simple comparison fuction that I would love to have as a macro
** (cant though, need a pointer)
*/
int username_compare(struct utmp_list *first,struct utmp_list *second) {
	return strcmp(first->user_name,second->user_name);
}






