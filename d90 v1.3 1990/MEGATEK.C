#include "dos.h"
#include "expanded.h"
#include "memory.h"
#include "c:\emulate\common\display.h"


#define END_OF_PAGE 79
#define END_OF_DL  75


#define TRUE  't'
#define FALSE  'f'



extern void far error_logger(char *);
extern char far no_more_ems_pages;
extern char far have_display_list;
extern short far *start_of_page_frame_0;
extern int far last_logical_page;



int far current_logical_page = 0;
char far *current_insert_pointer;


extern struct a_word far an_element;



void far insert_megatek()   /* into active megatek data segment */
{
short far *address_of_page;
unsigned long phy_address_of_page,phy_address_of_insert;
int i;
int temp_value;


	if((no_more_ems_pages == TRUE) || (have_display_list == FALSE))
		return;


/*decide if this page is full*/
	address_of_page = start_of_page_frame_0;
	phy_address_of_page = ((long)FP_SEG(address_of_page) << 4) +
		(long)FP_OFF(address_of_page);
	phy_address_of_insert = ((long)FP_SEG(current_insert_pointer) << 4) +
		(long)FP_OFF(current_insert_pointer);


	if(phy_address_of_insert - phy_address_of_page >= 16350L)
 /*goto next megatek page*/
	{
	/* store end of page marker */
		temp_value = an_element.op_code;

		an_element.op_code = END_OF_PAGE;
		memcpy(current_insert_pointer,&an_element,SIZE_A_WORD);

		an_element.op_code = temp_value;


		current_logical_page++;

		if(current_logical_page > num_unalloc_pages)
		{
			error_logger("RAN OUT OF DISPLAY LIST MEMORY\n");
			no_more_ems_pages = TRUE;
		}

		map_handle_page(current_logical_page,0,handle);
		current_insert_pointer = (char far *)start_of_page_frame_0;
	}



/* store it at current insert pointer */
	memcpy(current_insert_pointer,&an_element,SIZE_A_WORD);
	current_insert_pointer = current_insert_pointer + 
		SIZE_A_WORD;


	an_element.op_code = END_OF_DL;
	memcpy(current_insert_pointer,&an_element,SIZE_A_WORD);
}
