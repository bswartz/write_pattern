PROGS=write_pattern

all: ${PROGS}

clean:
	@rm -f *.o ${PROGS}

write_pattern: write_pattern.o
	@gcc -o $@ $^
	@strip $@

%.o: %.c
	@gcc -Wall -Werror -c $<
