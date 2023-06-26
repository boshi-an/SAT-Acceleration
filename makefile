# Make sat from microsat.c

CC = gcc

sat: microsat.c parallel.c sequential.c
	$(CC) -DUSING_PTHREAD -O2 -o parallel microsat.c parallel.c
	$(CC) -O2 -o sequential microsat.c sequential.c

clean: sat
	rm -f sat
