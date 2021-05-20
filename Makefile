# Requires sudo apt-get install libssl-dev and probably openssl.

CC     = gcc
CFLAGS = -Wall -std=gnu99 -O3 -march=native -DTEST
LIBS   = -lcrypto
OBJS   = pfcrypt.so pufferfish.o

pufferfish.o: pufferfish.c
	gcc -Wall -fPIC $(CFLAGS) -o $@ -c $<

pfcrypt.o: pfcrypt.c
	gcc -Wall -fPIC $(CFLAGS) -I/usr/include/mariadb -I/usr/include/mysql -o $@ -c $<

pfcrypt.so: pfcrypt.o pufferfish.o
	gcc -Wall -fPIC $(LDFLAGS) -shared pfcrypt.o pufferfish.o -o $@

all: pfcrypt.so

clean:
	rm -f pfcrypt $(OBJS)

distclean: clean
