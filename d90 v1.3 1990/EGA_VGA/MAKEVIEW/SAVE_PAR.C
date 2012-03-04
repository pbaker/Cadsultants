#include "stdio.h"
#include "display.h"

#define TRUE 1
#define FALSE 0

extern FILE far *display_save;
extern FILE far *display_file;

/*error logger stuff*/
extern void far error_logger();
extern char far diag_string[];


struct a_word far the_save_word;
char far save_part_write_error = FALSE;



void far save_part_display()
{
int row,col;


	fseek(display_file,0L,SEEK_SET);

	while(fread(&the_save_word,SIZE_A_WORD,1,display_file) > 0)
	{
		if(save_part_write_error == FALSE)
		{
			fwrite(&the_save_word,SIZE_A_WORD,1,display_save);

			if(ferror(display_save) != 0)
			{
				save_part_write_error = TRUE;

				sprintf(diag_string,
				     "ERROR: Not Enough Room On The Drive\n");
				error_logger(diag_string);

				sprintf(diag_string,
					"You Are Trying To Save The Part Onto\n");
				error_logger(diag_string);

				sprintf(diag_string,
					"Nothing More Will Be Saved\n");
				error_logger(diag_string);

				fseek(display_file,0L,SEEK_END);
					return;
			}
		}
	}


	fseek(display_file,0L,SEEK_END);
}
