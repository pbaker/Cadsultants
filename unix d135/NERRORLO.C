/* NERRORLO.C    ADVANCED  NEW  Error logging function() */




#include <stdio.h>

#define ERROR_LIMIT 25


typedef void (*ERROR_INSTRUCTIONS)();


extern FILE  *error_log_ptr;
extern char  path_error_log[];
extern char  errors_happened;
extern void  blinking_string();
extern void  normal_string();


extern char  previous_message[200];
extern char  error_path_string[100];
extern int  number_of_repeats;
extern int  number_of_errors;



extern void  clear_error_vars();


void  null_error_function()
{

}



/*puts the error messages into the error file*/
void  error_handler(the_string,severity,user_instructions)
char  the_string[];
int severity;
ERROR_INSTRUCTIONS user_instructions;
{
	if(error_log_ptr != NULL)  /*you wanted an error file in xxxx.set file*/
	{
		if(severity == 1)
		{
			putchar(7); putchar(7);
			(*user_instructions)();
		}


		if(number_of_errors > ERROR_LIMIT)
			return;

		number_of_errors++;

		/*make full path name of error file*/
		sprintf(error_path_string,"%serror_log.dat",path_error_log);
		error_log_ptr = fopen(error_path_string,"a");  /*append the file*/


		if((severity == 1) || (severity == 2))
		{
		    /*put up flashing message only once*/

	/*		if(errors_happened == 'f')  
				blinking_string("ERRORS",24,2);
	*/
		}
		else if(severity == 3)
			putchar(7);


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


