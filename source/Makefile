UNAME := $(shell uname)

CC	= gcc
CFLAGS	= -Wall -g -ansi -pedantic
FFLAGS	= ${CFLAGS} -lm
PROGRAM = optimizacion-bb
OBJECTS = fhandle.o branch_bound.o main.o
INCLUDE = include/prototipos.h include/data_types.h
ifeq ($(UNAME),Linux)
  GL_LIB  = -lglut -lGLU -lGL
endif
ifeq ($(UNAME),Darwin)
  GL_LIB = -framework GLUT -framework OpenGL
endif

all:	${PROGRAM}

${PROGRAM}: ${OBJECTS}
	${CC} ${FFLAGS} -o $@ ${OBJECTS}

.SUFFIXES: .c
.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

desplegar: gl_desplegar.c
	${CC} ${FFLAGS} $? -o $@ ${GL_LIB}

clean:
	- rm -rf *.o *~ include/*~ core

realclean:
	- rm -rf *.o *~ include/*~ core
	- rm ${PROGRAM}
	- rm -rf desplegar desplegar.dSYM

main.o: main.c ${INCLUDE}
fhandle.o: fhandle.c ${INCLUDE}
branch_bound.o: branch_bound.c

