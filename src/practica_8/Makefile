all: createvd format dumpsec dumpseclog

vdisk.o: vdisk.c vdisk.h global.h
	cc -c vdisk.c

createvd: createvd.c vdisk.o
	cc -o createvd createvd.c

format: vdformat.c
	cc -o vdformat vdformat.c

dumpsec: dumpsec.c vdisk.h
	cc -o dumpsec dumpsec.c

dumpseclog: dumpseclog.c vdisk.h
	cc -o dumpseclog dumpseclog.c