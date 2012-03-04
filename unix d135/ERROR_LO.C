/* ERROR_LO.C     Error logging function() */


/* NOVEMBER 1, 1989 added error reporting limit */


#include <stdio.h>

#define ERROR_LIMIT 25

extern FILE  *error_log_ptr;
extern char  path_error_log[];
extern char  errors_happened;
extern void  blinking_string();
extern void  normal_string();


char  previous_message[200] = "   ";
char  error_path_string[100];
int  number_of_repeats = 1;
int  number_of_errors = 0;



void  clear_error_vars()
{
/*
remove blinking "ERRORS" message
	normal_string("        ",24,2);
*/
	errors_happened = 'f';
	previous_message[0] = ' ';
	previous_message[1] = '\0';
}




/*puts the error messages into the error file*/
void  error_logger(the_string)
char the_string[];
{
	if(error_log_ptr != NULL)  /*you wanted an error file in xxxx.set file*/
	{
		if(number_of_errors > ERROR_LIMIT)
			return;

		number_of_errors++;
		putchar(7);

		/*make full path name of error file*/
		sprintf(error_path_string,"%serror_log.dat",path_error_log);
		error_log_ptr = fopen(error_path_string,"a");  /*append the file*/

#ifdef kdskdskdskdsk
		if(errors_happened == 'f')  /*put up flashing message only once*/
			blinking_string("ERRORS",24,2);
#endif

		errors_happened = 't';

		if(strcmp(the_string,previous_message) != 0) /*they're not = */
		{
			strcpy(previous_message,the_string); /*update old one*/

			if(number_of_repeats > 1)
			{ /*add the message with # of times*/
				fprintf(error_log_ptr,"(%d times)\n\n %s",
					number_of_repeats,the_string);
			}
			else  /*just add the message*/
				fprintf(error_log_ptr,"%s",the_string);

			number_of_repeats = 1;
		}
		else
			number_of_repeats++;

		if(ferror(error_log_ptr) != 0)  /*file is too big*/
		{  			        /*so clear the file*/
			fclose(error_log_ptr);

		/*make full path name of error file*/
			sprintf(error_path_string,"%serrorlog.dat",
				path_error_log);

		/*clear error file*/
			error_log_ptr = fopen(error_path_string,"w");


			fprintf(error_log_ptr,"Error File Overflow:\n\n");

		/*add the last message*/
			fprintf(error_log_ptr,"\n\n%s",the_string);

			number_of_repeats = 1;
			strcpy(previous_message,"    ");
		}

		fclose(error_log_ptr);
	}
}


