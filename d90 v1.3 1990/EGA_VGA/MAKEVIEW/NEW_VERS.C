#include "string.h" 


extern char far version_str[];
extern int far win_x_low;
extern int far win_y_low;
extern void far goto_row_col();


char far licensee[] = "BROADWAY COMPANIES";
char far serial[] = "1022";




void far release_version()
{
	strcpy(version_str,"1.24  5/8/89");
}


void far print_id()
{
	goto_row_col(win_y_low + 7,win_x_low + 1);
	printf("        Licensee: %s",licensee);
	goto_row_col(win_y_low + 8,win_x_low + 1);
	printf("        Software ID: %s",serial);
}

