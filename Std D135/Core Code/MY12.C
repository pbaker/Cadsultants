/*January 31, 1989 made changes to send error messages to the error file*/

#include "my_globs.h"
#include "my_utils.h"



#ifdef ekfklflkflflfllrlf


void far insert_save_file(value)
struct our_element far *value;
{

	if(save_write_error == 'f')
		fwrite(value,size_an_element,1,display_file);

	if(ferror(display_file) != 0) /*write error happened!!*/
	{
		if(save_write_error == 'f')
		{
			sprintf(diag_string,"******ERROR***** OUT OF DISK SPACE ON DRIVE %c:\n",
				drive_letter);
			error_logger(diag_string);
			sprintf(diag_string,"NOTHING MORE WILL BE STORED INTO THE SAVE FILE\n");
			error_logger(diag_string);
			save_write_error = 't';
		}
	}
}



void sv_comp_sub_data(sub_num_index,sub_end,sub_num)
int *sub_num_index,*sub_end,sub_num;
{
int i;

	for(i = 0 ; (i < 20) && (sub_nums[i] != sub_num) ; i++)
		;

	*sub_num_index = i;
	*sub_end = sub_insert_ptrs[i];
}






void far save_subs(the_subs_num)
int the_subs_num;
{
int which_sub_index,sub_counter,subs_end;

	sv_comp_sub_data(&which_sub_index,&subs_end,the_subs_num);

	for(sub_counter = 0 ; sub_counter <= subs_end ; sub_counter++)
	{
		get_sub_value(sub_counter,which_sub_index);

		if(ptr_sub -> op_code == 0)  /*its a call sub*/
			save_subs(ptr_sub -> arg1);
		else    /*copy it into the save file*/
		{
			an_element.op_code = ptr_sub -> op_code;
			an_element.arg1 = ptr_sub -> arg1;
			an_element.arg2 = ptr_sub -> arg2;
			an_element.arg3 = ptr_sub -> arg3;
			an_element.arg4 = ptr_sub -> arg4;
			an_element.flag1 = ptr_sub -> flag1;
			an_element.flag2 = ptr_sub -> flag2;

			insert_save_file(&an_element);

			if(save_write_error == 't')
				return;
		}
	}

/*put in a subroutine# "the_subs_num" end indicator*/
	an_element.op_code = 102;
	an_element.arg1 = the_subs_num;
	insert_save_file(&an_element);
}

#endif



void far save_part_display()  /*saves the part being displayed to a file*/
{
int i,save_which_cseg;

#ifdef titkktkktkkktkkkkkhkkkkkkk


	save_write_error = 'f';
	save_which_cseg = which_cseg;
	fclose(open_segment);

	for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
	{
		sprintf(out_string,"%c:segm%d",drive_letter,i);
		open_segment = fopen(out_string,"r+b");


		an_element.op_code = 101;
		an_element.arg1 = i;
		insert_save_file(&an_element);

		if((headers_segs[i].visibility == 'f') || 
			(segment_nums[i] == -1))
		{
			fclose(open_segment);
			continue;
		}


		if(save_write_error == 't')
		{
			fclose(open_segment);
			goto save_quit;
		}

		quit_segment = 'f';

		for( ; quit_segment == 'f' ; )
		{
			get_our_value(9,8);
			if(quit_segment == 't')
				break;

			switch(read_element.op_code)
			{
				case 0 : /*sub call*/
				{
					save_subs(read_element.arg1);
					break;
				}

				case 110 : /*header visibility flag*/
				{
					an_element.op_code = 110;
					an_element.flag1 = headers_segs[i].
						visibility;

					insert_save_file(&an_element);
					break;
				}

				case 111 : /*header Xform enable*/
				{
					an_element.op_code = 111;
					an_element.flag1 = headers_segs[i].
						Xform_enable;

					insert_save_file(&an_element);
					break;
				}


/*				case 112 : header 1st half of matrix
				{
					an_element.op_code = 112;
					an_element.arg1 = (int)(headers_segs[i].
						save_rotate3D[0][0]*2048.0);
					an_element.arg2 = (int)(headers_segs[i].
						save_rotate3D[1][0]*2048.0);
					an_element.arg3 = (int)(headers_segs[i].
						save_rotate3D[2][0]*2048.0);
					an_element.arg4 = (int)(headers_segs[i].
						save_rotate3D[3][0]);

					insert_save_file(&an_element);
					break;
				}

				case 113 : header 2nd half of matrix
				{
					an_element.op_code = 113;
					an_element.arg1 = (int)(headers_segs[i].
						save_rotate3D[0][1]*2048.0);
					an_element.arg2 = (int)(headers_segs[i].
						save_rotate3D[1][1]*2048.0);
					an_element.arg3 = (int)(headers_segs[i].
						save_rotate3D[2][1]*2048.0);
					an_element.arg4 = (int)(headers_segs[i].
						save_rotate3D[3][1]);

					insert_save_file(&an_element);
					break;
				}
*/

				case 114 : /*header x_origin and y_origin*/
				{
					an_element.op_code = 114;
					an_element.arg1 = headers_segs[i].
						x_origin;
					an_element.arg2 = headers_segs[i].
						y_origin;

					insert_save_file(&an_element);
					break;
				}

				case 115 : /*header clipping values*/
				{
					an_element.op_code = 115;
					an_element.arg1 = headers_segs[i].
						save_x_lower_clip;
					an_element.arg2 = headers_segs[i].
						save_y_lower_clip;
					an_element.arg3 = headers_segs[i].
						save_x_upper_clip;
					an_element.arg4 = headers_segs[i].
						save_y_upper_clip;

					insert_save_file(&an_element);
					break;
				}

				case 116 : /*header dash pattern*/
				{
					an_element.op_code = 116;
					an_element.arg1 = headers_segs[i].
						dash_pattern;

					insert_save_file(&an_element);
					break;
				}

				case 117 : /*header dash enable*/
				{
					an_element.op_code = 117;
					an_element.flag1 = 'f';
					

					insert_save_file(&an_element);
					break;
				}

				case 118 : /*header z_origin*/
				{
					an_element.op_code = 118;
					an_element.arg1 = headers_segs[i].
						z_origin;

					insert_save_file(&an_element);
					break;
				}


				default : /*everything else (just copy it)*/
				{
					insert_save_file(&read_element);
					break;
				}
			}

			if(save_write_error == 't')
			{
				fclose(open_segment);
				goto save_quit;
			}

		}

		fclose(open_segment);
	}

save_quit:


	which_cseg = save_which_cseg;
	sprintf(out_string,"%c:segm%d",drive_letter,which_cseg);
	open_segment = fopen(out_string,"r+b");
	fseek(open_segment,0L,SEEK_END);

	save_write_error = 'f';


	#endif
}

