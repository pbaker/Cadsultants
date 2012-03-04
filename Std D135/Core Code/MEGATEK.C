
#include "my_globs.h"
#include "expanded.h"
#include "memory.h"



#define MAX_MEGATEK 64

extern void far error_logger(char *);
extern int far get_next_free_page(void);
extern char far no_more_ems_pages;
extern short far *start_of_page_frame_0;


extern int far last_logical_page;




void far report_seek_error(which_seg,position)
int which_seg;
long position;
{

	sprintf(diag_string,
	     "MEGATEK_SEEK(seg = %d, position = %ld): Invalid position: Last position = %ld!!!\n"
		 ,which_seg,position,megatek_ends[which_seg]);
	error_logger(diag_string);
}




void far insert_megatek()   /* into active megatek data segment */
{
short far *address_of_page;
unsigned long phy_address_of_page,phy_address_of_insert;
int i;

	if(no_more_ems_pages == 't')
		return;


/*
if(which_cseg == 3)
{
	printf("INSERT_MEGATEK(seg = 3): current_megatek_pointer = %p  pos = %ld\n",
		current_megatek_pointer,current_megatek_insert_pos);
	printf("Last page = %d\n",last_logical_page);
}
*/


/*decide if this page is full*/
	address_of_page = start_of_page_frame_0;
	phy_address_of_page = ((long)FP_SEG(address_of_page) << 4) +
		(long)FP_OFF(address_of_page);
	phy_address_of_insert = ((long)FP_SEG(current_megatek_pointer) << 4) +
		(long)FP_OFF(current_megatek_pointer);

	if(phy_address_of_insert - phy_address_of_page >= (long)(
		NUM_MEGA_WORDS*size_an_element))
 /*goto next megatek page*/
	{
		current_megatek_page = get_next_free_page();

		if(current_megatek_page > num_unalloc_pages)
		{
			error_logger("RAN OUT OF EMS PAGES FOR MEGATEK DATA\n");
			no_more_ems_pages = 't';
		}

	/*find next free column*/
		for(i = 0 ; i < MAX_MEGATEK ; i++)
			if(megatek_locate[which_cseg][i] == 0xff)
				break;

		megatek_locate[which_cseg][i] = current_megatek_page;
		megatek_ends[which_cseg] = current_megatek_insert_pos;

	/*tell which page comes next*/
		*((char far *)start_of_page_frame_0 + NEXT_MEGATEK_PAGE) = 
			current_megatek_page;
		map_handle_page(current_megatek_page,0,handle);
		current_megatek_pointer = (char far *)start_of_page_frame_0;
	}


/* store it at current insert pointer */
	memcpy(current_megatek_pointer,&an_element,size_an_element);



	current_megatek_pointer = current_megatek_pointer + 
		size_an_element;
	current_megatek_insert_pos = current_megatek_insert_pos + 1L;

/* update megatek end if needed */
	if(current_megatek_insert_pos > megatek_ends[which_cseg])
		megatek_ends[which_cseg] = current_megatek_insert_pos;


}



void far goto_megatek_end()    /*of active megatek data segment*/
{
int i;

	for(i = 0 ; i < MAX_MEGATEK ; i++)
		if(megatek_locate[which_cseg][i] == 0xff)
			break;

	if(i > 0)
	{
		i = megatek_locate[which_cseg][i - 1];
		map_handle_page(i,0,handle);
		current_megatek_pointer = (char far *)start_of_page_frame_0 +
			(int)(megatek_ends[which_cseg] % (long)NUM_MEGA_WORDS)
				*size_an_element;
		current_megatek_page = i;
		current_megatek_insert_pos = megatek_ends[which_cseg];
	}
	else
		error_logger("GOTO_MEGATEK_END() for current segment: failure (its empty!!)\n");
}




int far megatek_seek(which_seg,position)
int which_seg;
long position;
{
int page_num,offset;

	if((position > megatek_ends[which_seg]) || (position < 0L))
	{
/*
		current_megatek_pointer = (char far *)start_of_page_frame_0;
		map_handle_page(megatek_locate[0][0],0,handle);
*/
		return(FAILURE);
	}


	page_num = (int)(position / (long)NUM_MEGA_WORDS);
	offset = (int)(position % (long)NUM_MEGA_WORDS);

/*
printf("MEGATEK_SEEK(seg = %d,position = %ld): locate column = %d  offset = %d\n",
	which_seg,position,page_num,offset);
*/

	page_num = megatek_locate[which_seg][page_num];

/*
printf("megatek_locate[%d][index] gives page number = %d\n\n",
	which_seg,page_num);
*/

	current_megatek_pointer = (char far *)start_of_page_frame_0 + 
		offset*size_an_element;
	map_handle_page(page_num,0,handle);
	current_megatek_page = page_num;
	current_megatek_insert_pos = position;

/*
printf("Current Megatek Page = %d, Current InsPos = %ld\n\n",
        current_megatek_page,current_megatek_insert_pos);
*/

	return(SUCCESS);
}


long far my_ftell()  /*active segment's megatek list*/
{
	return(current_megatek_insert_pos);
}

