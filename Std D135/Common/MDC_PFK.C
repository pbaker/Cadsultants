


extern void far my_outp(int);
char far mdc_pfk_in_use = 'f';


void far pfk_out(value)
int value;
{
	if(mdc_pfk_in_use == 'f')
		return;


	if(value == 24)
		my_outp('"');
	else if(value == 25)
		my_outp('!');
	else
		my_outp(value + 32);
}

