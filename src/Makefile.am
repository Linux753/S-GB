bin_PROGRAMS = sgb

sgb_SOURCES = cpu.c main.c opcode.c cpu.h emul.h emul.c opcode.h loadROM.c loadROM.h event.c event.h utils.h
sgb_LDADD = $(MATHLIB) -lSDL2main -lSDL2

noinst_LIBRARIES = libsgb.a
libsgb_a_SOURCES = opcode.c opcode.h cpu.c cpu.h
libsgb_a_LIBADD = $(MATHLIB) 
