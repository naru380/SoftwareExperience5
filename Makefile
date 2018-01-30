OBJS=main.o token-list.o scan.o parse.o x-reference.o code-generate.o

all: mpplc

mpplc: ${OBJS}
	${CC} -o $@ $^

clean:
	rm -f *.o
