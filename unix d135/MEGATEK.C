#include "my_globs.h"
#include "expanded.h"
#include <pw_lib.h>


#define MAX_MEGATEK 64



extern void  out_of_ems_pages();


extern void  error_logger();
extern int  get_next_free_page();
extern char  no_more_ems_pages;
extern short  *start_of_page_frame_0;
extern char do_not_dlrun_flag;




void  report_seek_error(which_seg,position)
int which_seg;
long position;
{

	sprintf(diag_string,
	     "MEGATEK_SEEK(seg = %d, position = %ld): Invalid position: Last position = %ld!!!\n"
		 ,which_seg,position,megatek_ends[which_seg]);
	error_logger(diag_string);

}




void  insert_megatek()   /* into active megatek data segment */
{
short  *address_of_page;
int i,old_current_page;

	if(no_more_ems_pages == 't')
		return;

	if(do_not_dlrun_flag == 't')
		return;


#ifdef lldfldflfdlfdlfdlfdl
/*decide if this page is full*/
	address_of_page = start_of_page_frame_0;
	phy_address_of_page = ((long)FP_SEG(address_of_page) << 4) +
		(long)FP_OFF(address_of_page);
	phy_address_of_insert = ((long)FP_SEG(current_megatek_pointer) << 4) +
		(long)FP_OFF(current_megatek_pointer);


	if(phy_address_of_insert - phy_address_of_page >= (long)(
		NUM_MEGA_WORDS*4))
#endif

/*
printf("Insert_megatek() : curr mega ptr = %lx  &my_ems_pages[%d][0] = %lx\n",
	current_megatek_pointer,current_megatek_page,&my_ems_pages[
	current_megatek_page][0]);
*/

	if((char *)current_megatek_pointer - (char *) &(sub_struct_ptr ->
		my_ems_buffer[SUB_HOLDER_MEGATEK][0]) >= NUM_MEGA_WORDS*4)
 /*goto next megatek page*/
	{
/*
printf("\nInside goto next page if\n");
*/

		old_current_page = current_megatek_page;

		current_megatek_page = get_next_free_page(MEGATEK);

		if(current_megatek_page > num_unalloc_pages)
		{
			out_of_ems_pages();
			no_more_ems_pages = 't';
		}

	/*find next free column*/
		for(i = 0 ; i < MAX_MEGATEK ; i++)
			if(megatek_locate[which_cseg][i] == 0xff)
				break;

		megatek_locate[which_cseg][i] = current_megatek_page;
		megatek_ends[which_cseg] = current_megatek_insert_pos;

	/*tell which page comes next*/
		*((char  *)&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_MEGATEK]
			 [0]) + NEXT_MEGATEK_PAGE) = current_megatek_page;

	/* copy physical buffer page into ems pages array */
		memcpy(&my_ems_pages[old_current_page][0],&(sub_struct_ptr ->
			my_ems_buffer[SUB_HOLDER_MEGATEK][0]),16384);

		megatek_map_handle(current_megatek_page,handle);
	}

/*
printf("after goto next page if\n");
*/

	*current_megatek_pointer = dl_pointer_element.op_code;
	*(current_megatek_pointer + 1) = dl_pointer_element.logical_page_num;
	*(current_megatek_pointer + 2) = (dl_pointer_element.offset >> 8) 
		& 0xff;
	*(current_megatek_pointer + 3) = dl_pointer_element.offset & 0xff;

	current_megatek_pointer = current_megatek_pointer + 4;
	current_megatek_insert_pos = current_megatek_insert_pos + 1L;

/* update megatek end if needed */
	if(current_megatek_insert_pos > megatek_ends[which_cseg])
		megatek_ends[which_cseg] = current_megatek_insert_pos;


}



void  goto_megatek_end()    /*of active megatek data segment*/
{
int i;
long long_temp;

	if(do_not_dlrun_flag == 't')
		return;


	for(i = 0 ; i < MAX_MEGATEK ; i++)
		if(megatek_locate[which_cseg][i] == 0xff)
			break;

	if(i > 0)
	{
		i = megatek_locate[which_cseg][i - 1];
		megatek_map_handle(i,handle);


/*   FEBRUARY 13, 1990  fix for ALT EDIT 6  (remainder of 0 is WRONG HERE)
		current_megatek_pointer = (char  *)&my_ems_pages[i][0] +
			(int)(megatek_ends[which_cseg] % (long)NUM_MEGA_WORDS)
				*4;

*/

		long_temp = megatek_ends[which_cseg] % (long)NUM_MEGA_WORDS;

		if(long_temp == 0L)
			current_megatek_pointer = 
				(unsigned char  *)&(sub_struct_ptr ->
				my_ems_buffer[SUB_HOLDER_MEGATEK][0]) + 
				NUM_MEGA_WORDS*4;
		else
			current_megatek_pointer = (unsigned char  *)
				&(sub_struct_ptr -> my_ems_buffer[
				SUB_HOLDER_MEGATEK][0]) + (int)long_temp*4;


		current_megatek_page = i;
		current_megatek_insert_pos = megatek_ends[which_cseg];
	}
	else
		error_logger("GOTO_MEGATEK_END() for current segment: failure (its empty!!)\n");
}




int  megatek_seek(which_seg,position)
int which_seg;
long position;
{
int page_num,offset;

	if(do_not_dlrun_flag == 't')
		return(FAILURE);


	if((position >= megatek_ends[which_seg]) || (position < 0L))
	{
		return(FAILURE);
	}


	page_num = (int)(position / (long)NUM_MEGA_WORDS);
	offset = (int)(position % (long)NUM_MEGA_WORDS);

/*
printf("MEGATEK_SEEK(seg = %d,position = %ld): locate index = %d  offset = %d\n",
	which_seg,position,page_num,offset);
*/

	page_num = megatek_locate[which_seg][page_num];

/*
printf("megatek_locate[%d][index] gives page number = %d\n\n",
	which_seg,page_num);
*/

	megatek_map_handle(page_num,handle);
	current_megatek_pointer = (unsigned char  *)&(sub_struct_ptr ->
		my_ems_buffer[SUB_HOLDER_MEGATEK][0]) + offset*4;

	current_megatek_page = page_num;
	current_megatek_insert_pos = position;

/*
printf("Current Megatek Page = %d, Current InsPos = %ld\n\n",
        current_megatek_page,current_megatek_insert_pos);
*/

	return(SUCCESS);
}


long  my_ftell()  /*active segment's megatek list*/
{
	if(do_not_dlrun_flag == 't')
		return(14L);

	return(current_megatek_insert_pos);
}


