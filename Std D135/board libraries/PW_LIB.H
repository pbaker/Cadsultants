/*
 * Copyright PIXELWORKS Incorporated 1989. ALL RIGHTS RESERVED.
 * No part of this work may be reproduced or distributed in any form or by
 * any means without prior permission in writing from PIXELWORKS Incorporated.
 */




/* DOS - Microsoft 5.1 */
#define	PW_DOS
#define	PW_microsoft

#define PW_COMP


#include	"c:\emulate\clipper\pw_gen.h"
#include	"c:\emulate\clipper\pw_opcds.h"
#include	"c:\emulate\clipper\pw_routs.h"

#ifndef	PW_UNIX
#include	"c:\emulate\clipper\pw_ems.h"
#endif


/* Define Library version (format "%d.%2d", x/100, x%100 )*/
#define	PW_VERSION		530

/* RESET/TEST */
#define reset()			pw_sreset()
#define boardin(out,in)		pw_ioio(3,2,BOARDIN,out,1,(short far *)(in))

/* DRAWING */
#define move2(x,y)		pw_mdio(3,3,MOVE2,x,y,0)
#define move2r(x,y)		pw_mdio(3,3,MOVE2R,x,y,0)
#define move3(x,y,z)		pw_mdio(4,4,MOVE3,x,y,z,0)
#define move3r(x,y,z)		pw_mdio(4,4,MOVE3R,x,y,z,0)

#define draw2(x,y)		pw_mdio(3,3,DRAW2,x,y,0)
#define draw2r(x,y)		pw_mdio(3,3,DRAW2R,x,y,0)
#define draw3(x,y,z)		pw_mdio(4,4,DRAW3,x,y,z,0)
#define draw3r(x,y,z)		pw_mdio(4,4,DRAW3R,x,y,z,0)
									
#define vect2(x1,y1,x2,y2)	pw_mdio(5,5,VECT2,x1,y1,x2,y2,0)
#define vect2r(x1,y1,x2,y2)	pw_mdio(5,5,VECT2R,x1,y1,x2,y2,0)
#define vect3(x1,y1,z1,x2,y2,z2)	pw_mdio(7,7,VECT3,x1,y1,z1,x2,y2,z2,0)
#define vect3r(x1,y1,z1,x2,y2,z2) 	pw_mdio(7,7,VECT3R,x1,y1,z1,x2,y2,z2,0)

#define arc(r,sa,ea)		pw_mdio(4,4,ARC,r,sa,ea,0)
#define circle(r)		pw_mdio(2,2,CIRCLE,r,0)
#define earc(xr,yr,sa,ea)	pw_mdio(5,5,EARC,xr,yr,sa,ea,0)
#define ellipse(xr,yr)		pw_mdio(3,3,ELLIPSE,xr,yr,0)
#define esector(xr,yr,sa,ea)	pw_mdio(5,5,ESECTOR,xr,yr,sa,ea,0)
#define sector(r,sa,ea)		pw_mdio(4,4,SECTOR,r,sa,ea,0)

#define box(x,y)		pw_mdio(3,3,BOX,x,y,0)
#define boxr(x,y)		pw_mdio(3,3,BOXR,x,y,0)

#define point2()		pw_mdio(1,1,POINT2,0)
#define point3()		pw_mdio(1,1,POINT3,0)
														
#define flood(color)		pw_mdio(2,2,FLOOD,color,0)

#define polyl2(cnt,array)	pw_mdio(2+cnt*2,-2,POLYL2,cnt,cnt*2, \
				(short far *)(array),0);
#define polyl2r(cnt,array)	pw_mdio(2+cnt*2,-2,POLYL2R,cnt,cnt*2, \
				(short far *)(array),0);
#define pll2cnt(cnt)		pw_polycnt(2,POLYL2,cnt)
#define pll2rcnt(cnt)		pw_polycnt(2,POLYL2R,cnt)
#define pll2pt(x,y)		pw_polypt(x,y)

#define polyl3(cnt,array)	pw_mdio(2+cnt*3,-2,POLYL3,cnt,cnt*3, \
				(short far *)(array),0);
#define polyl3r(cnt,array)	pw_mdio(2+cnt*3,-2,POLYL3R,cnt,cnt*3, \
				(short far *)(array),0);
#define pll3cnt(cnt)		pw_polycnt(3,POLYL3,cnt)
#define pll3rcnt(cnt)		pw_polycnt(3,POLYL3R,cnt)
#define pll3pt(x,y,z)		pw_polypt(x,y,z)

#define polyg2(cnt,array)	pw_mdio(2+cnt*2,-2,POLYG2,cnt,cnt*2, \
				(short far *)(array),0);
#define polyg2r(cnt,array)	pw_mdio(2+cnt*2,-2,POLYG2R,cnt,cnt*2, \
				(short far *)(array),0);
#define plg2cnt(cnt)		pw_polycnt(2,POLYG2,cnt)
#define plg2rcnt(cnt)		pw_polycnt(2,POLYG2R,cnt)
#define plg2pt(x,y)		pw_polypt(x,y)

#define polyg3(cnt,array)	pw_mdio(2+cnt*3,-2,POLYG3,cnt,cnt*3, \
				(short far *)(array),0);
#define polyg3r(cnt,array)	pw_mdio(2+cnt*3,-2,POLYG3R,cnt,cnt*3, \
				(short far *)(array),0);
#define plg3cnt(cnt)		pw_polycnt(3,POLYG3,cnt)
#define plg3rcnt(cnt)		pw_polycnt(3,POLYG2R,cnt)
#define plg3pt(x,y,z)		pw_polypt(x,y,z)

#define curve2(x1,y1,x2,y2,x3,y3)	pw_mdio(7,7,CURVE2,x1,y1,x2,y2,x3,y3,0)
#define curve2r(x1,y1,x2,y2,x3,y3)	pw_mdio(7,7,CURVE2R,x1,y1,x2,y2,x3,y3,0)
#define curve3(x1,y1,z1,x2,y2,z2,x3,y3,z3)	pw_mdio(10,10,CURVE3,x1,y1,z1,x2,y2,z2,x3,y3,z3,0)
#define curve3r(x1,y1,z1,x2,y2,z2,x3,y3,z3)	pw_mdio(10,10,CURVE3R,x1,y1,z1,x2,y2,z2,x3,y3,z3,0)

/* DRAWING ATTRIBUTES */
#define arcstep(inc)		pw_mdio(2,2,ARCSTEP,inc,0)
#define color(index)		pw_mdio(2,2,COLOR,index,0)
#define curvstep(inc)		pw_mdio(2,2,CURVSTEP,inc,0)
#define linfun(function)	pw_mdio(2,2,LINFUN,function,0)
#define linpat(index)		pw_mdio(2,2,LINPAT,index,0)
#define prmfill(flag)		pw_mdio(2,2,PRMFILL,flag,0)
#define rddrwflg(flag)		pw_mdio(2,1,RDDRWFLG,1,(short far *)(flag))
#define setfpat(pat)		pw_mdio(17,-1,SETFPAT,16,(short far *)(pat),0)
#define setlstyle(index,pat)	pw_setlstyle(index,pat)
#define slpat32(style,pat)	pw_mdio(4,4,SLPAT32,style,(short)((pat)>>16), \
				(short)((pat)&0xffff),0)
#define pw_wait(count)		pw_mdio(2,2,WAIT,count,0)
#define wrmask(mask)		pw_mdio(2,2,WRMASK,mask,0)

/* DISPLAY CONTROL */
#define blkmask(mask)		pw_mdio(2,2,BLKMASK,mask,0)
#define display(flag)		pw_display(flag)
#define lut4(i,red,grn,blu)	pw_mdio(5,5,LUT4,i,red,grn,blu,0)
#define lut8(i,red,grn,blu)	pw_mdio(5,5,LUT8,i,red,grn,blu,0)
#define lutinit()		pw_mdio(1,1,LUTINIT,0)
#define rdlut(i,red,grn,blu)	pw_mdio(5,2,RDLUT,i,3,(short far *)(red), \
				(short far *)(grn),(short far *)(blu))
#define rdmask(mask)		pw_mdio(2,2,RDMASK,mask,0)
#define setvideo(flag)		pw_mdio(2,2,SETVIDEO,flag,0)

/* GRAPHICS TEXT */
#define gtext(string)		pw_gtext((char far *)string)
#define gctinfo(a,b,c,d,e,f,g,h)   pw_mdio(9,1,GCTINFO,8,(short far *)(a), \
					(short far *)(b),(short far *)(c), \
					(short far *)(d),(short far *)(e), \
					(short far *)(f),(short far *)(g), \
					(short far *)(g),(short far *)(h))
#define gtangle(angle)		pw_mdio(2,2,GTANGLE,angle,0)
#define gtjust(horiz,vert)	pw_mdio(3,3,GTJUST,horiz,vert,0)
#define gtprop(flag)		pw_mdio(2,2,GTPROP,flag,0)
#define gtsize(hsize,vsize)	pw_mdio(3,3,GTSIZE,hsize,vsize,0)
#define gtspace(spacing)	pw_mdio(2,2,GTSPACE,spacing,0)
#define gtstyle(font)		pw_mdio(2,2,GTSTYLE,font,0)
#define rctext(color,chr)	pw_rctext(color,chr)
#define gchrwid(chr,x,y)	pw_mdio(4,2,GCHRWID,chr,2, \
				(short far *)(x),(short far *)(y),0)
#define tdefin(count,data)	pw_sdio(2+count,-2,TDEFIN,count,count, \
				(short far *)(data),0);
#define tundefin()		pw_mdio(1,1,TUNDEFIN,0)

/* CURSOR TEXT */
#define ctext(color,ch)		pw_mdio(2,2,CTEXT, \
				(((color)&0xff) << 8) | ((ch)&0xff),0)
#define rctaddr(ctaddr)		pw_mdio(2,1,RCTADDR,1, (short far *)(ctaddr),0)
#define sctaddr(ctaddr)		pw_mdio(2,2,SCTADDR,ctaddr,0)
#define sctbcol(color)		pw_mdio(2,2,SCTBCOL,color,0)

/* GRAPHICS MARKERS */
#define gmangle(angle)		pw_mdio(2,2,GMANGLE,angle,0)
#define gmark(marker)		pw_mdio(2,2,GMARK,marker,0)
#define gmsize(size)		pw_mdio(2,2,GMSIZE,size,0)

/* READ COMMANDS */
#define gipmask(planes)		pw_mdio(2,1,GIPMASK,1,(short far *)(planes))
#define gpixsize(lx,hx,ly,hy)	pw_mdio(5,1,GPIXSIZE,4, \
				(short far *)(lx),(short far *)(hx), \
				(short far *)(ly),(short far *)(hy))
#define gvpxsize(lx,hx,ly,hy)	pw_mdio(5,1,GVPXSIZE,4, \
				(short far *)(lx),(short far *)(hx), \
				(short far *)(ly),(short far *)(hy))

#define rdmatmd2(mat)		pw_mdio(10,1,RDMATMD2,-9,(short far *)(mat),0)
#define rdmatvw2(mat)		pw_mdio(10,1,RDMATVW2,-9,(short far *)(mat),0)
#define rdmatmd3(mat)		pw_mdio(17,1,RDMATMD3,-16,(short far *)(mat),0)
#define rdmatvw3(mat)		pw_mdio(17,1,RDMATVW3,-16,(short far *)(mat),0)

#define rdmemsiz(size)		pw_mdio(2,1,RDMEMSIZ,1,(short far *)(size))

#define rdpix2(color)		pw_mdio(2,1,RDPIX2,1,(short far *)(color))
#define rdpix3(color)		pw_mdio(2,1,RDPIX3,1,(short far *)(color))

#define rdpos2(x,y)		pw_mdio(3,1,RDPOS2,2,(short far *)(x), \
				(short far *)(y))
#define rdpos3(x,y,z)		pw_mdio(4,1,RDPOS3,3,(short far *)(x), \
				(short far *)(y),(short far *)(z),

#define rdstat(status)		pw_mdio(2,1,RDSTAT,1,(short far *)(status))
#define rdver(version)		pw_mdio(2,1,RDVER,1,(short far *)(version))

/* VIEWING TRANSFORMATIONS */
#define vwport(xmn,xmx,ymn,ymx)	pw_mdio(5,5,VWPORT,xmn,xmx,ymn,ymx,0)
#define window(xmn,xmx,ymn,ymx)	pw_mdio(5,5,WINDOW,xmn,xmx,ymn,ymx,0)

#define vwdist(distance)	pw_mdio(2,2,VWDIST,distance,0)
#define vwiden3()		pw_mdio(1,1,VWIDEN3,0)
#define vwmatx2(array)		pw_mdio(10,-1,VWMATX2,9,(short far *)(array),0)
#define vwmatx3(array)		pw_mdio(17,-1,VWMATX3,16,(short far *)(array),0)
#define vwpara(lambda,beta)	pw_mdio(3,3,VWPARA,lambda,beta,0)
#define vwrotx3(angle)		pw_mdio(2,2,VWROTX3,angle,0)
#define vwroty3(angle)		pw_mdio(2,2,VWROTY3,angle,0)
#define vwrotz3(angle)		pw_mdio(2,2,VWROTZ3,angle,0)
#define vwrpt3(x,y,z)		pw_mdio(4,4,VWRPT3,x,y,z,0)

/* MODELLING */
#define mdiden2()		pw_mdio(1,1,MDIDEN2,0)
#define mdiden3()		pw_mdio(1,1,MDIDEN3,0)
#define mdmatx2(array)		pw_mdio(10,-1,MDMATX2,9,(short far *)(array),0)
#define mdmatx3(array)		pw_mdio(17,-1,MDMATX3,16,(short far *)(array),0)
#define mdorg2(x,y)		pw_mdio(3,3,MDORG2,x,y,0)
#define mdorg3(x,y,z)		pw_mdio(4,4,MDORG3,x,y,z,0)
#define mdrot2(angle)		pw_mdio(2,2,MDROT2,angle,0)
#define mdrotx3(angle)		pw_mdio(2,2,MDROTX3,angle,0)
#define mdroty3(angle)		pw_mdio(2,2,MDROTY3,angle,0)
#define mdrotz3(angle)		pw_mdio(2,2,MDROTZ3,angle,0)
#define mdscal2(sx,sy)		pw_mdio(3,3,MDSCAL2,sx,sy,0)
#define mdscal3(sx,sy,sz)	pw_mdio(4,4,MDSCAL3,sx,sy,sz,0)
#define mdtran2(tx,ty)		pw_mdio(3,3,MDTRAN2,tx,ty,0)
#define mdtran3(tx,ty,tz)	pw_mdio(4,4,MDTRAN3,tx,ty,tz,0)

#define map2d3d()		pw_mdio(1,1,MAP2D3D,0)
#define convert()		pw_mdio(1,1,CONVERT,0)
#define reconcat()		pw_mdio(1,1,RECONCAT,0)
#define precat(preflag)		pw_mdio(2,2,PRECAT,preflag,0)

/* BLOCK TRANSFER COMMANDS */
#define imager(y,xmn,xmx,rld)		pw_imager(y,xmn,xmx,rld)
#define imagew(y,xmn,xmx,rld)		pw_imagew(y,xmn,xmx,rld)
#define imagewn(y,xmn,xmx,num,rld)	pw_imagewn(y,xmn,xmx,num,rld)

#define blt(x,y)		pw_mdio(3,3,BLT,x,y,0)
#define bltpc(addr,rcorder)	pw_pcbltpc(BLTPC,(void far *)addr,rcorder)
#define bltpcp(addr,rcorder)	pw_mdio(4,4,BLTPC, \
				(short)(((unsigned long)(addr))&0xffff), \
				(short)(((unsigned long)(addr))>>16),rcorder,0)
#define pcblt(addr,rcorder)	pw_pcbltpc(PCBLT,(void far *)addr,rcorder)
#define pcbltp(addr,rcorder)	pw_mdio(4,4,PCBLT, \
				(short)(((unsigned long)(addr))&0xffff), \
				(short)(((unsigned long)(addr))>>16),rcorder,0)
#define bltpcm(addr,rc,x,y)	pw_bltpcm((void far *)addr,rc,x,y)
#define bltpcmp(addr,rc,x,y)	pw_mdio(6,6,BLTPCM, \
				(short)(((unsigned long)(addr))&0xffff), \
				(short)(((unsigned long)(addr))>>16),rc,x,y,0)
#define bltmask(mask)		pw_mdio(2,2,BLTMASK,mask,0)
#define bltrwind(x,y)		pw_mdio(3,3,BLTRWIND,x,y,0)
#define byteswap(flag)		pw_mdio(2,2,BYTESWAP,flag,0)

/* PUSH AND POP COMMANDS  */
#define pushattr()		pw_mdio(1,1,PUSHATTR,0)
#define pushmd2()		pw_mdio(1,1,PUSHMD2,0)
#define pushvw2()		pw_mdio(1,1,PUSHVW2,0)
#define pushfm2()		pw_mdio(1,1,PUSHFM2,0)
#define pushtm()		pw_mdio(1,1,PUSHTM,0)
#define pushmm()		pw_mdio(1,1,PUSHMM,0)
#define pushmd3()		pw_mdio(1,1,PUSHMD3,0)
#define pushvw3()		pw_mdio(1,1,PUSHVW3,0)
#define pushfm3()		pw_mdio(1,1,PUSHFM3,0)
#define pushword(data)		pw_mdio(2,2,PUSHWORD,data,0)
#define pushlut()		pw_mdio(1,1,PUSHLUT,0)
#define pushcolor()		pw_mdio(1,1,PUSHCOLOR,0)

#define popattr()		pw_mdio(1,1,POPATTR,0)
#define popmd2()		pw_mdio(1,1,POPMD2,0)
#define popvw2()		pw_mdio(1,1,POPVW2,0)
#define popfm2()		pw_mdio(1,1,POPFM2,0)
#define poptm()			pw_mdio(1,1,POPTM,0)
#define popmm()			pw_mdio(1,1,POPMM,0)
#define popmd3()		pw_mdio(1,1,POPMD3,0)
#define popvw3()		pw_mdio(1,1,POPVW3,0)
#define popfm3()		pw_mdio(1,1,POPFM3,0)
#define popword(word)		pw_mdio(1,1,POPWORD,1,(short far *)(word))
#define poplut()		pw_mdio(1,1,POPLUT,0)
#define popcolor()		pw_mdio(1,1,POPCOLOR,0)

/* Pick commands */
#define picken(flag)		pw_mdio(2,2,PICKEN,flag,0)
#define pickwind(lx,hx,ly,hy)	pw_mdio(5,5,PICKWIND,ly,hy,lx,hx,0)
#define setpid(id)		pw_mdio(2,2,SETPID,id,0)
#define rdpid(id)		pw_sdio(2,1,RDPID,1,(short far *)(id))
#define setcid(id)		pw_mdio(2,2,SETCID,id,0)

/* Display list control */
#define branch(address)		pw_dl3(BRANCH,(OPCODE far *)(address))
#define branchp(address)	pw_dl3p(BRANCH,address)
#define branchid(address)	pw_dl3(BRANCHID,(OPCODE far *)(address))
#define branchidp(address)	pw_dl3p(BRANCHID,address)
#define sdladdr(addr)		pw_sdl((OPCODE far *)(addr))
#define sdladdrp(addr)		pw_sdlp(addr)
#define rdladdr(addr)		pw_rdl((OPCODE far * far *)(addr))
#define rdladdrp(addr)		pw_rdlp(addr)
#define call(address)		pw_dl3(CALL,(OPCODE far *)(address))
#define callp(address)		pw_dl3p(CALL,address)
#define ret()			pw_dl1(RET)
#define comment(numwords,words) pw_mdio(2+numwords,-2,COMMENT,numwords, \
				numwords,(short far *)(words),0)
#define noop()			pw_mdio(1,1,NOOP,0)
#define dlend()			pw_dl1(DLEND)
#define dlint(reason)		pw_dl2(DLINT,reason)

#define dlgo()			pw_dlgo()
#define dlrun(addr)		pw_dlrun((OPCODE far *)addr)
#define dlrunp(addr)		pw_dlrunp(1,addr)

/* C Library Defines for Floating Point opcodes */

/* DRAWING */   
#define fmove2(x,y)		pw_fgout(1,FMOVE2,2,x,y)
#define fmove2r(x,y)		pw_fgout(1,FMOVE2R,2,x,y)
#define fmove3(x,y,z)		pw_fgout(1,FMOVE3,3,x,y,z)
#define fmove3r(x,y,z)		pw_fgout(1,FMOVE3R,3,x,y,z)

#define fdraw2(x,y)		pw_fgout(1,FDRAW2,2,x,y)
#define fdraw2r(x,y)		pw_fgout(1,FDRAW2R,2,x,y)
#define fdraw3(x,y,z)		pw_fgout(1,FDRAW3,3,x,y,z)
#define fdraw3r(x,y,z)		pw_fgout(1,FDRAW3R,3,x,y,z)

#define fvect2(x1,y1,x2,y2)	pw_fgout(1,FVECT2,4,x1,y1,x2,y2)
#define fvect2r(x1,y1,x2,y2)	pw_fgout(1,FVECT2R,4,x1,y1,x2,y2)
#define fvect3(x1,y1,z1,x2,y2,z2)	pw_fgout(1,FVECT3,6,x1,y1,z1,x2,y2,z2)
#define fvect3r(x1,y1,z1,x2,y2,z2)	pw_fgout(1,FVECT3R,6,x1,y1,z1,x2,y2,z2)

#define fpoint2()		pw_mdio(1,1,FPOINT2,0)
#define fpoint3()		pw_mdio(1,1,FPOINT3,0)

#define fpolyl2(cnt,array)	pw_mdio(2+cnt<<2,-2,FPOLYL2,cnt,cnt<<2, \
				(short far *)(array),0);
#define fpolyl2r(cnt,array)	pw_mdio(2+cnt<<2,-2,FPOLYL2R,cnt,cnt<<2, \
				(short far *)(array),0);
#define fpll2cnt(cnt)		pw_polycnt(2,FPOLYL2,cnt)
#define fpll2rcnt(cnt)		pw_polycnt(2,FPOLYL2R,cnt)
#define fpll2pt(x,y)		pw_fpolypt(x,y)

#define fpolyl3(cnt,array)	pw_mdio(2+cnt*6,-2,FPOLYL3,cnt,cnt*6, \
				(short far *)(array),0);
#define fpolyl3r(cnt,array)	pw_mdio(2+cnt*6,-2,FPOLYL3R,cnt,cnt*6, \
				(short far *)(array),0);
#define fpll3cnt(cnt)		pw_polycnt(3,FPOLYL3,cnt)
#define fpll3rcnt(cnt)		pw_polycnt(3,FPOLYL3R,cnt)
#define fpll3pt(x,y,z)		pw_fpolypt(x,y,z)

#define fpolyg2(cnt,array)	pw_mdio(2+cnt<<2,-2,FPOLYG2,cnt,cnt<<2, \
				(short far *)(array),0);
#define fpolyg2r(cnt,array)	pw_mdio(2+cnt<<2,-2,FPOLYG2R,cnt,cnt<<2, \
				(short far *)(array),0);
#define fplg2cnt(cnt)		pw_polycnt(2,FPOLYG2,cnt)
#define fplg2rcnt(cnt)		pw_polycnt(2,FPOLYG2R,cnt)
#define fplg2pt(x,y)		pw_fpolypt(x,y)

#define fpolyg3(cnt,array)	pw_mdio(2+cnt*6,-2,FPOLYG3,cnt,cnt*6, \
				(short far *)(array),0);
#define fpolyg3r(cnt,array)	pw_mdio(2+cnt*6,-2,FPOLYG3R,cnt,cnt*6, \
				(short far *)(array),0);
#define fplg3cnt(cnt)	 	pw_polycnt(3,FPOLYG3,cnt)
#define fplg3rcnt(cnt)		pw_polycnt(3,FPOLYG3R,cnt)
#define fplg3pt(x,y,z)		pw_fpolypt(x,y,z)

#define fprmfill(flag)		pw_mdio(2,2,FPRMFILL,flag,0)

/* READ COMMANDS */
#define frdpos2(x,y)		pw_fgin(1,FRDPOS2,2,x,y)
#define frdpos3(x,y,z)		pw_fgin(1,FRDPOS3,3,x,y,z)
	
#define frdmatmd2(mat)		pw_mdio(19,1,FRDMATMD2,-18,(short far *)(mat),0)
#define frdmatvw2(mat)		pw_mdio(19,1,FRDMATVW2,-18,(short far *)(mat),0)
#define frdmatmd3(mat)		pw_mdio(33,1,FRDMATMD3,-32,(short far *)(mat),0)
#define frdmatvw3(mat)		pw_mdio(33,1,FRDMATVW3,-32,(short far *)(mat),0)

/* VIEWING TRANSFORMATIONS */
#define fvwport(xmn,xmx,ymn,ymx) pw_fgout(1,FVWPORT,4,xmn,xmx,ymn,ymx)
#define fwindow(xmn,xmx,ymn,ymx) pw_fgout(1,FWINDOW,4,xmn,xmx,ymn,ymx)
#define fvwdist(distance)	pw_fgout(1,FVWDIST,1,distance)
#define fvwcliph(flag)		pw_mdio(2,2,FVWCLIPH,flag,0)
#define fvwclipy(flag)		pw_mdio(2,2,FVWCLIPY,flag,0)
#define fvwdisth(distance)	pw_fgout(1,FVWDISTH,1,distance)
#define fvwdisty(distance)	pw_fgout(1,FVWDISTY,1,distance)
#define fvwmatx2(arr)		pw_mdio(19,-1,FVWMATX2,18,(short far *)(arr),0)
#define fvwmatx3(arr)		pw_mdio(33,-1,FVWMATX3,32,(short far *)(arr),0)
#define fvwiden3()		pw_mdio(1,1,FVWIDEN3,0)
#define fvwrpt3(x,y,z)		pw_fgout(1,FVWRPT3,3,x,y,z)

/* MODELLING */
#define fmdiden2()		pw_mdio(1,1,FMDIDEN2,0)
#define fmdiden3()		pw_mdio(1,1,FMDIDEN3,0)
#define fmdmatx2(arr)		pw_mdio(19,-1,FMDMATX2,18,(short far *)(arr),0)
#define fmdmatx3(arr)		pw_mdio(33,-1,FMDMATX3,32,(short far *)(arr),0)
#define fsettrn2d(arr)		pw_mdio(19,-1,FSETTRN2D,18,(short far *)(arr),0)
#define fsettrn3d(arr)		pw_mdio(33,-1,FSETTRN3D,32,(short far *)(arr),0)
#define freconcat()		pw_mdio(1,1,FRECONCAT,0)

/* PUSH AND POP COMMANDS  */
#define fpushmd2()		pw_mdio(1,1,FPUSHMD2,0)
#define fpushmd3()		pw_mdio(1,1,FPUSHMD3,0)
#define fpushvw2()		pw_mdio(1,1,FPUSHVW2,0)
#define fpushvw3()		pw_mdio(1,1,FPUSHVW3,0)
#define fpushfm2()		pw_mdio(1,1,FPUSHFM2,0)
#define fpushfm3()		pw_mdio(1,1,FPUSHFM3,0)
#define fpopmd2()		pw_mdio(1,1,FPOPMD2,0)
#define fpopmd3()		pw_mdio(1,1,FPOPMD3,0)
#define fpopvw2()		pw_mdio(1,1,FPOPVW2,0)
#define fpopvw3()		pw_mdio(1,1,FPOPVW3,0)
#define fpopfm2()		pw_mdio(1,1,FPOPFM2,0)
#define fpopfm3()		pw_mdio(1,1,FPOPFM3,0)

#ifdef	PW_COMP
/* Compatibility defines */
#define move(x,y)   		move2(x,y)
#define mover(x,y)		move2r(x,y)
#define mover2(x,y)		move2r(x,y)
#define mover3(x,y,z)		move3r(x,y,z)
#define draw(x,y)		draw2(x,y)
#define drawr(x,y)		draw2r(x,y)
#define drawr2(x,y)		draw2r(x,y)
#define drawr3(x,y,z)		draw3r(x,y,z)
#define frect(x,y)		box(x,y)
#define frectr(x,y)		boxr(x,y)
#define curve(x1,y1,x2,y2,x3,y3)   curve2(x1,y1,x2,y2,x3,y3)
#define curver(x1,y1,x2,y2,x3,y3)  curve2r(x1,y1,x2,y2,x3,y3)	
#define lut(i,red,green,blue)	lut4(i,red,green,blue)
#define prmfil(flag)		prmfill(flag)
#define pllcnt(cnt)		pll2cnt(cnt)
#define pllrcnt(cnt)		pll2rcnt(cnt)
#define pllpt(x,y)		pll2pt(x,y)
#define plgcnt(cnt)		plg2cnt(cnt)
#define plgrcnt(cnt)		plg2rcnt(cnt)
#define plgpt(x,y)		plg2pt(x,y)
#define cgaon()			pw_display(1)
#define cgaoff()		pw_display(0)
#define vidon()			setvideo(0x8)
#define vidoff()		setvideo(0x0)

/* old text commands */
#define tprop(flag)		gtprop(flag)
#define tspace(spacing)		gtspace(spacing)	
#define tangle(angle)		gtangle(angle)
#define tstyle(font)		gtstyle(font)
#define tsize(hsize,vsize)	gtsize(hsize,vsize)
#define tjust(horiz,vert)	gtjust(horiz,vert)	

#define rstat(status)		rdstat(status)	
#define rdv2pos(x,y)		rdpos2(x,y)
#define rdv3pos(x,y,z)		rdpos3(x,y,z)	
#define rd2md(mat)		rdmatmd2(mat)
#define rd2matmd(mat)		rdmatmd2(mat)
#define rd2vw(mat)		rdmatvw2(mat)
#define rd2matvw(mat)		rdmatvw2(mat)
#define rd3md(mat)		rdmatmd3(mat)
#define rd3matmd(mat)		rdmatmd3(mat)
#define rd3vw(mat)		rdmatvw3(mat)
#define rd3matvw(mat)		rdmatvw3(mat)
#define rdplanes(planes)	gipmask(planes)	
#define rdpixsiz(lx,hx,ly,hy)	gpixsize(lx,hx,ly,hy)
#define rdvissiz(lx,hx,ly,hy)	gvpxsize(lx,hx,ly,hy)	
#define parproj(lambda,beta)	vwpara(lambda,beta)
#define recat()			reconcat()
#define setpre(preflag)		precat(preflag)
#define bltrwin(x,y)		bltrwind(x,y)
#define setpic(flag)		picken(flag)
#define picwin(lx,hx,ly,hy)	pickwind(lx,hx,ly,hy)
#define setpicid(id)		setpid(id)
#define rdpicid(id)		rdpid(id)
#define rdpos(x,y)		rdpos2(x,y)
#define point()			point2()
#define viewport(xmn,xmx,ymn,ymx)	vwport(xmn,xmx,ymn,ymx)

/* For library compatability */
#define rdmat(count,code,size,mat)	pw_mdio( size+1, 1, code, -size, \
									(short far *)(mat), 0 )
#define pw_wrmat(count,code,array)	pw_mdio( size+1, -1, code, count, \
									(short far *)(array), 0 )
#define cbranch(addr)			pw_cbranch(addr)
#define sdlbptr(buf)			pw_dl_init(buf,0,(short *)0,0)
#define rdlbptr(buf)			*buf=pw_data.usr.ptr

#define initcga()			pw_initcga()
#define gsetup( irqn, dman, inth )	pw_setup( irqn, dman, 0x330, inth )
#define inmode()			pw_inmode()
#define outmode()			pw_outmode()
#define slaveon()			pw_slaveon()
#define slaveoff()			pw_slaveoff()
#endif
