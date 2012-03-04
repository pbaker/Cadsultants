/* MY11.C  the compiled curser subroutine interpretor */


/*January 31, 1989 changed the 1 error message to go to the error file*/

#include "my_globs.h"
#include "my_utils.h"
#include "c:\emulate\clipper\pw_lib.h"


int far unity_viewing_matrix[16]  = 
	{
		0x800,0,0,0,
		0,0x800,0,0,
		0,0,0x800,0,
		0,0,0,1
	};


extern int far io_masks[];


void far interp_my_sub6()
{
int sub_counter = 0;
unsigned char command;
int this_sub_num = NUMBER_OF_SUBS;
int i;
int save_x,save_y;


	no_draw_flag = 'f';
	wrmask(io_masks[which_page]);
	save_x = my_x;
	save_y = my_y;
	pushvw3();
	vwmatx3(&unity_viewing_matrix[0]);

	my_set_viewport(-2048,-2048,2048,2048,which_page);



	while(1==1)
	{
		if(sub_counter > sub_insert_ptrs[this_sub_num])
		{
			popvw3();

			return;
		}



		get_sub_value(sub_counter,this_sub_num);
		command = ptr_sub -> op_code;
		

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
				vect3(2*my_x,2*my_y,2*my_z,
					2*(ptr_sub -> arg1),
					2*(my_y + ptr_sub -> arg2),
					2*my_z);

				my_x = ptr_sub -> arg1;
				my_y += ptr_sub -> arg2;
				sub_counter++;
				break;
			}

			case 83 : /*draw rel dx, abs y*/
			{
				vect3(2*my_x,2*my_y,2*my_z,
					2*(my_x + ptr_sub -> arg1),
					2*(ptr_sub -> arg2),
					2*my_z);

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
				vect3(2*my_x,2*my_y,2*my_z,
					2*(ptr_sub -> arg1),
					2*(ptr_sub -> arg2),
					2*my_z);

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
					2*(my_y + ptr_sub -> arg2),
					2*my_z);

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

				vect3(2*my_x,2*my_y,2*my_z,2*(my_x + xx),
					2*(my_y + yy),2*(my_z + zz));

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
				my_3d_viewing_matrix[0] = ptr_sub -> arg1;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);
				my_3d_viewing_matrix[4] = ptr_sub -> arg1;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				my_3d_viewing_matrix[8] = 0;
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);
				my_3d_viewing_matrix[12] = ptr_sub -> arg1;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);
				my_3d_viewing_matrix[1] = ptr_sub -> arg1;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);
				my_3d_viewing_matrix[5] = ptr_sub -> arg1;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				my_3d_viewing_matrix[9] = 0;
				sub_counter++;
				get_sub_value(sub_counter,this_sub_num);
				my_3d_viewing_matrix[13] = ptr_sub -> arg1;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"call sub: ins 2D matrix %d\n",
					ptr_sub -> arg1);
				my_diag(diag_string);
			#endif
				sub_counter++;
				vwmatx3(&my_3d_viewing_matrix[0]);

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

				error_logger("CURSOR SUB charactor string\n");
				sub_counter++;

				break;
			}

			case 24 : /*insert dash pattern*/
			{
			#ifdef HAVE_DIAGS
				my_diag("insert dash pattern\n");
			#endif
			/*	usrlin(ptr_sub -> arg1);
				sltyp(line_type);
			*/
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