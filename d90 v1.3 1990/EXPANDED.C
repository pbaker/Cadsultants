
/* EXPANDED.C  has all expanded memory functions (FOR STD D90) */


/* august 31, 1989 ADDED check to map_handle_page
Gives MAJOR REWRITE improvement*/


#include "dos.h"
#include "stdio.h"


#define TRUE  't'
#define FALSE  'f'


#define NUMBER_OF_PAGE_FRAMES 4


extern void far close_host();
extern void far error_logger(char *);
extern void far restore_int9();
extern void far goto_row_col(int,int);
extern union REGS far my_input_regs;
extern union REGS far my_output_regs;
extern short far *start_of_page_frame_0;
extern int far row_curser;
extern int far col_curser;
extern int far handle;
extern char far diag_string[];
extern char far have_display_list;


int far mapping_table[NUMBER_OF_PAGE_FRAMES] = {-1,-1,-1,-1};
int far round_robin = 0;


/* save my map info variables */
int far save_mapping_table[NUMBER_OF_PAGE_FRAMES];
unsigned char far *save_megatek_pointer;
long far save_megatek_insert_pos;
int far save_megatek_page;



int far last_logical_page = 9999;



void far exit_dealloc_pages(the_handle)
int the_handle;
{
	if(have_display_list == FALSE)
		return;	


	my_input_regs.h.ah = 0x45;
	my_input_regs.x.dx = the_handle;

	int86(0x67,&my_input_regs,&my_output_regs);
}




void far ems_exit()
{
	exit_dealloc_pages(handle);
	close_host();
	restore_int9();
	exit();
}




#ifdef krgkmgrgklgkgkgkgkkgk

void far save_map_info()
{
int i;


return;    /* until the day it has Round Robin */


/* save EMM mapping registers */
	my_input_regs.h.ah = 0x47;
	my_input_regs.x.dx = handle;
	int86(0x67,&my_input_regs,&my_output_regs);

	if(my_output_regs.h.ah != 0)
	{
		sprintf(diag_string,"save page mapping registers error = %x",
			my_output_regs.h.ah);
		error_logger(diag_string);
	}


	for(i = 0 ; i < NUMBER_OF_PAGE_FRAMES ; i++)
		save_mapping_table[i] = mapping_table[i];


	save_megatek_pointer = current_megatek_pointer;
	save_megatek_insert_pos = current_megatek_insert_pos;
	save_megatek_page = current_megatek_page;
}



void far restore_map_info()
{
int i;


return;


/* restore EMM mapping registers */
	my_input_regs.h.ah = 0x48;
	my_input_regs.x.dx = handle;
	int86(0x67,&my_input_regs,&my_output_regs);

	if(my_output_regs.h.ah != 0)
	{
		sprintf(diag_string,"restore page mapping registers error = %x",
			my_output_regs.h.ah);
		error_logger(diag_string);
	}



	for(i = 0 ; i < NUMBER_OF_PAGE_FRAMES ; i++)
		mapping_table[i] = save_mapping_table[i];



	current_megatek_pointer = save_megatek_pointer;
	current_megatek_insert_pos = save_megatek_insert_pos;
	current_megatek_page = save_megatek_page;


#ifdef zeke_the_freak

/* restore megatek data mapping */
	my_input_regs.h.ah = 0x44;
	my_input_regs.x.bx = current_megatek_page;
	my_input_regs.h.al = 0;  /* physical_page */
	my_input_regs.x.dx = handle;

	int86(0x67,&my_input_regs,&my_output_regs);


/*restore clipper page mapping */
	for(i = 0 ; i < NUMBER_OF_PAGE_FRAMES ; i++)
	{
		my_input_regs.h.ah = 0x44;
		my_input_regs.x.bx = mapping_table[i];
		my_input_regs.h.al = i + 1;  /* physical_page */
		my_input_regs.x.dx = handle;

		int86(0x67,&my_input_regs,&my_output_regs);
	}

#endif

}


#endif




void far add_to_page_frame_ptr(add_on)
unsigned long add_on;
{
unsigned long physical_address;
char far *result;


/*

	physical_address = ((unsigned long)FP_SEG(start_of_page_frame_0)
		<< 4) + (unsigned long)FP_OFF(start_of_page_frame_0);

	physical_address = physical_address + 2L*add_on;

	result = (char far *)(((physical_address & 0xffff0L) << 12)
		+ (physical_address & 0x0fL));

	current_megatek_pointer = result;

*/
}




void far get_board_status()
{
/*	printf("get_board_status()\n");
*/
	my_input_regs.h.ah = 0x40;

	int86(0x67,&my_input_regs,&my_output_regs);

	switch(my_output_regs.h.ah)
	{
		case 0 :
		{
/*			printf("Everythings working fine\n");
*/
			return;
		}

		case 0x80 :
		{
			printf("Malfunction in EMM software\n");
			ems_exit();
		}

		case 0x81 :
		{
			printf("Malfunction in the expanded memory\n");
			ems_exit();
		}

		case 0x84 :
		{
			printf("Miscalled by my program\n");
			ems_exit();
		}
	}
}




int far get_page_frame_address()
{
/*	printf("get_page_frame_address()\n");
*/
	
	my_input_regs.h.ah = 0x41;

	int86(0x67,&my_input_regs,&my_output_regs);

	switch(my_output_regs.h.ah)
	{
		case 0 : 
		{
/*			printf("Worked OK: segment address = %u\n",
				my_output_regs.x.bx);
*/
			return(my_output_regs.x.bx);
		}

		case 0x80 :
		{
			printf("Malfunction in EMM software\n");
			ems_exit();
		}

		case 0x81 :
		{
			printf("Malfunction in expanded memory\n");
			ems_exit();
		}

		case 0x84 :
		{
			printf("Miscalled by my program\n");
			ems_exit();
		}
	}

	return(-1);
}



int far get_unalloc_page_count()
{
/*	printf("get_unalloc_page_count()\n");
*/

	my_input_regs.h.ah = 0x42;

	int86(0x67,&my_input_regs,&my_output_regs);

	switch(my_output_regs.h.ah)
	{
		case 0 : 
		{
/*			printf("Worked OK: # of unallocated pages = %d\n",
				my_output_regs.x.bx);
			printf("Total # of pages in expanded memory = %d\n",
				my_output_regs.x.dx);
*/
			return(my_output_regs.x.bx);
		}

		case 0x80 :
		{
			printf("Malfunction in EMM software\n");
			ems_exit();
		}

		case 0x81 :
		{
			printf("Malfunction in expanded memory\n");
			ems_exit();
		}

		case 0x84 :
		{
			printf("Miscalled by my program\n");
			ems_exit();
		}
	}

	return(-1);
}




int far allocate_pages(num_pages)
int num_pages;
{
/*	printf("allocate_pages(%d)\n",num_pages);
*/

	my_input_regs.h.ah = 0x43;
	my_input_regs.x.bx = num_pages;

	int86(0x67,&my_input_regs,&my_output_regs);

	switch(my_output_regs.h.ah)
	{
		case 0 : 
		{
/*			printf("Worked OK: handle = %d\n",my_output_regs.x.dx);
*/

			return(my_output_regs.x.dx);
		}

		case 0x80 :
		{
			printf("Malfunction in EMM software\n");
			ems_exit();
		}

		case 0x81 :
		{
			printf("Malfunction in expanded memory\n");
			ems_exit();
		}

		case 0x84 :
		{
			printf("Miscalled by my program\n");
			ems_exit();
		}

		case 0x85 :
		{
			printf("All EMM Handles are being used\n");
			ems_exit();
		}

		case 0x87 :
		{
			printf("Expanded memory's total size is too small\n");
			ems_exit();
		}

		case 0x88 :
		{
			printf("Not enough unallocated pages\n");
			ems_exit();
		}

		case 0x89 :
		{
			printf("Can't allocate zero pages\n");
			ems_exit();
		}
	}

	return(-1);
}




void far map_handle_page(logical_page,physical_page,handle)
int logical_page,physical_page,handle;
{
int i,save_row,save_col;



	if(have_display_list == FALSE)
		return;





if(logical_page == last_logical_page)
{
/*
	printf("NO SWAP MHP(lp = %d,pp = %d,handle = %d)   check = %d %d %d\n",
		logical_page,physical_page,handle,
			*(start_of_page_frame_0 + 8111),
			*(start_of_page_frame_0 + 8141),
			*(start_of_page_frame_0 + 8171));
*/

	return;
}


/*
printf("map_handle_page(lp = %d,pp = %d,handle = %d)  last = %d\n",logical_page,
	physical_page,handle,last_logical_page);
*/


last_logical_page = logical_page;




physical_page = 0;
goto skip_robin;



/*	printf("map_handle_page(lp = %d,pp = %d,handle = %d)\n",logical_page,
		physical_page,handle);
	printf("Physical_pages_pointer = %lx\n",physical_pages_pointer);

*/



	for(i = 0 ; i < NUMBER_OF_PAGE_FRAMES ; i++)
		if(mapping_table[i] == logical_page)
			break;

	if(i == NUMBER_OF_PAGE_FRAMES)    /*then its not mapped in anywhere*/
	{
		for(i = 0 ; i < NUMBER_OF_PAGE_FRAMES ; i++)
			if(mapping_table[i] == -1)
				break;

		if(i == NUMBER_OF_PAGE_FRAMES)   /*then no unused frames*/
		{
			physical_page = round_robin;
			add_to_page_frame_ptr((long)round_robin*8192L);
			mapping_table[round_robin] = logical_page;

			round_robin = (round_robin + 1) %
				 NUMBER_OF_PAGE_FRAMES;
		}
		else
		{
			physical_page = i;
			add_to_page_frame_ptr((long)i*8192L);
			mapping_table[i] = logical_page;
		}
	}
	else   /* set active page frame pointer to match "i" */
	{
		add_to_page_frame_ptr((long)i*8192L);

		return;
	}


/*
	save_row = row_curser;
	save_col = col_curser;
	read_curser_pos();
	goto_row_col(24,66);
	printf("EMS %3d",logical_page);
	goto_row_col(row_curser,col_curser);
	row_curser = save_row;
	col_curser = save_col;
*/


/*
printf("MHP %d\n",logical_page);
*/


skip_robin:


	my_input_regs.h.ah = 0x44;
	my_input_regs.x.bx = logical_page;
	my_input_regs.h.al = physical_page;
	my_input_regs.x.dx = handle;

	int86(0x67,&my_input_regs,&my_output_regs);


/*
	printf("SWAP MHP(lp = %d,pp = %d,handle = %d)   check = %d %d %d\n",
		logical_page,physical_page,handle,
			*(start_of_page_frame_0 + 8111),
			*(start_of_page_frame_0 + 8141),
			*(start_of_page_frame_0 + 8171));
*/


	switch(my_output_regs.h.ah)
	{
		case 0 : 
		{
/*			printf("Worked OK: \n");
*/

			return;
		}

		case 0x80 :
		{
			printf("Malfunction in EMM software\n");
			ems_exit();
		}

		case 0x81 :
		{
			printf("Malfunction in expanded memory\n");
			ems_exit();
		}

		case 0x83 :
		{
			printf("MAP_HANDLE_PAGE(lp = %d , pp = %d , handle = %d)\n",
				logical_page,physical_page,handle);
			printf("The EMM couldn't find the handle = %d\n",
				handle);
			ems_exit();
		}

		case 0x84 :
		{
			printf("Miscalled by my program\n");
			ems_exit();
		}

		case 0x8a :
		{
			sprintf(diag_string,
			    "MHP Logical page = %d is out of range of your handle\n",
				logical_page);
			error_logger(diag_string);

			break;
		}

		case 0x8b :
		{
			sprintf(diag_string,
				"MHP Physical page %d is out of range\n",
					physical_page);
			error_logger(diag_string);
		
			break;
		}
	}
}






void far dealloc_pages_for_handle(handle)
int handle;
{
/*	printf("deallocate_pages_for_handle(%d)\n",handle);
*/

	if(have_display_list == FALSE)
		return;



	my_input_regs.h.ah = 0x45;
	my_input_regs.x.dx = handle;

	int86(0x67,&my_input_regs,&my_output_regs);

	switch(my_output_regs.h.ah)
	{
		case 0 :
		{
/*			printf("Worked OK\n");
*/
			return;
		}

		case 0x80 :
		{
			printf("Malfunction in EMM software\n");
			ems_exit();
		}

		case 0x81 :
		{
			printf("Malfunction in expanded memory\n");
			ems_exit();
		}

		case 0x83 :
		{
			printf("Couldn't find your handle\n");
			ems_exit();
		}

		case 0x84 :
		{
			printf("Miscalled by my program\n");
			ems_exit();
		}

		case 0x86 :
		{
			printf("\"Save\" or \"restore\" page mapping context error\n");
			ems_exit();
		}
	}
}

