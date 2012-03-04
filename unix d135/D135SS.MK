
d135ss: rawkeyfuncs.o read_a_key.o undefined.o icu.o comio.o hpglmenu.o readmenu.o nmenus.o choose.o custread.o diagnost.o display.o error_lo.o expanded.o host_esc.o indicato.o license.o lights.o m_device.o makehpgl.o megatek.o mmfilter.o my10.o my11.o my12.o my13.o my1segm.o my1subr.o my2.o my2funcs.o my3.o my4.o my5.o my6.o my7.o my8.o my_gmark.o nerrorlo.o nreadcfg.o pixelbrd.o singleup.o sswindow.o readhard.o 
	cc rawkeyfuncs.o read_a_key.o undefined.o icu.o comio.o hpglmenu.o readmenu.o nmenus.o choose.o custread.o diagnost.o display.o error_lo.o expanded.o host_esc.o indicato.o license.o lights.o m_device.o makehpgl.o megatek.o mmfilter.o my10.o my11.o my12.o my13.o my1segm.o my1subr.o my2.o my2funcs.o my3.o my4.o my5.o my6.o my7.o my8.o my_gmark.o nerrorlo.o nreadcfg.o pixelbrd.o singleup.o sswindow.o readhard.o -lm -lpw -od135ss


rawkeyfuncs.o: rawkeyfuncs.c
	cc -c rawkeyfuncs.c

read_a_key.o: read_a_key.c
	cc -c read_a_key.c


undefined.o: undefined.c
	cc -c undefined.c

icu.o: icu.c
	cc -c icu.c

comio.o: comio.c
	cc -c comio.c


hpglmenu.o: hpglmenu.c
	cc -c hpglmenu.c

readmenu.o: readmenu.c
	cc -c readmenu.c

nmenus.o: nmenus.c
	cc -c nmenus.c

readhard.o: readhard.c
	cc -c readhard.c

choose.o: choose.c
	cc -c choose.c

custread.o: custread.c
	cc -c custread.c

diagnost.o: diagnost.c
	cc -c diagnost.c

display.o: display.c
	cc -c display.c

error_lo.o: error_lo.c
	cc -c error_lo.c

expanded.o: expanded.c
	cc -c expanded.c

host_esc.o: host_esc.c
	cc -c host_esc.c

indicato.o: indicato.c
	cc -c indicato.c

license.o: license.c
	cc -c license.c

lights.o: lights.c
	cc -c lights.c

m_device.o: m_device.c
	cc -c m_device.c

makehpgl.o: makehpgl.c
	cc -c makehpgl.c

megatek.o: megatek.c
	cc -c megatek.c

mmfilter.o: mmfilter.c
	cc -c mmfilter.c

my10.o: my10.c
	cc -c my10.c

my11.o: my11.c
	cc -c my11.c

my12.o: my12.c
	cc -c my12.c

my13.o: my13.c
	cc -c my13.c

my1segm.o: my1segm.c
	cc -c my1segm.c

my1subr.o: my1subr.c
	cc -c my1subr.c

my2.o: my2.c
	cc -c my2.c

my2funcs.o: my2funcs.c
	cc -c my2funcs.c

my3.o: my3.c
	cc -c my3.c

my4.o: my4.c
	cc -c my4.c

my5.o: my5.c
	cc -c my5.c

my6.o: my6.c
	cc -c my6.c

my7.o: my7.c
	cc -c my7.c

my8.o: my8.c
	cc -c my8.c

my_gmark.o: my_gmark.c
	cc -c my_gmark.c

nerrorlo.o: nerrorlo.c
	cc -c nerrorlo.c

nreadcfg.o: nreadcfg.c
	cc -c nreadcfg.c

pixelbrd.o: pixelbrd.c
	cc -c pixelbrd.c

singleup.o: singleup.c
	cc -c singleup.c

sswindow.o: sswindow.c
	cc -c sswindow.c


