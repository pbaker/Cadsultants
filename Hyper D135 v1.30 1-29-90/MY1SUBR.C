/* MY1SUBR.C   contains subroutine insert/retrieve  functions */




#include "my_globs.h"
#include "c:\emulate\clipper\pw_lib.h"
#include "dos.h"
#include "c:\emulate\clipper\expanded.h"

#define RES_1024_768 3
#define RES_1280_1024 4
#define MAX_MEGATEK 64


extern char far which_high_res;
extern char far gtext_string[];




void far get_sub_value(index,which_of_ours)
int index;
int which_of_ours;
{

 	if((which_of_ours < NUMBER_OF_SUBS) && (which_of_ours >= 0))
	{
		switch(which_of_ours)
		{
			case 3 :
			{
				ptr_sub = &our_sub3[index];
				break;
			}

			case 6 :
			{
				ptr_sub = &our_sub6[index];
				break;
			}

			case 7 :
			{
				ptr_sub = &our_sub7[index];
				break;
			}

			default :
			{
				sprintf(diag_string,
					"Illegal Subroutine #: %d in get_sub_value()\n",
						which_of_ours);
				error_logger(diag_string);
			}
		}
	}	
	else if(which_of_ours == NUMBER_OF_SUBS)
		ptr_sub = &my_sub6[index];
	else
	{
		sprintf(diag_string,"Illegal Subroutine #: %d in get_sub_value()\n",
			which_of_ours);
		error_logger(diag_string);
	}
}






void far insert_sub(value,which_sub)
struct our_element value;
int which_sub;
{
int i,x1,x2,y1,y2;
int real_sub,the_insert_ptr;


	if(which_sub < NUMBER_OF_SUBS)
		real_sub = sub_nums[which_sub];
	else
		real_sub = NUMBER_OF_SUBS;


	switch(real_sub)
	{
		case 3 :
		{
			sub_insert_ptrs[3]++;
			if(sub_insert_ptrs[3] >= SUB_NUM_ELS)
			{
				sprintf(diag_string,
					"subroutine # %d is full\n",3);
				error_logger(diag_string);
				sub_insert_ptrs[3]--;
				return;
			}

			the_insert_ptr = sub_insert_ptrs[3];
			our_sub3[the_insert_ptr].op_code = value.op_code;
			our_sub3[the_insert_ptr].arg1 = value.arg1;
			our_sub3[the_insert_ptr].arg2 = value.arg2;
			our_sub3[the_insert_ptr].flag1 = value.flag1;
			our_sub3[the_insert_ptr].flag2 = value.flag2;
			our_sub3[the_insert_ptr].arg3 = value.arg3;
			our_sub3[the_insert_ptr].arg4 = value.arg4;

			break;
		}


		case 6 :
		{
			sub_insert_ptrs[6]++;
			if(sub_insert_ptrs[6] >= SUB_NUM_ELS)
			{
				sprintf(diag_string,
					"subroutine # %d is full\n",6);
				error_logger(diag_string);
				sub_insert_ptrs[6]--;
				return;
			}

			the_insert_ptr = sub_insert_ptrs[6];
			our_sub6[the_insert_ptr].op_code = value.op_code;
			our_sub6[the_insert_ptr].arg1 = value.arg1;
			our_sub6[the_insert_ptr].arg2 = value.arg2;
			our_sub6[the_insert_ptr].flag1 = value.flag1;
			our_sub6[the_insert_ptr].flag2 = value.flag2;
			our_sub6[the_insert_ptr].arg3 = value.arg3;
			our_sub6[the_insert_ptr].arg4 = value.arg4;

			break;
		}

		case 7 :
		{
			sub_insert_ptrs[7]++;
			if(sub_insert_ptrs[7] >= SUB7_NUM_ELS)
			{
				sprintf(diag_string,
					"subroutine # %d is full\n",7);
				error_logger(diag_string);
				sub_insert_ptrs[7]--;
				return;
			}

			the_insert_ptr = sub_insert_ptrs[7];
			our_sub7[the_insert_ptr].op_code = value.op_code;
			our_sub7[the_insert_ptr].arg1 = value.arg1;
			our_sub7[the_insert_ptr].arg2 = value.arg2;
			our_sub7[the_insert_ptr].flag1 = value.flag1;
			our_sub7[the_insert_ptr].flag2 = value.flag2;
			our_sub7[the_insert_ptr].arg3 = value.arg3;
			our_sub7[the_insert_ptr].arg4 = value.arg4;

			break;
		}



		case NUMBER_OF_SUBS :
		{
			sub_insert_ptrs[real_sub]++;
			if(sub_insert_ptrs[real_sub] >= SUB7_NUM_ELS)
			{
				sprintf(diag_string,
					"subroutine # %d is full\n",real_sub);
				error_logger(diag_string);
				sub_insert_ptrs[real_sub]--;
				return;
			}

			the_insert_ptr = sub_insert_ptrs[real_sub];
			my_sub6[the_insert_ptr].op_code = value.op_code;
			my_sub6[the_insert_ptr].arg1 = value.arg1;
			my_sub6[the_insert_ptr].arg2 = value.arg2;
			my_sub6[the_insert_ptr].flag1 = value.flag1;
			my_sub6[the_insert_ptr].flag2 = value.flag2;
			my_sub6[the_insert_ptr].arg3 = value.arg3;
			my_sub6[the_insert_ptr].arg4 = value.arg4;

			break;
		}
	


		default :
		{
			sub_insert_ptrs[real_sub]++;
			if(sub_insert_ptrs[real_sub] >= 2*SUB_NUM_ELS)
			{
				sprintf(diag_string,
					   "subroutine # %d is full\n",
						real_sub);
				error_logger(diag_string);

				sub_insert_ptrs[real_sub]--;

				return;
			}

			sub_host_to_pix_ptrs[real_sub]
				[sub_insert_ptrs[real_sub]] = 
					current_sub_pointer;
			pw_dl_init(current_sub_pointer,0,NULL,0);


			switch(value.op_code)    /*inserts into sub display list*/
			{
				case 0 : /*call sub*/
				{
					call(&(sub_calling_table[value.arg1]
						[0]));

					break;
				}

				case 1 : /*insert blink enable*/
				{
					noop();noop();noop();
					break;
				}

				case 2 : /*insert dash enable*/
				{
					linpat(value.arg1 & 0x01);
					break;
				}

				case 3 : /*insert blink rate*/
				{
					noop();noop();noop();
					break;
				}

				case 4 : /*insert intensity/color*/
				{
					call(&(color_sub_table[
						value.arg1 & 0x0f][0]));

					break;
				}

				case 5 : /*insert color*/
				{
					break;
				}

				case 6 : /*insert Xform enable*/
				{
					noop();noop();noop();

					break;
				}

				case 7 : /*insert 2d origin*/
				{
					sprintf(diag_string,
						"INSERT_SUB: insert 2d origin: not implemented\n");
					error_logger(diag_string);
					break;
				}

				case 8 : /*insert 3d origin*/
				{
					sprintf(diag_string,
						"INSERT_SUB: insert 3d origin: not implemented\n");
					error_logger(diag_string);
					break;
				}

				case 9 : /*insert z origin*/
				{
					sprintf(diag_string,
						"INSERT_SUB: insert z origin: not implemented\n");
					error_logger(diag_string);
					break;
				}

				case 10 : /*insert 2d Xlate*/
				{
					noop();noop();noop();
					break;
				}

				case 11 : /*insert 2d matrix*/
				{
					vwmatx3(&my_3d_viewing_matrix[0]);

					break;
				}

				case 12 : /*insert 3d matrix*/
				{
					vwmatx3(&my_3d_viewing_matrix[0]);

					break;
				}

				case 13 : /*insert clipping*/
				{
				    if(which_high_res == RES_1024_768)
				    {
					x1 = (int)(value.arg1/5.33333333333333)
						+ 512;
					x2 = (int)(value.arg3/5.33333333333333)
						+ 512;
					y1 = (int)((value.arg2 + 2048)/5.33333);
					y2 = (int)((value.arg4 + 2048)/5.33333);
				    }
				    else
				    {
					x1 = (int)(value.arg1*1.25/5.33333333333)
						 + 640;
					x2 = (int)(value.arg3*1.25/5.33333333333)
						 + 640;
					y1 = (int)((value.arg2 + 2048)/5.12*1.28);
					y2 = (int)((value.arg4 + 2048)/5.12*1.28);
				    }

					window(value.arg1,value.arg3,
						value.arg2,value.arg4);
					viewport(x1,x2,y1,y2);

					break;
				}

				case 14 : /*move 2d absolute*/
				{
					move3(2*value.arg1,2*value.arg2,0);
					break;
				}

				case 15 : /*draw 2d absolute*/
				{
					draw3(2*value.arg1,2*value.arg2,0);
					break;
				}

				case 16 : /*move 2d relative*/
				{
					move3r(2*value.arg1,2*value.arg2,0);
					break;
				}

				case 17 : /*draw 2d relative*/
				{
					draw3r(2*value.arg1,2*value.arg2,0);
					break;
				}

				case 18 : /*move 3d absolute*/
				{
					move3(2*value.arg1,2*value.arg2,
						2*value.arg3);
					break;
				}

				case 19 : /*draw 3d absolute*/
				{
					draw3(2*value.arg1,2*value.arg2,
						2*value.arg3);
					break;
				}

				case 20 : /*move 3d relative*/
				{
					move3r(2*value.arg1,2*value.arg2,
						2*value.arg3);
					break;
				}

				case 21 : /*draw 3d relative*/
				{
					draw3r(2*value.arg1,2*value.arg2,
						2*value.arg3);
					break;
				}

				case 22 : /*first word of SRV*/
				{
					if((value.arg2 & 0x08) != 0) /*a draw*/
					{
						draw3r(2*value.arg3,
							2*value.arg4,0);
					}
					else
						move3r(2*value.arg3,
							2*value.arg4,0);

					break;
				}

				case 30 : /*SRV words after the 1st word*/
				{
					if(value.flag1 == 0x40)  /*the end*/
						break;

					if((value.flag1 & 0x08) != 0) /*a draw*/
					{
						draw3r(2*value.arg1,
							2*value.arg2,0);
					}
					else
						move3r(2*value.arg1,
							2*value.arg2,0);

					if(value.flag2 != 0x40)
					{
						if((value.flag2 & 0x08) != 0)
						{  /*its a draw*/
							draw3r(2*value.arg3,
								2*value.arg4,0);
						}
						else
							move3r(2*value.arg3,
								2*value.arg4,0);
					}

					break;
				}



				case 23 : /*1st word of charactor string*/
				{	
					if(gtext_string[0] != '\0')
				    	/*1st 23 for this character string*/
					{
					   	if(gtext_string[0] == '.')
						{
							move3r(32,0,0);

							draw3r(4,0,0);
							move3r(0,4,0);
							draw3r(-4,0,0);

							move3r(64,-4,0);
						}
						else if(gtext_string[0] == '+')
						{

						}
						else if(gtext_string[0] == '*')
						{

						}
						else
						{
							gtext(gtext_string);
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
					switch(value.arg1)
					{
						case 0xffff : /*solid*/
						{
							slpat32(1,0xffffffff);
							break;
						}

						case 0xeeee : /*dashed*/
						{
							slpat32(1,0xf0f0f0f0);
							break;
						}

						case 0xffb6 : /*phantom*/
						{
							slpat32(1,0xfff0f0f0);
							break;
						}

						case 0xfff6 : /*centerline*/
						{
							slpat32(1,0xfffff0f0);
							break;
						}
					}

					break;
				}


			}  /*end switch(value.op_code)*/


			pw_dl_rdptr(&current_sub_pointer);
			pw_dl_init(NULL,0,NULL,0);

		}  /* end of default case */

	}  /* end switch(real_sub) */

}



