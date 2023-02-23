CROSS_COMPILE ?= D:\tools\gcc-test\gcc-arm-10.3-2021.07-mingw-w64-i686-aarch64-none-linux-gnu.tar\gcc-arm-10.3-2021.07-mingw-w64-i686-aarch64-none-linux-gnu\bin\aarch64-none-linux-gnu-
CC := ${CROSS_COMPILE}gcc.exe
LD := ${CROSS_COMPILE}ld.exe
object = transelect.o test4head.o
headtest : ${object}
	${CC} ${object} -o headtest
test4head.o : test4head.c
transelect.o : transelect.c
clean :
	del headtest ${object}
