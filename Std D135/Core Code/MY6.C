/*October 17, 1988 10:33  Made New read PID use the seg_starts array.
Made New rewrite use the seg_starts array. */

/*October 20, 1988 changed "change something" functions to change the default
header info inside the segment file*/

/*November 15, 1988 made changes to use generic graphics board calls*/

/*January 31, 1989 made changes to send error messages to the error file*/

/*February 8, 1989 changed new rewrite segment: if(temp + 1L <= temp_seg_ptr){}
to if(temp <= temp_seg_ptr){} (was causing trouble) */

/*February 10, 1989 added ins_matrix_flag = 't'; to change visibility to on &
changed insert blink enable into a sub to insert color "system" instead, ONLY
WHEN ITS DOING SUB 2*/

/*February 24, 1989 added int far op_code_to_overwrite to record the op code
that NEW REWRITE is going to overwrite and
made insert intensity/color check this variable when rewrite_flag == 't'
also added long far save_host_id & long far save_host_displacement*/

#include "my_utils.h"
#include "my_globs.h"
#include "expanded.h"

int far save_system_color = 7;
int far op_code_to_overwrite;
long far save_host_id, far save_host_displacement;


extern int far joy_x_sample;
extern int far joy_y_sample;
extern int far last_mouse_x;
extern int far last_mouse_y;
extern int far last_neg_mouse_y;
extern char far my_text_buffer[100][200];


char far visibility_changed_to_on[NUMBER_OF_SEGS];



void far my_reset()
{
int i,subs,from_segs;
int row,col;


	segment_nums[0] = 0;
	which_cseg = 0;



	for(i = 0 ; i < 256 ; i++)
		available_pages[i] = -1;

	for(i = 0 ; i < 100 ; i++)
		my_text_buffer[i][0] = '\0';


	for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
	{
		segment_calls[i] = -1;
		headers_segs[i].visibility = 'f';
		visibility_changed_to_on[i] = 'f';
	}


	my_x = 0;
	my_y = 0;
	my_z = 0;



	for(row = 0 ; row < NUMBER_OF_SEGS ; row++)
	{
		megatek_ends[row] = 0L;

		for(col = 0 ; col < 63 ; col++)
			megatek_locate[row][col] = 0xff;
	}


	current_megatek_pointer = (char far *)start_of_page_frame_0;
	current_megatek_page = get_next_free_page();	
	megatek_locate[0][0] = current_megatek_page;
	map_handle_page(current_megatek_page,0,handle);
	current_megatek_insert_pos = 0L;




/*set up segment 0's inside-the-file default header info*/

	an_element.op_code = 110;  	/*visibility*/
	an_element.flag1 = 't';
	insert_megatek();
	
	an_element.op_code = 111;       /*Xform enable*/
	an_element.flag1 = 't';
	insert_megatek();

	an_element.op_code = 112;       /*first half of unity matrix*/
	an_element.arg1 = 2048;
	an_element.arg2 = 0;
	an_element.arg3 = 0;
	an_element.arg4 = 0;
	insert_megatek();

	an_element.op_code = 113;       /*second half of unity matrix*/
	an_element.arg1 = 0;
	an_element.arg2 = 2048;
	an_element.arg3 = 0;
	an_element.arg4 = 0;
	insert_megatek();

	an_element.op_code = 114;	/*x_origin , y_origin*/
	an_element.arg1 = 0;
	an_element.arg2 = 0;
	insert_megatek();

	an_element.op_code = 115; 	/*clipping values*/
	an_element.arg1 = -2048;
	an_element.arg2 = -2048;
	an_element.arg3 = 2048;
	an_element.arg4 = 2048;
	insert_megatek();

	an_element.op_code = 116;	/*dash_pattern*/
	an_element.arg1 = 0xffff;
	insert_megatek();

	an_element.op_code = 117;	/*dash_enable*/
	an_element.flag1 = 'f';
	insert_megatek();

	an_element.op_code = 118;	/*z_origin*/
	an_element.arg1 = 0;
	insert_megatek();


	an_element.op_code = 44;
	for(i = 0 ; i < 6 ; i++)   	/*put in the null words in the header*/
		insert_megatek();


	megatek_ends[0] = 14L;


	rewrite_file = NULL;


	headers_segs[0].is_transform = 't';
	headers_segs[0].visibility = 't';
	headers_segs[0].Xform_enable = 't';

	headers_segs[0].clip_x_lower = -2048;
	headers_segs[0].clip_y_lower = -2048;
	headers_segs[0].clip_x_upper = 2048;
	headers_segs[0].clip_y_upper = 2048;

	headers_segs[0].save_x_lower_clip = -2048;
	headers_segs[0].save_y_lower_clip = -2048;
	headers_segs[0].save_x_upper_clip = 2048;
	headers_segs[0].save_y_upper_clip = 2048;


	headers_segs[0].rotate3D[0][0] = 1.0;
	headers_segs[0].rotate3D[1][0] = 0.0;
	headers_segs[0].rotate3D[2][0] = 0.0;
	headers_segs[0].rotate3D[3][0] = 0.0;
	headers_segs[0].rotate3D[0][1] = 0.0;
	headers_segs[0].rotate3D[1][1] = 1.0;
	headers_segs[0].rotate3D[2][1] = 0.0;
	headers_segs[0].rotate3D[3][1] = 0.0;



        headers_segs[0].save_rotate3D[0][0] = 1.0;
        headers_segs[0].save_rotate3D[1][0] = 0.0;
        headers_segs[0].save_rotate3D[2][0] = 0.0;
        headers_segs[0].save_rotate3D[3][0] = 0.0;
        headers_segs[0].save_rotate3D[0][1] = 0.0;
        headers_segs[0].save_rotate3D[1][1] = 1.0;
        headers_segs[0].save_rotate3D[2][1] = 0.0;
        headers_segs[0].save_rotate3D[3][1] = 0.0;


        hcrst[0][0] = 1.0;
        hcrst[1][0] = 0.0;
        hcrst[2][0] = 0.0;
        hcrst[3][0] = 0.0;
        hcrst[0][1] = 0.0;
        hcrst[1][1] = 1.0;
        hcrst[2][1] = 0.0;
        hcrst[3][1] = 0.0;





	headers_segs[0].x_origin = 0;
	headers_segs[0].y_origin = 0;
	headers_segs[0].z_origin = 0;

	headers_segs[0].dash_enable = 'f';
	headers_segs[0].blink_enable = 'f';
	headers_segs[0].dash_pattern = 0xffff;
	headers_segs[0].intensity_color = 0x01;
	headers_segs[0].blink_rate = 0x10;

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		segment_nums[i] = -1;

	for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
		sub_nums[i] = -1;

	for(i = 0 ; i < NUMBER_OF_SUBS + 1 ; i++)
		sub_insert_ptrs[i] = -1;



	save_low_x_clip = -2048;
	save_low_y_clip = -2048;
	save_up_x_clip = 2048;
	save_up_y_clip = 2048;

	my_set_viewport(-2048,-2048,2048,2048,0);
	my_lock_viewport();

	fill_sub = 'f';
	our_call = 'f';
	curser_displayed = 'f';
	which_page = 0;
	my_scroll(0);
	clear_screen();
	my_dash_enable(line_type);

        for(subs = 0 ; subs < 10 ; subs++)
                for(from_segs = 0 ; from_segs < 10 ; from_segs++)
                        where_called[subs][from_segs] = -1;




        draw_black = 'f';
        black = 'f';
        no_draw_flag = 'f';
	rewrite_color = 'f';
	rewrite_flag = 'f';
	rw_sub1_0_flag = 'f';
	num_op_code_4s = 0;
	ins_matrix_flag = 'f';
	button1 = 'f';
}




void far n_func(sub_count,which_subr)  /* system reset */
int *sub_count,which_subr;
{

	read_more(1);
	
	
		diag_num = 65;
	
		switch(megatek_data[0] & 0x07)
		{
			case 0 :
			case 4 :
			case 5 :
			case 1 : /*no action*/
			{
			#ifdef HAVE_DIAGS
				my_diag("system reset no action\n");
			#endif
				break;
			}

			case 2 : /*software reset*/
			{
			#ifdef HAVE_DIAGS
				my_diag("system reset software reset\n");
			#endif
				my_reset();
				break;
			}

			case 3 : /*hardware reset*/
			{
			#ifdef HAVE_DIAGS
				my_diag("system reset hardware reset\n");
			#endif
				vm512.least_sig = 836;
				my_reset();
				break;
			}

			case 6 : /*software reset into EDLM mode*/
			{
			#ifdef HAVE_DIAGS
				my_diag("software reset into EDLM mode\n");
			#endif
				vm512.least_sig = 0;
				my_reset();
				break;
			}

			case 7 : /*hardware reset into EDLM mode*/
			{
			#ifdef HAVE_DIAGS
				my_diag("hardware reset into EDLM mode\n");
			#endif
				my_reset();
				break;
			}
		}
	
}


void far m_func(sub_count,which_subr)  /* refresh control */
int *sub_count,which_subr;
{

	read_more(4);

		diag_num = 66;
	#ifdef HAVE_DIAGS
		my_diag("refresh control\n");
	#endif
	
}





void far vert_line(sub_count,which_subr)  /* read current position ID */
int *sub_count,which_subr;
{
#ifdef rklrelrelkrrlrelrevel

unsigned char most_sig,least_sig,a_char,first,second,third;
unsigned int sub_seg_num,sub_seg_ptr;

	read_more(1);
	
	diag_num = 68;
#ifdef HAVE_DIAGS
	my_diag("read current position ID\n");
#endif
	a_char = 0x40;

	if(fill_sub == 't')  /*read subroutine position ID*/
	{
		a_char |= 16; 

		sub_seg_num = sub_nums[which_sub];
		sub_seg_ptr = sub_insert_ptrs[which_sub];

		sub_seg_ptr += sub_starts[sub_seg_num];
	}
	else   /* read segment position ID*/
	{
		sub_seg_num = segment_nums[which_cseg];
		sub_seg_ptr = (unsigned int)(ftell(open_segment)/(long)
			size_an_element);

		if(segment_nums[which_cseg] == 1)
			sub_seg_ptr += segm1_start;
		else if(segment_nums[which_cseg] == 2)
			sub_seg_ptr += segm2_start;
		else
			sub_seg_ptr += 1000;
	}




	most_sig = (sub_seg_num >> 6) & 0x003f;
	least_sig = sub_seg_num & 0x003f;

	first = (sub_seg_ptr >> 12) & 0x000f;
	second = (sub_seg_ptr >> 6) & 0x003f;
	third = sub_seg_ptr & 0x003f;

	my_ugout(0x1d);
	my_ugout(a_char);
	my_ugout(most_sig | 0x40);
	my_ugout(least_sig | 0x40);
	my_ugout(first | 0x40);
	my_ugout(second | 0x40);
	my_ugout(third | 0x40);
	my_ugout(0x0d);

#endif
}









void far new_read_pid()  /* NEW READ current position ID */
{
unsigned char most_sig,least_sig,a_char,first,second,third,fourth;
unsigned int sub_seg_num;
unsigned long sub_seg_ptr;

	read_more(1);
	
	diag_num = 68;
#ifdef HAVE_DIAGS
	my_diag("NEW read current position ID\n");
#endif

	a_char = 0x40;

	if(fill_sub == 't')  /*read subroutine position ID*/
	{
		a_char |= 16; 

		sub_seg_num = sub_nums[which_sub];
		sub_seg_ptr = (unsigned long)sub_insert_ptrs[which_sub];

		sub_seg_ptr = sub_seg_ptr +
			 (unsigned long)sub_starts[sub_seg_num];
	}
	else   /* read segment position ID*/
	{
		sub_seg_num = segment_nums[which_cseg];
		sub_seg_ptr = (unsigned long)my_ftell();

		sub_seg_ptr = sub_seg_ptr + seg_starts[which_cseg];
  /*Newest version*/
	}




	most_sig = (sub_seg_num >> 6) & 0x003f;
	least_sig = sub_seg_num & 0x003f;

	first = (sub_seg_ptr >> 18) & 0x0003;
	second = (sub_seg_ptr >> 12) & 0x003f;
	third = (sub_seg_ptr >> 6) & 0x003f;
	fourth = sub_seg_ptr & 0x003f;

	my_ugout(0x1d);
	my_ugout(a_char);
	my_ugout(most_sig | 0x40);
	my_ugout(least_sig | 0x40);
	my_ugout(first | 0x40);
	my_ugout(second | 0x40);
	my_ugout(third | 0x40);
	my_ugout(fourth | 0x40);
	my_ugout(0x0d);



}






void far cap_E_func(sub_count,which_subr)  /* rewrite segment/subroutine */
int *sub_count,which_subr;
{
#ifdef elrlrlrlrllrlrl
int i;
unsigned int id,displacement,sub_seg_ptr,temp_seg_ptr;
unsigned int temp;
unsigned int num_blocks;

	read_more(7);

		diag_num = 69;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);

		id = comp16_3(megatek_data[2],megatek_data[3],
			megatek_data[4]);

		displacement = ((megatek_data[5] & 0x3f) << 6) |
					(megatek_data[6] & 0x3f);

		if((megatek_data[2] & 32) != 0) /*specifies a subroutine*/
		{
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"old rewrite subroutine sub = %d ID = %d displacement = %d\n",
				a_segment,id,displacement);
			my_diag(diag_string);
		#endif
		

			for(i = 0 ; i < 20 ; i++)
				if(sub_nums[i] == a_segment)
					break;

			fill_sub = 't';

			if(i == 20) /*subroutine doesn't exist*/
			{
				for(i = 0 ; i < 20 ; i++)
					if(sub_nums[i] == -1)
						break;

				sub_nums[i] = a_segment;
				which_sub = i;
				return;
			}


			if((a_segment == 6) || (a_segment == 3) 
				|| (a_segment == 7))
				change_sub3_6_7 = 't';

                        if(a_segment >= 10) /*record which sub*/
                                last_sub_rewritten = a_segment;
                        else
                                last_sub_rewritten = -1;



			which_sub = i;
			sub_rewrite = 't';


			if(id + displacement - sub_starts[i]
				 < sub_insert_ptrs[i])
			{
				save_sub_insert_ptr = sub_insert_ptrs[i];
				sub_insert_ptrs[i] = id + displacement
				    - sub_starts[i];
			}
			else 
				save_sub_insert_ptr = -2;

		}
		else  /*specifies a segment*/
		{
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"old rewrite segment segment = %d ID = %d displacement = %d\n",
				a_segment,id,displacement);
			my_diag(diag_string);
		#endif

			for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
				if(segment_nums[i] == a_segment)
					break;

			if(i == NUMBER_OF_SEGS)
			{
				sprintf(diag_string,
					"no such segment as %d in rewrite segment\n",a_segment);
				error_logger(diag_string);
				return;
			}


                        if((a_segment == 1) && (rewrite_file != NULL))
                        {
                                fprintf(rewrite_file,"%9u  %5u\n",
                                          id,displacement);
                                fflush(rewrite_file);
                        }


			fclose(open_segment);
			which_cseg = i;
			sprintf(out_string,"%c:segm%d",drive_letter,which_cseg);
			open_segment = fopen(out_string,"r+b");
			fseek(open_segment,0L,SEEK_END);
			temp_seg_ptr = (unsigned int)(ftell(open_segment)/(long)
					size_an_element);
		
		   if(which_cseg == 1)
		   {
			temp = id + displacement - segm1_start;
			num_blocks = temp/64;
			temp += num_blocks*2;

			if(temp + 1 <= temp_seg_ptr)
				fseek(open_segment,(long)temp
					*(long)size_an_element,
						SEEK_SET);
		   }
		   else if(which_cseg == 2)
		   {
			temp = id + displacement - segm2_start;
			num_blocks = temp/64;
			temp += num_blocks*2;

			if(temp + 1 <= temp_seg_ptr)
				fseek(open_segment,(long)temp
					*(long)size_an_element,
						SEEK_SET);
		   }
		   else
		   {
			temp = id + displacement - 1000;
			num_blocks = temp/64;
			temp += num_blocks*2;

			if(temp + 1 <= temp_seg_ptr)
				fseek(open_segment,(long)temp
					*(long)size_an_element,
						SEEK_SET);
		   }


		get_our_value(99,8);
		printf("REWRITTEN WORD: op_code = %3d , arg1 = %6d , arg2 = %6d\n",
			read_element.op_code,read_element.arg1,read_element.arg2);
		printf("IS AT %u\n",temp);

		fseek(open_segment,(long)temp*(long)size_an_element,SEEK_SET);




			rewrite_flag = 't';
		}
#endif	
}







void far new_rewrite()  /*NEW REWRITE SEGMENT/SUBROUTINE */
{
int i;
unsigned long id,displacement,sub_seg_ptr,temp_seg_ptr;
unsigned long temp,old_num_blocks,new_num_blocks;
unsigned long block_adjust,remainder,final_result,end_adjust;


	read_more(8);

		diag_num = 69;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);

/*		id = comp16_3(megatek_data[2],megatek_data[3],
			megatek_data[4]);
*/
		id = (unsigned long)megatek_data[2] & 0x03;
		id <<= 6;
		id |= (unsigned long)megatek_data[3] & 0x3f;
		id <<= 6;
		id |= (unsigned long)megatek_data[4] & 0x3f;
		id <<= 6;
		id |= megatek_data[5] & 0x3f;

		displacement = (unsigned long)(((megatek_data[6] & 0x3f) << 6) 
					| (megatek_data[7] & 0x3f));

		if((megatek_data[2] & 32) != 0) /*specifies a subroutine*/
		{
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"new rewrite subroutine sub = %d ID = %d displacement = %d\n",
				a_segment,id,displacement);
			my_diag(diag_string);
		#endif
		

			for(i = 0 ; i < 20 ; i++)
				if(sub_nums[i] == a_segment)
					break;

			fill_sub = 't';

			if(i == 20) /*subroutine doesn't exist*/
			{
				for(i = 0 ; i < 20 ; i++)
					if(sub_nums[i] == -1)
						break;

				sub_nums[i] = a_segment;
				which_sub = i;
				return;
			}


			if((a_segment == 6) || (a_segment == 3) 
				|| (a_segment == 7))
				change_sub3_6_7 = 't';

                        if(a_segment >= 10) /*record which sub*/
                                last_sub_rewritten = a_segment;
                        else
                                last_sub_rewritten = -1;



			which_sub = i;
			sub_rewrite = 't';


			if(id + displacement - (unsigned long)
				sub_starts[sub_nums[i]] < (unsigned long)
				     sub_insert_ptrs[i])
			{
				save_sub_insert_ptr = sub_insert_ptrs[i];
				sub_insert_ptrs[i] = (int)(id + displacement)
				      - sub_starts[sub_nums[i]];
			}
			else 
				save_sub_insert_ptr = -2;

		}
		else  /*specifies a segment*/
		{
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"new rewrite segment segment = %d ID = %ld displacement = %ld\n",
				a_segment,id,displacement);
			my_diag(diag_string);
		#endif

			for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
				if(segment_nums[i] == a_segment)
					break;

			if(i == NUMBER_OF_SEGS)
			{
				sprintf(diag_string,
					"no such segment as %d in rewrite segment\n",
						a_segment);
				error_logger(diag_string);
				return;
			}


                        if((a_segment == 1) && (rewrite_file != NULL))
                        {
                                fprintf(rewrite_file,"%9lu  %5lu\n",
                                          id,displacement);
                                fflush(rewrite_file);
                        }


			if(megatek_ends[which_cseg] < 
				current_megatek_insert_pos)
					megatek_ends[which_cseg] =
						current_megatek_insert_pos;

			if(no_more_ems_pages == 't')
				return;




		if(rewrite_flag == 't')
		{
			rewrite_flag = 'f';


                        if((rw_sub1_0_flag == 't') || (rewrite_color == 't'))
			{
				display_part_seg();
				rw_sub1_0_flag = 'x';
                                rewrite_color = 'f';
				num_op_code_4s = 0;
				black = 'f';
			}

		}




			save_host_id = id;
			save_host_displacement = displacement;


/*
printf("Rewrite segment %d:   ID = %ld  Displ = %ld\n",
	i,id,displacement);
*/
	



/*			fclose(open_segment);
			which_cseg = i;
			sprintf(out_string,"%c:segm%d",drive_letter,which_cseg);
			open_segment = fopen(out_string,"r+b");
			fseek(open_segment,0L,SEEK_END);
			temp_seg_ptr = ftell(open_segment)/(long)
						size_an_element;
*/

			which_cseg = i;
			goto_megatek_end();  /*maps it too*/
			temp_seg_ptr = my_ftell();
		

			temp = id + displacement - seg_starts[which_cseg];
			block_adjust = (id - seg_starts[which_cseg])/64L;
			

/*JUNE 19, 1989 NEW VERSION OF WHERE WE REWRITE COMPUTATION*/

		old_num_blocks = 1L;
		new_num_blocks = 0L;
		final_result = temp;
		end_adjust = 0L;


		while(99)
		{
			old_num_blocks = new_num_blocks;

			new_num_blocks = final_result/64L - block_adjust
				+ end_adjust;

			final_result = temp + new_num_blocks*2L;
			remainder = final_result % 64L;

			if((remainder == 62L) || (remainder == 63L))
				end_adjust = 1L;
			else
			{
				end_adjust = 0L;
				if(old_num_blocks == new_num_blocks)
					break;
			}

		}





		
		if(final_result <= temp_seg_ptr)  /*allow it*/
		{
			if(megatek_seek(which_cseg,final_result) == FAILURE)
				report_seek_error(which_cseg,final_result);
		}
		else
		{
			sprintf(diag_string,
				"REWRITE AFTER END OF SEGMENT# %d: final result = %lu temp_seg_ptr = %lu\n",
					which_cseg,final_result,temp_seg_ptr);
			error_logger(diag_string);
			sprintf(diag_string,"THE HOST THINKS ITS: ID = %lu , DISPL = %lu\n",
				id,displacement);
			error_logger(diag_string);
		}



		current_megatek_insert_pos = final_result;
		if(megatek_seek(which_cseg,final_result) == FAILURE)
			report_seek_error(which_cseg,final_result);

		get_our_value(8,9);

		op_code_to_overwrite = read_element.op_code;

		if((read_element.op_code != 0) && (read_element.op_code != 4))
		{
			sprintf(diag_string,
				"********REWRITE ERROR******** IN SEGMENT# %d:\n",
					which_cseg);
			error_logger(diag_string);
			sprintf(diag_string,"WE REWRITE AT %lu\n",final_result);
			error_logger(diag_string);
			sprintf(diag_string,"THE OP CODE = %d\n",
				read_element.op_code);
			error_logger(diag_string);
			sprintf(diag_string,"THE HOST THINKS ITS: ID = %lu , DISPL = %lu\n",
					id,displacement);
			error_logger(diag_string);
		}


		rewrite_flag = 't';
	}
	
}







void far left_curly(sub_count,which_subr)  /* read segment starting address */
int *sub_count,which_subr;
{
int i;
unsigned int address;
char first,second,third;


	read_more(2);
	
	diag_num = 67;

	a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
#ifdef HAVE_DIAGS
	sprintf(diag_string,"read segment starting address: %d\n",a_segment);
	my_diag(diag_string);
#endif
	for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
		if(segment_nums[i] == a_segment)
			break;

	if(i == NUMBER_OF_SEGS)
	{
		sprintf(diag_string,"no such segment as %d in read segment start address",
				a_segment);
		error_logger(diag_string);

		my_ugout(0x1d);
		my_ugout(0x20);
		my_ugout(0x43);
		my_ugout(0x44);
		my_ugout(0x0d);

		return;
	}

	address = i * 2222;

	first = address >> 12;
	if(headers_segs[i].visibility = 't')
		first |= 32;

	second = (address >> 6) & 0x003f;
	third = address & 0x003f;

	my_ugout(0x1d);
	my_ugout(first | 0x40);
	my_ugout(second | 0x40);
	my_ugout(third | 0x40);
	my_ugout(0x0d);
	
}




void far o_func(sub_count,which_subr)  /* read/write vector memory */
int *sub_count,which_subr;
{
unsigned int most_sig,least_sig,i,j,k,l,check_sum = 0;
char first,second,third;


	read_more(3);


	for(l = 0 ; l < 3 ; l++)
		check_sum += megatek_data[l] & 0x3f;


	diag_num = 70;

	i = comp16_3(megatek_data[0],megatek_data[1],
		megatek_data[2]);

	if((megatek_data[0] & 32) != 0)  /*write at insert pointer*/
	{

		read_more(3);
	

		j = comp16_3(megatek_data[0],megatek_data[1],
				megatek_data[2]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"write at insert pointer: most sig data = %u least sig data = %u\n",i,j);
		my_diag(diag_string);
	#endif
		for(l = 0 ; l < 3 ; l++)
			check_sum += megatek_data[l] & 0x3f;
		
		if(fill_sub == 'f') /*write at insert pointer into a segment*/
		{
			an_element.op_code = 50;
			an_element.arg1 = i;
			an_element.arg2 = j;

			insert_ours(an_element,which_cseg);

		}
		else   /*write at insert pointer into a subroutine*/
		{
			if(i == 57358)
			{
				if(j == 65535)
					an_element.op_code = 60;
				else if(j == 65520)
					an_element.op_code = 70;
				else
				{
					an_element.op_code = 50;
					an_element.arg1 = i;
					an_element.arg2 = j;
				}
			}
			else
			{
				an_element.op_code = 50;
				an_element.arg1 = i;
				an_element.arg2 = j;
			}

			insert_sub(an_element,which_sub);
		}

/*		my_ugout(0x1d);
		my_ugout((check_sum % 63) | 0x40);
		my_ugout(0x0d);
*/
                switch(check_resp)
                {
                        case 0 : /*real checksums*/
                        {
                  		my_ugout(0x1d);
		                my_ugout((check_sum % 63) | 0x40);
		                my_ugout(0x0d);
                                break;
                        }

                        case 1 : /*fixed checksums = 0x41*/
                        {
                                my_ugout(0x1d);
                                my_ugout(0x41);
                                my_ugout(0x0d);
                                break;
                        }

                        case 2 : break;
                }


	}
	else if((megatek_data[0] & 16) != 0) /*write specified address*/
	{

		read_more(6);
	

			j = comp16_3(megatek_data[0],megatek_data[1],
				megatek_data[2]);

			k = comp16_3(megatek_data[3],megatek_data[4],
				megatek_data[5]);
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"write specified address: address = %u most sig data = %u least sig data = %u\n",i,j,k);
			my_diag(diag_string);
		#endif

		switch(i)
		{
			case 512 :
			{
				vm512.most_sig = j;
				vm512.least_sig = k;
				break;
			}

			case 345 :
			{
				vm345.most_sig = j;
				vm345.least_sig = k;
				break;
			}

			case 33536 :
			{
				vm33536.most_sig = j;
				vm33536.least_sig = k;
				break;
			}

			case 49536 :
			{
				vm49536.most_sig = j;
				vm49536.least_sig = k;
				break;
			}

			case 1170 :
			{
				vm1170.most_sig = j;
				vm1170.least_sig = k;
				break;
			}

			case 1166 :
			{
				vm1166.most_sig = j;
				vm1166.least_sig = k;
				break;
			}

			case 69 :
			{
				old_x = comp_coord(j & 0x0fff) + 1;
				old_y = -(comp_coord(k & 0x0fff) + 1);

				last_mouse_x = joy_x_sample = mouse_x = new_x 
					= old_x;
				last_mouse_y = joy_y_sample = mouse_y = new_y 
					= last_neg_mouse_y = old_y;

				vm69.most_sig = j;
				vm69.least_sig = k;
				break;
			}

			case 7 :
			{
				vm7.most_sig = j;
				vm7.least_sig = k;
				break;
			}

			default :
			{
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"Unknown address in write specified address %u\n",
					i);
				my_diag(diag_string);
			#endif
			}
		}

		for(l = 0 ; l < 6 ; l++)
			check_sum += megatek_data[l] & 0x3f;



                switch(check_resp)
                {
                        case 0 : /*real checksums*/
                        {
                  		my_ugout(0x1d);
		                my_ugout((check_sum % 63) | 0x40);
		                my_ugout(0x0d);
                                break;
                        }

                        case 1 : /*fixed checksums = 0x41*/
                        {
                                my_ugout(0x1d);
                                my_ugout(0x41);
                                my_ugout(0x0d);
                                break;
                        }

                        case 2 : break;
                }


	}
	else  /*read vector memory*/
	{
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"read vector memory: address = %u\n",i);
		my_diag(diag_string);
	#endif
		switch(i)
		{
			case 512 :
			{
				most_sig = vm512.most_sig;
				least_sig = vm512.least_sig;
				break;
			}

			case 345 :
			{
				most_sig = vm345.most_sig;
				least_sig = vm345.least_sig;
				break;
			}

			case 33536 :
			{
				most_sig = vm33536.most_sig;
				least_sig = vm33536.least_sig;
				break;
			}

			case 49536 :
			{
				most_sig = vm49536.most_sig;
				least_sig = vm49536.least_sig;
				break;
			}

			case 1170 :
			{
				most_sig = vm1170.most_sig;
				least_sig = vm1170.least_sig;
				break;
			}

			case 1166 :
			{
				most_sig = vm1166.most_sig;
				least_sig = vm1166.least_sig;
				break;
			}

			case 69 :
			{
				most_sig = vm69.most_sig;
				least_sig = vm69.least_sig;
				break;
			}

			case 7 :
			{
				most_sig = vm7.most_sig;
				least_sig = vm7.least_sig;
				break;
			}

			default : 
			{
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"Unknown address in read vector memory %u\n",
						i);
				my_diag(diag_string);
			#endif
			}
		}

		my_ugout(0x1d);

		first = most_sig >> 12;
		second = (most_sig >> 6) & 0x003f;
		third = most_sig & 0x003f;

		my_ugout(first | 0x60);
		my_ugout(second | 0x40);
		my_ugout(third | 0x40);

		first = least_sig >> 12;
		second = (least_sig >> 6) & 0x003f;
		third = least_sig & 0x003f;

		my_ugout(first | 0x40);
		my_ugout(second | 0x40);
		my_ugout(third | 0x40);

		my_ugout(0x0d);


	}

}

