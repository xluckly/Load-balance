all: log.o fdwrapper.o conn.o mgr.o springsnail

log.o: log.cpp log.h
	g++ -c log.cpp -o log.o -g
fdwrapper.o : fdwrapper.cpp fdwrapper.h
	g++ -c fdwrapper.cpp -o fdwrapper.o -g
conn.o : conn.cpp conn.h
	g++ -c conn.cpp -o conn.o -g
mgr.o : mgr.cpp mgr.h
	g++ -c mgr.cpp -o mgr.o -g
springsnail: processpool.h main.cpp log.o fdwrapper.o conn.o mgr.o
	g++ processpool.h log.o fdwrapper.o conn.o mgr.o main.cpp -o springsnail -g

clean : 
	rm *.o springsnail
