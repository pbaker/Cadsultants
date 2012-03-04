/* MY1SEGM.C  contains segment insert/retrieve code  */



/* 9/29/88 modified segment structure to include 2 word end of block jumps*/

/*November 15, 1988 Made Sub7 have SUB7_NUM_ELS "words" in insert_sub()*/

/*January 30, 1989 Made changes to send all error messages to the error logging
file & got rid of all the exit()'s*/

/*March 13, 1989 started adding pixel commands*/

/*March 20, 1989 stopped running dash patterns*/

/*March 22, 1989 fixed get_our_value() to use size_dl_ptr_element*/

/*March 24, 1989 replaced get_sub_value() and insert_sub() with the
d:\pixel versions

and put slpat32()'s back into segment display lists*/

/*March 27, 1989 corrected inserting of character strings into segments*/

/*March 28, 1989 carefully removed the calls to exit(), that were introduced
when copying in insert_sub() & get_sub_value() from the d:\pixel directory*/

#include "my_globs.h"
#include <pw_lib.h>
#include "expanded.h"
#include "mywindows.h"  /* defines & prototypes ONLY */


#define RES_1024_768 3
#define RES_1280_1024 4
#define MAX_MEGATEK 64

extern char do_not_dlrun_flag;

extern void  restore_graphics_viewport();
extern char  read_more_flag;

extern void  goto_row_col();
extern void  draw_box();
extern void  clear_win();
extern void  make_win();
extern void  save_win();
extern void  restore_win();


extern int  megatek_seek();
extern long  my_ftell();
extern void  goto_megatek_end();
extern void  insert_megatek();

extern void  display_segs();
extern void  my_insert_indicator();
extern void  error_logger();
extern char  gtext_string[];
extern char  a_segment_cleared[];
extern char  a_segment_deleted[];
extern char  visibility_changed_to_on[];
extern char  which_high_res;
extern short  *start_of_page_frame_0;
extern OPCODE io_masks[];
extern int  global_screen_color;

extern void wait_read_keyboard();

char  inserting_chars = 'f';
int  insert_char_index;
int  x_insert_chars;
int  y_insert_chars;
char  no_more_ems_pages = 'f';





void  out_of_ems_pages()
{
char save_read_more_flag;



	error_logger("\n\n\nOUT OF DISPLAY LIST MEMORY\n");
	error_logger("NO MORE GRAPHICS DATA WILL BE STORED\n\n\n");

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();


	which_page = !which_page;
	wrmask(io_masks[which_page]);
	rdmask(io_masks[which_page]);

	window(50,700,50,400);
	viewport(50,700,50,400);
	flood((15 << 4) | 15);
	gtsize(20,20);
	linpat(0);

	color(0);
	move2(70,350);
	gtext("OUT OF DISPLAY LIST MEMORY.");
	move2(70,270);
	gtext("NO MORE GRAPHICS DATA");
	move2(70,190);
	gtext("WILL BE STORED!!");
	move2(100,100);
	gtext("PRESS ENTER TO CONTINUE");
	
	wait_read_keyboard();

	viewport(0,1023,0,767);
	flood((short)((global_screen_color << 4) | global_screen_color));

	which_page = !which_page;
	wrmask(io_masks[which_page]);
	rdmask(io_masks[which_page]);

	save_read_more_flag = read_more_flag;
	read_more_flag = 'f';

	restore_graphics_viewport();

	display_pop_up_window(MAIN_MENU);
	display_pop_up_window(LIGHTS);
	display_pop_up_window(CALCULATOR);
	display_pop_up_window(PART_INFO);

	read_more_flag = save_read_more_flag;
	

#ifdef lslslslssll
	putchar(7);
	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf("  Out Of Display List Memory");
	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("     No More Graphics Data Will Be Stored");
	goto_row_col(win_y_low + 7,win_x_low + 1);
	printf("     Press Enter To Continue");
#endif
}





int  get_next_free_page(which_type)
int which_type;
{
int i;


	for(i = 0 ; i < 256 ; i++)
		if(available_pages[i] == -1)
		{
			if(which_type == MEGATEK)
				available_pages[i] = which_cseg + 100;
			else if(which_type == BLT_LIST)
				available_pages[i] = which_blt_list + BLT_CODE;
			else
				available_pages[i] = which_cseg;



			if(i >= num_unalloc_pages)
			{
				if(no_more_ems_pages == 'f')
				{
					out_of_ems_pages();
					no_more_ems_pages = 't';
				}
			}


			return(i);
		}



	if(no_more_ems_pages == 'f')
	{
		out_of_ems_pages();
		no_more_ems_pages = 't';
	}

	return(-1);
}





/* inserts into clipper display list at current insert pointer */

void  insert_ours(value,which_seg)
struct our_element value;
int which_seg;
{
long current_pos;
char break_flag;
char temp_op_code;
int err,x1,y1,x2,y2;
long phy_address_of_insert,phy_address_of_page;
short  *address_of_page;
int old_current_page;


#ifdef HAVE_DIAGS
	sprintf(diag_string,"insert segment # %d @ %ld\n",
		segment_nums[which_seg],my_ftell());
	my_diag(diag_string);
#endif


	if(segment_visibility[which_cseg] == 'f')   /*don't send to screen*/
		which_insert_mode = TO_LIST_ONLY;
	else
	{
		if(visibility_changed_to_on[which_cseg] == 't')
			which_insert_mode = TO_LIST_ONLY;
		else
			which_insert_mode = TO_BOTH;
	}



	if(no_more_ems_pages == 't')
		which_insert_mode = TO_SCREEN_ONLY;


	if(a_segment_cleared[which_cseg] == 't')
		a_segment_cleared[which_cseg] = 'f';

	if(a_segment_deleted[which_cseg] == 't')
		a_segment_deleted[which_cseg] = 'f';



	if(do_not_dlrun_flag == 't')
		which_insert_mode = TO_SCREEN_ONLY;



    if(no_more_ems_pages == 'f')
    {
/*switch to next logical page when needed*/
#ifdef lffffffffffffff
	address_of_page = physical_pages_pointer;
	phy_address_of_page = ((long)FP_SEG(address_of_page) << 4) +
		(long)FP_OFF(address_of_page);
	phy_address_of_insert = ((long)FP_SEG(current_pointer_to_dl) << 4) +
		(long)FP_OFF(current_pointer_to_dl);


	if(phy_address_of_insert - phy_address_of_page > 16360L) /*next page*/
#endif

	if((char *)current_pointer_to_dl - (char *)&(sub_struct_ptr ->
		my_ems_buffer[SUB_HOLDER_DL][0]) > 16360)
	{
		old_current_page = current_logical_page;
		current_logical_page = get_next_free_page(DISPLAY_LIST);

		if(current_logical_page > num_unalloc_pages)
		{
			out_of_ems_pages();
			no_more_ems_pages = 't';
		}

		current_word[current_logical_page] = last_word[which_cseg];
		pw_dl_init(current_pointer_to_dl,0,NULL,0);
		dlint((short)(DL_BRANCH_INTERRUPT + current_logical_page));
		pw_dl_init(NULL,0,NULL,0);
		current_offset = 0;

		save_map_handle_page = 't';
		map_handle_page(current_logical_page,current_physical_page,
			handle);
		current_pointer_to_dl = physical_pages_pointer;
	}
   }


	current_pos = my_ftell();

	if(current_pos > 0L)
	{
		if((current_pos + 2L) % 64L == 0L)   /*insert 2 Jump words*/
		{
			dl_pointer_element.op_code = 44;

			if(no_more_ems_pages == 'f')
			{
				insert_megatek();

				if(no_more_ems_pages == 'f')
					insert_megatek();
			}
		}
	}


	if(no_more_ems_pages == 'f')
	{
		dl_pointer_element.op_code = an_element.op_code;
		dl_pointer_element.logical_page_num = current_logical_page;
		dl_pointer_element.offset = current_offset;

		insert_megatek();
	}



		switch(an_element.op_code) /*decide which pixel command to use*/
		{
			case 0 : /*subroutine call*/
			{
				if((which_insert_mode == TO_LIST_ONLY) ||
					(which_insert_mode == TO_BOTH))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
				
					call(&(sub_struct_ptr ->
						sub_calling_table[an_element.
						arg1][0]));
			
					if(an_element.arg1 >= 11)
						dlint(MATRIX_SUB_INTERRUPT);

					pw_dl_rdptr((unsigned short**)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				break;
			}

			case 1 : /*insert blink enable*/
			{

				break;
			}

			case 2 : /*insert dash enable*/
			{
				if((which_insert_mode == TO_BOTH) ||
					(which_insert_mode == TO_SCREEN_ONLY))
						linpat((short)(an_element.arg1
							 & 0x01));

				if((which_insert_mode == TO_BOTH) ||
					(which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					linpat((short)(an_element.arg1 & 0x01));
					pw_dl_rdptr((unsigned short**)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				break;
			}

			case 3 : /*insert blink rate*/
			{

				break;
			}

			case 4 : /*insert intensity/color*/
			{
				if((which_insert_mode == TO_BOTH) ||
				     (which_insert_mode == TO_SCREEN_ONLY))
					color((short)(((an_element.arg1 & 0x0f)
						 << 4) | (an_element.arg1 & 0x0f)));

				if((which_insert_mode == TO_BOTH) ||
					(which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					noop();
					call(&(sub_struct_ptr ->
						color_sub_table[
						an_element.arg1 & 0x0f][0]));
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				break;
			}

			case 5 : /*insert color*/
			{
				break;
			}

			case 6 : /*insert Xform enable*/
			{
				if((which_insert_mode == TO_BOTH) ||
					(which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);

					if((an_element.arg1 & 1) == 0)  /*enable*/
						dlint(XFORM_ENABLE_INTERRUPT);
					else   /*disable*/
						dlint(XFORM_DISABLE_INTERRUPT);

					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				break;
			}

			case 7 : /*insert 2d origin*/
			{
				error_logger("INSERT_OURS(): insert 2d origin: not implemented\n");
				break;
			}

			case 8 : /*insert 3d origin*/
			{
				error_logger("INSERT_OURS(): insert 3d origin: not implemented\n");
				break;
			}

			case 9 : /*insert z origin*/
			{
				error_logger("INSERT_OURS(): insert z origin: not implemented\n");
				break;
			}

			case 10 : /*insert 2d Xlate*/
			{
				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_SCREEN_ONLY))
					vwmatx3(&my_3d_viewing_matrix[0]);

				if((which_insert_mode == TO_BOTH) ||
					(which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					vwmatx3(&my_3d_viewing_matrix[0]);
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				break;
			}

			case 11 : /*insert 2d matrix*/
			{
				if((which_insert_mode == TO_BOTH) ||
				     (which_insert_mode == TO_SCREEN_ONLY))
					vwmatx3(&my_3d_viewing_matrix[0]);

				if((which_insert_mode == TO_BOTH) ||
					(which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					vwmatx3(&my_3d_viewing_matrix[0]);
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				break;
			}

			case 12 : /*insert 3d matrix*/
			{

				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_SCREEN_ONLY))
					vwmatx3(&my_3d_viewing_matrix[0]);

				if((which_insert_mode == TO_BOTH) ||
					(which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					vwmatx3(&my_3d_viewing_matrix[0]);
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				break;
			}

			case 13 : /*insert clipping*/
			{
			    if(which_high_res == RES_1024_768)
			    {
				x1 = (int)(an_element.arg1/5.333333333333)
					+ 512;
				x2 = (int)(an_element.arg3/5.333333333333)
					+ 512;
				y1 = (int)((an_element.arg2 + 2048)/5.3333333);
				y2 = (int)((an_element.arg4 + 2048)/5.3333333);
			    }
			    else
			    {
				x1 = (int)(an_element.arg1*1.25/5.33333333333)
					 + 640;
				x2 = (int)(an_element.arg3*1.25/5.33333333333)
					 + 640;
				y1 = (int)((an_element.arg2 + 2048)/5.12*1.28);
				y2 = (int)((an_element.arg4 + 2048)/5.12*1.28);
			    }


				if((which_insert_mode == TO_BOTH) ||
					(which_insert_mode == TO_SCREEN_ONLY))
				{
					window(an_element.arg1,an_element.arg3,
						an_element.arg2,an_element.arg4);
					viewport(x1,x2,y1,y2);
				}


				if((which_insert_mode == TO_BOTH) ||
					(which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					window(an_element.arg1,an_element.arg3,
						an_element.arg2,an_element.arg4);
					viewport(x1,x2,y1,y2);
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				break;
			}

			case 14 : /*move 2d absolute*/
			{
				if((which_insert_mode == TO_BOTH) ||
				   (which_insert_mode == TO_LIST_ONLY))
				{
 					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					move3((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*my_z));
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_SCREEN_ONLY))
					move3((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*my_z));

				break;
			}

			case 15 : /*draw 2d absolute*/
			{
				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					draw3((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*my_z));
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_SCREEN_ONLY))
					draw3((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*my_z));

				break;
			}

			case 16 : /*move 2d relative*/
			{
				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					move3r((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),0);
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_SCREEN_ONLY))
					move3r((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),0);

				break;
			}

			case 17 : /*draw 2d relative*/
			{
				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					draw3r((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),0);
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				     (which_insert_mode == TO_SCREEN_ONLY))
					draw3r((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),0);

				break;
			}

			case 18 : /*move 3d absolute*/
			{
				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					move3((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*an_element.arg3));
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_SCREEN_ONLY))
					move3((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*an_element.arg3));

				break;
			}

			case 19 : /*draw 3d absolute*/
			{
				if((which_insert_mode == TO_BOTH) ||
				     (which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					draw3((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*an_element.arg3));
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_SCREEN_ONLY))
					draw3((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*an_element.arg3));

				break;
			}

			case 20 : /*move 3d relative*/
			{
				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					move3r((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*an_element.arg3));
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_SCREEN_ONLY))
					move3r((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*an_element.arg3));

				break;
			}

			case 21 : /*draw 3d relative*/
			{
				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);
					draw3r((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*an_element.arg3));
					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				     (which_insert_mode == TO_SCREEN_ONLY))
					draw3r((short)(2*an_element.arg1),
						(short)(2*an_element.arg2),
						(short)(2*an_element.arg3));

				break;
			}

			case 22 : /*first word of short relative vector*/
			{
				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);

					if((an_element.arg2 & 0x08) != 0) /*its a draw*/
					{
						draw3r((short)(2*an_element.arg3),
							(short)(2*an_element.arg4),
							0);
					}
					else
						move3r((short)(2*an_element.arg3),
							(short)(2*an_element.arg4),
							0);

					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				     (which_insert_mode == TO_SCREEN_ONLY))
				{
					if((an_element.arg2 & 0x08) != 0) /*its a draw*/
					{
						draw3r((short)(2*an_element.arg3),
							(short)(2*an_element.arg4),
							0);
					}
					else	
						move3r((short)(2*an_element.arg3),
							(short)(2*an_element.arg4),
							0);
				}

				break;
			}

			case 30 : /*SRV words after the first word*/
			{
				if(an_element.flag1 == 0x40) /*the end*/
					break;
				
				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_LIST_ONLY))
				{
					pw_dl_init(current_pointer_to_dl,0,
						NULL,0);

					if((an_element.flag1 & 0x08) != 0) /*its a draw*/
					{
						draw3r((short)(2*an_element.arg1),
							(short)(2*an_element.arg2),
							0);
					}
					else
						move3r((short)(2*an_element.arg1),
							(short)(2*an_element.arg2),
							0);
	
					if(an_element.flag2 != 0x40)
					{
						if((an_element.flag2 & 0x08) != 0)
						{    /*its a draw*/
							draw3r((short)(2*an_element.arg3),
							   (short)(2*an_element.arg4),0);
						}	
						else
							move3r((short)(2*an_element.arg3),
							   (short)(2*an_element.arg4),0);
					}

					pw_dl_rdptr((unsigned short **)
						&current_pointer_to_dl);
					pw_dl_init(NULL,0,NULL,0);
				}

				if((which_insert_mode == TO_BOTH) ||
				    (which_insert_mode == TO_SCREEN_ONLY))
				{
					if((an_element.flag1 & 0x08) != 0) /*its a draw*/
					{
						draw3r((short)(2*an_element.arg1),
							(short)(2*an_element.arg2),
							0);
					}
					else	
						move3r((short)(2*an_element.arg1),
							(short)(2*an_element.arg2),
							0);

					if(an_element.flag2 != 0x40)
					{
						if((an_element.flag2 & 0x08) != 0)
						{    /*its a draw*/
							draw3r((short)(2*an_element.arg3),
							   (short)(2*an_element.arg4),0);
						}
						else
							move3r((short)(2*an_element.arg3),
							   (short)(2*an_element.arg4),0);
					}
				}

				break;
			}  /*end case 30 : */


			case 23 : /*first word of charactor string*/
			{
				if(gtext_string[0] != '\0')
				    /*1st 23 for this character string*/
				{
				    if((gtext_string[0] == '*')  ||
					(gtext_string[0] == '+')   ||
					   (gtext_string[0] == '.')   ||
					((gtext_string[0] == 'o') &&
					   (gtext_string[1] == '\0'))  ||
					((gtext_string[0] == 'O') &&
					   (gtext_string[1] == '\0')))
				    {
					if((which_insert_mode == TO_LIST_ONLY)
						|| (which_insert_mode ==
							TO_BOTH))
					{
						pw_dl_init(current_pointer_to_dl
							,0,NULL,0);	   
						my_insert_indicator(gtext_string[0],
							2*an_element.arg1,
							2*an_element.arg2,
							2*my_z);
						pw_dl_rdptr((unsigned short **)
							&current_pointer_to_dl);
						pw_dl_init(NULL,0,NULL,0);
					}


					if((which_insert_mode == TO_SCREEN_ONLY)
						|| (which_insert_mode ==
							TO_BOTH))
					{
						my_insert_indicator(gtext_string[0],
							2*an_element.arg1,
							2*an_element.arg2,
							2*my_z);
					}
				    }
				    else   /* not an indicator */
				    {
					if((which_insert_mode == TO_LIST_ONLY)
						|| (which_insert_mode ==
							TO_BOTH))
					{
						pw_dl_init(current_pointer_to_dl
							,0,NULL,0);
						move2((short)an_element.arg1,
							(short)an_element.arg2);
						gtext(gtext_string);
						pw_dl_rdptr((unsigned short **)
							&current_pointer_to_dl);
						pw_dl_init(NULL,0,NULL,0);
					}


					if((which_insert_mode == TO_SCREEN_ONLY)
						|| (which_insert_mode ==
							TO_BOTH))
					{
						move2((short)an_element.arg1,
							(short)an_element.arg2);
						gtext(gtext_string);
					}
				    }
				}


				break;
			}


			case 27 : /*additional words of charactor string*/
			{
				break;
			}

			case 24 : /*insert dash pattern*/
			{
				switch((unsigned int)an_element.arg1)
				{
					case 0xffff : /*solid*/
					{
					/*to the screen if*/
						if((which_insert_mode ==
						    TO_BOTH) ||
							(which_insert_mode ==
							    TO_SCREEN_ONLY))
								my_dash_pattern
								  (an_element.
									arg1);

					/*to the display list if*/
						if((which_insert_mode ==
						    TO_BOTH) ||
							(which_insert_mode ==
							    TO_LIST_ONLY))
						{
							pw_dl_init(current_pointer_to_dl,
								0,NULL,0);
							slpat32(1,0xffffffff);
						
							pw_dl_rdptr((unsigned short **)&current_pointer_to_dl);
							pw_dl_init(NULL,0,NULL
								,0);
						}

						break;
					}

					case 0xeeee : /*dashed*/
					{
					/*to the screen if*/
						if((which_insert_mode ==
						    TO_BOTH) ||
							(which_insert_mode ==
							    TO_SCREEN_ONLY))
								my_dash_pattern
								  (an_element.
									arg1);


					/*to the display list if*/
						if((which_insert_mode ==
						    TO_BOTH) ||
							(which_insert_mode ==
							    TO_LIST_ONLY))
						{
							pw_dl_init(current_pointer_to_dl,
								0,NULL,0);
							slpat32(1,0xf0f0f0f0);
						
							pw_dl_rdptr((unsigned short **)&current_pointer_to_dl);
							pw_dl_init(NULL,0,NULL,
								0);
						}

						break;
					}

					case 0xffb6 : /*phantom*/
					{
					/*to the screen if*/
						if((which_insert_mode ==
						    TO_BOTH) ||
							(which_insert_mode ==
							    TO_SCREEN_ONLY))
								my_dash_pattern
								  (an_element.
									arg1);

					/*to the display list if*/
						if((which_insert_mode ==
						    TO_BOTH) ||
							(which_insert_mode ==
							    TO_LIST_ONLY))
						{
							pw_dl_init(current_pointer_to_dl,
								0,NULL,0);
							slpat32(1,0xfff0f0f0);
						
							pw_dl_rdptr((unsigned short **)&current_pointer_to_dl);
							pw_dl_init(NULL,0,NULL,
								0);
						}

						break;
					}

					case 0xfff6 : /*centerline*/
					{
					/*to the screen if*/
						if((which_insert_mode ==
						    TO_BOTH) ||
							(which_insert_mode ==
							    TO_SCREEN_ONLY))
								my_dash_pattern
								  (an_element.
									arg1);


					/*to the display list if*/
						if((which_insert_mode ==
						    TO_BOTH) ||
							(which_insert_mode ==
							    TO_LIST_ONLY))
						{
							pw_dl_init(current_pointer_to_dl,
								0,NULL,0);
							slpat32(1,0xfffff0f0);
						
							pw_dl_rdptr((unsigned short **)&current_pointer_to_dl);
							pw_dl_init(NULL,0,NULL,
								0);
						}

						break;
					}
				}  /*end which dash pattern switch()*/


				break;
			}


		}   /*end of pixel command insertion switch*/

#ifdef kkkkkkkkkkkkkkkkkkkkkkkkkkk
	address_of_page = physical_pages_pointer;
	phy_address_of_page = ((long)FP_SEG(address_of_page) << 4) +
		(long)FP_OFF(address_of_page);
	phy_address_of_insert = ((long)FP_SEG(current_pointer_to_dl) << 4) +
		(long)FP_OFF(current_pointer_to_dl);
	current_offset = (int)(phy_address_of_insert -
			phy_address_of_page)/2;
#endif
	current_offset = ((char *)current_pointer_to_dl - (char *)
		&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_DL][0]))/2;

	update_current_segments_end();
}



void  get_our_value(index,which_of_ours)
int index;
int which_of_ours;
{
	read_element.op_code = *current_megatek_pointer;
	read_element.logical_page_num = *(current_megatek_pointer + 1);
	read_element.offset = (*(current_megatek_pointer + 2) << 8) +
		*(current_megatek_pointer + 3);
}


