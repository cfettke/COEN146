all: tcpc tcps

tcpc: tcpc.c
	$gcc -o tcpc tcpc.c 

tcps: tcps.c
	$gcc -o tcps tcps.c -pthread

clean:
	rm -f tcpc