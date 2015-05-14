all: createvd format dumpsec dumpseclog shell

global.o:
	cc -c global.c

vdisk.o: global.o
	cc -c vdisk.c -iquote .

filesystem.o: inode.o
	cc -c filesystem.c -iquote .

sectorhandler.o: global.o
	cc -c sectorhandler.c -iquote .

inode.o: sectorhandler.o
	cc -c inode.c -iquote .

date.o: global.o
	cc -c date.c -iquote .

datamap.o: sectorhandler.o
	cc -c datamap.c -iquote .

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

shell: global.o vdisk.o date.o datamap.o sectorhandler.o inode.o filesystem.o
	cc -c shell.c -iquote .
	cc -o shell global.o vdisk.o date.o datamap.o sectorhandler.o inode.o filesystem.o shell.o

run-shell: shell
	./shell

clean:
	rm -f *.o
	rm -f createvd dumpsec dumpseclog vdformat shell
	rm -f *.vd
