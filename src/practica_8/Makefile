all: createvd format dumpsec dumpseclog

global.o:
	cc -c global.c

vdisk.o: global.o
	cc -c vdisk.c

filesystem.o: global.o
	cc -c filesystem.c

createvd:
	cc -o createvd createvd.c

format: vdisk.o
	cc -c vdformat.c -iquote .
	cc -o vdformat vdformat.o vdisk.o

dumpsec:
	cc -c dumpsec.c -iquote .
	cc -o dumpsec dumpsec.o vdisk.o

dumpseclog:
	cc -c dumpseclog.c -iquote .
	cc -o dumpseclog dumpseclog.o vdisk.o

shell: filesystem.o
	cc -c shell.c -iquote .
	cc -o shell shell.o global.o filesystem.o

clean:
	rm -f *.o
	rm -f createvd
