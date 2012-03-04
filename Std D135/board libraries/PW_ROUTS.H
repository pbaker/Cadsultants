/*
 * Copyright PIXELWORKS Incorporated 1989. ALL RIGHTS RESERVED.
 * No part of this work may be reproduced or distributed in any form or by
 * any means without prior permission in writing from PIXELWORKS Incorporated.
 */

/*
 * 11-Nov-88 ddarby; created
 */

/* Define Library Routines */
#ifndef PW_INTERACTIVE
void	pw_addr( void far *, unsigned short far *, unsigned short far * );
void	pw_addrl( void far *, unsigned long far * );

void	pw_bltpcm( void far *, int, int, int );
void	pw_pcbltpc( OPCODE, void far *, int );

void	pw_initcga( void );

void	pw_display( int );

void	pw_dl1( OPCODE );
void	pw_dl2( OPCODE, OPCODE );
void	pw_dl3( OPCODE, OPCODE far * );
void	pw_dl3p( OPCODE, unsigned long );
void	pw_dlgo( void );
void	pw_dlrun( OPCODE far * );
void	pw_dlrunp( int, unsigned long );

void	pw_error( int );

int	pw_setup( int, int, int, void (*)() );
int	pw_fsetup( int, int, int, void (*)(), char * );
void	pw_close( void );

void	pw_fgin( int, int, ... );
void	pw_fgout( int, ... );

void	pw_ioio( int, ... );
void	pw_mdio( int, ... );
void	pw_sdio( int, ... );

void	pw_polypt( OPCODE, OPCODE, ...);
void	pw_fpolypt( double, double, ...);

void	pw_polycnt( int, short, short);

void	pw_gtext( char far * );

int	pw_imager( int, int, int, char far * );
void	pw_imagew( int, int, int, char far * );
void	pw_imagewn( int, int, int, int, char far * );

void	pw_rctext( OPCODE *, char * );

void	pw_rdl( OPCODE far * far *);
void	pw_rdlp( unsigned long far * );
void	pw_sdl( OPCODE far * );
void	pw_sdlp( unsigned long );

void	pw_setlstyle( int, short *);

void	pw_slaveon( void );
void	pw_slaveoff( void );
void	pw_outmode( void );
void	pw_inmode( void );

void	pw_slaveio( int, OPCODE far * );

void	pw_sreset( void );

int	pw_testmc( void );
int	pw_rdps2( int, int far *, int far *, int far *);

/* All display list routines for double buffering */
void		pw_dl_init( OPCODE *, int, OPCODE *, int );
void		pw_dl_exec( int );

void		pw_dl_wait( void );
int		pw_dl_busy( void );
void		pw_dl_buf( short *, unsigned int );
void		pw_dl_chksize( int );
#ifdef	PW_UNIX
void	pw_sighndlr( void );
int	pw_valloc( char **, char **, int * );
#endif
#else
void	pw_addr( );
void	pw_addrl( );

void	pw_bltpcm( );
void	pw_pcbltpc( );

void	pw_initcga( );

void	pw_display( );

void	pw_dl1( );
void	pw_dl2( );
void	pw_dl3( );
void	pw_dl3p( );
void	pw_dlgo( );
void	pw_dlrun( );
void	pw_dlrunp( );

void	pw_error( );

int	pw_setup( );
int	pw_fsetup( );
void	pw_close( );

void	pw_fgin( );
void	pw_fgout( );

void	pw_ioio( );
void	pw_mdio( );
void	pw_sdio( );

void	pw_polypt( );
void	pw_fpolypt( );

void	pw_polycnt( );

void	pw_gtext( );

int	pw_imager( );
void	pw_imagew( );
void	pw_imagewn( );

void	pw_rctext( );

void	pw_rdl( );
void	pw_rdlp( );
void	pw_sdl( );
void	pw_sdlp( );

void	pw_setlstyle( );

void	pw_slaveon( );
void	pw_slaveoff( );
void	pw_outmode( );
void	pw_inmode( );

void	pw_slaveio( );

void	pw_sreset( );

int	pw_testmc( );
int	pw_rdps2( );

/* All display list routines for double buffering */
void		pw_dl_init( );
void		pw_dl_exec( );

void		pw_dl_wait( );
int		pw_dl_busy( );
void		pw_dl_chksize( );
#ifdef	PW_UNIX
void	pw_sighndlr( );
int	pw_valloc( );
#endif
#endif
#define	pw_dl_size()		((int)pw_data.usr.ptr - (int)pw_data.usr.curbuf)
#define	pw_dl_free()		(pw_data.usr.size - (int)((char *)pw_data.usr.ptr - (char *)pw_data.usr.curbuf))
#define	pw_dl_rdptr(addr)	*addr = pw_data.usr.ptr
#define	pw_dl_wrptr(addr)	pw_data.usr.ptr = addr

#define	pw_dl_char( x )		*((char*)pw_data.usr.ptr)++=((char)(x))
#define	pw_dl_short( x )	*((short*)pw_data.usr.ptr)++=((short)(x))
#define	pw_dl_long( x )		*((long*)pw_data.usr.ptr)++=((long)(x))
#define	pw_dl_even() 		if (((int)pw_data.usr.ptr)&0x1) pw_dl_char(NOOP)

