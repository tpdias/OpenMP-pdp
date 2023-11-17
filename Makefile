
FLAGS=-O3 -lssl -lcrypto

CC=gcc

RM=rm -f

EXEC=password_bf

all: $(EXEC)

$(EXEC):
	gcc -o password_bf password_bf.c -lcrypto -I/opt/homebrew/opt/openssl/include -L/opt/homebrew/opt/openssl/lib

run:
	./$(EXEC)

clean:
	$(RM) password_bf.o $(EXEC)
