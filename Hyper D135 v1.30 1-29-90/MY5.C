/* MY5.C    2D & 3D  Move & Draw  Relative & Absolute  + Short Relatives &
	Character Strings */


/*October 10, 1988 12:26 added add an extra word when crossing a block
boundry code to charactor string function*/

/*February 8, 1989 added code to cap_X_func() (charactor string) so when the
last valid word is totally full it inserts an empty extra word to show where
the string ends & tried to fix how it breaks the string right*/

/*February 23, 1989 added code to cap_X_func() (character string)
so when the last word of the string is right before the jump words, it adds
a 44 word to it*/


/*March 22, 1989 fixed cap_X_func() for the 2 character only string case*/

/*March 23, 1989 fixed cap_X_func() and cap_Y_func() to use size_dl_ptr_element
in computing the current insert position, instead of size_an_element (for
deciding when to insert the extra word*/

/*March 27, 1989 added code to cap_X_func() to store the character string
data into gtext_string also to be used for inserting into a segment's display
list*/

	
#include "my_utils.h"
#include "my_globs.h"

char far char_last_word_full = 'f';
char far gtext_string[200];


void far cap_P_func(sub_count,which_subr) /*move 2D absolute*/
int *sub_count,which_subr;
{
	diag_num = 48;
	read_more(4);

	xx = comp_abs_coord(megatek_data[0],megatek_data[1]);
	yy = comp_abs_coord(megatek_data[2],megatek_data[3]);

	if(fill_sub == 'f')
	{
		my_x = xx;
		my_y = yy;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"move 2D abs %d %d\n",my_x,my_y);
		my_diag(diag_string);
	#endif

		an_element.op_code = 14;
		an_element.arg1 = my_x;
		an_element.arg2 = my_y;

		insert_ours(an_element,which_cseg);

	}
	else
	{

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"move 2D abs %d %d\n",xx,yy);
		my_diag(diag_string);
	#endif

		an_element.op_code = 14;
		an_element.arg1 = xx;
		an_element.arg2 = yy;

		insert_sub(an_element,which_sub);

	}

}




void far cap_R_func(sub_count,which_subr) /*move 2D relative*/
int *sub_count,which_subr;
{
	diag_num = 49;
	read_more(4);

	xx = comp_rel_coord(megatek_data[0],megatek_data[1]);
	yy = comp_rel_coord(megatek_data[2],megatek_data[3]);

	if(fill_sub == 'f')
	{
		my_x += xx;
		my_y += yy;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"move 2D rel %d %d\n",xx,yy);
		my_diag(diag_string);
	#endif

		an_element.op_code = 16;
		an_element.arg1 = xx;
		an_element.arg2 = yy;
	
		insert_ours(an_element,which_cseg);		
	}
	else
	{
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"move 2D rel %d %d\n",xx,yy);
		my_diag(diag_string);
	#endif

		an_element.op_code = 16;
		an_element.arg1 = xx;
		an_element.arg2 = yy;
	
		insert_sub(an_element,which_sub);

	}
}




void far cap_Y_func(sub_count,which_subr) /*short relative vectors*/
int *sub_count,which_subr;
{
int fill_ours,num_words;
char break_flag;
long current_pos;


	diag_num = 50;

	read_more(4);
	
	if(fill_sub == 'f')
	{
		size = megatek_data[0];
		flag = megatek_data[1];
		short_x = megatek_data[2];
		short_y = megatek_data[3];
		size &= 0x07;
		break_flag = 'f';
		fill_ours = 0;
		comp_xy();

		my_x += xx;
		my_y += yy;

		num_words = 1;
		break_flag = 'f';

		an_element.op_code = 22;
		an_element.arg1 = (int)size;
		an_element.arg2 = (int)flag;
		an_element.arg3 = xx;
		an_element.arg4 = yy;

		insert_ours(an_element,which_cseg);
		diag_num = 50;
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"short rel vectors Move/Draw to %d %d\n",xx,yy);
		my_diag(diag_string);
	#endif

		diag_num = -50;
	

		read_more(1),flag = megatek_data[0];
		if(flag == 0x40)
		{
			return; 
		}

		for( ; flag != 0x40 ; 
			read_more(1),flag = megatek_data[0])
		{
	
			read_more(2);

/*			sprintf(diag_string,"HOST DATA: FLAG = %x, DX = %x, DY = %x\n",
				flag,megatek_data[0],megatek_data[1]);
			my_diag(diag_string);
*/	
				short_x = megatek_data[0];
				short_y = megatek_data[1];

				comp_xy();

				my_x += xx;
				my_y += yy;

				if(fill_ours == 0)
				{
					num_words++;
					an_element.op_code = 30;
					an_element.flag1 = flag;
					an_element.arg1 = xx;
					an_element.arg2 = yy;
				}
				else
				{
					an_element.flag2 = flag;
					an_element.arg3 = xx;
					an_element.arg4 = yy;

					current_pos = my_ftell();

					if((current_pos + 2L) % 64L == 0L)
						break_flag = 't';
					else
						break_flag = 'f';


					if(break_flag == 't')
					{
						an_element.op_code = 44;
						insert_ours(an_element,which_cseg);
						an_element.op_code = 30;
						break_flag = 'f';
					}

					insert_ours(an_element,which_cseg);
				}

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"MORE... %d %d\n",xx,yy);
				my_diag(diag_string);
			#endif

				fill_ours = !fill_ours;
			
		}

		if(fill_ours != 0) /*write half a word*/
		{
			current_pos = my_ftell();

			if((current_pos + 2L) % 64L == 0L)
				break_flag = 't';
			else
				break_flag = 'f';


			if(break_flag == 't')
			{
				an_element.op_code = 44;
				insert_ours(an_element,which_cseg);
				an_element.op_code = 30;
				an_element.flag2 = 0x40;
				break_flag = 'f';
			}


			an_element.flag2 = 0x40;
			insert_ours(an_element,which_cseg);
		}


	}
	else  /* insert into a subroutine */
	{
		size = megatek_data[0];
		flag = megatek_data[1];
		short_x = megatek_data[2];
		short_y = megatek_data[3];

		size &= 0x07;

		fill_ours = 0;
		comp_xy();

		num_words = 1;

		an_element.op_code = 22;
		an_element.arg1 = (int)size;
		an_element.arg2 = (int)flag;
		an_element.arg3 = xx;
		an_element.arg4 = yy;

		insert_sub(an_element,which_sub);

		diag_num = 50;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"short rel vectors Move/Draw to %d %d\n",xx,yy);
		my_diag(diag_string);
	#endif

		diag_num = -50;
	
		read_more(1),flag = megatek_data[0];
		if(flag == 0x40)
		{
			return; 
		}

		for( ; flag != 0x40 ; 
			read_more(1),flag = megatek_data[0])
		{

			read_more(2);


				short_x = megatek_data[0];
				short_y = megatek_data[1];

				comp_xy();


				if(fill_ours == 0)
				{
					num_words++;
					an_element.op_code = 30;
					an_element.flag1 = flag;
					an_element.arg1 = xx;
					an_element.arg2 = yy;
				}
				else
				{
					an_element.flag2 = flag;
					an_element.arg3 = xx;
					an_element.arg4 = yy;

					insert_sub(an_element,which_sub);

				}

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"MORE... %d %d\n",xx,yy);
				my_diag(diag_string);
			#endif

				fill_ours = !fill_ours;
			
		}

		if(fill_ours != 0) /*write half a word*/
		{

			an_element.flag2 = 0x40;
			insert_sub(an_element,which_sub);

		}

	
	}


}




void far cap_Q_func(sub_count,which_subr) /*draw 2D absolute*/
int *sub_count,which_subr;
{
	diag_num = 51;

	read_more(4);

	xx = comp_abs_coord(megatek_data[0],megatek_data[1]);
	yy = comp_abs_coord(megatek_data[2],megatek_data[3]);

	if(fill_sub == 'f')
	{
		my_x = xx;
		my_y = yy;

		an_element.op_code = 15;
		an_element.arg1 = my_x;
		an_element.arg2 = my_y;

		insert_ours(an_element,which_cseg);

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"draw 2D Absolute %d %d\n",my_x,my_y);
		my_diag(diag_string);
	#endif

	}
	else
	{
		an_element.op_code = 15;
		an_element.arg1 = xx;
		an_element.arg2 = yy;

		insert_sub(an_element,which_sub);

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"draw 2D Absolute %d %d\n",my_x,my_y);
		my_diag(diag_string);
	#endif
	}
}





void far cap_S_func(sub_count,which_subr) /*draw 2D relative*/
int *sub_count,which_subr;
{
	diag_num = 52;

	read_more(4);

	xx = comp_rel_coord(megatek_data[0],megatek_data[1]);
	yy = comp_rel_coord(megatek_data[2],megatek_data[3]);

	if(fill_sub == 'f')
	{
		my_x += xx;
		my_y += yy;

		an_element.op_code = 17;
		an_element.arg1 = xx;
		an_element.arg2 = yy;

		insert_ours(an_element,which_cseg);

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"draw 2D Relative %d %d\n",xx,yy);
		my_diag(diag_string);
	#endif
	}
	else
	{
		an_element.op_code = 17;
		an_element.arg1 = xx;
		an_element.arg2 = yy;

		insert_sub(an_element,which_sub);

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"draw 2D Relative %d %d\n",xx,yy);
		my_diag(diag_string);
	#endif
	}
}






void far cap_X_func(sub_count,which_subr) /*use charactor string*/
int *sub_count,which_subr;
{
int i,j,start_loop;
long current_pos;
char break_flag;

	read_more(2);
	
	diag_num = 16;
#ifdef HAVE_DIAGS
	my_diag("use a charactor string\n");
#endif

	char_last_word_full = 'f';

	for(i = 0, read_more(1),a_char = megatek_data[0] ; 
		(a_char & 0x7f) != 0x1d ;
		 i++, read_more(1),a_char = megatek_data[0])
		{
			gtext_string[i] = a_char & 0x7f;
			out_string[i] = a_char & 0x7f;
		}


/*mark the end of the string*/
	out_string[i] = '\0';
	gtext_string[i] = '\0';



/*put in enough '\0's so all tests of out_string[x] == '\0' will work right*/
	for(j = 1 ; j < 10 ; j++)
	{
		gtext_string[i + j] = '\0';
		out_string[i + j] = '\0';
	}



	if(fill_sub == 'f')
	{
		an_element.op_code = 23;
		an_element.arg1 = my_x;
		an_element.arg2 = my_y;
		an_element.arg3 = out_string[0];
		an_element.arg4 = out_string[1];


		if((out_string[2] == 0) && (out_string[1] != 0))
  /*only 2 chars in the whole string*/
		{
			insert_ours(an_element,which_cseg);
			an_element.op_code = 44;
			insert_ours(an_element,which_cseg);

			return;
		}

		                current_pos = my_ftell();

		                 if((current_pos + 3L) % 64L == 0L)
                		         break_flag = 't';
		                 else
                		         break_flag = 'f';


				if((break_flag == 't') && (out_string[1] != 0))
				{
				/*more than 1 character & break*/
					an_element.op_code = 23;
					an_element.arg3 = out_string[0];
					an_element.arg4 = 0;
					insert_ours(an_element,which_cseg);
					gtext_string[0] = '\0';

					an_element.op_code = 23;
					an_element.arg3 = out_string[1];
					an_element.arg4 = out_string[2];
					insert_ours(an_element,which_cseg);
					break_flag = 'f';

					if(out_string[3] == 0) /*3 char string*/
					{
						an_element.op_code = 27;
						an_element.arg1 = 0;
						an_element.arg2 = 0;
						an_element.arg3 = 0;
						an_element.arg4 = 0;

						insert_ours(an_element,
							which_cseg);

						return;
					}

					start_loop = 3;
				}
				else if((break_flag == 't') && (out_string[1]
					== 0))
				{
				/*only 1 character & break*/
					an_element.op_code = 44;
					insert_ours(an_element,which_cseg);
					an_element.op_code = 23;
					insert_ours(an_element,which_cseg);

					return;
				}
				else
				{
				/*no break*/
					insert_ours(an_element,which_cseg);
					start_loop = 2;
				}

				gtext_string[0] = '\0';

		if(i > start_loop)
		{
			for(j = start_loop ; (j < i) && (out_string[j] != '\0')
				 ; j += 4)
			{
				char_last_word_full = 'f';

				an_element.op_code = 27;
				an_element.arg1 = out_string[j];
				an_element.arg2 = out_string[j + 1];
				an_element.arg3 = out_string[j + 2];
				an_element.arg4 = out_string[j + 3];

				if(out_string[j + 3] != '\0')
				{
					char_last_word_full = 't';
				}


		                current_pos = my_ftell();

		                if((current_pos + 3L) % 64L == 0L)
                		       break_flag = 't';
		                else
                		       break_flag = 'f';

				if((break_flag == 't') && (char_last_word_full
					== 't'))
				{
				   /*this 27 word is fully filled*/
					an_element.arg4 = 0;
					insert_ours(an_element,which_cseg);

					gtext_string[0] = '\0';

					an_element.op_code = 23;
					an_element.arg3 = out_string[j + 3];
					an_element.arg4 = out_string[j + 4];

					if(an_element.arg4 == 0)
						char_last_word_full = 'f';
					else /*used up 5 chars, so advance by 1*/
						j++;

					insert_ours(an_element,which_cseg);

					break_flag = 'f';
				}
				else if(break_flag == 't')  /*27 word not full*/
				{
					if(an_element.arg3 != 0) /*3rd char*/
					{
						an_element.op_code = 44;
						insert_ours(an_element,
							which_cseg);
						an_element.op_code = 27;
						insert_ours(an_element,
							which_cseg);
					}
					else
						insert_ours(an_element,
							which_cseg);
				}
				else
					insert_ours(an_element,which_cseg);

			}    /*end of for() which handles additional words*/


			/*if last word was full, insert one more to mark the end*/
			if(char_last_word_full == 't')
			{
				char_last_word_full = 'f';
				an_element.op_code = 27;
				an_element.arg1 = 0;
				an_element.arg2 = 0;
				an_element.arg3 = 0;
				an_element.arg4 = 0;

				insert_ours(an_element,which_cseg);
			}

		}
		else   /*only 2 characters in the whole string*/
		{
			return;
		}

	}
	else     /*insert into a sub*/
	{
		an_element.op_code = 23;
		an_element.arg1 = (megatek_data[0] & 7);
		an_element.arg2 = (megatek_data[1] & 3);
		an_element.arg3 = out_string[0];
		an_element.arg4 = out_string[1];

		insert_sub(an_element,which_sub);

		if(i > 2)
		{
			for(j = 2 ; (j < i) && (out_string[j] != '\0') ; j += 4)
			{
				an_element.op_code = 27;

				an_element.arg1 = (int)out_string[j] & 0x007f;

				an_element.arg2 = (int)out_string
							[j + 1] & 0x007f;

				an_element.arg3 = (int)out_string
							[j + 2] & 0x007f;

				an_element.arg4 = (int)out_string
							[j + 3] & 0x007f;

				insert_sub(an_element,which_sub);

			}

		}
		else
		{
			return;
		}

	}
}







void far cap_T_func(sub_count,which_subr) /*move 3D absolute*/
int *sub_count,which_subr;
{
	read_more(6);

	xx = comp_abs_coord(megatek_data[0],megatek_data[1]);
	yy = comp_abs_coord(megatek_data[2],megatek_data[3]);
	zz = comp_abs_coord(megatek_data[4],megatek_data[5]);

	if(fill_sub == 'f')
	{
		diag_num = 61;

		my_x = xx;
		my_y = yy;
		my_z = zz;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"move 3D abs %d %d %d\n",my_x,my_y,my_z);
		my_diag(diag_string);
	#endif

		an_element.op_code = 18;
		an_element.arg1 = my_x;
		an_element.arg2 = my_y;
		an_element.arg3 = my_z;

		insert_ours(an_element,which_cseg);

		an_element.op_code = NULLCODE;
		an_element.arg1 = my_z;

		insert_ours(an_element,which_cseg);
		
	}
	else
	{
		diag_num = 61;


	#ifdef HAVE_DIAGS
		sprintf(diag_string,"move 3D abs %d %d %d\n",xx,yy,zz);
		my_diag(diag_string);
	#endif
		an_element.op_code = 18;
		an_element.arg1 = xx;
		an_element.arg2 = yy;

		insert_sub(an_element,which_sub);

		an_element.op_code = NULLCODE;
		an_element.arg1 = zz;

		insert_sub(an_element,which_sub);
	}
}




void far cap_U_func(sub_count,which_subr)  /*draw 3D absolute*/
int *sub_count,which_subr;
{
	read_more(6);

	xx = comp_abs_coord(megatek_data[0],megatek_data[1]);
	yy = comp_abs_coord(megatek_data[2],megatek_data[3]);
	zz = comp_abs_coord(megatek_data[4],megatek_data[5]);


	if(fill_sub == 'f')
	{
		diag_num = 62;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"draw 3D abs %d %d %d\n",xx,yy,zz);
		my_diag(diag_string);
	#endif
		my_x = xx;
		my_y = yy;
		my_z = zz;

		an_element.op_code = 19;
		an_element.arg1 = my_x;
		an_element.arg2 = my_y;
		an_element.arg3 = my_z;

		insert_ours(an_element,which_cseg);

		an_element.op_code = NULLCODE;
		an_element.arg1 = my_z;

		insert_ours(an_element,which_cseg);
		
	}
	else
	{
		diag_num = 62;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"draw 3D abs %d %d %d\n",xx,yy,zz);
		my_diag(diag_string);
	#endif
		an_element.op_code = 19;
		an_element.arg1 = xx;
		an_element.arg2 = yy;

		insert_sub(an_element,which_sub);

		an_element.op_code = NULLCODE;
		an_element.arg1 = zz;

		insert_sub(an_element,which_sub);
	}
}




void far cap_V_func(sub_count,which_subr) /*move 3D relative*/
int *sub_count,which_subr;
{
	read_more(6);

	xx = comp_rel_coord(megatek_data[0],megatek_data[1]);
	yy = comp_rel_coord(megatek_data[2],megatek_data[3]);
	zz = comp_rel_coord(megatek_data[4],megatek_data[5]);


	if(fill_sub == 'f')
	{
		diag_num = 63;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"move 3D rel %d %d %d\n",xx,yy,zz);
		my_diag(diag_string);
	#endif
		my_x += xx;
		my_y += yy;
		my_z += zz;

		an_element.op_code = 20;
		an_element.arg1 = xx;
		an_element.arg2 = yy;
		an_element.arg3 = zz;

		insert_ours(an_element,which_cseg);

		an_element.op_code = NULLCODE;
		an_element.arg1 = zz;

		insert_ours(an_element,which_cseg);

	}
	else
	{
		diag_num = 63;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"move 3D rel %d %d %d\n",xx,yy,zz);
		my_diag(diag_string);
	#endif
		an_element.op_code = 20;
		an_element.arg1 = xx;
		an_element.arg2 = yy;

		insert_sub(an_element,which_sub);

		an_element.op_code = NULLCODE;
		an_element.arg1 = zz;

		insert_sub(an_element,which_sub);
	}
}




void far cap_W_func(sub_count,which_subr) /*draw 3D relative*/
int *sub_count,which_subr;
{
	read_more(6);

	xx = comp_rel_coord(megatek_data[0],megatek_data[1]);
	yy = comp_rel_coord(megatek_data[2],megatek_data[3]);
	zz = comp_rel_coord(megatek_data[4],megatek_data[5]);

	if(fill_sub == 'f')
	{
		diag_num = 64;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"draw 3D rel %d %d %d\n",xx,yy,zz);
		my_diag(diag_string);
	#endif

		my_x += xx;
		my_y += yy;
		my_z += zz;

		an_element.op_code = 21;
		an_element.arg1 = xx;
		an_element.arg2 = yy;
		an_element.arg3 = zz;

		insert_ours(an_element,which_cseg);

		an_element.op_code = NULLCODE;
		an_element.arg1 = zz;

		insert_ours(an_element,which_cseg);
	}
	else
	{
		diag_num = 64;

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"draw 3D rel %d %d %d\n",xx,yy,zz);
		my_diag(diag_string);
	#endif
		an_element.op_code = 21;
		an_element.arg1 = xx;
		an_element.arg2 = yy;

		insert_sub(an_element,which_sub);

		an_element.op_code = NULLCODE;
		an_element.arg1 = zz;

		insert_sub(an_element,which_sub);
	}
}



