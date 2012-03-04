/* MY_SEGS.C    host segment operations  */




#include "my_globs.h"
#include "my_utils.h"
#include "expanded.h"
#include "memory.h"


/*
#define SEG_DIAGS
*/


char far delete_locate_seg;


struct our_element far copy_header_buffer[17];




void far at_func(sub_count,which_subr)  /* open segment */
int *sub_count,which_subr;
{
int i,j;
long num_blocks_total;

	read_more(3);

		rw_sub1_0_flag = 'x';	
		is_a_seg_open = 't';

		diag_num = 71;

		a_segment = comp_rel_coord(megatek_data[1],megatek_data[2]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"open segment %d\n",a_segment);
		my_diag(diag_string);
	#endif
		diag_num = -71;

		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
			{
			#ifdef SEG_DIAGS
				printf("RE open segment %d\n",i);
			#endif

				if(megatek_ends[which_cseg] <
				    current_megatek_insert_pos)
					megatek_ends[which_cseg] = 
						current_megatek_insert_pos;


				which_cseg = i;


			/* clear out already open segment */
				for(j = 0 ; j < 256 ; j++)
					if(available_pages[j] == which_cseg)
						available_pages[j] = -1;


				for(j = 0 ; megatek_locate[which_cseg][j]
				   != 0xff ; j++)
					megatek_locate[which_cseg][j] = 0xff;



				current_megatek_page = get_next_free_page();
				megatek_locate[which_cseg][0] = 
					current_megatek_page;
				megatek_ends[which_cseg] = 0L;
				current_megatek_insert_pos = 0L;
				current_megatek_pointer = (char far *)
					start_of_page_frame_0;
/*
				map_handle_page(current_megatek_page,0,handle);
*/



			/* copy segment 0's  header into a buffer */
				megatek_seek(0,0L);
				for(j = 0 ; j < size_header ; j++)
				{
					get_our_value(4,5);

					memcpy(&copy_header_buffer[j],
						&read_element,size_an_element);

					megatek_seek(0,
						current_megatek_insert_pos + 
							1L);
				}


			/* copy the buffered data into cleared segment */
				megatek_seek(which_cseg,0L);
				for(j = 0 ; j < size_header ; j++)
				{
					memcpy(&an_element,
						&copy_header_buffer[j],
						size_an_element);

					insert_megatek();
				}
			

				goto reset_header;

			} /*end if within for*/


		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == -1)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,
				"error no more segments available in open segment call\n");
			error_logger(diag_string);
			which_cseg = 22;
			return;
		}


			#ifdef SEG_DIAGS
				printf("OPEN segment %d\n",i);
			#endif


		if(megatek_ends[which_cseg] < current_megatek_insert_pos)
			megatek_ends[which_cseg] = current_megatek_insert_pos;


		segment_nums[i] = a_segment;

		which_cseg = i;


		current_megatek_page = get_next_free_page();
		megatek_locate[which_cseg][0] = current_megatek_page;
		megatek_ends[which_cseg] = 0L;
		current_megatek_insert_pos = 0L;
		current_megatek_pointer = (char far *)start_of_page_frame_0;

/*
		map_handle_page(current_megatek_page,0,handle);
*/



/*copy segment 0's in-the-file header to the new segment*/

   /* copy segment 0's  header into a buffer */
	megatek_seek(0,0L);
	for(j = 0 ; j < size_header ; j++)
	{
		get_our_value(4,5);
		memcpy(&copy_header_buffer[j],&read_element,size_an_element);
		megatek_seek(0,current_megatek_insert_pos + 1L);
	}


   /* copy the buffered data into new segment */
	megatek_seek(which_cseg,0L);
	for(j = 0 ; j < size_header ; j++)
	{
		memcpy(&an_element,&copy_header_buffer[j],size_an_element);
		insert_megatek();
	}
			



reset_header:
		if((megatek_data[0] & 2) != 0) /*transform segment*/
		{
		#ifdef HAVE_DIAGS
			my_diag("transform segment\n");
		#endif
			headers_segs[i].is_transform = 't';
			num_transform_segs++;
		}
		else  /*basic segment*/
		{
		#ifdef HAVE_DIAGS
			my_diag("basic segment\n");
		#endif
			headers_segs[i].is_transform = 'f';
		}


		megatek_seek(which_cseg,0L);
		an_element.op_code = 110;
	
		if((megatek_data[0] & 1) != 0) /*visability ON*/
		{
		#ifdef HAVE_DIAGS
			my_diag("visibility ON\n\n");
		#endif
			headers_segs[i].visibility = 't';
			an_element.flag1 = 't';
		}
		else  /*visability OFF*/
		{
		#ifdef HAVE_DIAGS
			my_diag("visibility OFF\n\n");
		#endif
			headers_segs[i].visibility = 'f';
			an_element.flag1 = 'f';
		}


		insert_megatek();


		headers_segs[i].Xform_enable = headers_segs[0].Xform_enable;
		if((a_segment % 2) != 0)
			headers_segs[i].Xform_enable = 't';
/*
		headers_segs[i].clip_x_lower = headers_segs[0].clip_x_lower;
		headers_segs[i].clip_y_lower = headers_segs[0].clip_y_lower;
		headers_segs[i].clip_x_upper = headers_segs[0].clip_x_upper;
		headers_segs[i].clip_y_upper = headers_segs[0].clip_y_upper;
*/

		headers_segs[i].save_x_lower_clip =
                         headers_segs[0].save_x_lower_clip;
		headers_segs[i].save_y_lower_clip =
                         headers_segs[0].save_y_lower_clip;
		headers_segs[i].save_x_upper_clip =
                         headers_segs[0].save_x_upper_clip;
		headers_segs[i].save_y_upper_clip =
                         headers_segs[0].save_y_upper_clip;

/*
		headers_segs[i].rotate3D[0][0] = headers_segs[0].rotate3D[0][0];
		headers_segs[i].rotate3D[1][0] = headers_segs[0].rotate3D[1][0];
		headers_segs[i].rotate3D[2][0] = headers_segs[0].rotate3D[2][0];
		headers_segs[i].rotate3D[3][0] = headers_segs[0].rotate3D[3][0];
		headers_segs[i].rotate3D[0][1] = headers_segs[0].rotate3D[0][1];
		headers_segs[i].rotate3D[1][1] = headers_segs[0].rotate3D[1][1];
		headers_segs[i].rotate3D[2][1] = headers_segs[0].rotate3D[2][1];
		headers_segs[i].rotate3D[3][1] = headers_segs[0].rotate3D[3][1];
*/

		headers_segs[i].save_rotate3D[0][0] =
                        headers_segs[0].save_rotate3D[0][0];
		headers_segs[i].save_rotate3D[1][0] =
                        headers_segs[0].save_rotate3D[1][0];
		headers_segs[i].save_rotate3D[2][0] =
                        headers_segs[0].save_rotate3D[2][0];
		headers_segs[i].save_rotate3D[3][0] =
                        headers_segs[0].save_rotate3D[3][0];
		headers_segs[i].save_rotate3D[0][1] =
                        headers_segs[0].save_rotate3D[0][1];
		headers_segs[i].save_rotate3D[1][1] =
                        headers_segs[0].save_rotate3D[1][1];
		headers_segs[i].save_rotate3D[2][1] =
                        headers_segs[0].save_rotate3D[2][1];
		headers_segs[i].save_rotate3D[3][1] =
                        headers_segs[0].save_rotate3D[3][1];


/*initialize hcrst from segment 0's in-the-file matrix*/

		megatek_seek(0,2L);
		get_our_value(9,999);

		hcrst[0][0] = read_element.arg1/2048.0;
		hcrst[1][0] = read_element.arg2/2048.0;
		hcrst[2][0] = read_element.arg3/2048.0;
		hcrst[3][0] = (float)read_element.arg4;

		megatek_seek(0,3L);
		get_our_value();

		hcrst[0][1] = read_element.arg1/2048.0;
		hcrst[1][1] = read_element.arg2/2048.0;
		hcrst[2][1] = read_element.arg3/2048.0;
		hcrst[3][1] = (float)read_element.arg4;

		goto_megatek_end();


		headers_segs[i].x_origin = headers_segs[0].x_origin;
		headers_segs[i].y_origin = headers_segs[0].y_origin;
		headers_segs[i].z_origin = headers_segs[0].z_origin;

		headers_segs[i].dash_enable = headers_segs[0].dash_enable;
		headers_segs[i].blink_enable = headers_segs[0].blink_enable;


		headers_segs[i].intensity_color = headers_segs[0].intensity_color;
		headers_segs[i].blink_rate = headers_segs[0].blink_rate;
		headers_segs[i].dash_pattern = headers_segs[0].dash_pattern;


}







void far cap_A_func(sub_count,which_subr)  /* append segment */
int *sub_count,which_subr;
{
int i;
	read_more(2);


		is_a_seg_open = 't';

	
		diag_num = 75;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"append segment %d\n",a_segment);
		my_diag(diag_string);
	#endif
		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"no such segment # as %d in append segment\n",
					a_segment);
			error_logger(diag_string);
			return;
		}



			#ifdef SEG_DIAGS
				printf("Append segment %d\n",i);
			#endif


		if(megatek_ends[which_cseg] < current_megatek_insert_pos)
			megatek_ends[which_cseg] = current_megatek_insert_pos;



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



		which_cseg = i;
		goto_megatek_end();


	        hcrst[0][0] = headers_segs[i].save_rotate3D[0][0];
	        hcrst[1][0] = headers_segs[i].save_rotate3D[1][0];
	        hcrst[2][0] = headers_segs[i].save_rotate3D[2][0];
	        hcrst[3][0] = headers_segs[i].save_rotate3D[3][0];
	        hcrst[0][1] = headers_segs[i].save_rotate3D[0][1];
	        hcrst[1][1] = headers_segs[i].save_rotate3D[1][1];
	        hcrst[2][1] = headers_segs[i].save_rotate3D[2][1];
	        hcrst[3][1] = headers_segs[i].save_rotate3D[3][1];

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
                        headers_segs[which_cseg].save_y_upper_clip,0);

                my_lock_viewport();
 	    }





	if(headers_segs[i].dash_enable == 't')
		my_dash_enable(4);
	else
		my_dash_enable(0);

}





void far cap_D_func(sub_count,which_subr)  /* close segment */
int *sub_count,which_subr;
{
	
		diag_num = 72;
	#ifdef HAVE_DIAGS
		my_diag("close segment\n");
	#endif

			#ifdef SEG_DIAGS
				printf("Close segment %d\n",which_cseg);
			#endif



		if(megatek_ends[which_cseg] < current_megatek_insert_pos)
			megatek_ends[which_cseg] = current_megatek_insert_pos;


		is_a_seg_open = 'f';

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
		
         	rw_sub1_0_flag = 'f';


		which_cseg = 0;
		goto_megatek_end();
}




void far cap_C_func(sub_count,which_subr)  /* delete segment */
int *sub_count,which_subr;
{
int i,j,k,save_which_cseg;
long num_words_seg,num_blocks_in_seg;


		read_more(2);

/*		if(joy_off == 't')
		{
			joy_off = 'f';
			set_joystick(1);
		}
*/

		diag_num = 73;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"delete segment %d\n",a_segment);
		my_diag(diag_string);
	#endif
		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"no such segment # as %d in delete segment\n",
				a_segment);
			error_logger(diag_string);
			return;
		}


			#ifdef SEG_DIAGS
				printf("Delete segment %d\n",i);
			#endif


		if(segment_nums[locate_seg] == a_segment)
			delete_locate_seg = 't';

		my_color((char)0);
		draw_black = 't';
		display_a_seg(i);
		draw_black = 'f';

		delete_locate_seg = 'f';

		headers_segs[i].visibility = 'f';
		segment_calls[i] = -1;


		if(segment_nums[locate_seg] == a_segment)
		{
			locate_1st_draw = 'u';
			locate_called = 'f';
		}


		if(which_cseg != i)   /*empty a closed megatek segment*/
		{
			for(k = 0 ; megatek_locate[i][k] != 0xff ; k++)
				megatek_locate[i][k] = 0xff;

			megatek_ends[i] = 0L;
		}
		else
		{
			for(k = 0 ; megatek_locate[which_cseg][k] != 0xff ; 
			     k++)
				megatek_locate[which_cseg][k] = 0xff;

			megatek_ends[which_cseg] = 0L;

		/*switch to segment 0*/
			save_which_cseg = which_cseg;
			which_cseg = 0;
			goto_megatek_end();
			which_cseg = save_which_cseg;
		}



		if(which_cseg > 0)
			segment_nums[i] = -1;

		if(which_cseg == i)
			which_cseg = 0;


		rw_sub1_0_flag = 'f';


		for(k = 0 ; k < 256 ; k++)
			if(available_pages[k] == i)
				available_pages[k] = -1;
		
}




void far cap_B_func(sub_count,which_subr)  /* clear segment */
int *sub_count,which_subr;
{
int i,j;
long num_blocks_in_seg,num_words_seg;

	read_more(2);

		diag_num = 74;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"clear segment %d\n",a_segment);
		my_diag(diag_string);
	#endif
		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"no such segment # as %d in clear segment\n",
				a_segment);
			error_logger(diag_string);
			return;
		}


			#ifdef SEG_DIAGS
				printf("Clear segment %d\n",i);
			#endif


		which_cseg = i;


		my_color((char)0);
		draw_black = 't';
		display_a_seg(i);
		draw_black = 'f';

                for(j = 0 ; j < 10 ; j++)
                        if(where_called[ins_matrix_sub - 10][j] == i)
                        {
                                ins_matrix_flag = 'f';
                                break;
                        }


		rw_sub1_0_flag = 'f';

		segment_calls[which_cseg] = -1;



/*deallocate megatek pages for the segment*/
	for(j = 0 ; j < 256 ; j++)
		if(available_pages[j] == which_cseg)  /*the page is for this segment*/
			available_pages[j] = -1;


/* clear out the EMS megatek variables */
		for(j = 0 ; megatek_locate[i][j] != 0xff ; j++)
			megatek_locate[i][j] = 0xff;


		current_megatek_page = get_next_free_page();
		megatek_locate[which_cseg][0] = current_megatek_page;
		megatek_ends[which_cseg] = 0L;
		current_megatek_insert_pos = 0L;
		current_megatek_pointer = (char far *)start_of_page_frame_0;
/*
		map_handle_page(current_megatek_page,0,handle);
*/




	/*copy segment 0's in-the-file header to the cleared segment */
	
	   /* copy segment 0's  header into a buffer */
		megatek_seek(0,0L);
		for(j = 0 ; j < size_header ; j++)
		{
			get_our_value(4,5);
			memcpy(&copy_header_buffer[j],&read_element,size_an_element);
			megatek_seek(0,current_megatek_insert_pos + 1L);
		}


	   /* copy the buffered data into cleared segment */
		megatek_seek(which_cseg,0L);
		for(j = 0 ; j < size_header ; j++)
		{
			memcpy(&an_element,&copy_header_buffer[j],size_an_element);
			insert_megatek();
		}
			

		display_a_seg(i);  /*to execute the in-the-file header*/

}



void far cap_F_func(sub_count,which_subr)  /* rename segment */
int *sub_count,which_subr;
{
int i;
	read_more(4);
	

			#ifdef SEG_DIAGS
				printf("Rename segment\n");
			#endif


		diag_num = 76;

		old_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
		new_segment = comp_rel_coord(megatek_data[2],megatek_data[3]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"rename segment from %d to %d\n",old_segment,
				new_segment);
		my_diag(diag_string);
	#endif
		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == old_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"no such segment # as %d in rename segment\n",
					old_segment);
			error_logger(diag_string);
			return;
		}

		segment_nums[i] = new_segment;
		which_cseg = 0;
		goto_megatek_end();
		which_sub = -1;
	
}

