.c.o:
	gcc -c -std=gnu99 -I /usr/local/include  $<

LIBS :=  -L /usr/local/lib -lCscNet -lpthread

all: netSrvDemo

netSrvDemo: netSrvDemo.o
	gcc netSrvDemo.o $(LIBS) -o netSrvDemo

clean:
	rm netSrvDemo *.o server.log
