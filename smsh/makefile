smsh:smsh.o splitline.o execute.o process.o controlflow.o builtin.o varlib.o
	gcc -o smsh smsh.o splitline.o execute.o process.o controlflow.o builtin.o varlib.o
smsh.o:smsh.c
	gcc -c smsh.c
splitline.o:splitline.c
	gcc -c splitline.c
execute.o:execute.c
	gcc -c execute.c
process.o:process.c
	gcc -c process.c
controlflow.o:controlflow.c
	gcc -c controlflow.c
builtin.o:builtin.c
	gcc -c builtin.c
varlib.o:varlib.c
	gcc -c varlib.c
clean:
	rm -f *.o
