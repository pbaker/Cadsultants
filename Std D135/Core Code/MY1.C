/* 9/29/88 modified segment structure to include 2 word end of block jumps*/

/*November 15, 1988 Made Sub7 have SUB7_NUM_ELS "words" in insert_sub()*/

/*January 30, 1989 Made changes to send all error messages to the error logging
file & got rid of all the exit()'s*/

#include "my_globs.h"
#include "my_utils.h"
#include "memory.h"



#include "expanded.h"
extern int far last_logical_page;
extern short far *start_of_page_frame_0;





char far no_more_ems_pages = 'f';


int far get_next_free_page()
{
int i,j;
int page_in_use;
short far *temp_ptr;


	page_in_use = last_logical_page;
	temp_ptr = start_of_page_frame_0;

	for(i = 0 ; i < 256 ; i++)
		if(available_pages[i] == -1)
		{
			available_pages[i] = which_cseg;


			map_handle_page(i,0,handle);

			for(j = 0 ; j < 3333 ; j++)
				*(temp_ptr + j) = i;

			map_handle_page(page_in_use,0,handle);


			return(i);
		}


	return(-1);
}








void far insert_ours(value,which_seg)
struct our_element value;
int which_seg;
{
long current_pos;
char break_flag;
char temp_op_code;


#ifdef HAVE_DIAGS
	sprintf(diag_string,"insert segment # %d @ %ld\n",
		segment_nums[which_seg],my_ftell());
	my_diag(diag_string);
#endif

	current_pos = my_ftell();


	if(current_pos > 0L)
	{
		if((current_pos + 2L) % 64L == 0L)  /*insert 2 NULL words*/
		{
			temp_op_code = an_element.op_code;
			an_element.op_code = 44;

			if(no_more_ems_pages == 'f')
			{
				insert_megatek();
				insert_megatek();
			}

			an_element.op_code = temp_op_code;
		}
	}


	if(no_more_ems_pages == 'f')
		insert_megatek();


}







void far get_our_value(index,which_of_ours)
int index;
int which_of_ours;
{
	memcpy(&read_element,current_megatek_pointer,size_an_element);
}




void far get_sub_value(index,which_of_ours)
int index;
int which_of_ours;
{
	if((which_of_ours < 0) || (which_of_ours > NUMBER_OF_SUBS))
	{
		sprintf(diag_string,"invalid which_of_ours: %d in get_sub_value\n",
				which_of_ours);
		error_logger(diag_string);

		return;
	}	


	if(which_of_ours == 7)
		ptr_sub = &our_sub7[index];
	else if(which_of_ours == NUMBER_OF_SUBS)
		ptr_sub = &my_sub6[index];
	else
		ptr_sub = &(our_subs[which_of_ours][index]);
}






void far insert_sub(value,which_sub)
struct our_element value;
int which_sub;
{
int i;
int an_index;


	if((which_sub < 0) || (which_sub > NUMBER_OF_SUBS))
	{
		sprintf(diag_string,"INSERT_SUB() invalid sub # = %d\n",
			which_sub);
		error_logger(diag_string);

		return;
	}

	

	if(which_sub == 7)
	{
		sub_insert_ptrs[7]++;
		if(sub_insert_ptrs[7] >= SUB7_NUM_ELS)
		{
			sprintf(diag_string,"subroutine # %d is full\n",sub_nums[which_sub]);
			error_logger(diag_string);
			sub_insert_ptrs[7]--;
			return;
		}

		an_index = sub_insert_ptrs[7];

		our_sub7[an_index].op_code = value.op_code;
		our_sub7[an_index].arg1 = value.arg1;
		our_sub7[an_index].arg2 = value.arg2;
		our_sub7[an_index].flag1 = value.flag1;
		our_sub7[an_index].flag2 = value.flag2;
		our_sub7[an_index].arg3 = value.arg3;
		our_sub7[an_index].arg4 = value.arg4;

	}
	else if(which_sub == NUMBER_OF_SUBS)
	{
		sub_insert_ptrs[NUMBER_OF_SUBS]++;
		if(sub_insert_ptrs[NUMBER_OF_SUBS] >= SUB7_NUM_ELS)
		{
			sprintf(diag_string,"subroutine # %d is full\n",sub_nums[which_sub]);
			error_logger(diag_string);
			sub_insert_ptrs[NUMBER_OF_SUBS]--;
			return;
		}

		an_index = sub_insert_ptrs[NUMBER_OF_SUBS];

		my_sub6[an_index].op_code = value.op_code;
		my_sub6[an_index].arg1 = value.arg1;
		my_sub6[an_index].arg2 = value.arg2;
		my_sub6[an_index].flag1 = value.flag1;
		my_sub6[an_index].flag2 = value.flag2;
		my_sub6[an_index].arg3 = value.arg3;
		my_sub6[an_index].arg4 = value.arg4;

	}
	else
	{
		sub_insert_ptrs[which_sub]++;
		if(sub_insert_ptrs[which_sub] >= SUB_NUM_ELS)
		{
			sprintf(diag_string,"subroutine # %d is full\n",sub_nums[which_sub]);
			error_logger(diag_string);
			sub_insert_ptrs[which_sub]--;
			return;
		}

		an_index = sub_insert_ptrs[which_sub];

		our_subs[which_sub][an_index].op_code = value.op_code;
		our_subs[which_sub][an_index].arg1 = value.arg1;
		our_subs[which_sub][an_index].arg2 = value.arg2;
		our_subs[which_sub][an_index].flag1 = value.flag1;
		our_subs[which_sub][an_index].flag2 = value.flag2;
		our_subs[which_sub][an_index].arg3 = value.arg3;
		our_subs[which_sub][an_index].arg4 = value.arg4;
	}

}



