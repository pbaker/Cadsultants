#include "stdio.h"
#include "my_utils.h"


#define GRAPHICS 1
#define TEXT 0


extern char diag_flag;
extern int far graphics_text;
extern FILE far *out_ptr;
extern char far print_flag;
extern char far ins_matrix_flag;
extern char far our_call;
extern char far diag_string[];


extern void far menu();


extern void far print_id();
extern void far new_rewrite();
extern void far new_read_pid();
extern void far print_char();
extern void far delete_last_char();
extern void far cap_L_func();
extern void far cap_K_func();
extern void far cap_P_func();
extern void far cap_R_func();
extern void far cap_Y_func();
extern void far cap_Q_func();
extern void far cap_S_func();
extern void far y_func();
extern void far cap_X_func();
extern void far cap_W_func();
extern void far cap_V_func();
extern void far cap_U_func();
extern void far cap_T_func();
extern void far cap_M_func();
extern void far cap_J_func();
extern void far cap_H_func();
extern void far cap_I_func();
extern void far e_func();
extern void far cap_A_func();
extern void far at_func();
extern void far o_func();
extern void far left_curly();
extern void far p_func();
extern void far q_func();
extern void far r_func();
extern void far s_func();
extern void far t_func();
extern void far w_func();
extern void far n_func();
extern void far m_func();
extern void far vert_line();
extern void far cap_E_func();
extern void far cap_D_func();
extern void far cap_C_func();
extern void far cap_B_func();
extern void far cap_F_func();
extern void far single_quote();
extern void far c_func();
extern void far b_func();
extern void far a_func();
extern void far f_func();
extern void far l_func();
extern void far cap_N_func();
extern void far v_func();
extern void far d_func();
extern void far k_func();
extern void far u_func();
extern void far erase_func();
extern void far scroll_func();
extern void far enter_graphics();
extern void far exit_graphics();









void far megatek_device(command)
int command;
{
int dummy;


		if(graphics_text == TEXT)
			switch(command & 0x007f)
			{
				case 0x01 : /*diags on*/
				{

					diag_flag = 't';
					break;
				}


				case 0x02 : /*diags off*/
				{
					diag_flag = 'f';
					break;
				}


				case 0x03 : /*send diags to printer*/
				{
					out_ptr = fopen("lpt1","w");
					print_flag = 't';
					break;
				}


				case 0x04 : /*stop sending diags to printer*/
				{
					my_size_chars(0);
					ins_matrix_flag = 't';
					megatek_update();
					ins_matrix_flag = 'f';
					break;
				}


				case 0x05 : 
				{
					menu();
					break;
				}


				case 0x0c : /*erase text monitor buffer*/
				{
					erase_func(&dummy,dummy);
					break;
				}


				case 0x0d : /*scroll text monitor buffer*/
				{
					scroll_func(&dummy,dummy);
					break;
				}


				case 0x1d : /*enter graphics mode*/
				{
					enter_graphics(&dummy,dummy);
					break;
				}



				case 0x1f : /*exit graphics mode*/
				{
					exit_graphics(&dummy,dummy);
					break;
				}


				case 0x7f : /*delete last char*/
				{
					delete_last_char();
					break;
				}

				default : print_char(command);
			}
		else

		switch(command & 0x007f)
		{
			case 0x01 : /*diags on*/
			{

				diag_flag = 't';
				break;
			}


			case 0x02 : /*diags off*/
			{
				diag_flag = 'f';
				break;
			}


			case 0x03 : /*send diags to printer*/
			{
				out_ptr = fopen("lpt1","w");
				print_flag = 't';
				break;
			}


			case 0x04 : /*stop sending diags to printer*/
			{
				my_size_chars(0);
				ins_matrix_flag = 't';
				megatek_update();
				ins_matrix_flag = 'f';
				break;
			}


			case 0x05 : 
			{
				menu();
				break;
			}


			case 0x0c : /*erase text monitor buffer*/
			{
				erase_func(&dummy,dummy);
				break;
			}


			case 0x0d : /*scroll text monitor buffer*/
			{
				scroll_func(&dummy,dummy);
				break;
			}


			case 0x1d : /*enter graphics mode*/
			{
				enter_graphics(&dummy,dummy);
				break;
			}



			case 0x1f : /*exit graphics mode*/
			{
				exit_graphics(&dummy,dummy);
				break;
			}


			case 0x7f : /*delete last char*/
			{
				delete_last_char();
				break;
			}


		/*	case 'G' : reserved
			{

				break;
			}



			case 'O' : reserved
			{

				break;
			}



			case 'Z' : reserved
			{

				break;
			}

*/
			case '[' : /*reserved*/
			{
				new_read_pid();
				break;
			}



			case '\\' :/* reserved*/
			{
				new_rewrite();
				break;
			}

/*
			case ']' : reserved
			{

				break;
			}


			case '^' : reserved
			{

				break;
			}


			case '\b' : reserved
			{

				break;
			}


			case 'g' : reserved
			{

				break;
			}


			case 'h' : reserved
			{

				break;
			}


			case 'i' : reserved
			{

				break;
			}


			case 'j' : reserved
			{

				break;
			}


			case '}' : reserved
			{

				break;
			}


			case '~' : reserved
			{

				break;
			}


			case '?' : reserved
			{

				break;
			}
		*/

			case 'd' : /*close subroutine*/
			{
				d_func(&dummy,dummy);
				break;
			}


			case 'k' : /*copy command*/
			{
				k_func(&dummy,dummy);
				break;
			}



			case 'u' : /*set position*/
			{
				u_func();
				break;
			}


			case 'n' : /* system reset */
			{
				n_func(&dummy,dummy);
				break;
			}


			case 'm' : /* refresh control */
			{
				m_func(&dummy,dummy);
				break;
			}



			case '{' : /* read segment starting address */
			{
				left_curly(&dummy,dummy);
				break;
			}



			case '|' : /* read current position ID */
			{
				vert_line(&dummy,dummy);  
				break;
			}



			case 'E' : /* rewrite segment/subroutine */
			{
				cap_E_func(&dummy,dummy);
				break;
			}


			case 'o' : /* read/write vector memory */
			{
				o_func(&dummy,dummy);
				break;
			}


			case '@' : /* open segment */
			{
				at_func(&dummy,dummy);
				break;
			}


			case 'D' : /* close segment */
			{
				cap_D_func(&dummy,dummy);
				break;
			}


			case 'C' : /* delete segment */
			{
				cap_C_func(&dummy,dummy);
				break;
			}


			case 'B' : /* clear segment */
			{
				cap_B_func(&dummy,dummy);
				break;
			}


			case 'A' : /* append segment */
			{
				cap_A_func(&dummy,dummy);
				break;
			}


			case 'F' : /* rename segment */
			{
				cap_F_func(&dummy,dummy);
				break;
			}


			case '`' : /* open subroutine */
			{
				single_quote(&dummy,dummy);
				break;
			}


			case 'c' : /* delete subroutine */
			{
				c_func(&dummy,dummy);
				break;
			}


			case 'b' : /* clear subroutine */
			{
				b_func(&dummy,dummy);
				break;
			}


			case 'a' : /* append subroutine */
			{
				a_func(&dummy,dummy);
				break;
			}


			case 'e' : /* call subroutine */
			{
				our_call = 'f';
				e_func(&dummy,dummy);
				break;
			}


			case 'f' : /* rename subroutine */
			{
				f_func(&dummy,dummy);
				break;
			}



			case 'I' : /* change something */
			{
				cap_I_func(&dummy,dummy);
				break;
			}


			case 'H' : /* change something */
			{
				cap_H_func(&dummy,dummy);
				break;
			}


			case 'l' : /* change raster color table */
			{
				l_func(&dummy,dummy);
				break;
			}


			case 'J' : /* change something */
			{
				cap_J_func(&dummy,dummy);
				break;
			}


			case 'M' : /* insert something */
			{
				cap_M_func(&dummy,dummy);
				break;
			}


			case 'T' : /* move 3D absolute */
			{

				cap_T_func(&dummy,dummy);
				break;
			}


			case 'U' : /* draw 3D absolute */
			{

				cap_U_func(&dummy,dummy);
				break;
			}


			case 'V' : /* move 3D relative */
			{
				cap_V_func(&dummy,dummy);
				break;
			}


			case 'W' : /* draw  3D relative */
			{
				cap_W_func(&dummy,dummy);
				break;
			}


			case 'X' : /* character string */
			{
				cap_X_func(&dummy,dummy);
				break;
			}
			

			case 'N' : /* insert detectability */
			{
				cap_N_func(&dummy,dummy);
				break;
			}


			case 'p' : /* enable something */
			{
				p_func(&dummy,dummy);
				break;
			}



			case 'q' : /* disable something */
			{

				q_func(&dummy,dummy);
				break;
			}


			case 'r' : /* sample something */
			{
				r_func(&dummy,dummy);
				break;
			}



			case 's' : /* read something */
			{
				s_func(&dummy,dummy);
				break;
			}


			case 't' : /* set limits */
			{
				t_func(&dummy,dummy);
				break;
			}



			case 'v' : /*select something*/
			{
				v_func(&dummy,dummy);
				break;
			}


			case 'w' : /*set something*/
			{
				w_func(&dummy,dummy);
				break;
			}
			


			case 'y' : /*locate something*/
			{
				y_func(&dummy,dummy);
				break;
			}




			case 'P': /* move 2d abs */
			{
				cap_P_func(&dummy,dummy);
				break;
			}

			case 'R' : /* move 2d rel */
			{
				cap_R_func(&dummy,dummy);
				break;
			}

			case 'Y' : /* short relative vectors */
			{
				cap_Y_func(&dummy,dummy);
				break;
			}

			case 'Q' : /* draw 2d abs */
			{
				cap_Q_func(&dummy,dummy);
				break;
			}

			case 'S':  /* do 2d relative draw */
			{
				cap_S_func(&dummy,dummy);
				break;
			}

		
			case 'K' : /* insert attributes 1 value */
			{
				cap_K_func(&dummy,dummy);
				break;
			}
		

			case 'L' : /* insert attributes 3 values */
			{
				cap_L_func(&dummy,dummy);
				break;
			}

			default :
			{
				if(command != 0x18)
				{
					sprintf(diag_string,"UNKNOWN OP CODE IN MEGATEK DEVICE: %c = %x\n"
						,command,command);
					error_logger(diag_string);
				}
			}
		}  /* end switch */


}


