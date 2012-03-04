/*January 31, 1989 changed the 1 error message to go to the error file*/

#include "my_globs.h"
#include "my_utils.h"



void far interp_my_sub6()

{
int sub_counter = 0;
unsigned char command;
int this_sub_num = NUMBER_OF_SUBS;
int i;
int save_x,save_y;
float save_hcrst[4][4];

	no_draw_flag = 'f';
	save_x = my_x;
	save_y = my_y;


        save_hcrst[0][0] = hcrst[0][0];
        save_hcrst[1][0] = hcrst[1][0];
        save_hcrst[2][0] = hcrst[2][0];
        save_hcrst[3][0] = hcrst[3][0];
        save_hcrst[0][1] = hcrst[0][1];
        save_hcrst[1][1] = hcrst[1][1];
        save_hcrst[2][1] = hcrst[2][1];
        save_hcrst[3][1] = hcrst[3][1];


	hcrst[0][0] = 1.0;
	hcrst[1][0] = 0.0;
	hcrst[2][0] = 0.0;
	hcrst[3][0] = 0.0;
	hcrst[0][1] = 0.0;
	hcrst[1][1] = 1.0;
	hcrst[2][1] = 0.0;
	hcrst[3][1] = 0.0;

	if(num_pages == 2)
	{
		my_set_viewport(-2048,-2048,2048,2048,which_page);
		my_lock_viewport();
	}
	else
	{
		my_set_viewport(-2048,-2048,2048,2048,0);
		my_lock_viewport();
	}

	while(1==1)
	{
		get_sub_value(sub_counter,this_sub_num);
		command = ptr_sub -> op_code;
		
		if(sub_counter > sub_insert_ptrs[NUMBER_OF_SUBS])
		{
		    if(num_pages == 2)
		    {
			my_set_viewport(save_low_x_clip,
				save_low_y_clip,
				save_up_x_clip,
				save_up_y_clip,
				which_page);
			my_lock_viewport();
		    }
		    else
		    {
			my_set_viewport(save_low_x_clip,
				save_low_y_clip,
				save_up_x_clip,
				save_up_y_clip,0);
			my_lock_viewport();
		    }

                        hcrst[0][0] = save_hcrst[0][0];
                        hcrst[1][0] = save_hcrst[1][0];
                        hcrst[2][0] = save_hcrst[2][0];
                        hcrst[3][0] = save_hcrst[3][0];
                        hcrst[0][1] = save_hcrst[0][1];
                        hcrst[1][1] = save_hcrst[1][1];
                        hcrst[2][1] = save_hcrst[2][1];
                        hcrst[3][1] = save_hcrst[3][1];


			return;
		}

		switch(command)
		{
			case 90 : /*locate*/
			{
				my_x = save_x;
				my_y = save_y;
				sub_counter++;
				break;
			}

			case 80 : /*move abs x, rel dy*/
			{
				my_x = ptr_sub -> arg1;
				my_y += ptr_sub -> arg2;
				sub_counter++;
				break;
			}

			case 81 : /*move rel dx, abs y*/
			{
				my_x += ptr_sub -> arg1;
				my_y = ptr_sub -> arg2;
				sub_counter++;
				break;
			}

			case 82 : /*draw abs x, rel dy*/
			{
				my_line3(my_x,my_y,my_z,
					ptr_sub -> arg1,my_y + ptr_sub -> arg2,
					my_z);

				my_x = ptr_sub -> arg1;
				my_y += ptr_sub -> arg2;
				sub_counter++;
				break;
			}

			case 83 : /*draw rel dx, abs y*/
			{
				my_line3(my_x,my_y,my_z,
					my_x + ptr_sub -> arg1,ptr_sub -> arg2,
					my_z);

				my_x += ptr_sub -> arg1;
				my_y = ptr_sub -> arg2;
				sub_counter++;
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
					ptr_sub -> arg1,ptr_sub -> arg2,
					my_z);

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
					my_y + ptr_sub -> arg2,
					my_z);
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
			/*	if((ptr_sub -> arg1 & 1) != 0) enable
				{
					line_type = 4;
					my_dash_enable(4);
				}
				else
				{
					line_type = 0;
					my_dash_enable(0);
				}
                        */
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
				my_color((char) ptr_sub -> arg1 & 0x0f);

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
				sub_counter++;
				break;
			}

			case 8 : /*insert 3D origin*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert 3D origin\n");
			#endif
				sub_counter += 2;
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
				sub_counter += 2;
				break;
			}

			case 11 : /*insert 2D matrix*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert 2D matrix\n");
			#endif
				hcrst[0][0] =
					(ptr_sub -> arg1/2048.0);
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[1][0] =
					(ptr_sub -> arg1/2048.0);
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif

				hcrst[2][0] = 0.0;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[3][0] =
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
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[1][1] =
					(ptr_sub -> arg1/2048.0);
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				hcrst[2][1] = 0.0;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[3][1] =
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

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[1][0] = /*M1*/
					(ptr_sub -> arg1/2048.0);

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[3][0] = /*M2*/
					(float)ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[0][1] = /*M3*/
					(ptr_sub -> arg1/2048.0);

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[1][1] = /*M4*/
					(ptr_sub -> arg1/2048.0);

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[3][1] = /*M5*/
					(float)ptr_sub -> arg1;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[2][0] = /*M6*/
					(ptr_sub -> arg1/2048.0);

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				hcrst[2][1] = /*M7*/
					(ptr_sub -> arg1/2048.0);

				sub_counter++;

				break;
			}

			case 13 : /*insert clipping*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert clipping\n");
			#endif
				sub_counter += 2;
				break;
			}


			case 22 : /*short relative vectors*/
			{
			#ifdef HAVE_DIAGS
				my_diag("SHORT RELATIVE VECTORS\n");
			#endif
				flag = (unsigned char)ptr_sub -> arg2;

				if((flag & 0x08) != 0) /*then draw*/
					my_line3(my_x,my_y,
						headers_segs[which_cseg].
						z_origin,
						my_x + ptr_sub -> arg3,
						my_y + ptr_sub -> arg4,
						headers_segs[which_cseg].
						z_origin);

				my_x += ptr_sub -> arg3;
				my_y += ptr_sub -> arg4;

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				while(ptr_sub -> op_code == 30)
				{
					flag = ptr_sub -> flag1;

					if((flag & 0x08) != 0) /*then draw*/
						my_line3(my_x,my_y,
							headers_segs[which_cseg].
							z_origin,
							my_x + ptr_sub -> arg1,
							my_y + ptr_sub -> arg2,
							headers_segs[which_cseg].
							z_origin);

					my_x += ptr_sub -> arg1;
					my_y += ptr_sub -> arg2;

					flag = ptr_sub -> flag2;

					if(flag != 0x40)
					{
						if((flag & 0x08) != 0) /*then draw*/
							my_line3(my_x,my_y,
							    headers_segs[which_cseg].
							    z_origin,
							    my_x + ptr_sub -> arg3,
							    my_y + ptr_sub -> arg4,
							    headers_segs[which_cseg].
							    z_origin);

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
					my_char((char)ptr_sub -> arg4,my_x + 8,
						my_y,0,6);

				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);

				i = 2;
				while(ptr_sub -> op_code == 27)
				{
					my_char((char)ptr_sub -> arg1,my_x + i*8,
						my_y,0,6);

					if(ptr_sub -> arg2 != 0)
						my_char((char)ptr_sub -> arg2,
							my_x + (i + 1)*8,my_y,0,6);

					if(ptr_sub -> arg3 != 0)
						my_char((char)ptr_sub -> arg3,
							my_x + (i + 2)*8,my_y,0,6);

					if(ptr_sub -> arg4 != 0)
						my_char((char)ptr_sub -> arg4,
							my_x + (i + 3)*8,my_y,0,6);

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
			/*	usrlin(ptr_sub -> arg1);
				sltyp(line_type);*/
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
				no_draw_flag = 't';
				sub_counter++;
				break;
			}

			case 70 : /*do draw*/
			{
				no_draw_flag = 'f';
				sub_counter++;
				break;
			}


			default :
			{
				sprintf(diag_string,"Invalid op_code %d in the my_sub_6 interpretor\n",
					ptr_sub -> op_code);
				error_logger(diag_string);
				sub_counter++;
			}
		}  /* end switch */


	}   /* end while 1==1 */


}
