#ifndef _alpha_h_
#define _alpha_h_   

/*
** File: alpha.h
** Desc: just an assitor function to add order to
** 	 utmp_list list
** Auth: Colm Mac Carthaigh, colmmacc@redbrick.dcu.ie
*/        

int username_compare(struct utmp_list *,struct utmp_list *);

/*
** returns a utmp_list doubly linked list which
** is in aplhabetical order based on user_name
** field
*/
#define order_a2z_utmp_list()	utmp_list_sort(get_utmp_list(),username_compare)


#endif
