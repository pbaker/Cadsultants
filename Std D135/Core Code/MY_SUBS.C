#include "my_globs.h"
#include "my_utils.h"
#include "expanded.h"


extern char far visibility_changed_to_on[];


/*for rewrite with sub call test*/
extern int far op_code_to_overwrite;
extern long far save_host_id;
extern long far save_host_displacement;

int far the_last_sub_run = 999;



void far single_quote(sub_count,which_subr) /* open subroutine */
int *sub_count,which_subr;
{
int i;
	read_more(2);
	
		diag_num = 77;

		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);	
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"open subroutine %d\n",a_subroutine);
		my_diag(diag_string);
	#endif
		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i != NUMBER_OF_SUBS)
		{
			which_sub = i;
			fill_sub = 't';
			which_cseg = 0;

			sub_insert_ptrs[i] = -1;

			goto_megatek_end();

			which_sub = i;
			fill_sub = 't';

			an_element.op_code = 44;
			insert_sub(an_element,which_sub);

			return;
		}


		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == -1)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no more subroutine segments available in open subroutine\n");
			error_logger(diag_string);

			which_sub = NUMBER_OF_SUBS + 9;
			fill_sub = 't';

			which_cseg = 0;
			goto_megatek_end();
		}
		else
		{
			which_sub = i;
			sub_nums[i] = a_subroutine;
			fill_sub = 't';

			which_cseg = 0;
			goto_megatek_end();

			an_element.op_code = 44;
			insert_sub(an_element,which_sub);

		}

}

	
void far c_func(sub_count,which_subr)  /* delete subroutine */
int *sub_count,which_subr;
{
int i;
	read_more(2);
	
		diag_num = 78;

		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"delete subroutine %d\n",a_subroutine);
		my_diag(diag_string);
	#endif
		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in delete subroutine\n",
					a_subroutine);
			error_logger(diag_string);
			return;
		}

		sub_nums[i] = -1;

		if(which_sub == i)
		{
			fill_sub = 'f';
			which_sub = -1;
		}


		sub_insert_ptrs[i] = -1;
	
}



void far b_func(sub_count,which_subr)  /* clear subroutine */
int *sub_count,which_subr;
{
int i;

		read_more(2);

		diag_num = 79;

		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"*******************************clear subroutine %d\n",a_subroutine);
		my_diag(diag_string);
	#endif
		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in clear subroutine\n",
					a_subroutine);
			error_logger(diag_string);
			return;
		}

		which_sub = i;
		fill_sub = 't';


		if((a_subroutine == 3) || (a_subroutine == 6)
			|| (a_subroutine == 7))
			change_sub3_6_7 = 't';


		sub_insert_ptrs[i] = -1;


		an_element.op_code = 44;
		insert_sub(an_element,which_sub);
	
}



void far a_func(sub_count,which_subr)  /* append subroutine */
int *sub_count,which_subr;
{
int i;

		read_more(2);
	
		diag_num = 80;

		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"append subroutine %d\n",a_subroutine);
		my_diag(diag_string);
	#endif
		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in append subroutine\n",
					a_subroutine);
			error_logger(diag_string);
			return;
		}

		which_sub = i;
		fill_sub = 't';
	
}



void far f_func(sub_count,which_subr)  /* rename subroutine */
int *sub_count,which_subr;
{
int i;

		read_more(4);

		diag_num = 82;

		old_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
		new_subroutine = comp_rel_coord(megatek_data[2],megatek_data[3]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"rename subroutine from %d to %d\n",
				old_subroutine,new_subroutine);
		my_diag(diag_string);
	#endif

		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == old_subroutine)
				break;
	
		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in rename subroutine\n",
					old_subroutine);
			error_logger(diag_string);
			return;
		}

		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == new_subroutine)
				break;

		if(i < NUMBER_OF_SUBS)
		{
			return;
		}

		sub_nums[i] = new_subroutine;
		which_cseg = 0;
		goto_megatek_end();

		which_sub = -1;
		fill_sub = 'f';
	
}






void far e_func(sub_count,which_subr)  /* call subroutine */
int *sub_count,which_subr;
{
int i,j,k;

	if(our_call == 'f')
	{
		read_more(2);
	}

	if(fill_sub == 't')
	{
		an_element.op_code = 0;
		an_element.arg1 = comp_rel_coord(megatek_data[0],megatek_data[1]);
		insert_sub(an_element,which_sub);
		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"call subroutine %d\n",a_subroutine);
		my_diag(diag_string);
	#endif

                if((sub_rewrite == 't') && (last_sub_rewritten >= 10))
                {
                        if((a_subroutine == 0) || (a_subroutine == 1))
                        {
                                rw_sub_w_0_1 = 't';
                                rw_which_seg = where_called[last_sub_rewritten
                                         - 10][0];

                                if(a_subroutine == 0)
                                        black = 't';
                                else
                                        black = 'f';
                        }
                        else
                        {
                                rw_sub_w_0_1 = 'f';
                                black = 'f';
                        }
                        
                }
                else
                {
                        rw_sub_w_0_1 = 'f';
                        black = 'f';
                }

	}
	else
	{
		diag_num = 81;

		if(our_call == 'f')
			a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
		else if(our_call == 's')
			a_subroutine = ptr_sub -> arg1;
		else
			a_subroutine = read_element.arg1;


	#ifdef HAVE_DIAGS
		sprintf(diag_string,"call subroutine %d\n",a_subroutine);
		my_diag(diag_string);
	#endif


		if(our_call == 'f')
		{
			an_element.op_code = 0;
			an_element.arg1 = a_subroutine;


			insert_ours(an_element,which_cseg);

                        if(a_subroutine >= 10) 
                        {
                                for(k = 0 ; (k < 10) && (where_called[a_subroutine
                                        - 10][k] != -1) && (where_called[
                                          a_subroutine - 10][k] != which_cseg)
                                           ; k++)
                                                ;
                                if(where_called[a_subroutine - 10][k] == -1)
                                        where_called[a_subroutine - 10][k] = 
                                             which_cseg;
                        }


                        if(a_subroutine >= 10)
                                segment_calls[which_cseg] = a_subroutine;

		}
	

		our_call = 'f';


		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in call subroutine\n",
					a_subroutine);
			error_logger(diag_string);
			return;
		}


		if(rewrite_flag == 't')
		{
			if((a_subroutine == 0) || (a_subroutine == 1) ||
				(a_subroutine == 2))
			{
				if(a_subroutine == 0)
				{
				#ifdef HAVE_DIAGS
					my_diag("rewrite call sub 0\n");
				#endif
					black = 't';
					my_color((char)0);
				}
				else
				{
					black = 'f';

				}

				rw_sub1_0_flag = 't';
				sub1_0_seg = which_cseg;
				rw_op_code = 4;
				num_op_code_0s = 0;

				ptr_rw_sub1_0 = my_ftell();

				if(op_code_to_overwrite != 0)
				{
					sprintf(diag_string,
						"SUB CALL REWRITE ERROR in seg %d: Host ID = %ld , DISP = %ld\n",
						which_cseg,save_host_id,
						save_host_displacement);
					error_logger(diag_string);
					sprintf(diag_string,
						"OPCODE OVERWRITTEN = %d\n",
						op_code_to_overwrite);
					error_logger(diag_string);
					sprintf(diag_string,
						"WE REWRITE at %ld",ptr_rw_sub1_0);
					error_logger(diag_string);
				}


				seek_prev_4();


			}
			else
				rw_sub1_0_flag = 'x';

		}
		else
		{

			if(a_subroutine == 0)
				no_draw_flag = 't';

			if(a_subroutine == 1)
				no_draw_flag = 'f';

		}


		which_subr = i;



		interp_sub(which_subr);

		the_last_sub_run = which_subr;
	}
}







void far d_func(sub_count,which_subr) /*close subroutine*/
int *sub_count,which_subr;
{
int i,save_which_cseg,j;


	fill_sub = 'f';


	if(change_sub3_6_7 == 't')
	{
		change_sub3_6_7 = 'f';
		sub_insert_ptrs[NUMBER_OF_SUBS] = -1;
		compile_sub_curser(6);
	}



	if(sub_rewrite == 't')
	{
		sub_rewrite = 'f';

		if(save_sub_insert_ptr > -2)
			sub_insert_ptrs[which_sub] = save_sub_insert_ptr;


                if(rw_sub_w_0_1 == 't')
                {
                        for(i = 0 ; (sub_nums[i] != last_sub_rewritten) && 
                                (i < NUMBER_OF_SUBS) ; i++)
                	                ;


                        if(i != NUMBER_OF_SUBS)
                        {
                             save_which_cseg = which_cseg;

				for(j = 1 ; j < NUMBER_OF_SEGS ; j++)
					if(segment_calls[j] ==
						last_sub_rewritten)
							break;

				if((j == NUMBER_OF_SEGS) || ((headers_segs[j].
					visibility == 'f') && 
					(visibility_changed_to_on[j] == 'f')))
				{
					rw_sub_w_0_1 = 'f';
					black = 'f';
					which_sub = -1;
					return;
				}

	                        which_cseg = j;


				my_set_viewport(-2048,-2048,2048,2048,
					which_page);
				my_lock_viewport();


			        hcrst[0][0] = 1.0;
			        hcrst[1][0] = 0.0;
			        hcrst[2][0] = 0.0;
			        hcrst[3][0] = 0.0;
			        hcrst[0][1] = 0.0;
			        hcrst[1][1] = 1.0;
			        hcrst[2][1] = 0.0;
			        hcrst[3][1] = 0.0;
                             
                             interp_sub(i);
                             black = 'f';

                             which_cseg = save_which_cseg;
                        }


                        rw_sub_w_0_1 = 'f';
                }

	}

	diag_num = 88;
#ifdef HAVE_DIAGS
	my_diag("close subroutine\n");
#endif
	which_sub = -1;

}

