# Acn-project

Q1: 
Server:
	gcc q1.c; ./a.out
Listener: socat -u udp-recv:9999,reuseport -
Q2:

Q3:
gcc q3.c; ./a.out
curl 127.0.0.1:9999
