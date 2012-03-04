#include "stdio.h"
#include "graph.h"
#include "dos.h"
#include "string.h"
#include "stdlib.h"

#define TRUE 1
#define FALSE 0


#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1



#define RES_1024_768 1
#define RES_1280_1024 2


/*my C versions of BIOS calls*/
extern void far make_win();
extern void far clear_win();
extern void far save_win();
extern void far restore_win();
extern void far goto_row_col();
extern void far read_curser_pos();
extern void far blinking_string();
extern void far normal_string();
extern void far turn_on_curser();
extern void far turn_off_curser();



union REGS far my_input_regs,far my_output_regs;
char far char_save_page[25][80],far attr_save_page[25][80];

int far win_x_low,far win_y_low,far win_x_up,far win_y_up;

int far row_curser,far col_curser;
int far selection = 0;
int far previous_switch = 0;


FILE far *config_file, far *test_file;

struct config
{
	int baud_rate,cursor_speed;
	char drive_letter,listing_size,screen_update_mode,
		print_listings,update_timer,ems_mode;
	char error_path[80];
	char screen_resolution;
	char have_display_list;
};


struct config far config_info;
int far size_config_file = sizeof(struct config);
char far input_string[80],far temp_string[80];



struct a_controller
{
	char name[66];
	int id;
};



struct a_controller far controllers[] =
	{
		{"Artist10  640 x 480" , 0},
		{"Artist10  800 x 560" , 1},
		{"Artist10  1024 x 768" , 2},
		{"Clipper 1024 x 768" , 3},
		{"Clipper 1280 x 1024" , 4},
		{"Ega" , 5},
		{"Metheus 1104" , 6},
		{"Vectrix Pepe" , 7},
		{"Vga" , 8}
	};



int far num_controllers = sizeof(controllers) / sizeof(struct a_controller);
int far which_controller = 0;


int far find_controller(id)
int id;
{
int j;

	for(j = 0 ; j <= num_controllers ; j++)
		if(controllers[j].id == id)
			return(j);


	return(0);
}




make_default_data()
{
	config_info.baud_rate = 12;
	config_info.cursor_speed = 4;
	config_info.drive_letter = 'c';   /* c: */
	config_info.listing_size = 'n';   /*normal size*/
	config_info.screen_update_mode = 'm';    /*manual*/
	config_info.print_listings = FALSE;
	config_info.update_timer = FALSE;
	config_info.ems_mode = 's';   /*software EMS*/
	strcpy(config_info.error_path,"c:\\");  /* c:\ as the path*/
	config_info.screen_resolution = 0;
	config_info.have_display_list = TRUE;
}



void far read_rest_string(row,col,first_char)
int row,col,first_char;
{
int i,ch;


	goto_row_col(row,col);
	putchar(first_char);
	input_string[0] = first_char;


	i = 1;
	for(ch = getch() ; (ch != 13) /*enter key*/ && (ch != 0) ;
		 ch = getch())
	{
		if(ch != 8)
		{
			putchar(ch);
			input_string[i] = ch;
			i++;
		}
		else
		{
			if(i > 0)
			{
				i--;

				goto_row_col(row,col + i);
				putchar(' ');
				goto_row_col(row,col + i);
			}
		}
	}


	input_string[i] = '\0';
}




int far the_baud_rate()
{
	switch(config_info.baud_rate)
	{
		case 6 :
			return(19200);

		case 12 :
			return(9600);

		case 24 :
			return(4800);

		case 48 :
			return(2400);

		case 96 :
			return(1200);
	}
}




void far switches_menu()
{
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf(" Switches Menu");


	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("   F1 Expanded Memory Mode         :  ");

	if(config_info.ems_mode == 's')
		printf("Software");
	else
		printf("Hardware");


	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("   F2 Listing Size                 :  ");

	if(config_info.listing_size == 'n')
		printf("Normal");
	else
		printf("Double");

	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("   F3 Printer Control              :  ");

	if(config_info.print_listings == TRUE)
		printf("Enabled");
	else
		printf("Disabled");

	goto_row_col(win_y_low + 6,win_x_low + 1);
	printf("   F4 Graphic Controller           :  ");
	printf("%s",controllers[find_controller(config_info.screen_resolution)
		].name);




	goto_row_col(win_y_low + 7,win_x_low + 1);
	printf("   F5 Screen Update Mode           :  ");

	if(config_info.screen_update_mode == 'm')
		printf("Manual");
	else
		printf("Automatic");

	goto_row_col(win_y_low + 8,win_x_low + 1);
	printf("   F6 Screen Update Timer          :  ");

	if(config_info.update_timer == TRUE)
		printf("Enabled");
	else
		printf("Disabled");


	goto_row_col(win_y_low + 9,win_x_low + 1);
	printf("   F7 D90 Display List             :  ");

	if(config_info.have_display_list == TRUE)
		printf("Enabled");
	else
		printf("Disabled");


	if(previous_switch >= 0x3b)
	{
		goto_row_col(win_y_low + previous_switch - 0x3b + 3,
			win_x_low + 6);
		putchar('>');
	}
}





void far main_menu()
{
	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf(" PC Workwindow");
	goto_row_col(win_y_low + 2,win_x_low + 1);
	printf(" Configuration File Editor");

	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("   F1 Baud Rate                    :  %d",the_baud_rate());
	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("   F2 Cursor Speed                 :  ");

	if(config_info.cursor_speed >= 1)
		printf("%d",config_info.cursor_speed);
	else
		printf("%f",-1.0/config_info.cursor_speed);

	goto_row_col(win_y_low + 6,win_x_low + 1);
	printf("   F3 Display File Storage Device  :  %c:",
		config_info.drive_letter);

	goto_row_col(win_y_low + 7,win_x_low + 1);
	printf("   F4 Error Log File Path          :  %s",
		config_info.error_path);

	goto_row_col(win_y_low + 8,win_x_low + 1);
	printf("   F5 Switches Menu");



	goto_row_col(win_y_low + 10,win_x_low + 1);
	printf("  F10 EXIT TO DOS");
}



void far change_baud_rate()
{
int baud_rate;

baud_again:
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf(" Change Baud Rate");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("  Current Value = %d",the_baud_rate());
	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("    F1 1200");
	goto_row_col(win_y_low + 6,win_x_low + 1);
	printf("    F2 2400");
	goto_row_col(win_y_low + 7,win_x_low + 1);
	printf("    F3 4800");
	goto_row_col(win_y_low + 8,win_x_low + 1);
	printf("    F4 9600");
	goto_row_col(win_y_low + 9,win_x_low + 1);
	printf("    F5 19200");

	baud_rate = getch();

	if(baud_rate != 0)
		goto baud_again;

	baud_rate = getch();

	switch(baud_rate)
	{
		case 0x3b : /*1200*/
		{
			config_info.baud_rate = 96;
			break;
		}

		case 0x3c : /*2400*/
		{
			config_info.baud_rate = 48;
			break;
		}

		case 0x3d : /*4800*/
		{
			config_info.baud_rate = 24;
			break;
		}

		case 0x3e : /*9600*/
		{
			config_info.baud_rate = 12;
			break;
		}

		case 0x3f : /*19200*/
		{
			config_info.baud_rate = 6;
			break;
		}


		case 75 :  /*left arrow*/
		case 77 :  /*right arrow*/
		case 80 :  /*down_arrow*/
			return;


		default :
			goto baud_again;
	}
}



void far change_cursor_speed()
{
double ratio;
int first_char;


	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf(" Change Curser Speed Ratio");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("   Current Ratio = ");

	if(config_info.cursor_speed >= 1)
		printf("%d",config_info.cursor_speed);
	else
		printf("%f",-1.0/config_info.cursor_speed);

	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("     Enter New Value : ");

	first_char = getch();

	if(first_char == 0)   /*extended code*/
	{
		getch();
		return;
	}

	read_rest_string(win_y_low + 5,win_x_low + 24,first_char);

	ratio = atof(input_string);

	if(ratio <= 0.0)
		config_info.cursor_speed = 1;
	else if(ratio < 1.0)
		config_info.cursor_speed = (int)(-1.0/ratio);
	else
		config_info.cursor_speed = (int)ratio;

}


void far change_drive_letter()
{
int drive_letter;

drive_again:
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf(" Change Display File Storage Device");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("   Current Device = %c:",config_info.drive_letter);
	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("    Enter The Device : ");

	drive_letter = getch();

	if(drive_letter == 0)
	{
		getch();
		return;
	}

	read_rest_string(win_y_low + 5,win_x_low + 24,drive_letter);

	drive_letter = input_string[0];


	if((drive_letter == 'a') || (drive_letter == 'A') ||
		(drive_letter == 'b') || (drive_letter == 'B'))
	{
		config_info.drive_letter = drive_letter;
		return;
	}


	sprintf(temp_string,"%c:drivetst",drive_letter);
	test_file = fopen(temp_string,"w");

	if(test_file == NULL)
	{
		putchar(7);
		goto_row_col(win_y_low + 7,win_x_low + 1);
		printf("      Drive %c: Does Not Exist",drive_letter);
		goto_row_col(win_y_low + 8,win_x_low + 1);
		printf("      Press Enter To Continue");
		getch();

		goto drive_again;
	}
	else
	{
		fclose(test_file);
		remove(temp_string);
	}


	config_info.drive_letter = drive_letter;
}




void far error_file_path()
{
int first_char,i,j;

path_again:
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf(" Change Path For Error Log File");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("   Current Path = %s",config_info.error_path);
	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("     Enter Full Path Name : ");

	first_char = getch();

	if(first_char == 0)
	{
		getch();
		return;
	}

	read_rest_string(win_y_low + 5,win_x_low + 29,first_char);


/*skip leading blanks and tabs*/
	for(i = 0 ; (input_string[i] == ' ') || (input_string[i] == '\t') ;
		 i++)
			;

/*check for no error log file case*/
	strcpy(temp_string,input_string);  /*copy it (strupr changes its arg)*/

	if(strcmp(strupr(temp_string),"NONE") == 0)
	{
		strcpy(config_info.error_path,temp_string);
		return;
	}


	if((input_string[i] == 'a') || (input_string[i] == 'A') ||
		(input_string[i] == 'b') || (input_string[i] == 'B'))
	{
		if(input_string[i + 1] == ':')
		{
			config_info.error_path[0] = input_string[i];
			config_info.error_path[1] = ':';
			config_info.error_path[2] = '\0';
			return;
		}
	}



/*find end of input string*/
	for(j = i ; input_string[j] != '\0' ; j++)
		;

	if(input_string[j - 1] != '\\')
	{
		input_string[j] = '\\';
		input_string[j + 1] = '\0';
	}



	sprintf(temp_string,"%spathtest",input_string + i);
	test_file = fopen(temp_string,"w");

	if(test_file == NULL)
	{
		putchar(7);
		goto_row_col(win_y_low + 7,win_x_low + 1);
		printf("     That Path Does Not Exist");
		goto_row_col(win_y_low + 8,win_x_low + 1);
		printf("     Press Enter To Continue");
		getch();

		goto path_again;
	}
	else
	{
		fclose(test_file);
		remove(temp_string);
	}


	strcpy(config_info.error_path,input_string + i);
}


void far exit_code()
{
int exit_mode;

	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf(" EXIT TO DOS");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("   F1 Exit And Save Changes");
	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("   F2 Exit And Ignore Changes");

	exit_mode = getch();

	if(exit_mode != 0)
		return;

	exit_mode = getch();

	if(exit_mode == 0x3b)
	{
		fseek(config_file,0L,SEEK_SET);
		fwrite(&config_info,size_config_file,1,config_file);
		fclose(config_file);
	}
	else if(exit_mode == 0x3c)
		fclose(config_file);
	else
		return;

	make_win(0,0,79,24);
	clear_win(NORMAL_VIDEO);
	turn_on_curser();
	goto_row_col(0,0);
	exit(0);
}





void far process_switches()
{
int selection;

	while(2001)
	{
		switches_menu();

		selection = getch();

		if(selection != 0)   /*not extended code*/
			continue;

		selection = getch();

		switch(selection)
		{
			case 0x3b : /*ems mode*/
			{
				if(config_info.ems_mode == 's')  /*software*/
					config_info.ems_mode = 'h';  /*make hardware*/
				else  
					config_info.ems_mode = 's';  /*make software*/

				previous_switch = 0x3b;

				break;
			}

			case 0x3c : /*listing size*/
			{
				if(config_info.listing_size == 'n')  /*normal*/
					config_info.listing_size = 'd';  /*make double*/
				else
					config_info.listing_size = 'n';  /*make normal*/

				previous_switch = 0x3c;

				break;
			}

			case 0x3d : /*printer control*/
			{
				if(config_info.print_listings == TRUE)
					config_info.print_listings = FALSE;
				else
					config_info.print_listings = TRUE;

				previous_switch = 0x3d;

				break;
			}

			case 0x3e : /*screen resolution*/
			{
				which_controller++;

				if(which_controller >= num_controllers)
					which_controller = 0;

				config_info.screen_resolution = 
					controllers[which_controller].id;


				previous_switch = 0x3e;

				break;
			}


			case 0x3f : /*screen update mode*/
			{
				if(config_info.screen_update_mode == 'm')  /*manual*/
					config_info.screen_update_mode = 'a'; 
				else
					config_info.screen_update_mode = 'm';

				previous_switch = 0x3f;

				break;
			}

			case 0x40 : /*update timer*/
			{
				if(config_info.update_timer == TRUE)
					config_info.update_timer = FALSE;
				else
					config_info.update_timer = TRUE;
	
				previous_switch = 0x40;

				break;
			}


			case 0x41 : /*d90 display list use*/
			{
				if(config_info.have_display_list == TRUE)
					config_info.have_display_list = FALSE;
				else
					config_info.have_display_list = TRUE;

				previous_switch = 0x41;

				break;
			}


			case 75 :  /*arrow*/
			case 77 : 
			case 80 :  /*keys*/
				return;

		}
	}
}




void far process_selection()
{
int temp;

	temp = selection;
	selection = getch();

	if(selection != 0)
	{
		selection = temp;
		return;
	}

	selection = getch();

	switch(selection)
	{
		case 0x3b : /*Change Baud Rate*/
		{
			change_baud_rate();
			break;
		}

		case 0x3c : /*change cursor speed*/
		{
			change_cursor_speed();
			break;
		}

		case 0x3d : /*drive letter*/
		{
			change_drive_letter();
			break;
		}

		case 0x3e : /*error log file path*/
		{
			error_file_path();
			break;
		}


		case 0x3f : /*switches menu*/
		{
			process_switches();
			break;
		}



		case 0x44 : /*exit this program*/
		{
			exit_code();
			selection = temp;
			break;
		}


		default :
			selection = temp;
	}
}





main()
{
	config_file = fopen("work.cfg","r+b");

	if(config_file == NULL)
	{
		config_file = fopen("work.cfg","w+b");
		make_default_data();
		fwrite(&config_info,size_config_file,1,config_file);
		fseek(config_file,0L,SEEK_SET);  /*go to beginning of file*/
	}


	fread(&config_info,size_config_file,1,config_file);


	turn_off_curser();

	make_win(0,0,79,24);
	clear_win(NORMAL_VIDEO);

	make_win(10,3,70,15);


	while(99)
	{
		clear_win(REVERSE_VIDEO);
		draw_box();

		main_menu();

	/*show previous valid main menu selection*/
		if((selection >= 0x3b) && (selection < 0x44))
		{
			goto_row_col(win_y_low + 4 + (selection - 0x3b),
				win_x_low + 6);
			putchar('>');
		}


		process_selection();
	}
}
