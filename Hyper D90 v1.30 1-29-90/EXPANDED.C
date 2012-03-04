#include "dos.h"
#include "stdio.h"


extern void far close_host();
extern void far restore_int9();


extern union REGS far my_input_regs;
extern union REGS far my_output_regs;

int far handle,far physical_page,far logical_page;
int far num_unalloc_pages;
int far page_frame_segment_address,far num_pages_requested;
int far *physical_pages_pointer;
int far current_logical_page = 0;
int far current_physical_page = 0;





void far exit_dealloc_pages(the_handle)
int the_handle;
{

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
/*	printf("map_handle_page(lp = %d,pp = %d,handle = %d)\n",logical_page,
		physical_page,handle);
*/
	my_input_regs.h.ah = 0x44;
	my_input_regs.x.bx = logical_page;
	my_input_regs.h.al = physical_page;
	my_input_regs.x.dx = handle;

	int86(0x67,&my_input_regs,&my_output_regs);

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
			printf("The EMM couldn't find the handle\n");
			ems_exit();
		}

		case 0x84 :
		{
			printf("Miscalled by my program\n");
			ems_exit();
		}

		case 0x8a :
		{
			printf("Logical page is out of range of your handle\n");
			break;
		}

		case 0x8b :
		{
			printf("Physical page is out of range\n");
			break;
		}
	}
}






void far dealloc_pages_for_handle(handle)
int handle;
{
/*	printf("deallocate_pages_for_handle(%d)\n",handle);
*/

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