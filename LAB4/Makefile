all: udpc udps

udpc: udpc.c
	$gcc -o udpc udpc.c -pthread

udps: udps.c
	$gcc -o udps udps.c -pthread

clean:
	rm -f udpc