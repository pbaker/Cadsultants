



extern void far open_megatek(int);
extern void far open_host(int);
extern void far ugout(int);
extern void far megatek_out(int);
extern int far read_megatek_buffer(void);
extern int far read_host_buffer(void);


extern void far mono_string(char far *,int,int,int);
extern void far output_a_char(int,int,int,int);
extern void far read_mono_string(char far *,int,int,int);


extern void far control_window(void);
extern int far choose_menu(char far *,char far *[],int,int);
extern int far information_menu(char far *,char far *[],int,char,char);
extern void far error_message(int,char far *[],int);

