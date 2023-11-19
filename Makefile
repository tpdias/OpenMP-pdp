FLAGS=-O3 -lssl -lcrypto

CC=gcc

RM=rm -f

EXEC=password_bf

all: $(EXEC)

pass:
	clang -pg -o password_bf password_bf.c -fopenmp -lomp -lcrypto -I/opt/homebrew/opt/openssl/include -L/opt/homebrew/opt/openssl/lib

seq:
	gcc -o seq seq.c -lcrypto -I/opt/homebrew/opt/openssl/include -L/opt/homebrew/opt/openssl/lib

run:
	./$(EXEC)

clean:
	$(RM) password_bf.o $(EXEC)
