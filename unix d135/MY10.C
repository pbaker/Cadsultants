/* MY10.C  the regular subroutine interpretor */



/*October 14, 1988 3:20 fixed svwpt() to use headers_segs[which_cseg].
save_*_*_clip instead of headers_segs[which_cseg].clip_*_*      */

/*November 15, 1988 made changes to use generic graphics board calls*/

/*November 21, 1988 Made dash enable/disable set header's dash_enable flag*/

/*January 31, 1989 Made change to send the 1 error message to the error file*/

/*March 24, 1989 made changes to use non-interpreted subs*/

/*March 27, 1989 took out calling non-interpreted subs*/


/*
 October 18, 1990  (case 90 : locate)  stopped setting new_x = old_x = pen2_x
	new_y = old_y = pen2_y  BECAUSE it was screwing up dragging
*/


#include "my_globs.h"
#include "my_utils.h"
#include <pw_lib.h>
#include "mywindows.h"


extern struct a_window  my_windows[];


extern void  e_func();
extern OPCODE  io_masks[];
extern float  virtual_size,  actual_size;
extern char do_not_dlrun_flag;

short  temp_call_dl[100];


void  interp_sub(this_sub_num)
int this_sub_num;
{
int sub_counter = 0;
unsigned char command;
int dummy,i,j,temp_x1,temp_x2,temp_y1,temp_y2;
int graphics_row,graphics_col;

	if(do_not_dlrun_flag == 't')
		return;


	graphics_col = my_windows[GRAPHICS_WINDOW].full_size.screen_col;
	graphics_row = my_windows[GRAPHICS_WINDOW].full_size.screen_row;


	sub_level++;
	if(sub_level > 4)
	{
		sub_level--;
		return;
	}


	while(1==1)
	{
		get_sub_value(sub_counter,this_sub_num);
		command = ptr_sub -> op_code;

		if(sub_counter > sub_insert_ptrs[this_sub_num])
		{
			sub_level--;

			return;
		}



/*			case 20 :
			{
				if(sub_counter > my_sub6_ptr)
				{
					sub_level--;

					return;
				}
				break;
			}
*/



		switch(command)
		{
			case 0 :  /*call subroutine*/
			{
				fill_sub = 'f';

				if((ptr_sub -> arg1 >= 11) ||
					(ptr_sub -> arg1 == 3))
				{
				/*interpret it*/
					our_call = 's';
					e_func(&dummy,dummy);
				}
				else   /*a non-interpreted subroutine*/
				{
/*make a temporary display list to call the sub display list*/

					pw_dl_init(&temp_call_dl[0],0,NULL,0);
					call(&(sub_struct_ptr -> 
						sub_calling_table[ptr_sub -> 
						arg1][0]));
					dlend();
					pw_dl_init(NULL,0,NULL,0);

/*run the temporary display list (to execute the subroutine)*/

					dlrun(&temp_call_dl[0]);

					pw_dl_wait();
				}


				sub_counter++;
				break;
			}


			case 1 : /*insert blink enable*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert blink enable\n");
#endif
				sub_counter++;
				break;
			}

			case 2 : /*insert dash enable*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert dash enable\n");
#endif
				if((ptr_sub -> arg1 & 1) != 0) /*enable*/
				{
					line_type = 4;
					my_dash_enable(4);
				}
				else
				{
					line_type = 0;
					my_dash_enable(0);
				}

				sub_counter++;
				break;
			}

			case 3 : /*insert blink rate*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert blink rate\n");
#endif
				sub_counter++;
				break;
			}

			case 4 : /*insert (intensity/color)*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert (intensity/color)\n");
#endif
				if((draw_black == 'f') && (black == 'f'))
					my_color((char) ptr_sub -> arg1 & 0x0f);
				else
					my_color((char)0);

				save_color = (char) ptr_sub -> arg1 & 0x0f;

				sub_counter++;
				break;
			}
			
			case 5 : /*insert color*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert color\n");
#endif
				sub_counter++;
				break;
			}

			case 6 : /*insert_Xform enable*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert Xform enable\n");
#endif
				if((ptr_sub -> arg1 & 1) == 0)
				{
#ifdef HAVE_DIAGS
					my_diag("XFORM ENABLE\n");
#endif

				}
				else
				{
#ifdef HAVE_DIAGS
					my_diag("XFORM DISABLE\n");
#endif

				}

				sub_counter++;
				break;
			}

			case 7 : /*insert 2D origin*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert 2D origin\n");
#endif

				sub_counter++;
				break;
			}

			case 8 : /*insert 3D origin*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert 3D origin\n");
#endif

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				sub_counter++;
				break;
			}

			case 9 : /*insert Z origin*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert Z origin\n");
#endif

				sub_counter++;
				break;
			}

			case 10 : /*insert 2D Xlate*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert 2D Xlate\n");
#endif

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				sub_counter++;

				break;
			}

			case 11 : /*insert 2D matrix*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert 2D matrix\n");
#endif

#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);


#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
#endif


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
#endif

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
#endif
				sub_counter++;

				break;
			}

			case 12 : /*insert 3D matrix*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert 3D matrix\n");
#endif

				my_3d_viewing_matrix[0] = ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				my_3d_viewing_matrix[4] = ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				my_3d_viewing_matrix[12] = ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				my_3d_viewing_matrix[1] = ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				my_3d_viewing_matrix[5] = ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				my_3d_viewing_matrix[13] = ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				my_3d_viewing_matrix[8] = ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				my_3d_viewing_matrix[9] = ptr_sub -> arg1;

				sub_counter++;

				vwmatx3(&my_3d_viewing_matrix[0]);

				break;
			}

			case 13 : /*insert clipping*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert clipping\n");
#endif

				temp_x1 = ptr_sub -> arg1;
				temp_y1 = ptr_sub -> arg2;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				temp_x2 = ptr_sub -> arg1;
				temp_y2 = ptr_sub -> arg2;

				sub_counter++;


				my_set_viewport(temp_x1,temp_y1,temp_x2,
					temp_y2,which_page);

				break;
			}

			case 14 : /*move 2D absolute*/
			{
#ifdef HAVE_DIAGS
				my_diag("move 2D absolute\n");
#endif
				my_x = ptr_sub -> arg1;
				my_y = ptr_sub -> arg2;
				sub_counter++;
				break;
			}

			case 15 : /*draw 2D absolute*/
			{
#ifdef HAVE_DIAGS
				my_diag("draw 2D absolute\n");
#endif

				vect3(2*my_x,2*my_y,2*my_z,
					2*(ptr_sub -> arg1),2*(ptr_sub -> arg2)
						,2*my_z);


				my_x = ptr_sub -> arg1;
				my_y = ptr_sub -> arg2;
				sub_counter++;
				break;
			}

			case 16 : /*move 2D relative*/
			{
#ifdef HAVE_DIAGS
				my_diag("move 2D relative\n");
#endif
				my_x += ptr_sub -> arg1;
				my_y += ptr_sub -> arg2;
				sub_counter++;
				break;
			}

			case 17 : /*draw 2D relative*/
			{
#ifdef HAVE_DIAGS
				my_diag("draw 2D relative\n");
#endif

				vect3(2*my_x,2*my_y,2*my_z,
					2*(my_x + ptr_sub -> arg1),
					2*(my_y + ptr_sub -> arg2),2*my_z);

				my_x += ptr_sub -> arg1;
				my_y += ptr_sub -> arg2;
				sub_counter++;
				break;
			}

			case 18 : /*move 3D absolute*/
			{
#ifdef HAVE_DIAGS
				my_diag("move 3D absolute\n");
#endif
				my_x = ptr_sub -> arg1;
				my_y = ptr_sub -> arg2;
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);
				my_z = ptr_sub -> arg1;
				sub_counter++;
				break;
			}

			case 19 : /*draw 3D absolute*/
			{
#ifdef HAVE_DIAGS
				my_diag("draw 3D absolute\n");
#endif
				xx = ptr_sub -> arg1;
				yy = ptr_sub -> arg2;
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);
				zz = ptr_sub -> arg1;
				vect3(2*my_x,2*my_y,2*my_z,2*xx,2*yy,2*zz);

				my_x = xx;
				my_y = yy;
				my_z = zz;
				sub_counter++;
				break;
			}

			case 20 : /*move 3D relative*/
			{
#ifdef HAVE_DIAGS
				my_diag("move 3D relative\n");
#endif
				my_x += ptr_sub -> arg1;
				my_y += ptr_sub -> arg2;
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);
				my_z += ptr_sub -> arg1;
				sub_counter++;
				break;
			}

			case 21 : /*draw 3D relative*/
			{
#ifdef HAVE_DIAGS
				my_diag("draw 3D relative\n");
#endif
				xx = ptr_sub -> arg1;
				yy = ptr_sub -> arg2;
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);
				zz = ptr_sub -> arg1;
				vect3(2*my_x,2*my_y,2*my_z,
					2*(my_x + xx),2*(my_y + yy),
						2*(my_z + zz));

				my_x += xx;
				my_y += yy;
				my_z += zz;
				sub_counter++;
				break;
			}

			case 22 : /*short relative vectors*/
			{
#ifdef HAVE_DIAGS
				my_diag("SHORT RELATIVE VECTORS\n");
#endif
				flag = (unsigned char)ptr_sub -> arg2;

				if((flag & 0x08) != 0) /*then draw*/
					vect3(2*my_x,2*my_y,2*my_z,
						2*(my_x + ptr_sub -> arg3),
						2*(my_y + ptr_sub -> arg4),
						2*my_z);


				my_x += ptr_sub -> arg3;
				my_y += ptr_sub -> arg4;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				while(ptr_sub -> op_code == 30)
				{
					flag = ptr_sub -> flag1;

					if((flag & 0x08) != 0) /*then draw*/
						vect3(2*my_x,2*my_y,2*my_z,
							2*(my_x + ptr_sub -> arg1),
							2*(my_y + ptr_sub -> arg2),
							2*my_z);


					my_x += ptr_sub -> arg1;
					my_y += ptr_sub -> arg2;

					flag = ptr_sub -> flag2;

					if(flag != 0x40)
					{
						if((flag & 0x08) != 0) /*then draw*/
							vect3(2*my_x,2*my_y,
								2*my_z,
							    2*(my_x + ptr_sub -> arg3),
							    2*(my_y + ptr_sub -> arg4),
							    2*my_z);


						my_x += ptr_sub -> arg3;
						my_y += ptr_sub -> arg4;
					}

					sub_counter++;
					get_sub_value(sub_counter,this_sub_num);
				}

				break;
			}

			case 23 : /*charactor string*/
			{
#ifdef HAVE_DIAGS
				my_diag("charactor string\n");
#endif

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				i = 2;
				while(ptr_sub -> op_code == 27)
				{
					sub_counter++;
					get_sub_value(sub_counter,this_sub_num);

					i += 4;
				}

				break;
			}

			case 24 : /*insert dash pattern*/
			{
#ifdef HAVE_DIAGS
				my_diag("insert dash pattern\n");
#endif

				my_dash_pattern(ptr_sub -> arg1);
				sub_counter++;
				break;
			}

			case 50 : /*compressed data*/
			{
#ifdef HAVE_DIAGS
				sprintf(diag_string,"compressed data in subroutine %d @ %d\n",
						sub_nums[this_sub_num],sub_counter);
				my_diag(diag_string);
#endif
				sub_counter++;
				break;
			}

			case 44 : /*null word*/
			{
				sub_counter++;
				break;
			}
			
			case 60 : /*don't draw*/
			{
#ifdef HAVE_DIAGS
				my_diag("OP CODE 60 (don't draw)\n");
#endif

				no_draw_flag = 't';
				wrmask(0);
				sub_counter++;
				break;
			}

			case 70 : /*do draw*/
			{
#ifdef HAVE_DIAGS
				my_diag("OP CODE 70 (do draw)\n");
#endif

				no_draw_flag = 'f';
				wrmask(io_masks[which_page]);
				sub_counter++;
				break;
			}


			case 90 : /*locate joystick*/
			{
				if(which_redis == 1)
				{
					sub_counter++;
					quit_redis = 't';
					break;
				}


				locate_called = 't';
				locate_seg = which_cseg;

				if(pen_press2 == 'f')
				{
					my_x = save_joy_x;
					my_y = save_joy_y;
				}
				else
				{
					my_x = (int)(virtual_size*(pen2_x -
						graphics_col)/
						actual_size) - 2048;
					my_y = (int)(virtual_size*(pen2_y -
						graphics_row)/
						actual_size) - 2048;
				/*
					new_x = old_x = pen2_x;
					new_y = old_y = pen2_y;
				*/
					pen_press2 = 'f';
				}


				move3(2*my_x,2*my_y,2*my_z);


				if(is_a_seg_open == 't')  
			/*don't draw incoming host data*/
				{
                                        wrmask(0);

					pw_dl_init(&(sub_struct_ptr ->
						sub_calling_table[1][0]),
						0,NULL,0);
					call(&(sub_struct_ptr ->
						our_versions_of_subs[
						NO_DRAW_SUB1][0]));
					pw_dl_init(NULL,0,NULL,0);
				}
				else   /*do draw incoming host data*/
				{
					if(draw_black == 'f')
						draw_mode(3);

                                        wrmask(io_masks[which_page]);

					pw_dl_init(&(sub_struct_ptr ->
						sub_calling_table[1][0]),
						0,NULL,0);

					if(which_page == 0)
						call(&(sub_struct_ptr ->
							our_versions_of_subs[
							PAGE1_SUB1][0]));
					else
						call(&(sub_struct_ptr ->
							our_versions_of_subs[
							PAGE2_SUB1][0]));

					pw_dl_init(NULL,0,NULL,0);
				}


				sub_counter++;
				break;
			}


			default :
			{
				sprintf(diag_string,"Invalid op_code %d in the subroutine interpretor\n",
					ptr_sub -> op_code);
				error_logger(diag_string);
				sub_counter++;
			}
		}  /* end switch */


	}   /* end while 1==1 */


}


