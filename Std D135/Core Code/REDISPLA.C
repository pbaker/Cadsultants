/*October 18, 1988 11:00 made megatek_update and matrix_update only switch
pages when num_pages == 2*/

/*October 20, 1988 changed open segment so it copies the header data from
segment 0's file to the header of the new file, same with clear segment,
added header in-the-file commands to interp_seg_command(), and removed
header array matrix to hcrst copying from display_segs() and display_a_seg()*/

/*October 21, 1988 made void far get_next_non44() to skip over "words" with
an op code of 44 and made interp_seg_command() us it instead of get_our_value()
*/

/*November 1, 1988 made show time taken by redisplay (display_segs()) use
my bios routines for positioning of the message*/

/*November 2, 1988 made display_segs() (when should display time) put up
"TIMER" in blink mode and then "%d Seconds" in normal mode*/

/*November 3, 1988 replaced    if(kbhit() != 0) 
			       {
				   .
				   .
				   .
				   .
			       }

in display_segs() with a call to my_pckb() */

/*November 8, 1988 added curser undraw/redraw code to megatek_update()*/

/*November 15, 1988 made changes to use generic graphics board calls*/

/*January 31, 1989 made changes to send the error messages to the error file*/

/*February 8, 1989 changed interp_seg_command() so the character string part
can handle an empty last word*/

/*February 24, 1989 fixed character string case in interp_seg_command()
so my_x gets incremented for each call to my_char()*/

/*February 24, 1989 added code to call sub to look for rewrite of sub call
over a color*/


#include "my_utils.h"
#include "my_globs.h"
#include "time.h"
#include "expanded.h"


/*for rewrite with sub call test*/
extern int far op_code_to_overwrite;
extern long far save_host_id;
extern long far save_host_displacement;



/*tells for each segment, its current change of visibility state.
 used by a_visibility_switched_to_on() */

extern char far visibility_changed_to_on[];

extern float far res_factor;
extern int my_invert_y;
extern char far doing_an_alt_u;
extern void far clear_my_listing();




extern int far last_logical_page;



char far a_visibility_switched_to_on()
{
int i;
char flag;

	flag = 'f';

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		if(visibility_changed_to_on[i] == 't')
		{
			headers_segs[i].visibility = 't';
			flag = 't';
		}



	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		visibility_changed_to_on[i] = 'f';

	return(flag);
}








void far seek_prev_4()  /*sets ptr_rw_sub1_0 to position of previous op code 4*/
			/*in rewrite a sub code, without changing the */
			/*open_segment FILE pointer's position at all*/
{
long save_file_ptr,backwards_ptr;


	backwards_ptr = save_file_ptr = my_ftell();
	quit_segment = 'f';
	
	get_our_value(8,8);

	while((read_element.op_code != 4) && (quit_segment == 'f'))
	{
		backwards_ptr -= 1L;
		if(megatek_seek(which_cseg,backwards_ptr) == FAILURE)
			quit_segment = 't';

		get_our_value(9,2);
	}

	ptr_rw_sub1_0 = backwards_ptr;
	megatek_seek(which_cseg,save_file_ptr);
	quit_segment = 'f';
}




void far matrix_update()
{
int save_which_cseg,my_pause,save_x_origin,save_y_origin,save_z_origin;



        if(ins_matrix_flag == 'f')
                button1 = 'f';

	if(ins_matrix_flag == 't') 
	{
	    a_visibility_switched_to_on();
	    clear_my_listing();
	    my_size_chars(0);

            save_which_cseg = which_cseg;

			if(rotate_mode == 'm')
			{ 
		        	if(curser_displayed == 't')
				{

					save_my_x = my_x;
					save_my_y = my_y;
	
					my_x = old_x;
					my_y = -old_y;

					save_line_type = line_type;
					my_dash_enable(0);

					draw_mode(3);

					interp_my_sub6();

					draw_mode(0);
/*					scolr(save_color);
					line_type = save_line_type;
					sltyp(line_type);
*/
					my_x = save_my_x;
					my_y = save_my_y;

				}
			}

           if((rotate_mode == 'a') && (num_pages == 2))
 	         which_page = !which_page;


            if(num_pages == 2)
            {
                if(rotate_mode == 'a')
                {
	         	my_set_viewport(headers_segs[which_cseg].save_x_lower_clip,
			      headers_segs[which_cseg].save_y_lower_clip,
			      headers_segs[which_cseg].save_x_upper_clip,
			      headers_segs[which_cseg].save_y_upper_clip,
				which_page);

	         	my_lock_viewport();
			my_wrmask(which_page);
	        	clear_page(which_page);
                }

                if(rotate_mode == 'm')
                {

	         	my_set_viewport(headers_segs[which_cseg].save_x_lower_clip,
			      headers_segs[which_cseg].save_y_lower_clip,
			      headers_segs[which_cseg].save_x_upper_clip,
			      headers_segs[which_cseg].save_y_upper_clip,
				which_page);

	         	my_lock_viewport();
			my_wrmask(which_page);
	        	clear_page(which_page);
			my_rdmask(which_page);
		        my_scroll(which_page);

                }
           
            }
            else
            {
                my_set_viewport(-2048,-2048,2048,2048,which_page);
                my_lock_viewport();
		my_rdmask(which_page);
		my_wrmask(which_page);
                clear_page(which_page);
	    }


				if(curser_displayed == 't')
				{

					save_my_x = my_x;
					save_my_y = my_y;
	
					my_x = old_x;
					my_y = -old_y;

					save_line_type = line_type;
					my_dash_enable(0);
				/*	scolr(0x0f);*/
					draw_mode(3);

					interp_my_sub6();

					draw_mode(0);
					my_color(save_color);
					line_type = save_line_type;
					my_dash_enable(line_type);

					my_x = save_my_x;
					my_y = save_my_y;

				}


			

	    display_segs();


            if(num_pages == 2)
            {
                if(rotate_mode == 'a')
                {
			my_rdmask(which_page);
		        my_scroll(which_page);
                }
            }
            else
	    {
		my_rdmask(which_page);
                my_scroll(which_page);
	    }


		which_redis = 3;
	

		ins_matrix_flag = 'f';
                button1 = 'f';

             which_cseg = save_which_cseg;
        }

}







void far megatek_update()
{
int save_which_cseg;

	if(update_level > 0)
		return;

	update_level++;

	if((ins_matrix_flag == 't') || (a_visibility_switched_to_on() == 't'))
	{
		my_size_chars(0);
		clear_my_listing();
		save_which_cseg = which_cseg;


			if(rotate_mode == 'm')
			{ 
		        	if(curser_displayed == 't')
				{

					save_my_x = my_x;
					save_my_y = my_y;
	
					my_x = old_x;
					my_y = -old_y;

					save_line_type = line_type;
					my_dash_enable(0);

					draw_mode(3);

					interp_my_sub6();

					draw_mode(0);
/*					scolr(save_color);
					line_type = save_line_type;
					sltyp(line_type);
*/
					my_x = save_my_x;
					my_y = save_my_y;

				}
			}


		if(num_pages == 2)
			which_page = !which_page;

	     if(num_pages == 2)
	     {
		if(rotate_mode == 'a')
		{
			my_set_viewport(headers_segs[which_cseg].clip_x_lower,
				headers_segs[which_cseg].clip_y_lower,
				headers_segs[which_cseg].clip_x_upper,
				headers_segs[which_cseg].clip_y_upper,
				  which_page);

			my_lock_viewport();
			my_wrmask(which_page);
			clear_page(which_page);
		}
		
		if(rotate_mode == 'm')
		{
			my_set_viewport(-2048,-2048,2048,2048,which_page);  
			my_lock_viewport();
			my_wrmask(which_page);
			clear_page(which_page);
			my_rdmask(which_page);
			my_scroll(which_page);
		}
	    }
	    else
	    {
		my_set_viewport(-2048,-2048,2048,2048,which_page);
		my_lock_viewport();
		my_rdmask(which_page);
		my_wrmask(which_page);
		clear_page(which_page);
	    }



		        	if(curser_displayed == 't')
				{

					save_my_x = my_x;
					save_my_y = my_y;
	
					my_x = old_x;
					my_y = -old_y;

					save_line_type = line_type;
					my_dash_enable(0);

					draw_mode(3);

					interp_my_sub6();

					draw_mode(0);
/*					scolr(save_color);
					line_type = save_line_type;
					sltyp(line_type);
*/
					my_x = save_my_x;
					my_y = save_my_y;

				}


		
			
	    display_segs();

	    if(num_pages == 2)
	    {
		if(rotate_mode == 'a')
		{
			my_rdmask(which_page);
			my_scroll(which_page);
		}
	    }
	    else
	    {
		my_rdmask(which_page);
		my_scroll(which_page);
	    }


		which_redis = 3;
	
		ins_matrix_flag = 'f';

		which_cseg = save_which_cseg;
	}


	update_level--;
}




void far get_next_non44()  /*gets the next non-44 op code "word"*/
{
	get_our_value(9,9);

	if(megatek_seek(which_cseg,current_megatek_insert_pos + 1L) 
	    == FAILURE)
		quit_segment = 't';


	while((read_element.op_code == 44) && (quit_segment == 'f'))
	{
		get_our_value(9,9);

		if(megatek_seek(which_cseg,current_megatek_insert_pos + 1L)
		   == FAILURE)
			quit_segment = 't';
	}
}





void interp_seg_command(index,which_seg)
int *index,which_seg;
{
int dummy,i,j;

	get_next_non44();

	if(quit_segment == 't')
		return;

	switch(read_element.op_code)
	{

		case 0 : /*call subroutine*/
		{
			fill_sub = 'f';
			our_call = 't';

/*			if((rw_sub1_0_flag == 't') && (rw_op_code == 0))
			{
				rw_sub1_0_flag = 'x';
				black = 'f';
				quit_segment = 't';

				return;
			}
*/
			e_func(&dummy,dummy);

			break;
		}

		case 1 : /*insert blink enable*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert blink enable\n");
		#endif
			break;
		}

		case 2 : /*insert dash enable*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert dash enable\n");
		#endif
			if((read_element.arg1 & 1) != 0) /*enable*/
			{
				line_type = 4;
				my_dash_enable(4);
			}
			else  /*disable*/
			{
				line_type = 0;
				my_dash_enable(0);
			}

	
			break;
		}

		case 3 : /*insert blink rate*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert blink rate\n");
		#endif
			break;
		}

		case 4 : /*insert (intensity/color)*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert (intensity/color)\n");
		#endif
			if(((rewrite_color == 't') || (rw_sub1_0_flag == 't'))
				 && (num_op_code_4s > 0))
			{
				quit_segment = 't';
				rewrite_color = 'x';
				rw_sub1_0_flag = 'x';
				num_op_code_4s = 0;
				return;
			}

			if((rewrite_color == 't') || (rw_sub1_0_flag == 't'))
				num_op_code_4s++;


			if((rw_sub1_0_flag  == 't') && (black == 't'))
			{
				save_color = (char)read_element.arg1 & 0x0f;
				my_color((char)0);
			}
			else if(draw_black == 't')
			{
				save_color = (char)read_element.arg1 & 0x0f;
				my_color((char)0);
			}
			else
				my_color((char)read_element.arg1 & 0x0f);

			save_color = (char)read_element.arg1 & 0x0f;

	
			break;
		}

		case 5 : /*insert color*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert color\n");
		#endif
			break;
		}

		case 6 : /*insert Xform enable*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert Xform enable\n");
		#endif
			if((read_element.arg1 & 1) == 0)
			{
			#ifdef HAVE_DIAGS
				my_diag("ENABLE XFORM\n");
			#endif
				headers_segs[which_cseg].Xform_enable = 't';
			}
			else
			{
			#ifdef HAVE_DIAGS
				my_diag("DISABLE XFORM\n");
			#endif
				headers_segs[which_cseg].Xform_enable = 'f';
			}


			break;
		}

		case 7 : /*insert 2D origin*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert 2D origin\n");
		#endif
			headers_segs[which_cseg].x_origin =
				read_element.arg1;

			headers_segs[which_cseg].y_origin =
				read_element.arg2;


			break;
		}

		case 8 : /*insert 3D origin*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert 3D origin\n");
		#endif
			headers_segs[which_cseg].x_origin =
				read_element.arg1;

			headers_segs[which_cseg].y_origin =
				read_element.arg2;


			get_next_non44();

			headers_segs[which_cseg].z_origin =
				read_element.arg1;


			break;
		}

		case 9 : /*insert Z origin*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert Z origin\n");
		#endif
			headers_segs[which_cseg].z_origin =
				read_element.arg1;


			break;
		}

		case 10 : /*insert 2D Xlate*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert 2D Xlate\n");
		#endif
			hcrst[3][0] =
				(float)read_element.arg1;
			headers_segs[which_cseg].save_rotate3D[3][0] =
				(float)read_element.arg1;

			get_next_non44();

			hcrst[3][1] =
				(float)read_element.arg1;
			headers_segs[which_cseg].save_rotate3D[3][1] =
				(float)read_element.arg1;

			break;
		}

		case 11 : /*insert 2D matrix*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert 2D matrix\n");
		#endif
			hcrst[0][0] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[0][0] =
				read_element.arg1/2048.0;

			get_next_non44();

			hcrst[1][0] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[1][0] =
				read_element.arg1/2048.0;

			hcrst[2][0] = 0.0;
			headers_segs[which_cseg].save_rotate3D[2][0] = 0.0;

			get_next_non44();

			hcrst[3][0] =
				(float)read_element.arg1;
			headers_segs[which_cseg].save_rotate3D[3][0] =
				(float)read_element.arg1;

			get_next_non44();

			hcrst[0][1] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[0][1] =
				read_element.arg1/2048.0;

			get_next_non44();

			hcrst[1][1] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[1][1] =
				read_element.arg1/2048.0;


			hcrst[2][1] = 0.0;
			headers_segs[which_cseg].save_rotate3D[2][1] = 0.0;

			get_next_non44();

			hcrst[3][1] =
				(float)read_element.arg1;
			headers_segs[which_cseg].save_rotate3D[3][1] =
				(float)read_element.arg1;


			break;
		}

		case 12 : /*insert 3D matrix*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert 3D matrix\n");
		#endif
			hcrst[0][0] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[0][0] =
				read_element.arg1/2048.0;


			get_next_non44();

			hcrst[1][0] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[1][0] =
				read_element.arg1/2048.0;


			get_next_non44();

			hcrst[3][0] =
				(float)read_element.arg1;
			headers_segs[which_cseg].save_rotate3D[3][0] =
				(float)read_element.arg1;


			get_next_non44();

			hcrst[0][1] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[0][1] =
				read_element.arg1/2048.0;


			get_next_non44();

			hcrst[1][1] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[1][1] =
				read_element.arg1/2048.0;


			get_next_non44();

			hcrst[3][1] =
				(float)read_element.arg1;
			headers_segs[which_cseg].save_rotate3D[3][1] =
				(float)read_element.arg1;


			get_next_non44();

			hcrst[2][0] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[2][0] =
				read_element.arg1/2048.0;


			get_next_non44();

			hcrst[2][1] =
				read_element.arg1/2048.0;
			headers_segs[which_cseg].save_rotate3D[2][1] =
				read_element.arg1/2048.0;


			break;
		}

		case 13 : /*insert clipping*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert clipping\n");
		#endif
			j = (float)(read_element.arg1 - 
			    headers_segs[which_cseg].x_origin)/
				1.33333333;

			headers_segs[which_cseg].save_x_lower_clip =
			  ((int)j + headers_segs[which_cseg].
				x_origin)/4;

			j = (float)(read_element.arg2 -
			    headers_segs[which_cseg].y_origin)/
				1.333333333;

			headers_segs[which_cseg].save_y_upper_clip =
			   my_invert_y - ((int)j + headers_segs[which_cseg].
				y_origin)/4;


			get_next_non44();

			j = (float)(read_element.arg1 - 
			   headers_segs[which_cseg].x_origin)/
				1.333333333;

			headers_segs[which_cseg].save_x_upper_clip =
			  ((int)j + headers_segs[which_cseg].x_origin)/4;

			j = (float)(read_element.arg2 -
			   headers_segs[which_cseg].y_origin)/
				1.333333333;

			headers_segs[which_cseg].save_y_lower_clip =
			   my_invert_y - ((int)j + headers_segs[which_cseg].
				y_origin)/4;



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
                              headers_segs[which_cseg].save_y_upper_clip,0);
                        my_lock_viewport();
		    }

			break;
		}

		case 14 : /*move 2D absolute*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: move 2D absolute\n");
		#endif
			my_x = read_element.arg1;
			my_y = read_element.arg2;

			break;
		}

		case 15 : /*draw 2D absolute*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: draw 2D absolute\n");
		#endif
			my_line3(my_x,my_y,my_z,
				read_element.arg1,read_element.arg2,my_z);

			my_x = read_element.arg1;
			my_y = read_element.arg2;

			break;
		}

		case 16 : /*move 2D relative*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: move 2D relative\n");
		#endif
			my_x += read_element.arg1;
			my_y += read_element.arg2;

			break;
		}

		case 17 : /*draw 2D relative*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: draw 2D relative\n");
		#endif
			my_line3(my_x,my_y,my_z,
			       my_x + read_element.arg1,my_y + read_element.arg2,
				my_z);

			my_x += read_element.arg1;
			my_y += read_element.arg2;

			break;
		}

		case 18 : /*move 3D absolute*/
		{
		#ifdef HAVE_DIAGS
			my_diag("move 3D absolute\n");
		#endif
			my_x = read_element.arg1;
			my_y = read_element.arg2;

			get_next_non44();
			my_z = read_element.arg1;

			break;
		}

		case 19 : /*draw 3D absolute*/
		{
		#ifdef HAVE_DIAGS
			my_diag("draw 3D absolute\n");
		#endif
			xx = read_element.arg1;
			yy = read_element.arg2;

			get_next_non44();
			zz = read_element.arg1;
			my_line3(my_x,my_y,my_z,xx,yy,zz);
			my_x = xx;
			my_y = yy;
			my_z = zz;

			break;
		}

		case 20 : /*move 3D relative*/
		{
		#ifdef HAVE_DIAGS
			my_diag("move 3D relative\n");
		#endif
			my_x += read_element.arg1;
			my_y += read_element.arg2;

			get_next_non44();
			my_z += read_element.arg1;

			break;
		}

		case 21 : /*draw 3D relative*/
		{
		#ifdef HAVE_DIAGS
			my_diag("draw 3D relative\n");
		#endif
			xx = read_element.arg1;
			yy = read_element.arg2;

			get_next_non44();
			zz = read_element.arg1;
			my_line3(my_x,my_y,my_z,my_x + xx,my_y + yy,my_z + zz);
			my_x += xx;
			my_y += yy;
			my_z += zz;

			break;
		}

		case 22 : /*short relative vectors*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: SHORT RELATIVE VECTORS\n");
		#endif
			flag = (unsigned char)read_element.arg2;


			if((flag & 0x08) != 0) /*then its a draw*/
				my_line3(my_x,my_y,my_z,
					my_x + read_element.arg3,
					my_y + read_element.arg4,my_z);

			my_x += read_element.arg3;
			my_y += read_element.arg4;


			get_next_non44();
			if(quit_segment == 't')
				return;

			while(read_element.op_code == 30) 
			{
				flag = read_element.flag1;

				if(flag == 0x40)
				{
					return;
				}

				if((flag & 0x08) != 0) /*then its a draw*/
				      my_line3(my_x,my_y,my_z,
						my_x + read_element.arg1,
						my_y + read_element.arg2,my_z);

				my_x += read_element.arg1;
				my_y += read_element.arg2;

				flag = read_element.flag2;
				if(flag != 0x40)
				{
					if((flag & 0x08) != 0) /*then its a draw*/
					      my_line3(my_x,my_y,my_z,
							my_x + read_element.arg3,
							my_y + read_element.arg4,
							my_z);

					my_x += read_element.arg3;
					my_y += read_element.arg4;
				}


				get_next_non44();
				if(quit_segment == 't')
					return;
			}


			if(my_ftell() >= 1L)
				megatek_seek(which_cseg,my_ftell() - 1L);


			break;	
		}


		case 23 : /*character string*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: character string\n");
		#endif

			if(read_element.arg3 != 0)
			{
				my_char((char)read_element.arg3,my_x,my_y,0,6);
				my_x += (int)(44*res_factor);
			}


			if(read_element.arg4 != 0)
			{
				my_char((char)read_element.arg4,my_x,
					my_y,0,6);
				my_x += (int)(44*res_factor);
			}


			get_next_non44();
			if(quit_segment == 't')
				return;


			while(read_element.op_code == 27)
			{
				if(read_element.arg1 != 0)
				{
					my_char((char)read_element.arg1,
						my_x,my_y,0,6);
					my_x += (int)(44*res_factor);
				}
				

				if(read_element.arg2 != 0)
				{
					my_char((char)read_element.arg2,
						my_x,my_y,0,6);
					my_x += (int)(44*res_factor);
				}

				if(read_element.arg3 != 0)
				{
					my_char((char)read_element.arg3,
						my_x,my_y,0,6);
					my_x += (int)(44*res_factor);
				}

				if(read_element.arg4 != 0)
				{
					my_char((char)read_element.arg4,
						my_x,my_y,0,6);
					my_x += (int)(44*res_factor);
				}


				get_next_non44();
				if(quit_segment == 't')
					return;
			}
		

			if(my_ftell() >= 1L)
				megatek_seek(which_cseg,my_ftell() - 1L);



			break;
		}


		case 24 : /*insert dash pattern*/
		{
		#ifdef HAVE_DIAGS
			my_diag("redisplay: insert dash pattern\n");
		#endif
			my_dash_pattern(read_element.arg1);


			break;
		}


		case 50 : /*compressed data*/
		{
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"compressed data in our segment# %d, index %d\n",
				which_seg,*index);
			my_diag(diag_string);
		#endif
			break;
		}


		case 44 : /*null word*/
		{

			break;
		}


		case 90 : /*locate joystick*/
		{
			if(which_redis == 1)
			{
				quit_redis = 't';

				break;
			}


			locate_called = 't';
			if(locate_1st_draw == 'u')
				locate_1st_draw = 't';
			locate_seg = which_cseg;
			my_x = save_joy_x;
			my_y = save_joy_y;
			draw_mode(3);

			break;
		}


		case 110 : /*copy visibility from segment's in-the-file header*/
		{
			headers_segs[which_cseg].visibility = read_element.flag1;
			break;
		}


		case 111 : /*copy Xform_enable from segment's in-the-file header*/
		{
			headers_segs[which_cseg].Xform_enable = read_element.flag1;
			break;
		}


		case 112 : /*copy 1st half of hcrst from the in-the-file header*/
		{
			hcrst[0][0] = read_element.arg1/2048.0;
			hcrst[1][0] = read_element.arg2/2048.0;
			hcrst[2][0] = read_element.arg3/2048.0;
			hcrst[3][0] = (float)read_element.arg4;

			headers_segs[which_cseg].save_rotate3D[0][0] =
				hcrst[0][0];
			headers_segs[which_cseg].save_rotate3D[1][0] =
				hcrst[1][0];
			headers_segs[which_cseg].save_rotate3D[2][0] =
				hcrst[2][0];
			headers_segs[which_cseg].save_rotate3D[3][0] =
				hcrst[3][0];

			break;
		}


		case 113 : /*copy 2nd half of hcrst from the in-the-file header*/
		{
			hcrst[0][1] = read_element.arg1/2048.0;
			hcrst[1][1] = read_element.arg2/2048.0;
			hcrst[2][1] = read_element.arg3/2048.0;
			hcrst[3][1] = (float)read_element.arg4;

			headers_segs[which_cseg].save_rotate3D[0][1] =
				hcrst[0][1];
			headers_segs[which_cseg].save_rotate3D[1][1] =
				hcrst[1][1];
			headers_segs[which_cseg].save_rotate3D[2][1] =
				hcrst[2][1];
			headers_segs[which_cseg].save_rotate3D[3][1] =
				hcrst[3][1];

			break;
		}


		case 114 : /*copy x_origin , y_origin from the in-the-file header*/
		{
			headers_segs[which_cseg].x_origin = read_element.arg1;
			headers_segs[which_cseg].y_origin = read_element.arg2;

			break;
		}


		case 115 : /*copy & set clipping from the in-the-file header*/
		{
			headers_segs[which_cseg].save_x_lower_clip =
				read_element.arg1;
			headers_segs[which_cseg].save_y_lower_clip =
				read_element.arg2;
			headers_segs[which_cseg].save_x_upper_clip =
				read_element.arg3;
			headers_segs[which_cseg].save_y_upper_clip =
				read_element.arg4;

			save_low_x_clip = read_element.arg1;
			save_low_y_clip = read_element.arg2;
			save_up_x_clip = read_element.arg3;
			save_up_y_clip = read_element.arg4;

			if(num_pages == 2)
			{
				my_set_viewport(read_element.arg1,
					read_element.arg2,
					read_element.arg3,
					read_element.arg4,
					which_page);
				my_lock_viewport();
			}
			else
			{
				my_set_viewport(read_element.arg1,
					read_element.arg2,
					read_element.arg3,
					read_element.arg4,0);
				my_lock_viewport();
			}

			break;
		}


		case 116 : /*copy dash pattern from the in-the-file header*/
		{
			headers_segs[which_cseg].dash_pattern = 
				read_element.arg1;
			my_dash_pattern(read_element.arg1);

			break;
		}


		case 117 : /*copy dash enable from the in-the-file header*/
		{
			headers_segs[which_cseg].dash_enable =
				read_element.flag1;

			if(read_element.flag1 == 't')
				my_dash_enable(4);
			else
				my_dash_enable(0);

			break;
		}


		case 118 : /*copy z_origin from the in-the-file header*/
		{
			headers_segs[which_cseg].z_origin = 
				read_element.arg1;

			break;
		}


		default : 
		{
			sprintf(diag_string,"Invalid op_code %d in interp_segment_command \n",
				read_element.op_code);
			error_logger(diag_string);
		}

	} /*end switch*/
}











void far display_segs()
{
int i,j,k,num_sub_entries,num_seg_entries,original_seg;
char tt,save_fill_sub;
int ch;
long dummy,start_time,end_time;



	save_map_info();


	doing_redisplay = 't';


	read_curser_pos();
	blinking_string("SCREEN UPDATE",24,20);
	goto_row_col(row_curser,col_curser);


	if((time_redisplay == 't') && (doing_an_alt_u == 't'))
	{
		start_time = time(&dummy);

		read_curser_pos();

		blinking_string("TIMER      ",24,68);
		goto_row_col(row_curser,col_curser);
	}

	which_redis = 1;
	save_fill_sub = fill_sub;
	quit_redis = 'f';

	original_seg = which_cseg;



	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
	{
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"SEGMENT %d STARTS HERE\n",i);
		my_diag(diag_string);
	#endif


		if(segment_nums[i] == -1)
			continue;

		which_cseg = i;

		if(headers_segs[i].visibility == 'f')
			continue;

		megatek_seek(which_cseg,0L);


		for(quit_segment = 'f' ; quit_segment == 'f' ; )
		{
			interp_seg_command(&j,i);

			if(quit_redis == 't')
				goto my_quit;


			my_pckb();

		}




	} /*end for i*/


my_quit:
#ifdef HAVE_DIAGS
	my_diag("END OF REDISPLAY SEGMENTS\n");
#endif

	which_cseg = original_seg;
	fill_sub = save_fill_sub;


	if((time_redisplay == 't') && (doing_an_alt_u == 't'))
	{
		end_time = time(&dummy);

		read_curser_pos();

		putchar(0x07);
		sprintf(diag_string,"%ld Seconds ",end_time -
			start_time);
		normal_string(diag_string,24,68);
		goto_row_col(row_curser,col_curser);
	}


	read_curser_pos();
	normal_string("              ",24,20);
	goto_row_col(row_curser,col_curser);

	doing_redisplay = 'f';

	restore_map_info();
}





void far display_part_seg()
{
int i,j,num_seg_entries,save_which_cseg,seg_calls;


	save_which_cseg = which_cseg;
	quit_redis = 'f';
	which_redis = 2;

	i = sub1_0_seg;

	which_cseg = i;


	hcrst[0][0] = headers_segs[i].save_rotate3D[0][0];
	hcrst[1][0] = headers_segs[i].save_rotate3D[1][0];
	hcrst[2][0] = headers_segs[i].save_rotate3D[2][0];
	hcrst[3][0] = headers_segs[i].save_rotate3D[3][0];
	hcrst[0][1] = headers_segs[i].save_rotate3D[0][1];
	hcrst[1][1] = headers_segs[i].save_rotate3D[1][1];
	hcrst[2][1] = headers_segs[i].save_rotate3D[2][1];
	hcrst[3][1] = headers_segs[i].save_rotate3D[3][1];

	
        my_set_viewport(headers_segs[which_cseg].save_x_lower_clip,
                headers_segs[which_cseg].save_y_lower_clip,
                headers_segs[which_cseg].save_x_upper_clip,
                headers_segs[which_cseg].save_y_upper_clip,
		which_page);

        my_lock_viewport();



	megatek_seek(which_cseg,ptr_rw_sub1_0);


	for(quit_segment = 'f' ; quit_segment == 'f' ; )
	{
		if(black == 't')
			my_dash_enable(0);

		interp_seg_command(&j,i);

		my_pckb();
	}


	if(black == 't')
		my_dash_enable(line_type);



	which_cseg = save_which_cseg;
	goto_megatek_end();

/*
	hcrst[0][0] = headers_segs[which_cseg].rotate3D[0][0];
	hcrst[1][0] = headers_segs[which_cseg].rotate3D[1][0];
	hcrst[2][0] = headers_segs[which_cseg].rotate3D[2][0];
	hcrst[3][0] = headers_segs[which_cseg].rotate3D[3][0];
	hcrst[0][1] = headers_segs[which_cseg].rotate3D[0][1];
	hcrst[1][1] = headers_segs[which_cseg].rotate3D[1][1];
	hcrst[2][1] = headers_segs[which_cseg].rotate3D[2][1];
	hcrst[3][1] = headers_segs[which_cseg].rotate3D[3][1];
*/
}






void far display_a_seg(which_seg)
int which_seg;
{
int i,j,num_seg_entries,save_which_cseg;


	if((which_seg < 0) || (which_seg > NUMBER_OF_SEGS))
		return;

/*

printf("Display_a_seg(%d)   last_logical_page = %d\n",which_seg,
	last_logical_page);

printf("Before Save map:  check =  %d  %d  %d\n",
		*(start_of_page_frame_0 + 8050),
		*(start_of_page_frame_0 + 8110),
		*(start_of_page_frame_0 + 8170));
*/

	save_map_info();

/*
printf("After Save map:  check =  %d  %d  %d\n",
		*(start_of_page_frame_0 + 8050),
		*(start_of_page_frame_0 + 8110),
		*(start_of_page_frame_0 + 8170));
*/


	#ifdef HAVE_DIAGS
		my_diag("***********DISPLAY_A_SEG STARTS HERE\n");
	#endif

	draw_mode(0);
	which_redis = 3;
	quit_redis = 'f';

	save_which_cseg = which_cseg;

	which_cseg = which_seg;



	megatek_seek(which_cseg,0L);

/*
printf("after seek:  last page = %d\n",last_logical_page);
printf("After the seek:  check =  %d  %d  %d\n",
		*(start_of_page_frame_0 + 8050),
		*(start_of_page_frame_0 + 8110),
		*(start_of_page_frame_0 + 8170));

*/


	for(quit_segment = 'f' ; quit_segment == 'f' ; )
	{
		interp_seg_command(&j,which_seg);
		my_dash_enable(0);
		my_pckb();
	}

	
	which_cseg = save_which_cseg;

/*
printf("Before restore:  check =  %d  %d  %d\n",
		*(start_of_page_frame_0 + 8050),
		*(start_of_page_frame_0 + 8110),
		*(start_of_page_frame_0 + 8170));
*/

	restore_map_info();

/*
printf("After restore:  check =  %d  %d  %d\n",
		*(start_of_page_frame_0 + 8050),
		*(start_of_page_frame_0 + 8110),
		*(start_of_page_frame_0 + 8170));
*/

}


