all: frontend backend 

frontend:
	gcc -c frontend.c
	gcc header.h frontend.c -o frontend

backend:
	gcc -c backend.c
	gcc header.h -o backend backend.o users_lib.o -lpthread

clean:
	rm frontend
	rm backend
	rm frontend.o
	rm backend.o
