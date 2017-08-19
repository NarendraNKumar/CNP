
#include <stdlib.h>
#include <stdio.h>

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
	{	printf("%s,%s: \"%s\"\n", section, key, value);

		if(key == "IP")
		{	//Validating the Server IP 
	 		if (!csc_isValid_ipV4(value))csc_log_printf(log, 1, "invalid IP address %s", value);
	 		//return value;
	 	}
		if(key == "Port")
		{	//Validating target Port to be within 1025 and 65535
	 		if (!csc_isValidRange_int(value, 1025, 65535, &Port)) csc_log_printf(log, 1, "invalid Port %d", Port);
	 		return Port;
	 	}
		if(key == "Level")
		{	// Check the log level from configurations (2)
	 		if (!csc_isValidRange_int(value, csc_log_TRACE, csc_log_FATAL, &LogLevel))
			{	csc_log_printf(log, csc_log_FATAL, "invalid Log level %d", LogLevel);
			}
			//return LogLevel;
		}
	
        	free(value);
	}	
	else
	{	printf("%s,%s: (not found)\n", section, key);		
    	}
    	// Release log resources.
	csc_log_free(log);
	return 0;
}
int main()
{  
	// Create the iniFile object. 
	char *iniFilePath;
	csc_ini_t *ini;
	int errLineNo;
	char *section;
	int Port;
	char *IP;
 	// Read in the iniFile.
	iniFilePath = "config.ini";
	ini = csc_ini_new();
	errLineNo = csc_ini_read(ini, iniFilePath);
	// Create the logging object
	csc_log_t *log = csc_log_new("server.log", 2);
 
	if (errLineNo == -1)
	{   fprintf(stderr, "Error opening in ini file \"%s\"\n", iniFilePath);
	}
	else if (errLineNo > 0)
	{	fprintf(stderr, "Error at line number %d in ini file \"%s\"\n",
		errLineNo, iniFilePath);
	}
	else
	{	section = "Server"; 
		testLookup(ini,section,"IP");
		IP = "127.0.0.1";
		Port = testLookup(ini,section,"Port");
	 	section = "Logging";   
		testLookup(ini,section,"Level");
	} 
	csc_log_printf(log, 7,  "Now serving on IP \"%s\" on port number\"%d\".\n", IP,Port);
	fprintf( stdout, "Now serving on IP\"%s\" on port number\"%d\".\n", IP,Port);
	// Release log resources.
	csc_log_free(log);
    // Release file resources.   
    csc_ini_free(ini);
    exit(0);
}

    

