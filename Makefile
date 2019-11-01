ssh_honeypot.so :
	cc -Wall -fPIC -O -g src/ssh_honeypot.c -c -o ssh_honeypot.o
	cc -shared  ssh_honeypot.o -lsqlite3 -o ssh_honeypot.so

install :
	mkdir -p /lib/security/
	cp ssh_honeypot.so /lib/security/ssh_honeypot.so

clean :
	rm -f ssh_honeypot.so
	rm -f ssh_honeypot.o
