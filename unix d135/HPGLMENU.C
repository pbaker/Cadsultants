/* HPGLMENU.C menus for hardcopy selections*/




#include <stdio.h>
#include <string.h>





#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1


#define TERMINATE  -1
#define ENTRY_COMPLETE  -2
#define REJECT  -3






extern char  found_hardcopy_file;



extern void  control_window();
extern int  choose_menu();
extern int  information_menu();
extern void  error_message();

extern void  read_menu_integer();
extern void  read_menu_string();


extern void  create_hp_file();
extern int  win_y_low;
extern int  win_x_low;




/*bios routines actually used in d135 emulation*/

extern void  read_character();
extern void  output_character();
extern void  make_win();
extern void  clear_win();
extern void  save_win();
extern void  restore_win();
extern void  goto_row_col();
extern void  read_curser_pos();
extern void  blinking_string();
extern void  normal_string();
extern void  turn_on_curser();
extern void  turn_off_curser();
extern void  reverse_string();

extern void  error_logger();



struct hard_info
{
	char the_version[45];
	char encoded_block[222];
	char menu_text[3][111];
	char dos_commands[3][222];
	char user_file_names[3][111];
	char use_colors[4];
	int default_target;
	int size_of_paper[4],type_of_plotter[4],type_of_7580b[4];
};



extern struct hard_info  hardcopy_info;



char  use_hardcopy_colors = 't';



FILE  *hp_output, *pixel_ems_save_file;
char  hp_output_name[80];
char  pixel_save_file_name[80];
char  temp_7_string[222];

int  hp_x_offset = 0;
int  hp_y_offset = 0;
float  hp_scale = 1.0;





struct offset_values
{
	int x_offset,y_offset;
};



struct offset_values offset_table[3][5] = 
{
/* A */	 {
		{0,0},{0,0},{-4809,-4013},{-4741,-4013},{-4809,-4013}
	 },


/* B */  {
		{0,0},{0,0},{-7857,-5283},{-7513,-5283},{-7722,-5283}
	 },


/* C */  {
		{0,0},{0,0},{-10431,-8128},{-9889,-8128},{-10228,-8128}
	 }
};


float  paper_scale[3][5] = 
{
   /* A */   {2.533 , 2.533 , 2.348 , 2.314 , 2.348},
   /* B */   {3.335 , 3.335 , 3.335 , 3.335 , 3.335},
   /* C */   {5.131 , 5.131 , 5.093 , 4.828 , 4.994}
};



int  size_of_paper = 0;
int  type_of_plotter = 0;
int  type_of_7580b = 0;
int  hpgl_destination = 3;




char  *please_wait_text[3] =
{
	"  ",
	"  ",
	"           Please Wait..."
};



char  *invalid_file2[2] = 
{
	"Invalid File Name",
	"      "
};



void  just_make_file()
{
	hp_output_name[0] = '\0';

	read_menu_string("Enter Hardcopy File Name",hp_output_name);

	if(hp_output_name[0] == '\0')
		return;

	hp_output = fopen(hp_output_name,"w");

	if(hp_output != NULL)
	{
		information_menu("  ",please_wait_text,3,'f','f');
		create_hp_file('f');   /*read from EMS pages 
				 ( 't' reads from pixel save file) */ 
		fclose(hp_output);
	}
	else
	{
		putchar(7);
		error_message(7,invalid_file2,2);
	}

}







char  *invalid_hp_file[4] =
{
	"Generate Hardcopy File:                                 ",
	"Invalid User Hardcopy Output File Name \0              ",
	"                                                             ",
	"Run  HARDCFE.EXE  To Correct The Name"
};




char  *invalid_dest[4] = 
{
	"Generate Hardcopy:",
	"Invalid Destination",
	"                                 ",
	"Reselect Using Change Parameters Option"
};






void  generate_hpgl()
{
	hpgl_destination = hardcopy_info.default_target - 1;
	size_of_paper = hardcopy_info.size_of_paper[hpgl_destination];
	type_of_plotter = hardcopy_info.type_of_plotter[hpgl_destination];
	use_hardcopy_colors = hardcopy_info.use_colors[hpgl_destination];


	hp_scale = paper_scale[size_of_paper][type_of_plotter];
	hp_x_offset = offset_table[size_of_paper][type_of_plotter].x_offset;
	hp_y_offset = offset_table[size_of_paper][type_of_plotter].y_offset;



	switch(hpgl_destination)
	{
		case 0 :  /* user command 1 */
		case 1 :  /* user command 2 */
		case 2 :  /* user command 3 */
		{
			if(strcmp(strupr(&(hardcopy_info.
				dos_commands[hpgl_destination][0])),
					"NONE") == 0)
			{
				putchar(7);
				error_message(7,invalid_dest,4);
				return;
			}



			if(hardcopy_info.user_file_names[hpgl_destination][0]
				== '\0')
			{
				sprintf(hp_output_name,"hardcopy.dat");
			}
			else
				sprintf(hp_output_name,"%s",&(hardcopy_info.
					user_file_names[hpgl_destination][0]));


			hp_output = fopen(hp_output_name,"w");

			if(hp_output != NULL)
			{
				information_menu("  ",please_wait_text,3,'f','f');
				create_hp_file('f');   /*read from EMS pages 
					 ( 't' reads from pixel save file) */ 
				fclose(hp_output);
			}
			else
			{
				putchar(7);

				sprintf(invalid_hp_file[2],"   %s",
					hp_output_name);

				error_message(7,invalid_hp_file,4);

				return;
			}


/*
			restore_win();
			make_win(0,0,79,24);
			save_win();
			clear_win(NORMAL_VIDEO);

			printf("\n%s\n\n",
				&(hardcopy_info.
					dos_commands[hpgl_destination][0]));
*/

			system(&(hardcopy_info.
				dos_commands[hpgl_destination][0]));

/*
			printf("\n\nPress Enter To Continue\n");


			turn_off_curser();
			restore_win();
			make_win(10,3,60,17);
			save_win();
*/

			break;
		}


		case 3 : /*to a file*/
		{
			just_make_file();
			break;
		}

	}
	
}










char  *paper_size_text[3] = 
{
	" A ",
	" B ",
	" C "
};


void  select_paper_size(the_number)
int the_number;
{
int selection;

	while(99)
	{
/*
		sprintf(temp_7_string,"ANSI Paper Size  Target #%d",
			the_number);
*/
		if(the_number < 4)
		{
			sprintf(temp_7_string,"ANSI Paper Size  %s",
				&(hardcopy_info.menu_text[the_number - 1][0]));
		}
		else
			strcpy(temp_7_string,"ANSI Paper Size  File Only");


		selection = choose_menu(temp_7_string,paper_size_text,3,
			hardcopy_info.size_of_paper[the_number - 1] + 1);

		switch(selection)
		{
			case 1 :
			case 2 :
			case 3 :
			{
				hardcopy_info.size_of_paper[the_number - 1]
					= selection - 1;
				break;
			}

			case ENTRY_COMPLETE :
			case TERMINATE :
			case REJECT :
				return;
		}
	}
}



char  *type_7580b_text[3] =
{
	"Special",
	"Normal",
	"Expanded"
};




void  select_7580b_mode(the_number)
int the_number;
{
int selection;

	while(99)
	{
/*
		sprintf(temp_7_string,"7580B Mode  Target #%d",the_number);
*/

		if(the_number < 4)
		{
			sprintf(temp_7_string,"7580B Mode  %s",
				&(hardcopy_info.menu_text[the_number - 1][0]));
		}
		else
			strcpy(temp_7_string,"7580B Mode  File Only");


		selection = choose_menu(temp_7_string,type_7580b_text,3,
			hardcopy_info.type_of_7580b[the_number - 1] + 1);

		switch(selection)
		{
			case 1 :
			case 2 :
			case 3 :
			{
				hardcopy_info.type_of_7580b[the_number - 1] =
					selection - 1;
				hardcopy_info.type_of_plotter[the_number - 1]
					= selection + 1;


				break;
			}


			case ENTRY_COMPLETE :
			case TERMINATE :
			case REJECT :
				return;
		}
	}
}



char  *plotter_type_text[3] =
{
	"7475A",
	"7550A",
	"7580B"
};


void  select_plotter_type(the_number)
int the_number;
{
int selection;
int my_temp;


	while(99)
	{
/*
		sprintf(temp_7_string,"Plotter Type  Target #%d",the_number);
*/
		if(the_number < 4)
		{
			sprintf(temp_7_string,"Plotter Type  %s",
				&(hardcopy_info.menu_text[the_number - 1][0]));
		}
		else
			strcpy(temp_7_string,"Plotter Type  File Only");



		if(hardcopy_info.type_of_plotter[the_number - 1] < 2)
			my_temp = hardcopy_info.
				type_of_plotter[the_number - 1];
		else
			my_temp = 2;

		selection = choose_menu(temp_7_string,plotter_type_text,3,
			my_temp + 1);

		switch(selection)
		{
			case 1 :  /* 7475A */
			case 2 :  /* 7550A */
			{
				hardcopy_info.type_of_plotter[the_number - 1]
					= selection - 1;
				break;
			}

			case 3 :  /* 7580B */
			{
				select_7580b_mode(the_number);

				break;
			}


			case ENTRY_COMPLETE :
			case TERMINATE :
			case REJECT :
				return;
		}
	}
}




char  *output_controls_text[3] =
{
	"Ansi Paper Size                     ",
	"Plotter Type                        ",
	"Select Pens      :                  "
};



void  the_output_controls(the_number)
int the_number;
{
int selection;
int my_temp;

	while(99)
	{
		if(the_number < 4)
		{
/*
			sprintf(temp_7_string,"Output Controls  Target #%d",
				the_number);
*/
			sprintf(temp_7_string,"Output Controls  %s",
				&(hardcopy_info.menu_text[the_number - 1][0]));
		}
		else
			sprintf(temp_7_string,"Output Controls  File Only");


		sprintf(output_controls_text[0],
			"ANSI Paper Size  :  %c",
				hardcopy_info.size_of_paper[the_number - 1] 
					+ 'A');


		if(hardcopy_info.type_of_plotter[the_number - 1] < 2)
			my_temp = hardcopy_info.type_of_plotter[
				the_number - 1];
		else
			my_temp = 2;


		sprintf(output_controls_text[1],
			"Plotter Type     :  %s",
			 	plotter_type_text[my_temp]);



		if(hardcopy_info.use_colors[the_number - 1] == 't')
		{
			sprintf(output_controls_text[2],
				"Select Pens      :  By Color");
		}
		else
			sprintf(output_controls_text[2],
				"Select Pens      :  Disabled");


		selection = choose_menu(temp_7_string,output_controls_text,3,0);


		switch(selection)
		{
			case 1 :
			{
				select_paper_size(the_number);
				break;
			}


			case 2 :
			{
				select_plotter_type(the_number);
				break;
			}


			case 3 :
			{
				if(hardcopy_info.use_colors[the_number - 1] 
					== 't')
					hardcopy_info.use_colors[
						the_number - 1] = 'f';
				else
					hardcopy_info.use_colors[
						the_number - 1] = 't';


				break;
			}


			case ENTRY_COMPLETE :
			case TERMINATE :
			case REJECT :
				return;
		}
	}
}






char  *change_target_text[4] = 
{
	"Send To Target # 1                                       ",
	"Send To Target # 2                                       ",
	"Send To Target # 3                                       ",
	"File Only"
};




void  change_target()
{
int selection;
int i;
char is_undefined[3];


	while(99)
	{
		for(i = 0 ; i < 3 ; i++)
		{
			strcpy(temp_7_string,&(hardcopy_info.
				dos_commands[i][0]));

			if((strcmp(strupr(temp_7_string),"NONE") == 0) ||
				(temp_7_string[0] == '\0'))
			{
				is_undefined[i] = 't';
				strcpy(change_target_text[i],"Undefined");
			}
			else
			{
				is_undefined[i] = 'f';
				strcpy(change_target_text[i],&(hardcopy_info.
					menu_text[i][0]));
			}
		}
		

		selection = choose_menu("Change Hardcopy Target",
			change_target_text,4,hardcopy_info.default_target);

		switch(selection)
		{
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			{
				if(is_undefined[selection - 1] == 't')
					break;


				hardcopy_info.default_target = selection;
				break;
			}

			case ENTRY_COMPLETE :
			case TERMINATE :
			case REJECT :
				return;
		}
	}
}




char  *hpgl_main_text[3] = 
{
	"hardcopy on                                           ",
	"Change Target        ",
	"Output Controls      "
};





void  hpgl_menu()
{
int selection;


	if(found_hardcopy_file == 'f')
		return;



	while(99)
	{
		if(hardcopy_info.default_target < 4)
		{
			sprintf(hpgl_main_text[0],"%s",hardcopy_info.
				menu_text[hardcopy_info.default_target - 1]);
		}
		else
			sprintf(hpgl_main_text[0],"Hardcopy To File Only");


		selection = choose_menu("HARDCOPY MODULE",hpgl_main_text,3,0);

		switch(selection)
		{
			case 1 :  /* generate hardcopy */
			{
				generate_hpgl();
				break;
			}


			case 2 :  /* change target */
			{
				change_target();
				break;
			}


			case 3 :  /* output controls */
			{
				the_output_controls(hardcopy_info.
					default_target);
				break;
			}


			case REJECT :
			case ENTRY_COMPLETE :
			case TERMINATE :
				return;
		}
	}
}

