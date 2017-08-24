#include <stdio.h>
#include <stdlib.h>
#include <CscNetLib/netSrv.h>
#include <CscNetLib/std.h>
#include <CscNetLib/iniFile.h>
#include <CscNetLib/isvalid.h>
#include <CscNetLib/logger.h>
#include <CscNetLib/json.h>

#define MaxLineLen 255
#define MaxLineWords 2

//Splite the values to key and value
int testLookup(csc_ini_t *ini, char *section, char *key) 
{	int Port=0,LogLevel=0;
	char *value = csc_ini_getAllocStr(ini, section, key);
	// Create the logging object
	csc_log_t *log = csc_log_new("server.log", 2);
	//if (log == NULL)return csc_FALSE;
	
	if (value != NULL)
	{	if(key == "IP")
		{	//Validating the Server IP 
	 		if (!csc_isValid_ipV4(value))
	 		{	csc_log_printf(log, csc_log_FATAL, "FATAL: invalid IP address \"%s\"\n", value);
	 			exit(1);
	 		}
	 		//else return value;
	 		
	 	}
		if(key == "Port")
		{	//Validating target Port to be within 1025 and 65535
	 		if (!csc_isValidRange_int(value, 1025, 65535, &Port))
	 		{	csc_log_printf(log, csc_log_FATAL, "FATAL: invalid Port \"%d\"\n", Port);
	 			exit(1);
	 		}
	 		else return Port;
	 	}
		if(key == "Level")
		{	// Check the log level from configurations
	 		if (!csc_isValidRange_int(value, csc_log_TRACE, csc_log_FATAL, &LogLevel))
			{	csc_log_printf(log, csc_log_FATAL, "FATAL: invalid Log level \"%d\"\n", LogLevel);
			}
			else return LogLevel;
		}
	
        	free(value);
	}	
	else
	{	csc_log_printf(log, csc_log_FATAL,"\"%s,\"%s: (not found)\"\n", section, key);
		exit(1);		
    	}
    	// Release log resources.
	csc_log_free(log);
	return 0;
}     



int main(int argc, char **argv)
{   	csc_srv_t *ntp = NULL;
    	int fd = -1;int Port;
	const char *IP, *iniFilePath;
    	char line[MaxLineLen+1], logLevel;
 	// Create the iniFile object.
	csc_ini_t *ini;
	int errLineNo;
	char *section;const char *Server_IP;
	//Defination of response resource members
	csc_jsonErr_t errNum;
	const char *requestedProt, *rRequest, *requestedFile,*requestedHost,*requestedConn;
	int file_length, statusCode;
	
 	// Read in the iniFile.
	iniFilePath = "config.ini";
	ini = csc_ini_new();
	errLineNo = csc_ini_read(ini, iniFilePath);
	
	// Create the logging object
	csc_log_t *log = csc_log_new("server.log", csc_log_NOTICE);
 
	if (errLineNo == -1)
	{   csc_log_printf(log,3, "Error opening in ini file \"%s\"\n", iniFilePath);
	}
	else if (errLineNo > 0)
	{	csc_log_printf(log,3 , "Error at line number %d in ini file \"%s\"\n",
		errLineNo, iniFilePath);
	}
	else
	{	section = "Server"; 
		IP = csc_ini_getStr(ini, "ServerAddress", "IP");
		Port = testLookup(ini,section,"Port");
	 	section = "Logging";   
		logLevel = testLookup(ini,section,"Level");
	} 
	// Create netSrv object.
	ntp = csc_srv_new();
	if( ntp == NULL)
	{	csc_log_printf(log,csc_log_FATAL, "FATAL: Failed to Create new netCli object!\"\n");
		exit(1);
	}
	//stpcpy(IP,Server_IP);
	IP ="127.0.0.1";
	int servConn = csc_srv_setAddr(ntp, "TCP", IP, Port, -1);
	if( servConn == 0)
	{	csc_log_printf(log,csc_log_ERROR ,"ERROR: Invalid server endpoint parameters: \"%s\"\n", csc_srv_getErrMsg(ntp));
		exit(1);
	}	
	else
	{	csc_log_printf(log, csc_log_NOTICE,  " INFO: Now serving on IP \"%s\" on port number\"%d\".\n", IP,Port);
		fprintf( stdout, "Now serving on IP\"%s\" on port number\"%d\".\n", IP,Port);
	}

	// For each successful connection.
	while ((fd = csc_srv_accept(ntp)) >= 0)    
	{   	const char *CliConnAccepted = csc_srv_acceptAddr(ntp);
		if( &CliConnAccepted == NULL)
		{	csc_log_printf(log,csc_log_ERROR,"ERROR: The Client failed to connect!\"\n");
		}
		fprintf(stdout, "Connection from \"%s\"\n", CliConnAccepted);
		FILE *tcpStream = fdopen(fd, "r+");
		if(tcpStream == NULL)
		{	csc_log_printf(log,csc_log_ERROR,"ERROR: Failed to open and read the stream!\"\n");
			fclose(tcpStream);
		}
		else 
		{	//Received client input stream
			csc_fgetline(tcpStream,line,MaxLineLen);
			//Print the client request on standard output        
			fprintf(stdout, "Received: \"%s\"\n", line);
			//Print into log file
			csc_log_printf(log,6,"INFO: Got line: \"%s\"\n", line);
			//Interprate the client request
			if(line!="")
			{	fprintf(stdout,"Client requires \"%s\"\n", line);
				//Respond to the equest by processing JSON object stored in house.json file
				fprintf(tcpStream,"From Server: You requested \"%s\"\n", line);
			}
			FILE *fin = fopen("house.json", "r");
			
			//Processing response from Json file
			// Read json data.
			csc_json_t *inData = csc_json_newParseFILE(fin);
	
			//Split into sub-data
			requestedProt = csc_json_getStr(inData, "protocol", &errNum);
			rRequest = csc_json_getStr(inData, "request", &errNum);
			requestedFile = csc_json_getStr(inData, "path", &errNum);
			requestedHost = csc_json_getStr(inData, "host", &errNum);
			requestedConn = csc_json_getStr(inData, "connection", &errNum);
			csc_json_free(inData);
			printf("Json Data read successfuly!\n\n");
			fprintf(stdout, "\tprotocol: \"%s\"\n", requestedProt);
			fprintf(stdout, "\trequest: \"%s\"\n", rRequest);
			fprintf(stdout, "\tpath: \"%s\"\n", requestedFile);
			fprintf(stdout, "\thost: \"%s\"\n", requestedHost);
			fprintf(stdout, "\tconnection: \"%s\"\n\n", requestedConn);
	
		 	// Check the existence of requested resource
			if (requestedFile != "")
			{	//Prepare the response code and fle size on Success.
				fprintf(stdout, "Found: \"%s\"\n", requestedFile);
				file_length = 20; 
				statusCode = 200;				
			}	
			else 
			{	//Prepare the response code and file size
				file_length = 0; 
				statusCode = 404;
				fprintf(stdout, "requested file not available\"\n");			
			}
	
			//Sending out json data 
			fprintf(tcpStream, requestedFile,225);
	
			//Interprate the response on standard output
			fprintf(stdout, 
				"The status code is %d and the file size is %d .\"\n", 
				statusCode,file_length
				);
			//Send to client the interpreted response
			fprintf(tcpStream, 
				"The status code is %d and the file size is %d .\"\n", 
				statusCode,file_length
				);
	
			fclose(fin);
		}
		fclose(tcpStream);
	}
		
	csc_srv_free(ntp);
	exit(0);
}



