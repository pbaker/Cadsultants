#include "stdio.h"
#include "memory.h"
#include "c:\emulate\common\display.h"
#include "expanded.h"



#define TRUE  't'
#define FALSE  'f'

#define END_OF_PAGE 79
#define END_OF_DL  75



extern char far *ems_read_pointer;
extern int far ems_read_page;

extern int far current_logical_page;


extern FILE far *display_save;


/*error logger stuff*/
extern void far error_logger();
extern char far diag_string[];

extern struct a_word far read_element;
extern void far get_word();


char far save_part_write_error = FALSE;



void far save_part_display()
{
/*copy the display file to the save file*/


	ems_read_page = 0;
	ems_read_pointer = (char far *)start_of_page_frame_0;
	map_handle_page(0,0,handle);


	for(get_word ; read_element.op_code != END_OF_DL ; get_word())
	{
	    if(save_part_write_error == FALSE)
	    {
		fwrite(&read_element,SIZE_A_WORD,1,display_save);

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


			map_handle_page(current_logical_page,0,handle);

			return;
		}
	    }
	}

	map_handle_page(current_logical_page,0,handle);
}
