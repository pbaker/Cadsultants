#include "my_globs.h"
#include "my_utils.h"
#include "expanded.h"




extern int far save_system_color;
extern char far visibility_changed_to_on[];
extern int far op_code_to_overwrite;
extern long far save_host_id;
extern long far save_host_displacement;
extern int far the_last_sub_run;




void far insert_sub_1_attr()
{
		if((flag & 32) != 0) /* high bit set */
		{
			if((flag & 16) != 0) /* Xform Enable */
			{
				an_element.op_code = 6;
				an_element.arg1 = flag;

				diag_num = 53;
			#ifdef HAVE_DIAGS
				my_diag("insert Xform enable\n");
			#endif

				insert_sub(an_element,which_sub);

			}
			else   /*Blink or Dash Enable */
			{
				if((flag & 8) != 0) /*Dash Enable */
				{
					diag_num = 54;

					an_element.op_code = 2;
					an_element.arg1 = flag;

					insert_sub(an_element,which_sub);

				}
				else   /*Blink Enable */
				{
					/*monitor can't do it use flag */

					if(sub_nums[which_sub] != 2)
					{
						an_element.op_code = 1; /*blink*/
						an_element.arg1 = flag;
					}
					else    /*going into sub 2, use color*/
					{
						an_element.op_code = 4; /*color*/
						an_element.arg1 = 
							save_system_color;
					}

					diag_num = 55;
				#ifdef HAVE_DIAGS
					my_diag("insert blink enable\n");
				#endif

					insert_sub(an_element,which_sub);

				}
			}
				
		}
		else   /* high bit not set */
		{
			if((flag & 16) != 0)  /*Blink Rate*/
			{
				flag &= 0x0f;
				diag_num = 56;
			#ifdef HAVE_DIAGS
				my_diag("insert blink rate\n");
			#endif
				an_element.op_code = 3;
				an_element.arg1 = flag;

				insert_sub(an_element,which_sub);

			}
			else  /*Intensity/color*/
			{
				flag &= 0x0f;
				my_color(flag);

				diag_num = 57;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert intensity/color %d\n",flag);
				my_diag(diag_string);
			#endif
				an_element.op_code = 4;
				an_element.arg1 = flag;

				if(sub_nums[which_sub] == 6) /*color into sub 6*/
					save_system_color = flag;

				insert_sub(an_element,which_sub);

			}
		}

}




void far cap_K_func(sub_count,which_subr) /*insert attributes 1 argument*/
int *sub_count,which_subr;
{

	read_more(1);
	flag = megatek_data[0];

	flag &= 0x3f;			

	if(fill_sub == 'f')
	{		
		if((flag & 32) != 0) /* high bit set */
		{
			if((flag & 16) != 0) /* Xform Enable */
			{
				an_element.op_code = 6;
				an_element.arg1 = flag;

				diag_num = 53;
			#ifdef HAVE_DIAGS
				my_diag("insert Xform enable\n");
			#endif
				if((flag & 1) == 0) /*enable*/
				{
				#ifdef HAVE_DIAGS
					my_diag("XFORM ENABLE\n");
				#endif
					headers_segs[which_cseg].
							Xform_enable = 't';
				}
				else
				{
				#ifdef HAVE_DIAGS
					my_diag("XFORM DISABLE\n");
				#endif
					headers_segs[which_cseg].
							Xform_enable = 'f';
				}

				insert_ours(an_element,which_cseg);
				
			}
			else   /*Blink or Dash Enable */
			{
				if((flag & 8) != 0) /*Dash Enable */
				{
					diag_num = 54;
					if((flag & 1) != 0)
					{
						headers_segs[which_cseg].
							dash_enable = 't';
					#ifdef HAVE_DIAGS
						my_diag("insert dash enable\n");
					#endif
						my_dash_enable(4);
						line_type = 4;
					}
					else
					{
						headers_segs[which_cseg].
							dash_enable = 'f';
					#ifdef HAVE_DIAGS
						my_diag("insert dash disable\n");
					#endif
						my_dash_enable(0);
						line_type = 0;
					}

					an_element.op_code = 2;
					an_element.arg1 = flag;


					insert_ours(an_element,which_cseg);
					
				}
				else   /*Blink Enable */
				{
					/*monitor can't do it use flag */
					an_element.op_code = 1;
					an_element.arg1 = flag;

					diag_num = 55;
				#ifdef HAVE_DIAGS
					my_diag("insert blink enable\n");
				#endif

					insert_ours(an_element,which_cseg);
					
				}
			}
				
		}
		else   /* high bit not set */
		{
			if((flag & 16) != 0)  /*Blink Rate*/
			{
				flag &= 0x0f;
				diag_num = 56;
			#ifdef HAVE_DIAGS
				my_diag("insert blink rate\n");
			#endif
				an_element.op_code = 3;
				an_element.arg1 = flag;

				insert_ours(an_element,which_cseg);

			}
			else  /*Intensity/color*/
			{
				rw_op_code = 4;
				flag &= 0x0f;

				if(rewrite_flag == 't')
				{
					rewrite_color = 't';
					sub1_0_seg = which_cseg;
					ptr_rw_sub1_0 = my_ftell();
					num_op_code_4s = 0;

					if(op_code_to_overwrite != 4)
					{
						sprintf(diag_string,
						   "COLOR REWRITE ERROR in seg %d: Host ID = %ld , DISP = %ld\n",
							which_cseg,save_host_id,
							    save_host_displacement);
						error_logger(diag_string);
						sprintf(diag_string,
						    "OPCODE OVERWRITTEN = %d\n",
							op_code_to_overwrite);
						error_logger(diag_string);
						sprintf(diag_string,
							"WE REWRITE at %ld",
							   ptr_rw_sub1_0);
						error_logger(diag_string);
					}


				}
				else
					rewrite_color = 'f';

				my_color(flag);

				save_color = flag;

				diag_num = 57;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert intensity/color %d\n",flag);
				my_diag(diag_string);
			#endif

				an_element.op_code = 4;
				an_element.arg1 = flag;


				insert_ours(an_element,which_cseg);
				
			}
		}

	}
	else
		insert_sub_1_attr();
}




void far cap_L_func(sub_count,which_subr) /*insert attributes 3 arguments*/
int *sub_count,which_subr;
{
	read_more(3);

	flag = megatek_data[0];
	flag &= 0x3f;

	if(((flag & 32) != 0) && ((flag & 16) == 0))
	{ /* insert dash pattern */
		if(fill_sub == 'f')
		{
			dash_pat = comp16_3(flag,megatek_data[1],megatek_data[2]);

			diag_num = 58;
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"insert dash pattern %x %x %x  %x\n",
				flag,megatek_data[1],megatek_data[2],dash_pat);
			my_diag(diag_string);
		#endif
			an_element.op_code = 24;
			an_element.arg1 = dash_pat;

			insert_ours(an_element,which_cseg);
			
			headers_segs[which_cseg].dash_pattern = dash_pat;

			my_dash_pattern(dash_pat);
			
			the_last_sub_run = 999;
		}
		else
		{
 			dash_pat = comp16_3(flag,megatek_data[1],megatek_data[2]);
			diag_num = 58;
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"insert dash pattern %x %x %x  %x\n",
				flag,megatek_data[1],megatek_data[2],dash_pat);
			my_diag(diag_string);
		#endif
			an_element.op_code = 24;
			an_element.arg1 = dash_pat;

			insert_sub(an_element,which_sub);

		}
	}
	else if(((flag & 32) == 0) && ((flag & 16) != 0))
	{

			diag_num = 59;
		#ifdef HAVE_DIAGS
			my_diag("insert pick label\n");
		#endif
	}
	else	  /* insert stroke-refresh color */
	{
		if(fill_sub == 'f')
		{
			megatek_data[2] &= 0x0f;
			my_color(megatek_data[2]);
	
			diag_num = 60;
		#ifdef HAVE_DIAGS
			my_diag("insert stroke-refresh color\n");
		#endif
			an_element.op_code = 5;
			an_element.arg1 = megatek_data[2];

			insert_ours(an_element,which_cseg);
			
		}
		else
		{
			megatek_data[2] &= 0x0f;
	
			diag_num = 60;
		#ifdef HAVE_DIAGS
			my_diag("insert stroke-refresh color\n");
		#endif
			an_element.op_code = 5;
			an_element.arg1 = megatek_data[2];

			insert_sub(an_element,which_sub);
		}
	}

}






void far cap_I_func(sub_count,which_subr)  /* change something */
int *sub_count,which_subr;
{
int i;
long save_insert_ptr;
int save_which_cseg;


	read_more(5);

		diag_num = 83;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);

		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;
		if(i == NUMBER_OF_SEGS)
			return;


		save_which_cseg = which_cseg;
		save_insert_ptr = my_ftell();

		which_cseg = i;

		if((megatek_data[2] & 32) != 0) /*change dash pattern*/
		{
			dash_pat = comp16_3(megatek_data[2],megatek_data[3],
				megatek_data[4]);

			if(save_which_cseg == which_cseg)
			{
				my_dash_pattern(dash_pat);
			}
	

			an_element.op_code = 116;
			an_element.arg1 = dash_pat;

			megatek_seek(which_cseg,6L);
			memcpy(current_megatek_pointer,&an_element,
				size_an_element);

		#ifdef HAVE_DIAGS
			sprintf(diag_string,"change dash pattern in segment %d: %x %x %x  %x\n",
				a_segment,megatek_data[2],megatek_data[3],
					megatek_data[4],dash_pat);
			my_diag(diag_string);
		#endif
		}
		else  /*change stroke-refresh color*/
		{
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"change stroke-refresh color in segment %d\n",
					a_segment);
			my_diag(diag_string);
		#endif
		}


		which_cseg = save_which_cseg;	
		megatek_seek(which_cseg,save_insert_ptr);
}





void far cap_H_func(sub_count,which_subr)  /* change something */
int *sub_count,which_subr;
{
int i;
long save_insert_ptr;
int save_which_cseg;


	read_more(3);

		diag_num = 84;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);

		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"Invalid segment # %d in change something in a segment\n",
					a_segment);
			error_logger(diag_string);
			return;
		}

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"change something in segment %d\n",a_segment);
		my_diag(diag_string);
	#endif
		diag_num = - 84;


		save_insert_ptr = my_ftell();
		save_which_cseg = which_cseg;
		which_cseg = i;


		if((megatek_data[2] & 32) != 0)
		{
			if((megatek_data[2] & 16) != 0) 
			{
				if((megatek_data[2] & 8) != 0) /*change visibility*/
				{
					an_element.op_code = 110;

					if((megatek_data[2] & 1) != 0) /*is visible*/
					{
					#ifdef HAVE_DIAGS
						my_diag("is visible\n");
					#endif

						if(headers_segs[i].visibility
						   == 'f')
						      visibility_changed_to_on[
							  i] = 't';
						else
						      visibility_changed_to_on[
							  i] = 'f';

					/*
						headers_segs[i].visibility = 't';
					*/

						an_element.flag1 = 't';
					}
					else /*segment is not visible*/
					{
					#ifdef HAVE_DIAGS
						my_diag("segment is not visible\n");
					#endif
						headers_segs[i].visibility = 'f';

						an_element.flag1 = 'f';

						visibility_changed_to_on[i] = 
							'f';
					}

					megatek_seek(which_cseg,0L);
					memcpy(current_megatek_pointer,
						&an_element,size_an_element);
	
				}
				else  /*change transformation enable*/
				{
					an_element.op_code = 111;

					if((megatek_data[2] & 1) == 0)
					{  /*HCRST is enabled*/
					#ifdef HAVE_DIAGS
						my_diag("HCRST is enabled\n");
					#endif
/*						headers_segs[i].Xform_enable = 't';
*/
						an_element.flag1 = 't';
					}
					else  /*HCRST is disabled*/
					{
					#ifdef HAVE_DIAGS
						my_diag("HCRST is disabled\n");
					#endif
/*						headers_segs[i].Xform_enable = 'f';
*/
						an_element.flag1 = 'f';
					}

					megatek_seek(which_cseg,1L);
					memcpy(current_megatek_pointer,
						&an_element,size_an_element);
				}
			}
			else
			{
				if((megatek_data[2] & 8) != 0) /*change dash enable*/
				{
					an_element.op_code = 117;

					if((megatek_data[2] & 1) != 0) /*enable dashing*/
					{
					#ifdef HAVE_DIAGS
						my_diag("dash enabled\n");
					#endif
/*						headers_segs[i].dash_enable = 't';
*/
						an_element.flag1 = 't';
						my_dash_enable(4);
						line_type = 4;
					}
					else  /*disable dashing*/
					{
					#ifdef HAVE_DIAGS
						my_diag("dash disabled\n");
					#endif
/*						headers_segs[i].dash_enable = 'f';
*/
						an_element.flag1 = 'f';
						my_dash_enable(0);
						line_type = 0;
					}
	
					megatek_seek(which_cseg,7L);
					memcpy(current_megatek_pointer,
						&an_element,size_an_element);

				#ifdef HAVE_DIAGS
					my_diag("change dash enable\n");
				#endif
				}

				else  /*change blink enable*/
				{
				#ifdef HAVE_DIAGS
					my_diag("change blink enable\n");
				#endif
				}
			}
		}
		else
		{
			if((megatek_data[2] & 16) != 0)  /*change blink rate*/
			{
			#ifdef HAVE_DIAGS
				my_diag("change blink rate\n");
			#endif
			}
			else  /*change intensity/color*/
			{
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"change intensity/color %d\n",
					megatek_data[2] & 0x0f);
				my_diag(diag_string);
			#endif
				my_color(megatek_data[2] & 0x0f);
			}
		}


		which_cseg = save_which_cseg;
		megatek_seek(which_cseg,save_insert_ptr);
}

