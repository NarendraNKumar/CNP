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
int main(int argc, char **argv)
{   	csc_srv_t *ntp = NULL;
    	int fd = -1;int Port,servConn;
	char IP, logLevel;
    	char line[MaxLineLen+1],CliConnAccepted;
 	// Create the iniFile object. 
	char *iniFilePath;
	csc_ini_t *ini;
	int errLineNo;
	char *section;
	//Defination of response resource members
	csc_jsonErr_t errNum;
	char requestedProt, rRequest, requestedFile,requestedHost,requestedConn;
	int file_length, statusCode;
	
 	// Read in the iniFile.
	iniFilePath = "config.ini";
	ini = csc_ini_new();
	errLineNo = csc_ini_read(ini, iniFilePath);
	
	// Create the logging object
	csc_log_t *log = csc_log_new("server.log", 2);
 
	if (errLineNo == -1)
	{   csc_log_printf(log,3, "Error opening in ini file \"%s\"\n", iniFilePath);
	}
	else if (errLineNo > 0)
	{	csc_log_printf(log,3 , "Error at line number %d in ini file \"%s\"\n",
		errLineNo, iniFilePath);
	}
	else
	{	section = "Server"; 
		testLookup(ini,section,"IP");
		Port = testLookup(ini,section,"Port");
	 	section = "Logging";   
		logLevel = testLookup(ini,section,"Level");
	} 
	// Create netSrv object.
	ntp = csc_srv_new();
	if( ntp == NULL)
	{	csc_log_printf(log,3, "Failed to Create new netCli object!\"\n");
		exit(1);
	}
	IP ="127.0.0.1";
	servConn = csc_srv_setAddr(ntp, "TCP", IP, Port, -1);
	if( servConn == 0)
	{	csc_log_printf(log,3 ,"Invalid server endpoint parameters: \"%s\"\n", csc_srv_getErrMsg(servConn));
		exit(1);
	}	
	

	// For each successful connection.
	while ((fd = csc_srv_accept(ntp)) >= 0)    
	{   	CliConnAccepted = csc_srv_acceptAddr(ntp);
		if( CliConnAccepted == NULL)
		{	csc_log_printf(log,2,"The Client failed to connect!\"\n");
		}
		fprintf(stdout, "Connection from \"%s\"\n", CliConnAccepted);
		FILE *tcpStream = fdopen(fd, "r+");
		if(tcpStream == NULL)
		{	csc_log_printf(log,2,"Failed to open and read the stream!\"\n");
			close(fd);
		}
		else 
		{	//Received client input stream
			csc_fgetline(tcpStream,line,MaxLineLen);
			//Print the client request on standard output        
			fprintf(stdout, "Received: \"%s\"\n", line);
			//Print into log file
			csc_log_printf(log,6,"Got line: \"%s\"\n", line);
			//Interprate the client request
			printf("Client requires the HEAD of /index.html using the HTTP 1.1 protocol\"\n");
			//Respond to the equest by processing JSON object stored in house.json file
			FILE *fin = fopen("house.json", "r");
			
			//Processing response from Json file
			// Read json data.
			csc_json_t *inData = csc_json_newParseFILE(fin);
	
			// Print out json object on standard output 
			fprintf(fin, "\n");
	
			//Split into sub-data
			requestedProt = csc_json_getStr(inData, "protocol", &errNum);
			rRequest = csc_json_getStr(inData, "request", &errNum);
			requestedFile = csc_json_getStr(inData, "path", &errNum);
			requestedHost = csc_json_getStr(inData, "host", &errNum);
			requestedConn = csc_json_getStr(inData, "connection", &errNum);
			csc_json_free(inData);
	
		 	// Check the existence of requested resourse
			if (requestedFile == "/index.html")
			{	//Prepare the response code and fle size on Success.
				file_length = 20; 
				statusCode = 200;				
			}	
			else 
			{	//Prepare the response code and fle size
				file_length = 0; 
				statusCode = 404;			
			}
	
			//Sending out json data 
			fprintf(tcpStream, fin,"\n");
	
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
	 		{	csc_log_printf(log, 1, "invalid IP address \"%s\"\n", value);
	 			exit(1);
	 		}
	 		//else return value;
	 		
	 	}
		if(key == "Port")
		{	//Validating target Port to be within 1025 and 65535
	 		if (!csc_isValidRange_int(value, 1025, 65535, &Port))
	 		{	csc_log_printf(log, 1, "invalid Port \"%d\"\n", Port);
	 			exit(1);
	 		}
	 		else return Port;
	 	}
		if(key == "Level")
		{	// Check the log level from configurations
	 		if (!csc_isValidRange_int(value, csc_log_TRACE, csc_log_FATAL, &LogLevel))
			{	csc_log_printf(log, csc_log_FATAL, "invalid Log level \"%d\"\n", LogLevel);
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


