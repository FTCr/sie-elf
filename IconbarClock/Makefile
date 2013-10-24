NAME = IconbarClock

FNAME   = ${NAME}.elf
OBJECTS = *.o
SOURCES = *.c
ifdef SOURCES
	sources=${SOURCES}
else
	sources=*.c
endif

CCX   = arm-eabi-g++
CC    = arm-eabi-gcc
LD    = arm-eabi-ld
LC_M := LC_MESSAGES=C



CFLAGS = -I../include -I inc -W -msoft-float -mlittle-endian -mcpu=arm926ej-s -mtune=arm926ej-s -nostdlib -fshort-wchar \
		-fno-exceptions -fno-builtin -fdata-sections -ffunction-sections -std=gnu99 -O2
LDFLAGS = -L../libs -marmelf -z max-page-size=1 --defsym __ex=0 --gc-section -s -pie
LIB_LDFLAGS = ${LDFLAGS} -shared -Bsymbolic -Bsymbolic-function -soname=${FNAME}

LIBS = -lgcc -lcrt -lcrt_helper -lxce -lc -lmxml

.PHONY = clean

elf_elka: compile_elka link_elf
elf_nsg:  compile_nsg  link_elf
elf_sg:   compile_sg   link_elf

compile_elka:
	${LC_M} ${CC} ${CFLAGS} -DELKA -DNEWSGOLD -c ${sources}
compile_nsg:
	${LC_M} ${CC} ${CFLAGS} -DNEWSGOLD -c ${sources}
compile_sg:
	${LC_M} ${CC} ${CFLAGS} -DSGOLD -c ${sources}

link_elf:
	${LC_M} ${LD} -o ${FNAME} ${OBJECTS} ${LDFLAGS} ${LIBS}

clean:
	rm -f $(OBJECTS)
