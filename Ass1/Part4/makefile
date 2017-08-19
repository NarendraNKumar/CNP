.c.o:
	gcc -c -std=gnu99 -I /usr/local/include  $<

LIBS :=  -L /usr/local/lib -lCscNet -lpthread

all: EdolasHTTPServer EdolasHTTPClient

EdolasHTTPServer: EdolasHTTPServer.o
	gcc EdolasHTTPServer.o $(LIBS) -o EdolasHTTPServer
EdolasHTTPClient: EdolasHTTPClient.o
	gcc EdolasHTTPClient.o $(LIBS) -o EdolasHTTPClient

clean:
	rm EdolasHTTPServer EdolasHTTPClient *.o server.log
