## Acn-project

###Q1 
######Server
- gcc q1.c; ./a.out
######Listener 
- socat -u udp-recv:9999,reuseport

###Q2
1. gcc q2.c -o Alice -pthread; ./Alice <Bob_ip_addr> <port_1> <port_2>
2. gcc q2.c -o Bob -pthread; ./Bob <Alice_ip_addr> <port_2> <port_1>

###Q3
- gcc q3.c; ./a.out
- curl 127.0.0.1:9999
