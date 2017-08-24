#include <CscNetLib/netSrv.h>
#include <CscNetLib/std.h>
#include <CscNetLib/iniFile.h>
#include <CscNetLib/isvalid.h>
#include <CscNetLib/logger.h>

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
	 		{	csc_log_printf(log, 1, "FATAL: invalid IP address \"%s", value);
	 			exit(1);
	 		}
	 		//else return value;
	 		
	 	}
		if(key == "Port")
		{	//Validating target Port to be within 1025 and 65535
	 		if (!csc_isValidRange_int(value, 1025, 65535, &Port))
	 		{	csc_log_printf(log, 1, "FATAL: invalid Port \"%d", Port);
	 			exit(1);
	 		}
	 		else return Port;
	 	}
		if(key == "Level")
		{	// Check the log level from configurations
	 		if (!csc_isValidRange_int(value, csc_log_TRACE, csc_log_FATAL, &LogLevel))
			{	csc_log_printf(log, csc_log_FATAL, "FATAL: invalid Log level \"%d", LogLevel);
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
	char *section;
	
 	// Read in the iniFile.
	iniFilePath = "config.ini";
	ini = csc_ini_new();
	errLineNo = csc_ini_read(ini, iniFilePath);
	
	// Create the logging object
	csc_log_t *log = csc_log_new("server.log", 2);
 
	if (errLineNo == -1)
	{   csc_log_printf(log,3, "Error opening ini file \"%s\"\n", iniFilePath);
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
	{	csc_log_printf(log,3, "FATAL:Failed to Create new netCli object! \"\n");
		exit(1);
	}
	IP ="127.0.0.1";
	int servConn = csc_srv_setAddr(ntp, "TCP", IP, Port, -1);
	if( servConn == 0)
	{	csc_log_printf(log,3 ,"FATAL: Invalid server endpoint parameters: \"%s\"\n", csc_srv_getErrMsg(ntp));
			exit(1);
	}
	else
	{	csc_log_printf(log, 7,  "INFO: Now serving on IP \"%s\" on port number\"%d\".\n", IP,Port);
		fprintf( stdout, "Now serving on IP\"%s\" on port number\"%d\".\n", IP,Port);
	}
	// For each successful connection.
	while ((fd = csc_srv_accept(ntp)) >= 0)    
	{   	const char *CliConnAccepted = csc_srv_acceptAddr(ntp);
		if( &CliConnAccepted == NULL)
		{	csc_log_printf(log,2,"The Client failed to connect!\"\n");
		}
		fprintf(stdout, "Connection from : \"%s\"\n", CliConnAccepted);
		FILE *tcpStream = fdopen(fd, "r");
		if(tcpStream == NULL)
		{	csc_log_printf(log,2,"Failed to open and read the stream!\"\n");
			fclose(tcpStream);
		}
		else 
		{	csc_fgetline(tcpStream,line,MaxLineLen);        
			fprintf(stdout, "Got line: \"%s\"\n", line);
			csc_log_printf(log,6,"INFO: Got line: \"%s\"\n", line);
			fprintf(tcpStream, "You said \"%s\"\n", line);
		}
		fclose(tcpStream);
	}
	csc_srv_free(ntp);
	exit(0);
}



 
        


