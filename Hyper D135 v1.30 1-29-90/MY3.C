/* MY3.C   Mainly clipping & matrix   inserts and changes */


/*October 20, 1988 made changes to "change something" to change default
 header info inside each segment file*/

/*November 15, 1988 Made changes to use generic graphic board calls*/


/*January 11, 1989 added code to print the sub's matrix before and after
 inserting it*/
/*January 12,1989 took out printfs & added code to prevent multiple
redisplays*/

/*January 30, 1989 Made changes to send all error messages to the error logger
file*/

/*March 17, 1989 fixed insert clipping stuff for pixel board use*/

/*March 24, 1989 modified code to use non-interpreted subs when possible*/

/*March 28, 1989 added code to record results of change-something commands*/


#include "my_utils.h"
#include "my_globs.h"
#include "c:\emulate\clipper\pw_lib.h"


int far old_sub_3d_matrix[8];
int far new_sub_3d_matrix[8];
char far overwrite_blinks = 'f';

extern char far js_flag;





void far update_a_header(the_data,which_field,which_segment)
struct header_data the_data;
int which_field,which_segment;
{
	if(which_segment == 0)
		return;

	if((which_segment > 0) && (which_segment < NUMBER_OF_SEGS))
	{

	}
	else
	{
		sprintf(diag_string,
			"Update_a_header(): Invalid segment = %d\n",
				which_segment);
		error_logger(diag_string);
	}
}




int far read_subs_matrix(which_sub)
int which_sub;
{
int i,real_sub;
short far *matrix_pointer;


	real_sub = sub_nums[which_sub];

	switch(real_sub)
	{
		case 3 :
		case 6 :
		case 7 :
		{
			for(i = 0 ; i < 8 ; i++)
				old_sub_3d_matrix[i] = 0;

			return(FAILURE);
		}

		default :
		{
			for(i = 0 ; sub_host_to_pix_ptrs[real_sub][i] != NULL
			    ; i++)
			{
				if(*(sub_host_to_pix_ptrs[real_sub][i]) 
				    == 0xa7)   /* vwmatx3 */
					break;
			}

			if(sub_host_to_pix_ptrs[real_sub][i] == NULL)
			{
				for(i = 0 ; i < 8 ; i++)
					old_sub_3d_matrix[i] = 0;
	
				return(FAILURE);
			}
			else
			{
				matrix_pointer =
					sub_host_to_pix_ptrs[real_sub][i] + 1;

				old_sub_3d_matrix[0] = *matrix_pointer;
				old_sub_3d_matrix[1] = *(matrix_pointer + 4);
				old_sub_3d_matrix[2] = *(matrix_pointer + 8);
				old_sub_3d_matrix[3] = *(matrix_pointer + 12);
				old_sub_3d_matrix[4] = *(matrix_pointer + 1);
				old_sub_3d_matrix[5] = *(matrix_pointer + 5);
				old_sub_3d_matrix[6] = *(matrix_pointer + 9);
				old_sub_3d_matrix[7] = *(matrix_pointer + 13);


				return(SUCCESS);
			}
		}
	}
}




char far new_is_identity()
/*determines whether the sub's new matrix is equal to identity*/
{
int i;

	if((new_sub_3d_matrix[0] == 2048) &&
		(new_sub_3d_matrix[1] == 0) &&
		(new_sub_3d_matrix[2] == 0) &&
		(new_sub_3d_matrix[3] == 0) &&
		(new_sub_3d_matrix[4] == 0) &&
		(new_sub_3d_matrix[5] == 2048) &&
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



void far ins_2D_matrix()
{

}





void far ins_sub_2D_matrix()
{

}





void far ins_3D_matrix()
{
	error_logger("Insert 3D Matrix INTO a segment: NOT IMPLEMENTED\n");

#ifdef klrrldllfed
	an_element.op_code = 12;
	an_element.arg1 = comp16_3(megatek_data[0],
		megatek_data[1],megatek_data[2]); /*M0*/
        headers_segs[which_cseg].save_rotate3D[0][0] = an_element.arg1/2048.0;
	insert_ours(an_element,which_cseg);

	an_element.op_code = NULLCODE;
	an_element.arg1 = comp16_3(megatek_data[3],
		megatek_data[4],megatek_data[5]); /*M1*/
        headers_segs[which_cseg].save_rotate3D[1][0] = an_element.arg1/2048.0;
	insert_ours(an_element,which_cseg);

	an_element.arg1 = comp16_3(megatek_data[6],
		megatek_data[7],megatek_data[8]); /*M2*/
        headers_segs[which_cseg].save_rotate3D[3][0] = (float)an_element.arg1;
	insert_ours(an_element,which_cseg);

	an_element.arg1 = comp16_3(megatek_data[9],
		megatek_data[10],megatek_data[11]); /*M3*/
        headers_segs[which_cseg].save_rotate3D[0][1] = an_element.arg1/2048.0;
	insert_ours(an_element,which_cseg);

	an_element.arg1 = comp16_3(megatek_data[12],
		megatek_data[13],megatek_data[14]); /*M4*/
        headers_segs[which_cseg].save_rotate3D[1][1] = an_element.arg1/2048.0;
	insert_ours(an_element,which_cseg);

	an_element.arg1 = comp16_3(megatek_data[15],
		megatek_data[16],megatek_data[17]); /*M5*/
        headers_segs[which_cseg].save_rotate3D[3][1] = (float)an_element.arg1;
	insert_ours(an_element,which_cseg);

	an_element.arg1 = comp16_3(megatek_data[18],
		megatek_data[19],megatek_data[20]); /*M6*/
        headers_segs[which_cseg].save_rotate3D[2][0] = an_element.arg1/2048.0;
	insert_ours(an_element,which_cseg);

	an_element.arg1 = comp16_3(megatek_data[21],
		megatek_data[22],megatek_data[23]); /*M7*/
        headers_segs[which_cseg].save_rotate3D[2][1] = an_element.arg1/2048.0;
	insert_ours(an_element,which_cseg);		

#endif
}





void far ins_sub_3D_matrix()
{
int i;

	overwrite_blinks = 'f';  /*assume there are no blink enables*/

	if(read_subs_matrix(which_sub) == FAILURE)  /* no matrix in it */
		overwrite_blinks = 't';


	an_element.op_code = 12;
	an_element.arg1 = comp16_3(megatek_data[0],
		megatek_data[1],megatek_data[2]); /*M0 [0][0]*/
	new_sub_3d_matrix[0] = an_element.arg1;
	my_3d_viewing_matrix[0] = an_element.arg1;


	an_element.op_code = NULLCODE;
	an_element.arg1 = comp16_3(megatek_data[3],
		megatek_data[4],megatek_data[5]); /*M1 [1][0]*/
	new_sub_3d_matrix[1] = an_element.arg1;
	my_3d_viewing_matrix[4] = an_element.arg1;


	an_element.arg1 = comp16_3(megatek_data[6],
		megatek_data[7],megatek_data[8]); /*M2  [3][0]*/
	new_sub_3d_matrix[3] = an_element.arg1;
	my_3d_viewing_matrix[12] = an_element.arg1;


	an_element.arg1 = comp16_3(megatek_data[9],
		megatek_data[10],megatek_data[11]); /*M3 [0][1]*/
	new_sub_3d_matrix[4] = an_element.arg1;
	my_3d_viewing_matrix[1] = an_element.arg1;


	an_element.arg1 = comp16_3(megatek_data[12],
		megatek_data[13],megatek_data[14]); /*M4 [1][1]*/
	new_sub_3d_matrix[5] = an_element.arg1;
	my_3d_viewing_matrix[5] = an_element.arg1;


	an_element.arg1 = comp16_3(megatek_data[15],
		megatek_data[16],megatek_data[17]); /*M5  [3][1]*/
	new_sub_3d_matrix[7] = an_element.arg1;
	my_3d_viewing_matrix[13] = an_element.arg1;


	an_element.arg1 = comp16_3(megatek_data[18],
		megatek_data[19],megatek_data[20]); /*M6  [2][0]*/
	new_sub_3d_matrix[2] = an_element.arg1;
	my_3d_viewing_matrix[8] = an_element.arg1;


	an_element.arg1 = comp16_3(megatek_data[21],
		megatek_data[22],megatek_data[23]); /*M7  [2][1]*/
	new_sub_3d_matrix[6] = an_element.arg1;
	my_3d_viewing_matrix[9] = an_element.arg1;



	an_element.op_code = 12;
	an_element.arg1 = new_sub_3d_matrix[0];
	insert_sub(an_element,which_sub);

	an_element.op_code = NULLCODE;
	an_element.arg1 = new_sub_3d_matrix[1];
	insert_sub(an_element,which_sub);

	an_element.arg1 = new_sub_3d_matrix[3];
	insert_sub(an_element,which_sub);

	an_element.arg1 = new_sub_3d_matrix[4];
	insert_sub(an_element,which_sub);

	an_element.arg1 = new_sub_3d_matrix[5];
	insert_sub(an_element,which_sub);

	an_element.arg1 = new_sub_3d_matrix[7];
	insert_sub(an_element,which_sub);

	an_element.arg1 = new_sub_3d_matrix[2];
	insert_sub(an_element,which_sub);

	an_element.arg1 = new_sub_3d_matrix[6];
	insert_sub(an_element,which_sub);




/*

printf("INS SUB 3D Matrix: overwrite_blinks = %c \n OLD Matrix =  ",
	overwrite_blinks);
for(i = 0 ; i < 8 ; i++)
	printf(" %d ",old_sub_3d_matrix[i]);
printf("\n NEW Matrix =  ");
for(i = 0 ; i < 8 ; i++)
	printf(" %d ",new_sub_3d_matrix[i]);
printf("\nsub_rewrite = %c\n\n",sub_rewrite);

*/


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
	read_more(1);


	switch(megatek_data[0] & 0x07)
	{
		case 0 : /*insert 2D origin*/
		{
			read_more(4);

			an_element.op_code = 7;
			an_element.arg1 = comp_abs_coord(megatek_data[0],
				megatek_data[1]);
			an_element.arg2 = comp_abs_coord(megatek_data[2],
				megatek_data[3]);


			if(fill_sub == 'f')
			{

				diag_num = 9;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert 2D origin of %d %d\n",
					an_element.arg1,an_element.arg2);
				my_diag(diag_string);
			#endif

				insert_ours(an_element,which_cseg);
			}
			else
				insert_sub(an_element,which_sub);

			break;
		}

		case 1 : /*insert 3D origin*/
		{
			read_more(6);

			an_element.op_code = 8;
			an_element.arg1 = comp_abs_coord(megatek_data[0]
				,megatek_data[1]);
			an_element.arg2 = comp_abs_coord(megatek_data[2]
				,megatek_data[3]);
			an_element.arg3 = comp_abs_coord(megatek_data[4]
				,megatek_data[5]);


			if(fill_sub == 'f')
			{
				diag_num = 10;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"segment insert 3D origin of %d %d",
					an_element.arg1,an_element.arg2);
				my_diag(diag_string);
			#endif
				insert_ours(an_element,which_cseg);
		
				an_element.op_code = NULLCODE;
				an_element.arg1 = comp_abs_coord(megatek_data[4]
						,megatek_data[5]);


			#ifdef HAVE_DIAGS
				sprintf(diag_string," %d\n",an_element.arg1);
				my_diag(diag_string);
			#endif
				
				insert_ours(an_element,which_cseg);
							
			}
			else
			{
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


			an_element.op_code = 13;
			an_element.arg1 = comp_abs_coord(megatek_data[0]
				,megatek_data[1]);
			an_element.arg2 = comp_abs_coord(megatek_data[2]
				,megatek_data[3]);
			an_element.arg3 = comp_abs_coord(megatek_data[4]
				,megatek_data[5]);
			an_element.arg4 = comp_abs_coord(megatek_data[6]
				,megatek_data[7]);


			if(fill_sub == 'f')
			{
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"%d %d ",an_element.arg1,
					an_element.arg2);
				my_diag(diag_string);
			#endif

				insert_ours(an_element,which_cseg);

				an_element.op_code = NULLCODE;
				an_element.arg1 = an_element.arg3;
				an_element.arg2 = an_element.arg4;

			#ifdef HAVE_DIAGS
				sprintf(diag_string," %d %d\n",an_element.arg1,
						an_element.arg2);
				my_diag(diag_string);
			#endif
			
				insert_ours(an_element,which_cseg);

			}
			else
			{
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"%d %d  ",an_element.arg1,
					an_element.arg2);
				my_diag(diag_string);
			#endif

				insert_sub(an_element,which_sub);

				an_element.op_code = NULLCODE;
				an_element.arg1 = an_element.arg3;
				an_element.arg2 = an_element.arg4;

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

				update_header_data.origin[0] = an_element.arg1;
				update_header_data.origin[1] = an_element.arg2;
				update_a_header(update_header_data,
					THE_2D_ORIGIN,i);
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

				update_header_data.origin[0] = an_element.arg1;
				update_header_data.origin[1] = an_element.arg2;

				an_element.op_code = 118;
				an_element.arg1 = comp_abs_coord(megatek_data[
					4],megatek_data[5]);

				update_header_data.origin[2] = an_element.arg1;
				update_a_header(update_header_data,
					THE_3D_ORIGIN,i);
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
				an_element.op_code = 118;
				an_element.arg1 = comp_abs_coord(
					megatek_data[0],megatek_data[1]);

				update_header_data.origin[2] = an_element.arg1;
				update_a_header(update_header_data,
					THE_Z_ORIGIN,i);
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
				update_header_data.viewing_3d_matrix[12] = j;

				j = comp16_3(megatek_data[3],
					megatek_data[4],megatek_data[5]);
				update_header_data.viewing_3d_matrix[13] = j;

				update_a_header(update_header_data,
					THE_2D_XLATE,i);
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
				update_header_data.viewing_3d_matrix[0] = j;

				j = comp16_3(megatek_data[3],megatek_data[4],
						megatek_data[5]); /*M1*/
				update_header_data.viewing_3d_matrix[4] = j;

				j = comp16_3(megatek_data[6],megatek_data[7],
						megatek_data[8]); /*M2*/
				update_header_data.viewing_3d_matrix[12] = j;

				j = comp16_3(megatek_data[9],megatek_data[10],
						megatek_data[11]); /*M3*/
				update_header_data.viewing_3d_matrix[1] = j;

				j = comp16_3(megatek_data[12],megatek_data[13],
						megatek_data[14]); /*M4*/
				update_header_data.viewing_3d_matrix[5] = j;

				j = comp16_3(megatek_data[15],megatek_data[16],
						megatek_data[17]); /*M5*/
				update_header_data.viewing_3d_matrix[13] = j;

				j = comp16_3(megatek_data[18],megatek_data[19],
						megatek_data[20]); /*M6*/
				update_header_data.viewing_3d_matrix[8] = j;

				j = comp16_3(megatek_data[21],megatek_data[22],
						megatek_data[23]); /*M7*/
				update_header_data.viewing_3d_matrix[9] = j;

				update_a_header(update_header_data,
					THE_MATRIX,i);
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
				an_element.arg1 = comp_abs_coord(megatek_data[0],
					megatek_data[1]);
				an_element.arg2 = comp_abs_coord(megatek_data[2],
					megatek_data[3]);
				an_element.arg3 = comp_abs_coord(megatek_data[4],
					megatek_data[5]);
				an_element.arg4 = comp_abs_coord(megatek_data[6],
					megatek_data[7]);


				update_header_data.my_set_viewport_values[0] =
					an_element.arg1;
				update_header_data.my_set_viewport_values[1] =
					an_element.arg2;
				update_header_data.my_set_viewport_values[2] =
					an_element.arg3;
				update_header_data.my_set_viewport_values[3] =
					an_element.arg4;

				update_a_header(update_header_data,
					THE_CLIPPING,i);
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

}



