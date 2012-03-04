/* EXPANDED.C  has all expanded memory functions */

/* august 31, 1989 ADDED check to map_handle_page
Gives MAJOR REWRITE improvement*/


/* ALL EMS operations that can occur while Clipper board is Busy  MUST
	loop   while(pw_dl_busy() == TRUE)  BECAUSE Clipper board
	interferes sometimes with EMS ....... 
*/



#include <stdio.h>
#include <pw_lib.h>
#include "my_globs.h"



#define NUM_EMS_PAGES 60



extern char do_not_dlrun_flag;

extern struct sub_holder *sub_struct_ptr;

extern void  error_logger();
extern short  *physical_pages_pointer;
extern short  *start_of_page_frame_0;
extern unsigned char  *current_megatek_pointer;
extern long  current_megatek_insert_pos;
extern int  current_megatek_page;
extern int  handle;
extern char  diag_string[];

extern char  *blt_page_frame_ptr;

int  last_map_handle_page = NUM_EMS_PAGES - 2;
int  last_megatek_page = NUM_EMS_PAGES - 2;
int  last_blt_page = NUM_EMS_PAGES - 2;

/* save my map info variables */
short  *save_physical_pointer;
unsigned char  *save_megatek_pointer;
long  save_megatek_insert_pos;
int  save_megatek_page;
int save_last_map_handle_page,save_last_megatek_page;

char my_ems_pages[NUM_EMS_PAGES][16384];

char save_map_handle_page = 'f';



void  exit_dealloc_pages(the_handle)
int the_handle;
{

}




void  ems_exit()
{
	exit_dealloc_pages(handle);

/*
	close_host();
*/

	exit(0);
}




void  save_map_info()
{
int i;



	if(do_not_dlrun_flag == 't')
		return;

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();

	save_physical_pointer = physical_pages_pointer;
	save_megatek_pointer = current_megatek_pointer;
	save_megatek_insert_pos = current_megatek_insert_pos;
	save_megatek_page = current_megatek_page;

	save_last_map_handle_page = last_map_handle_page;
	save_last_megatek_page = last_megatek_page;


/* save display list physical buffer page */
	memcpy(&(my_ems_pages[last_map_handle_page][0]),&(sub_struct_ptr ->
		my_ems_buffer[SUB_HOLDER_DL][0]),(int)16384);
/* save megatek physical buffer page */
	memcpy(&(my_ems_pages[last_megatek_page][0]),&(sub_struct_ptr ->
		my_ems_buffer[SUB_HOLDER_MEGATEK][0]),(int)16384);

}



void  restore_map_info()
{
int i;



	if(do_not_dlrun_flag == 't')
		return;

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();

	physical_pages_pointer = save_physical_pointer;
	current_megatek_pointer = save_megatek_pointer;
	current_megatek_insert_pos = save_megatek_insert_pos;
	current_megatek_page = save_megatek_page;

	last_map_handle_page = save_last_map_handle_page;
	last_megatek_page = save_last_megatek_page;

/*
printf("\nrestore_map_info() before memcpy 1\n");
*/

/* restore display list physical page buffer */
	memcpy(&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_DL][0]),
		&(my_ems_pages[last_map_handle_page][0]),(int)16384);
/*
printf("restore_map_info() before memcpy 2\n");
*/

/* restore megatek physical page buffer */
	memcpy(&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_MEGATEK][0]),
		&(my_ems_pages[last_megatek_page][0]),(int)16384);
/*
printf("restore_map_info() returns\n\n");
*/
}





void partial_map_handle(logical_page,handle)
int logical_page,handle;
{
	pw_dl_wait();

	if(logical_page >= NUM_EMS_PAGES)
	{
		sprintf(diag_string,
			"partial_map_handle() logical page %d out of range",
			logical_page);
		error_logger(diag_string);
		return;
	}


	blt_page_frame_ptr = (char *)
		&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][0]);


	memcpy(&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT]
		[0]),&(my_ems_pages[logical_page][0]),(int)16384);

}



void  blt_map_handle(logical_page,handle)
int logical_page,handle;
{
int save_row,save_col;

/* printf("BMH(logical = %d, handle = %d)\n",logical_page,handle);

 
   printf("BMH  last_blt_page = %d\n\n",last_blt_page);
*/

/*
	while(pw_dl_busy() == TRUE)
		;
*/

	if(last_blt_page == logical_page)
		return;

	pw_dl_wait();

/* printf("BMH %d\n",logical_page);
*/

	if(logical_page >= NUM_EMS_PAGES)
	{
		sprintf(diag_string,
			"blt_map_handle() logical page %d out of range",
			logical_page);
		error_logger(diag_string);
		return;
	}

	last_blt_page = logical_page;

	blt_page_frame_ptr = (char *)
		&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][0]);

}






void  megatek_map_handle(logical_page,handle)
int logical_page,handle;
{
int save_row,save_col,save_last_page;

/*
	while(pw_dl_busy() == TRUE)
		;
*/

	if(logical_page >= NUM_EMS_PAGES)
	{
		sprintf(diag_string,
			"megatek_map_handle() logical page %d out of range",
			logical_page);
		error_logger(diag_string);
		return;
	}


	if(last_megatek_page == logical_page)
		return;

	save_last_page = last_megatek_page;
	last_megatek_page = logical_page;

	pw_dl_wait();

/*
printf("MMH %d\n",logical_page);
*/

	current_megatek_pointer = &(sub_struct_ptr -> 
		my_ems_buffer[SUB_HOLDER_MEGATEK][0]);

	if(do_not_dlrun_flag == 'f')
	{
		memcpy(&(my_ems_pages[save_last_page][0]),&(sub_struct_ptr ->
			my_ems_buffer[SUB_HOLDER_MEGATEK][0]),(int)16384);

		memcpy(&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_MEGATEK]
			[0]),&(my_ems_pages[logical_page][0]),(int)16384);
	}
}



#ifdef lfdlflflflflflfl
void  add_to_page_frame_ptr(add_on)
unsigned long add_on;
{
unsigned long physical_address;
short  *result;

	physical_address = ((unsigned long)FP_SEG(start_of_page_frame_0)
		<< 4) + (unsigned long)FP_OFF(start_of_page_frame_0);

	physical_address = physical_address + 2L*add_on;

	result = (short  *)(((physical_address & 0xffff0L) << 12)
		+ (physical_address & 0x0fL));

	physical_pages_pointer = result;
}
#endif



void  get_board_status()
{
/*	printf("get_board_status()\n");
*/
}




int  get_page_frame_address()
{
/*	printf("get_page_frame_address()\n");
*/

	return(0);
}	



int  get_unalloc_page_count()
{
/*	printf("get_unalloc_page_count()\n");
*/

	return(NUM_EMS_PAGES);
}




int  allocate_pages(num_pages)
int num_pages;
{
/*	printf("allocate_pages(%d)\n",num_pages);
*/


	return(NUM_EMS_PAGES);
}




void  map_handle_page(logical_page,physical_page,handle)
int logical_page,physical_page,handle;
{
int i,save_row,save_col;

/*
	while(pw_dl_busy() == TRUE)
		;
*/


/*	printf("map_handle_page(lp = %d,pp = %d,handle = %d)\n",logical_page,
		physical_page,handle);
	printf("Physical_pages_pointer = %lx\n",physical_pages_pointer);

*/

	if(logical_page >= NUM_EMS_PAGES)
	{
		sprintf(diag_string,
			"map_handle_page() logical page %d is out of range",
			logical_page);
		error_logger(diag_string);
		return;
	}


	if(last_map_handle_page == logical_page)
		return;

	pw_dl_wait();

/*
printf("MHP %d\n",logical_page);
*/

	if(do_not_dlrun_flag == 'f')
	{
		if(save_map_handle_page == 't')
			memcpy(&(my_ems_pages[last_map_handle_page][0]),
				&(sub_struct_ptr -> my_ems_buffer[
				SUB_HOLDER_DL][0]),(int)16384);

		memcpy(&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_DL][0]),
			&(my_ems_pages[logical_page][0]),(int)16384);
	}

	physical_pages_pointer = (short *) &(sub_struct_ptr ->
	 	my_ems_buffer[SUB_HOLDER_DL][0]);

	last_map_handle_page = logical_page;
}






void  dealloc_pages_for_handle(handle)
int handle;
{
/*	printf("deallocate_pages_for_handle(%d)\n",handle);
*/

}


