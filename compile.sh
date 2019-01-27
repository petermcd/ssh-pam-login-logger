sudo rm /lib/security/ssh_honeypot.so

gcc -Wall -fPIC -O -g src/ssh_honeypot.c -c -o ssh_honeypot.o
gcc -shared  ssh_honeypot.o -lsqlite3 -o ssh_honeypot.so

rm ssh_honeypot.o

sudo mkdir /lib/security/

sudo mv ssh_honeypot.so /lib/security/ssh_honeypot.so

sudo service sshd restart