#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <CscNetLib/netCli.h>
#include <CscNetLib/std.h>

#define LINE_MAX 99


int main(int argc, char **argv)
{   char line[LINE_MAX+1];
    int fdes;
    int isFin;
    FILE *fp;
    int lineNo;
    csc_cli_t *ntp;
    int servConn;

// Make the connection.
    ntp = csc_cli_new();
    if( ntp == NULL)
    {	printf("Sorry Failed to Create new netCli object!\n");
    	exit(1);
    }
    else printf("Created new netCli object!\n");
    servConn = csc_cli_setServAddr(ntp, "TCP", "127.0.0.1", 9991);
    if( servConn == -1)
    {	fprintf(stdout,"Sorry invalid client endpoint parameters: \"%s\"\n", csc_cli_getErrMsg(ntp));
    	exit(1);
    }
    else printf("All Client Parameters loaded successful!\n");
    fdes = csc_cli_connect(ntp);
    if( fdes == -1)
    {	fprintf(stdout,"Sorry, server connection unsuccessful: \"%s\"\n", csc_cli_getErrMsg(ntp));
    	exit(1);
    }
    else printf("Server connected successfuly!\n");
    csc_cli_free(ntp);

// Make FILE* of file descriptor.
    fp = fdopen(fdes, "r+");
    if(fp == NULL)
    {	printf("Sorry failed to open and read the stream!\n");
    	fclose(fp);
      	exit(1);
    }
    else printf("Stream read Successfully\n");

// Data to send.
    const char *sendData[] = 
    { "HEAD /index.html HTTP/1.1"
    , "Host: localhost" 
    , "Connection: close" 
    , "" 
    };

// Send a request.
    lineNo = 0;
    for (int i=0; i<csc_dim(sendData); i++)
    {   fprintf(fp, "%s\n", sendData[i]);
        fprintf(stdout, "Sent %3d \"%s\"\n", ++lineNo, sendData[i]);
    }
    fflush(fp);

// Get the response.
    isFin = csc_FALSE;
    lineNo = 0;
    while(!isFin)
    {   int lineLen = csc_fgetline(fp, line, LINE_MAX);
        if (lineLen < 0)
        {   if (csc_streq(line,""))
             {  printf("Got %3d \"%s\"\n", ++lineNo, line);
             	isFin = csc_TRUE;
            	}
		else
		{	printf("EOF\n");
		    	isFin = csc_TRUE;
		}  
        }
    }
    fclose(fp);

// Bye.
    exit(0);
}

