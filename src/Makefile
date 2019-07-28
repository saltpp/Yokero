# Makefile / Programmed by Salt
# This is for Microsoft nmake.exe.

#----- Add suffix for startup.s
.SUFFIXES: .s

#----- Macros
CC	= ee-gcc
CFLAGS	= -march=r4000 -O3 -g -mgp32 -mlong32 -c
AS	= ee-gcc
AFLAGS	= -march=r4000 -g -mgp32 -c -xassembler -O -a=startup.lst
LD	= ee-ld
LDFLAGS	= -M -Ttext 8900000 -q -o out
LIBS	= 
TARGET	= EBOOT.PBP
OBJS	= startup.o main.o SaltLibPSP.o Level.o Bullet.o Bitmaps.o \
	  SinCos.o Star.o Fonts5x7.o PadRecorder.o

PROGRAMNAME = "Yokero Ver.1.01"
MAPFILE	= map.txt

#----- Interface Rules
.c.o:
	$(CC) $(CFLAGS) $<

.s.o:
	$(CC) $(AFLAGS) $<


#----- Target
$(TARGET): outp
	elf2pbp outp $(PROGRAMNAME)

outp: out
	outpatch

out: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $@ $(LIBS) > $(MAPFILE)

clean:
	del $(TARGET)
	del $(OBJS)
	del out
	del outp

#----- Dependencies
$(OBJS): std.h

main.o:	SaltLibPSP.h \
	syscall.h \
	Level.h \
	Bitmaps.h \
	Star.h \
	PadRecorder.h \
