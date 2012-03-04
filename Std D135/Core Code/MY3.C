/*October 20, 1988 made changes to "change something" to change default
 header info inside each segment file*/

/*November 15, 1988 Made changes to use generic graphic board calls*/


/*January 11, 1989 added code to print the sub's matrix before and after
 inserting it*/
/*January 12,1989 took out printfs & added code to prevent multiple
redisplays*/

/*January 30, 1989 Made changes to send all error messages to the error logger
file*/

#include "my_utils.h"
#include "my_globs.h"
#include "memory.h"



extern int far my_invert_y;
extern char far js_flag;


int far old_sub_3d_matrix[8];
int far new_sub_3d_matrix[8];
char far overwrite_blinks = 'f';



char far new_is_identity()
/*determines whether the sub's new matrix is equal to identity*/
{
int i;

	if((new_sub_3d_matrix[0] == 2048) &&
		(new_sub_3d_matrix[1] == 0) &&
		(new_sub_3d_matrix[2] == 0) &&
		(new_sub_3d_matrix[3] == 0) &&
		(new_sub_3d_matrix[4] == 2048) &&
		(new_sub_3d_matrix[5] == 0) &&
		(new_sub_3d_matrix[6] == 0) &&
		(new_sub_3d_matrix[7] == 0))
		{
			return('t');
		}
		else
			return('f');
}

	



char far equal_old_new()
/*determines whether the sub's new matrix equals the old one*/
{
int i;
char equal;

	equal = 't';

	for(i = 0 ; i < 8 ; i++)
		if(abs(old_sub_3d_matrix[i] - new_sub_3d_matrix[i]) > 1)
			equal = 'f';

	return(equal);
}



void far get_sub_current_ptr(which_sub)
/* retrieves the word at subroutine "which_sub" 's current insert pointer*/
int which_sub;
{	
	get_sub_value(sub_insert_ptrs[which_sub] + 1,which_sub);
}



void far ins_2D_matrix()
{

		an_element.op_code = 11;
		an_element.arg1 = comp16_3(megatek_data[0],
				megatek_data[1],megatek_data[2]);

		hcrst[0][0] = 
				(an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[0][0] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);

		an_element.op_code = NULLCODE;
		an_element.arg1 = comp16_3(megatek_data[3],
				megatek_data[4],megatek_data[5]);

		hcrst[1][0] =
			 (an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[1][0] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);


		hcrst[2][0] = 0.0;
                headers_segs[which_cseg].save_rotate3D[2][0] = 0.0;



		an_element.arg1 = comp16_3(megatek_data[6],
				megatek_data[7],megatek_data[8]);

		hcrst[3][0] = (float) an_element.arg1;
                headers_segs[which_cseg].save_rotate3D[3][0] =
                                (float)an_element.arg1;


		insert_ours(an_element,which_cseg);

		an_element.arg1 = comp16_3(megatek_data[9],
			      megatek_data[10],megatek_data[11]);

		hcrst[0][1] =
				 (an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[0][1] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);

		an_element.arg1 = comp16_3(megatek_data[12],
			      megatek_data[13],megatek_data[14]);

		hcrst[1][1] = 
				 (an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[1][1] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);
		
		hcrst[2][1] = 0.0;
                headers_segs[which_cseg].save_rotate3D[2][1] = 0.0;



		an_element.arg1 = comp16_3(megatek_data[15],
			      megatek_data[16],megatek_data[17]);

		hcrst[3][1] = (float) an_element.arg1;
                headers_segs[which_cseg].save_rotate3D[3][1] =
                                (float)an_element.arg1;


		insert_ours(an_element,which_cseg);

}





void far ins_sub_2D_matrix()
{

		an_element.op_code = 11;
		an_element.arg1 = comp16_3(megatek_data[0],
				megatek_data[1],megatek_data[2]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"ins 2D matrix M0 = %x %x %x\n",
			megatek_data[0],megatek_data[1],megatek_data[2]);
		my_diag(diag_string);
	#endif

		insert_sub(an_element,which_sub);

		an_element.op_code = NULLCODE;
		an_element.arg1 = comp16_3(megatek_data[3],
				megatek_data[4],megatek_data[5]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"ins 2D matrix M1 = %x %x %x\n",
			megatek_data[3],megatek_data[4],megatek_data[5]);
		my_diag(diag_string);
	#endif

		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[6],
				megatek_data[7],megatek_data[8]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"ins 2D matrix M2 = %x %x %x\n",
			megatek_data[6],megatek_data[7],megatek_data[8]);
		my_diag(diag_string);
	#endif

		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[9],
			      megatek_data[10],megatek_data[11]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"ins 2D matrix M3 = %x %x %x\n",
			megatek_data[9],megatek_data[10],megatek_data[11]);
		my_diag(diag_string);
	#endif

		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[12],
			      megatek_data[13],megatek_data[14]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"ins 2D matrix M4 = %x %x %x\n",
			megatek_data[12],megatek_data[13],megatek_data[14]);
		my_diag(diag_string);
	#endif

		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[15],
			      megatek_data[16],megatek_data[17]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"ins 2D matrix M5 = %x %x %x\n",
			megatek_data[15],megatek_data[16],megatek_data[17]);
		my_diag(diag_string);
	#endif

		insert_sub(an_element,which_sub);
	
                if(sub_rewrite == 't')
                {
                        ins_matrix_flag = 't';
                        ins_matrix_sub = sub_nums[which_sub];
                        matrix_update();
                }
                else
		{
                        ins_matrix_flag = 'f';
		}

}





void far ins_3D_matrix()
{
		num_dims = '3';
		an_element.op_code = 12;
		an_element.arg1 = comp16_3(megatek_data[0],
				megatek_data[1],megatek_data[2]); /*M0*/

		hcrst[0][0] = 
				(an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[0][0] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);

		an_element.op_code = NULLCODE;
		an_element.arg1 = comp16_3(megatek_data[3],
				megatek_data[4],megatek_data[5]); /*M1*/

		hcrst[1][0] = 
				(an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[1][0] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);

		an_element.arg1 = comp16_3(megatek_data[6],
				megatek_data[7],megatek_data[8]); /*M2*/

		hcrst[3][0] = (float)
				an_element.arg1;
                headers_segs[which_cseg].save_rotate3D[3][0] =
                                (float)an_element.arg1;


		
		insert_ours(an_element,which_cseg);

		an_element.arg1 = comp16_3(megatek_data[9],
			      megatek_data[10],megatek_data[11]); /*M3*/

		hcrst[0][1] = 
				(an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[0][1] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);

		an_element.arg1 = comp16_3(megatek_data[12],
			      megatek_data[13],megatek_data[14]); /*M4*/

		hcrst[1][1] = 
				(an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[1][1] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);

		an_element.arg1 = comp16_3(megatek_data[15],
			      megatek_data[16],megatek_data[17]); /*M5*/

		hcrst[3][1] = (float)
				an_element.arg1;
                headers_segs[which_cseg].save_rotate3D[3][1] =
                                (float)an_element.arg1;


		insert_ours(an_element,which_cseg);

		an_element.arg1 = comp16_3(megatek_data[18],
			      megatek_data[19],megatek_data[20]); /*M6*/

		hcrst[2][0] = 
				(an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[2][0] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);

		an_element.arg1 = comp16_3(megatek_data[21],
			      megatek_data[22],megatek_data[23]); /*M7*/

		hcrst[2][1] = 
				(an_element.arg1/2048.0);
                headers_segs[which_cseg].save_rotate3D[2][1] =
                                (an_element.arg1/2048.0);


		insert_ours(an_element,which_cseg);
		
}





void far ins_sub_3D_matrix()
{
		overwrite_blinks = 'f';  /*assume there are no blink enables*/

		num_dims = '3';
		an_element.op_code = 12;
		an_element.arg1 = comp16_3(megatek_data[0],
				megatek_data[1],megatek_data[2]); /*M0*/

/* read existing subroutine data to determine if opcode is Blink Enable */
		get_sub_current_ptr(which_sub);
		if(ptr_sub -> op_code == 1)
			overwrite_blinks = 't';

/*store current sub matrix element and new sub matrix element*/
		old_sub_3d_matrix[0] = ptr_sub -> arg1;
		new_sub_3d_matrix[0] = an_element.arg1;

/*insert new data into subroutine */
		insert_sub(an_element,which_sub);

		an_element.op_code = NULLCODE;
		an_element.arg1 = comp16_3(megatek_data[3],
				megatek_data[4],megatek_data[5]); /*M1*/

/*read next word of sub data*/
		get_sub_current_ptr(which_sub);

/*store current sub matrix element and new sub matrix element*/
		old_sub_3d_matrix[1] = ptr_sub -> arg1;
		new_sub_3d_matrix[1] = an_element.arg1;

/*insert new data into subroutine */
		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[6],
				megatek_data[7],megatek_data[8]); /*M2*/

/*read next word of sub data*/
		get_sub_current_ptr(which_sub);

/*store current sub matrix element and new sub matrix element*/
		old_sub_3d_matrix[2] = ptr_sub -> arg1;
		new_sub_3d_matrix[2] = an_element.arg1;

/*insert new data into subroutine */
		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[9],
			      megatek_data[10],megatek_data[11]); /*M3*/

/*read next word of sub data*/
		get_sub_current_ptr(which_sub);

/*store current sub matrix element and new sub matrix element*/
		old_sub_3d_matrix[3] = ptr_sub -> arg1;
		new_sub_3d_matrix[3] = an_element.arg1;

/*insert new data into subroutine */
		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[12],
			      megatek_data[13],megatek_data[14]); /*M4*/

/*read next word of sub data*/
		get_sub_current_ptr(which_sub);

/*store current sub matrix element and new sub matrix element*/
		old_sub_3d_matrix[4] = ptr_sub -> arg1;
		new_sub_3d_matrix[4] = an_element.arg1;

/*insert new data into subroutine */
		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[15],
			      megatek_data[16],megatek_data[17]); /*M5*/

/*read next word of sub data*/
		get_sub_current_ptr(which_sub);

/*store current sub matrix element and new sub matrix element*/
		old_sub_3d_matrix[5] = ptr_sub -> arg1;
		new_sub_3d_matrix[5] = an_element.arg1;

/*insert new data into subroutine */
		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[18],
			      megatek_data[19],megatek_data[20]); /*M6*/

/*read next word of sub data*/
		get_sub_current_ptr(which_sub);

/*store current sub matrix element and new sub matrix element*/
		old_sub_3d_matrix[6] = ptr_sub -> arg1;
		new_sub_3d_matrix[6] = an_element.arg1;

/*insert new data into subroutine */
		insert_sub(an_element,which_sub);

		an_element.arg1 = comp16_3(megatek_data[21],
			      megatek_data[22],megatek_data[23]); /*M7*/

/*read next word of sub data*/
		get_sub_current_ptr(which_sub);

/*store current sub matrix element and new sub matrix element*/
		old_sub_3d_matrix[7] = ptr_sub -> arg1;
		new_sub_3d_matrix[7] = an_element.arg1;

/*insert new data into subroutine */
		insert_sub(an_element,which_sub);

                if(sub_rewrite == 't')  /*then this sub is being rewritten*/
                {
			if(overwrite_blinks == 'f')
			  /*doesn't contain Blink Enables*/
			{
				if(equal_old_new() == 'f')  /*matrixes*/
				{
		                        ins_matrix_flag = 't';
        		                ins_matrix_sub = sub_nums[which_sub];

					if(js_flag == 1)
	                		        matrix_update();   /*redisplays all*/
				}
				else
					ins_matrix_flag = 'f';
			}
			else
			{
				if(new_is_identity() == 'f')
				/*sub's new matrix is not an identity matrix*/
				{
		                        ins_matrix_flag = 't';
        		                ins_matrix_sub = sub_nums[which_sub];

					if(js_flag == 1)
	                		        matrix_update();   /*redisplays all*/
				}
				else
					ins_matrix_flag = 'f';
			}
                }
                else
                        ins_matrix_flag = 'f';

/*ins_matrix_flag = 'f' means do not redisplay (inside of matrix_update())*/
}





void far cap_M_func(sub_count,which_subr) /*insert something*/
int *sub_count,which_subr;
{
float j;

	read_more(1);


	switch(megatek_data[0] & 0x07)
	{
		case 0 : /*insert 2D origin*/
		{

			read_more(4);

			if(fill_sub == 'f')
			{
				an_element.op_code = 7;
				an_element.arg1 = comp_abs_coord(megatek_data[0],
						megatek_data[1]);
				an_element.arg2 = comp_abs_coord(megatek_data[2],
						megatek_data[3]);

				headers_segs[which_cseg].x_origin = an_element.
								arg1;
				headers_segs[which_cseg].y_origin = an_element.
								arg2;
			

				diag_num = 9;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert 2D origin of %d %d\n",
					an_element.arg1,an_element.arg2);
				my_diag(diag_string);
			#endif

				insert_ours(an_element,which_cseg);
				
			}
			else
			{
				an_element.op_code = 7;
				an_element.arg1 = comp_abs_coord(megatek_data[0],
						megatek_data[1]);
				an_element.arg2 = comp_abs_coord(megatek_data[2],
						megatek_data[3]);

				insert_sub(an_element,which_sub);
			}
			break;
		}

		case 1 : /*insert 3D origin*/
		{

			read_more(6);

			if(fill_sub == 'f')
			{
				an_element.op_code = 8;
				an_element.arg1 = comp_abs_coord(megatek_data[0]
							,megatek_data[1]);
				an_element.arg2 = comp_abs_coord(megatek_data[2]
							,megatek_data[3]);

				headers_segs[which_cseg].x_origin = an_element.
									arg1;
				headers_segs[which_cseg].y_origin = an_element.
									arg2;

				diag_num = 10;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert 3D origin of %d %d",
					an_element.arg1,an_element.arg2);
				my_diag(diag_string);
			#endif

				insert_ours(an_element,which_cseg);
		
				an_element.op_code = NULLCODE;
				an_element.arg1 = comp_abs_coord(megatek_data[4]
						,megatek_data[5]);

				headers_segs[which_cseg].z_origin = an_element.
									arg1;
			#ifdef HAVE_DIAGS
				sprintf(diag_string," %d\n",an_element.arg1);
				my_diag(diag_string);
			#endif
				
				insert_ours(an_element,which_cseg);
							
			}
			else
			{
				an_element.op_code = 8;
				an_element.arg1 = comp_abs_coord(megatek_data[0]
							,megatek_data[1]);
				an_element.arg2 = comp_abs_coord(megatek_data[2]
							,megatek_data[3]);

				diag_num = 10;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert 3D origin of %d %d",
					an_element.arg1,an_element.arg2);
				my_diag(diag_string);
			#endif

				insert_sub(an_element,which_sub);


				an_element.op_code = NULLCODE;
				an_element.arg1 = comp_abs_coord(megatek_data[4]
						,megatek_data[5]);

			#ifdef HAVE_DIAGS
				sprintf(diag_string," %d\n",an_element.arg1);
				my_diag(diag_string);
			#endif
				
				insert_sub(an_element,which_sub);
			
			}
			break;
		}

		case 2 : /*insert Z origin*/
		{
			read_more(2);
		
			if(fill_sub == 'f')
			{
				an_element.op_code = 9;
				an_element.arg1 = comp_abs_coord(megatek_data[0]
							,megatek_data[1]);

				headers_segs[which_cseg].z_origin = an_element.
									arg1;

				diag_num = 11;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert Z origin of %d\n",
					an_element.arg1);
				my_diag(diag_string);
			#endif

				insert_ours(an_element,which_cseg);
		
			}
			else
			{
				an_element.op_code = 9;
				an_element.arg1 = comp_abs_coord(megatek_data[0]
							,megatek_data[1]);
				insert_sub(an_element,which_sub);
			}
			break;
		}

		case 3 : /*insert 2D Xlate*/
		{
			read_more(6);
		

			if(fill_sub == 'f')
			{
				diag_num = 12;

				an_element.op_code = 10;
				an_element.arg1 = comp16_3(megatek_data[0],
						megatek_data[1],
						megatek_data[2]);

				headers_segs[which_cseg].save_rotate3D[3][0] = 
					(float) an_element.arg1;
				hcrst[3][0] = 
					(float) an_element.arg1;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert 2D Xlate %u",
					an_element.arg1);
				my_diag(diag_string);
			#endif

				insert_ours(an_element,which_cseg);

				an_element.op_code = NULLCODE;
				an_element.arg1 = comp16_3(megatek_data[3],
						megatek_data[4],
						megatek_data[5]);

				headers_segs[which_cseg].save_rotate3D[3][1] = 
					(float) an_element.arg1;
				hcrst[3][1] = 
					(float) an_element.arg1;

			#ifdef HAVE_DIAGS
				sprintf(diag_string," %u\n",an_element.arg1);
				my_diag(diag_string);
			#endif

				insert_ours(an_element,which_cseg);
		
			}
			else
			{
				diag_num = 12;

				an_element.op_code = 10;
				an_element.arg1 = comp16_3(megatek_data[0],
						megatek_data[1],
						megatek_data[2]);

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert 2D Xlate %u",
					an_element.arg1);
				my_diag(diag_string);
			#endif

				insert_sub(an_element,which_sub);


				an_element.op_code = NULLCODE;
				an_element.arg1 = comp16_3(megatek_data[3],
						megatek_data[4],
						megatek_data[5]);

			#ifdef HAVE_DIAGS
				sprintf(diag_string," %u\n",an_element.arg1);
				my_diag(diag_string);
			#endif

				insert_sub(an_element,which_sub);

			}
			break;
		}

		case 4 : /*insert 2D matrix*/
		{
			read_more(18);

			diag_num = 13;
		#ifdef HAVE_DIAGS
			my_diag("insert 2D matrix\n");
		#endif

			if(fill_sub == 'f')
				ins_2D_matrix();
			else
				ins_sub_2d_matrix();

			break;
		}

		case 5 : /*insert 3D matrix */
		{
			read_more(24);
	
			diag_num = 14;
		#ifdef HAVE_DIAGS
			my_diag("insert 3D matrix\n");
		#endif

			if(fill_sub == 'f')
				ins_3D_matrix();
			else
				ins_sub_3D_matrix();
			break;
		}

		case 6 : /*insert clipping*/
		{
			read_more(8);

			diag_num = 15;
		#ifdef HAVE_DIAGS
			my_diag("insert clipping of ");
		#endif
			diag_num = -15;

			if(fill_sub == 'f')
			{
				an_element.op_code = 13;
				an_element.arg1 = comp_abs_coord(megatek_data[0]
						,megatek_data[1]);
				an_element.arg2 = comp_abs_coord(megatek_data[2]
						,megatek_data[3]);


				headers_segs[which_cseg].save_x_lower_clip = 
					an_element.arg1;


				headers_segs[which_cseg].save_y_lower_clip = 
					an_element.arg2;

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"%d %d ",an_element.arg1,
					an_element.arg2);
				my_diag(diag_string);
			#endif

				insert_ours(an_element,which_cseg);

				an_element.op_code = NULLCODE;
				an_element.arg1 = comp_abs_coord(megatek_data[4]
						,megatek_data[5]);
				an_element.arg2 = comp_abs_coord(megatek_data[6]
						,megatek_data[7]);


				headers_segs[which_cseg].save_x_upper_clip = 
					an_element.arg1;

				headers_segs[which_cseg].save_y_upper_clip = 
					an_element.arg2;

			#ifdef HAVE_DIAGS
				sprintf(diag_string," %d %d\n",an_element.arg1,
						an_element.arg2);
				my_diag(diag_string);
			#endif
			
				insert_ours(an_element,which_cseg);

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
                                      headers_segs[which_cseg].save_y_upper_clip,
						0);

                                my_lock_viewport();
			    }
			}
			else
			{
				an_element.op_code = 13;
				an_element.arg1 = comp_abs_coord(megatek_data[0]
						,megatek_data[1]);
				an_element.arg2 = comp_abs_coord(megatek_data[2]
						,megatek_data[3]);

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"%d %d  ",an_element.arg1,
					an_element.arg2);
				my_diag(diag_string);
			#endif

				insert_sub(an_element,which_sub);

				an_element.op_code = NULLCODE;
				an_element.arg1 = comp_abs_coord(megatek_data[4]
						,megatek_data[5]);
				an_element.arg2 = comp_abs_coord(megatek_data[6]
						,megatek_data[7]);

			#ifdef HAVE_DIAGS
				sprintf(diag_string,"%d %d\n",an_element.arg1,
						an_element.arg2);
				my_diag(diag_string);
			#endif

				insert_sub(an_element,which_sub);

			}
			break;
		}

		case 7 : /*error*/
		{
			sprintf(diag_string,"Error in flag in command M\n");
			error_logger(diag_string);
			break;
		}
	}  /* end switch */

}




void far cap_J_func(sub_count,which_subr) /*change something*/
int *sub_count,which_subr;
{
int i,j;
float k;
long save_insert_ptr;
int save_which_cseg;


	read_more(3);	

	a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);

	for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
		if(segment_nums[i] == a_segment)
			break;

	if(i == NUMBER_OF_SEGS)
	{
		sprintf(diag_string,"Invalid segment # %d in change something\n",
			a_segment);
		error_logger(diag_string);
	}

	diag_num = 1;
#ifdef HAVE_DIAGS
	sprintf(diag_string,"change something: segment# %d\n",a_segment);
	my_diag(diag_string);
#endif


	save_insert_ptr = my_ftell();   /* active segment */

	save_which_cseg = which_cseg;
	which_cseg = i;


	switch(megatek_data[2] & 0x07)
	{
		case 0 : /*change 2D origin*/
		{
			diag_num = 2;

		
			read_more(4);
		#ifdef HAVE_DIAGS
			my_diag("change 2D origin\n");
		#endif

			if(i < NUMBER_OF_SEGS)
			{
				an_element.op_code = 114;
				an_element.arg1 = comp_abs_coord(megatek_data[
					0],megatek_data[1]);
				an_element.arg2 = comp_abs_coord(megatek_data[
					2],megatek_data[3]);

				megatek_seek(which_cseg,4L);
				memcpy(current_megatek_pointer,&an_element,
					size_an_element);
			}

		#ifdef HAVE_DIAGS
			sprintf(diag_string,"change 2D origin to %d %d\n",
				 	headers_segs[i].x_origin,
					headers_segs[i].y_origin);
			my_diag(diag_string);
		#endif

			break;
		}

		case 1 : /*change 3D origin*/
		{
			diag_num = 3;

			read_more(6);
		#ifdef HAVE_DIAGS
			my_diag("change 3D origin\n");
		#endif

			if(i < NUMBER_OF_SEGS)
			{
				an_element.op_code = 114;
				an_element.arg1 = comp_abs_coord(megatek_data[
					0],megatek_data[1]);
				an_element.arg2 = comp_abs_coord(megatek_data[
					2],megatek_data[3]);

				megatek_seek(which_cseg,4L);
				memcpy(current_megatek_pointer,&an_element,
					size_an_element);

				an_element.op_code = 118;
				an_element.arg1 = comp_abs_coord(megatek_data[
					4],megatek_data[5]);

				megatek_seek(which_cseg,8L);
				memcpy(current_megatek_pointer,&an_element,
					size_an_element);
			}

		#ifdef HAVE_DIAGS
			sprintf(diag_string,"change 3D origin to %d %d %d\n",
				headers_segs[i].x_origin,
				headers_segs[i].y_origin,
				headers_segs[i].z_origin);

			my_diag(diag_string);
		#endif

			break;
		}

		case 2 : /*change Z origin */
		{
			diag_num = 4;

			read_more(2);
		#ifdef HAVE_DIAGS
			my_diag("change Z origin\n");	
		#endif

			if(i < NUMBER_OF_SEGS)
			{
/*				headers_segs[i].z_origin = comp_abs_coord(
					megatek_data[0],megatek_data[1]);
*/
				an_element.op_code = 118;
				an_element.arg1 = comp_abs_coord(
					megatek_data[0],megatek_data[1]);
				megatek_seek(which_cseg,8L);
				memcpy(current_megatek_pointer,&an_element,
					size_an_element);
			}

		#ifdef HAVE_DIAGS
			sprintf(diag_string,"change Z origin to %d\n",
					headers_segs[i].z_origin);
			my_diag(diag_string);
		#endif

			break;
		}

		case 3 : /*change 2D Xlate */
		{
			diag_num = 5;
		#ifdef HAVE_DIAGS
			my_diag("change 2D Xlate\n");
		#endif

			read_more(6);
			
			if(i < NUMBER_OF_SEGS)
			{
				j = comp16_3(megatek_data[0],
					megatek_data[1],megatek_data[2]);


				megatek_seek(which_cseg,2L);
				memcpy(&an_element,current_megatek_pointer,
					size_an_element);
				an_element.arg4 = j;
				megatek_seek(which_cseg,2L);
				memcpy(current_megatek_pointer,&an_element,
					size_an_element);

				j = comp16_3(megatek_data[3],
					megatek_data[4],megatek_data[5]);


				megatek_seek(which_cseg,3L);
				memcpy(&an_element,current_megatek_pointer,
					size_an_element);
				an_element.arg4 = j;
				megatek_seek(which_cseg,3L);
				memcpy(current_megatek_pointer,&an_element,
					size_an_element);
			}

			break;
		}

		case 4 : /* change 2D Matrix */
		{
			diag_num = 6;
		#ifdef HAVE_DIAGS
			my_diag("change 2D Matrix\n");
		#endif

			read_more(18);

			if(i < NUMBER_OF_SEGS)
			{
					j = comp16_3(megatek_data[0],
							megatek_data[1],
							megatek_data[2]);

/*					headers_segs[i].rotate3D[0][0] = 
						(j/2048.0);
*/
					an_element.op_code = 112;
					an_element.arg1 = j;

					j = comp16_3(megatek_data[3],
							megatek_data[4],
							megatek_data[5]);

/*					headers_segs[i].rotate3D[1][0] = 
						(j/2048.0);
*/
					an_element.arg2 = j;

					j = comp16_3(megatek_data[6],
							megatek_data[7],
							megatek_data[8]);

/*					headers_segs[i].rotate3D[2][0] = 0.0;

					headers_segs[i].rotate3D[3][0] = 
							(float) j;
*/
					an_element.arg3 = 0;

					an_element.arg4 = j;

					megatek_seek(which_cseg,2L);
					memcpy(current_megatek_pointer,
						&an_element,
						size_an_element);


					j = comp16_3(megatek_data[9],
							megatek_data[10],
							megatek_data[11]);

/*					headers_segs[i].rotate3D[0][1] = 
						(j/2048.0);
*/
					an_element.op_code = 113;
					an_element.arg1 = j;

					j = comp16_3(megatek_data[12],
							megatek_data[13],
							megatek_data[14]);

/*					headers_segs[i].rotate3D[1][1] = 
						(j/2048.0);
*/
					an_element.arg2 = j;

					j = comp16_3(megatek_data[15],
							megatek_data[16],
							megatek_data[17]);

/*					headers_segs[i].rotate3D[2][1] = 0.0;

					headers_segs[i].rotate3D[3][1] = 
							(float) j;
*/
					an_element.arg3 = 0;
					an_element.arg4 = j;

					megatek_seek(which_cseg,3L);
					memcpy(current_megatek_pointer,
						&an_element,
						size_an_element);
			}

			break;
		}

		case 5 : /*change 3D Matrix */
		{
			diag_num = 7;
		#ifdef HAVE_DIAGS
			my_diag("change 3D Matrix\n");
		#endif

			read_more(24);
		
		
			if(i < NUMBER_OF_SEGS)
			{
				j = comp16_3(megatek_data[0],megatek_data[1],
						megatek_data[2]); /*M0*/

/*				headers_segs[i].rotate3D[0][0] = (j/2048.0);
*/
				an_element.op_code = 112;
				read_element.op_code = 113;
				an_element.arg1 = j;

				j = comp16_3(megatek_data[3],megatek_data[4],
						megatek_data[5]); /*M1*/

/*				headers_segs[i].rotate3D[1][0] = (j/2048.0);
*/
				an_element.arg2 = j;


				j = comp16_3(megatek_data[6],megatek_data[7],
						megatek_data[8]); /*M2*/

/*				headers_segs[i].rotate3D[3][0] = (float)j;
*/
				an_element.arg4 = j;

				j = comp16_3(megatek_data[9],megatek_data[10],
						megatek_data[11]); /*M3*/

/*				headers_segs[i].rotate3D[0][1] = (j/2048.0);
*/
				read_element.arg1 = j;

				j = comp16_3(megatek_data[12],megatek_data[13],
						megatek_data[14]); /*M4*/

/*				headers_segs[i].rotate3D[1][1] = (j/2048.0);
*/
				read_element.arg2 = j;

				j = comp16_3(megatek_data[15],megatek_data[16],
						megatek_data[17]); /*M5*/

/*				headers_segs[i].rotate3D[3][1] = (float)j;
*/
				read_element.arg4 = j;

				j = comp16_3(megatek_data[18],megatek_data[19],
						megatek_data[20]); /*M6*/

/*				headers_segs[i].rotate3D[2][0] = (j/2048.0);
*/
				an_element.arg3 = j;

				j = comp16_3(megatek_data[21],megatek_data[22],
						megatek_data[23]); /*M7*/

/*				headers_segs[i].rotate3D[2][1] = (j/2048.0);
*/
				read_element.arg3 = j;

				megatek_seek(which_cseg,2L);
				memcpy(current_megatek_pointer,&an_element,
					size_an_element);
				megatek_seek(which_cseg,3L);
				memcpy(current_megatek_pointer,&read_element,
					size_an_element);
			}

			break;
		}

		case 6 : /*change clipping*/
		{
			diag_num = 8;
		#ifdef HAVE_DIAGS
			my_diag("change clipping\n");
		#endif

			read_more(8);
	
			if(i < NUMBER_OF_SEGS)
			{
				an_element.op_code = 115;

				an_element.arg1 =
					comp_abs_coord(megatek_data[0],
						megatek_data[1]);

				an_element.arg2 = 
					comp_abs_coord(megatek_data[2],
						megatek_data[3]);

				an_element.arg3 =
					comp_abs_coord(megatek_data[4],
						megatek_data[5]);

				an_element.arg4 = 
					comp_abs_coord(megatek_data[6],
						megatek_data[7]);

				megatek_seek(which_cseg,5L);
				memcpy(current_megatek_pointer,&an_element,
					size_an_element);

				save_low_x_clip = an_element.arg1;
				save_low_y_clip = an_element.arg2;
				save_up_x_clip = an_element.arg3;
				save_up_y_clip = an_element.arg4;

			    if(num_pages == 2)
			    {
				my_set_viewport(an_element.arg1,
					an_element.arg2,
					an_element.arg3,
					an_element.arg4,
					which_page);
				my_lock_viewport();
			    }
			    else
			    {
                                my_set_viewport(an_element.arg1,
                                      an_element.arg2,
                                      an_element.arg3,
                                      an_element.arg4,0);

                                my_lock_viewport();
			    }
			}

			break;
		}

		case 7 : /* error */
		{
			sprintf(diag_string,"error in flag in command J\n");
			error_logger(diag_string);
			break;
		}

	}  /* end switch */



	which_cseg = save_which_cseg;
	megatek_seek(which_cseg,save_insert_ptr);

}



