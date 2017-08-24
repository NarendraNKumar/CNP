#include <stdio.h>
#include <stdlib.h>
#include <CscNetLib/std.h>
#include <CscNetLib/json.h>


void processData(FILE *fin, FILE *fout)
{	csc_jsonErr_t errNum;
	const char *requestedProt, *rRequest, *requestedFile,*requestedHost,*requestedConn;
	int file_length, statusCode;
 
// Read json data.
	csc_json_t *inData = csc_json_newParseFILE(fin);
	requestedProt = csc_json_getStr(inData, "protocol", &errNum);
	rRequest = csc_json_getStr(inData, "request", &errNum);
	requestedFile = csc_json_getStr(inData, "path", &errNum);
	requestedHost = csc_json_getStr(inData, "host", &errNum);
	requestedConn = csc_json_getStr(inData, "connection", &errNum);
	csc_json_free(inData);
 
// Print out the data.
	printf("Json Data read successfuly!\n\n");
	fprintf(stdout, "\tprotocol: \"%s\"\n", requestedProt);
	fprintf(stdout, "\trequest: \"%s\"\n", rRequest);
	fprintf(stdout, "\tpath: \"%s\"\n", requestedFile);
	fprintf(stdout, "\thost: \"%s\"\n", requestedHost);
	fprintf(stdout, "\tconnection: \"%s\"\n\n", requestedConn);
	
// Check the existence of requestedFile
if (requestedFile != "/index.html")
{	file_length = 20; 
	statusCode = 200;
	
	fprintf(stdout, "The file exists, the file length is %d and status code is %d \n.", file_length, statusCode);
}
else 
{	file_length = 0; 
	statusCode = 404;
	fprintf(stdout, "The file does not exist, the file length is %d and status code is %d \n.", file_length, statusCode);	
}
}


int main(int argc, char **argv)
{	FILE *fin = fopen("house.json", "r");
	processData(fin, stdout);
	fclose(fin);
	exit(0);
}


