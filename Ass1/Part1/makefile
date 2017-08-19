.c.o:
	gcc -c -std=gnu99 -I /usr/local/include  $<

LIBS :=  -L /usr/local/lib -lCscNet -lpthread

all: netCliDemo

netCliDemo: netCliDemo.o 
	gcc netCliDemo.o $(LIBS) -o netCliDemo

clean:
	rm netCliDemo *.o netCliDemo.log
