void  my_diag(s)
char s[];
{
int i;

	if(diag_flag == 't')
	{
		if(print_flag == 'f')
		{
			for(i = 0 ; i < sub_level ; i++)
				printf("      ");

			printf("%s",s);
		}
		else
		{
/*
			for(i = 0 ; i < sub_level ; i++)
				fprintf(out_ptr,"       ");

			fprintf(out_ptr,"%s",s);
			i = fflush(out_ptr);
*/
		}
	}

/*	if(diag_num >= 0)
		call_count[diag_num]++;
*/
}
