/*October 14, 1988 3:20 fixed svwpt() to use headers_segs[which_cseg].
save_*_*_clip instead of headers_segs[which_cseg].clip_*_*      */

/*November 15, 1988 made changes to use generic graphics board calls*/

/*November 21, 1988 Made dash enable/disable set header's dash_enable flag*/

/*January 31, 1989 Made change to send the 1 error message to the error file*/

#include "my_globs.h"
#include "my_utils.h"


extern void far e_func();

extern char far delete_locate_seg;
extern int far my_invert_y;



void far interp_sub(this_sub_num)
int this_sub_num;
{
int sub_counter = 0;
unsigned char command;
int dummy,i,j;

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


		switch(command)
		{
			case 0 :  /*call subroutine*/
			{
				fill_sub = 'f';
				our_call = 's';
				e_func(&dummy,dummy);
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
					headers_segs[which_cseg].dash_enable
						= 't';

					line_type = 4;
					my_dash_enable(4);
				}
				else
				{
					headers_segs[which_cseg].dash_enable
						= 'f';

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

				if((rw_sub_w_0_1 == 't') && (black == 't'))
					my_color((char)0);
				else if((draw_black == 'f') && (black == 'f'))
					my_color((char) ptr_sub -> arg1 & 0x0f);
				else   /*draw in black Except for locate segs*/
				{
					if(delete_locate_seg == 'f')
						my_color((char)0);
					else
						my_color((char) ptr_sub -> arg1
							 & 0x0f);
				}


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
					headers_segs[which_cseg].Xform_enable
						= 't';
				}
				else
				{
				#ifdef HAVE_DIAGS
					my_diag("XFORM DISABLE\n");
				#endif
					headers_segs[which_cseg].Xform_enable
						= 'f';
				}

				sub_counter++;
				break;
			}

			case 7 : /*insert 2D origin*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert 2D origin\n");
			#endif
				headers_segs[which_cseg].x_origin =
					ptr_sub -> arg1;

				headers_segs[which_cseg].y_origin =
					ptr_sub -> arg2;

				sub_counter++;
				break;
			}

			case 8 : /*insert 3D origin*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert 3D origin\n");
			#endif
				headers_segs[which_cseg].x_origin =
					ptr_sub -> arg1;

				headers_segs[which_cseg].y_origin =
					ptr_sub -> arg2;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				headers_segs[which_cseg].z_origin =
					ptr_sub -> arg1;

				sub_counter++;
				break;
			}

			case 9 : /*insert Z origin*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert Z origin\n");
			#endif
				headers_segs[which_cseg].z_origin =
					ptr_sub -> arg1;

				sub_counter++;
				break;
			}

			case 10 : /*insert 2D Xlate*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert 2D Xlate\n");
			#endif
				hcrst[3][0] =
					(float)ptr_sub -> arg1;
				headers_segs[which_cseg].save_rotate3D[3][0] =
					(float)ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[3][1] =
					(float)ptr_sub -> arg1;
				headers_segs[which_cseg].save_rotate3D[3][1] =
					(float)ptr_sub -> arg1;


				sub_counter++;
				break;
			}

			case 11 : /*insert 2D matrix*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert 2D matrix\n");
			#endif
				hcrst[0][0] =
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[0][0] =
					ptr_sub -> arg1/2048.0;

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[1][0] =
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[1][0] =
					ptr_sub -> arg1/2048.0;

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif

				hcrst[2][0] = 0.0;
				headers_segs[which_cseg].save_rotate3D[2][0] =
					0.0;



				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[3][0] =
					(float)ptr_sub -> arg1;
				headers_segs[which_cseg].save_rotate3D[3][0] =
					(float)ptr_sub -> arg1;

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[0][1] =
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[0][1] =
					ptr_sub -> arg1/2048.0;

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[1][1] =
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[1][1] =
					ptr_sub -> arg1/2048.0;

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				hcrst[2][1] = 0.0;
				headers_segs[which_cseg].save_rotate3D[2][1] =
					0.0;


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[3][1] =
					(float)ptr_sub -> arg1;
				headers_segs[which_cseg].save_rotate3D[3][1] =
					(float)ptr_sub -> arg1;

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
				hcrst[0][0] = /*M0*/
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[0][0] =
					ptr_sub -> arg1/2048.0;


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[1][0] = /*M1*/
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[1][0] =
					ptr_sub -> arg1/2048.0;


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[3][0] = /*M2*/
					(float)ptr_sub -> arg1;
				headers_segs[which_cseg].save_rotate3D[3][0] =
					(float)ptr_sub -> arg1;


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[0][1] = /*M3*/
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[0][1] =
					ptr_sub -> arg1/2048.0;


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[1][1] = /*M4*/
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[1][1] =
					ptr_sub -> arg1/2048.0;


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[3][1] = /*M5*/
					(float)ptr_sub -> arg1;
				headers_segs[which_cseg].save_rotate3D[3][1] =
					(float)ptr_sub -> arg1;


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[2][0] = /*M6*/
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[2][0] =
					ptr_sub -> arg1/2048.0;


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[2][1] = /*M7*/
					(ptr_sub -> arg1/2048.0);
				headers_segs[which_cseg].save_rotate3D[2][1] =
					ptr_sub -> arg1/2048.0;


				sub_counter++;

				break;
			}

			case 13 : /*insert clipping*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert clipping\n");
			#endif
				

				headers_segs[which_cseg].save_x_lower_clip =
					ptr_sub -> arg1;
				
				headers_segs[which_cseg].save_y_lower_clip =
					ptr_sub -> arg2;


				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);


				headers_segs[which_cseg].save_x_upper_clip =
					ptr_sub -> arg1;

				headers_segs[which_cseg].save_y_upper_clip =
					ptr_sub -> arg2;


				sub_counter++;


				save_low_x_clip = headers_segs[which_cseg].
					save_x_lower_clip;
				save_low_y_clip = headers_segs[which_cseg].
					save_y_lower_clip;
				save_up_x_clip = headers_segs[which_cseg].
					save_x_upper_clip;
				save_up_y_clip = headers_segs[which_cseg].
					save_y_upper_clip;

			    if(num_pages == 2)
			    {
				my_set_viewport(headers_segs[which_cseg].save_x_lower_clip,
				      headers_segs[which_cseg].save_y_lower_clip,
				      headers_segs[which_cseg].save_x_upper_clip,
				      headers_segs[which_cseg].save_y_upper_clip,
					  which_page);

				my_lock_viewport();
			    }
			    else
			    {
				my_set_viewport(headers_segs[which_cseg].save_x_lower_clip,
				      headers_segs[which_cseg].save_y_lower_clip,
				      headers_segs[which_cseg].save_x_upper_clip,
				      headers_segs[which_cseg].save_y_upper_clip
					,0);

				my_lock_viewport();
			    }

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

				my_line3(my_x,my_y,my_z,
					ptr_sub -> arg1,ptr_sub -> arg2,my_z);

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

				my_line3(my_x,my_y,my_z,
					my_x + ptr_sub -> arg1,
					my_y + ptr_sub -> arg2,my_z);

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
				my_line3(my_x,my_y,my_z,xx,yy,zz);
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
				my_line3(my_x,my_y,my_z,my_x + xx,my_y + yy,my_z + zz);
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
					my_line3(my_x,my_y,my_z,
						my_x + ptr_sub -> arg3,
						my_y + ptr_sub -> arg4,my_z);

				my_x += ptr_sub -> arg3;
				my_y += ptr_sub -> arg4;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				while(ptr_sub -> op_code == 30)
				{
					flag = ptr_sub -> flag1;

					if((flag & 0x08) != 0) /*then draw*/
						my_line3(my_x,my_y,my_z,
							my_x + ptr_sub -> arg1,
							my_y + ptr_sub -> arg2,
							my_z);

					my_x += ptr_sub -> arg1;
					my_y += ptr_sub -> arg2;

					flag = ptr_sub -> flag2;

					if(flag != 0x40)
					{
						if((flag & 0x08) != 0) /*then draw*/
							my_line3(my_x,my_y,my_z,
							    my_x + ptr_sub -> arg3,
							    my_y + ptr_sub -> arg4,
							    my_z);

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
				my_char((char)ptr_sub -> arg3,my_x,my_y,0,6);
				if(ptr_sub -> arg4 != 0)
					my_char((char)ptr_sub -> arg4,my_x + 44,
						my_y,0,6);

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				i = 2;
				while(ptr_sub -> op_code == 27)
				{
				      my_char((char)ptr_sub -> arg1,my_x + i*44,
						my_y,0,6);

					if(ptr_sub -> arg2 != 0)
						my_char((char)ptr_sub -> arg2,
							my_x + (i + 1)*44,my_y
								,0,6);

					if(ptr_sub -> arg3 != 0)
						my_char((char)ptr_sub -> arg3,
							my_x + (i + 2)*44,my_y
								,0,6);

					if(ptr_sub -> arg4 != 0)
						my_char((char)ptr_sub -> arg4,
							my_x + (i + 3)*44,my_y
								,0,6);

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
				sub_counter++;
				break;
			}

			case 70 : /*do draw*/
			{
			#ifdef HAVE_DIAGS
				my_diag("OP CODE 70 (do draw)\n");
			#endif
				no_draw_flag = 'f';
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
					my_x = pen2_x;
					my_y = -pen2_y;
					new_x = old_x = pen2_x;
					new_y = old_y = pen2_y;
					pen_press2 = 'f';
				}

				if(draw_black == 'f')
					draw_mode(3);

				if(is_a_seg_open == 't')
					no_draw_flag = 't';
				else
					no_draw_flag = 'f';

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
