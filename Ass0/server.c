
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
{   char *value = csc_ini_getAllocStr(ini, section, key);
    if (value != NULL)
    {   printf("%s,%s: \"%s\"\n", section, key, value);
    	validate_andLog(section, key, value);
        free(value);
        return valiable;
    }
    else
    {   printf("%s,%s: (not found)\n", section, key);
    return 0;
    }
}

void validate_andLog(char *section, char *key, char *val)
	
{	int Port,logLevel;
 	char *IP;
	if(key == "IP")
		IP = val;
	if(key == "Port")
		Port = val;
	if(key == "Level")
		logLevel = &val;
		
	// Create the logging object.
    	csc_log_t *log = csc_log_new("server.log", 2);
    	if (log == NULL)
        	return csc_FALSE;
        	
	//Validating the Server IP 
	 if (!csc_isValid_ipV4(IP))
		 csc_log_printf(log, 1, "invalid IP address %s", IP);
	 //Validating target Port to be within 1025 and 65535
	 else if (!csc_isValidRange_int(Port, 1025, 65535, &Port))
		 csc_log_printf(log, 1, "invalid Port %d", &Port);
	 // Check the log level from configurations (2)
	 else if (!csc_isValidRange_int(logLevel, csc_log_TRACE, csc_log_FATAL, &logLevel))
	{	csc_log_printf(log, csc_log_FATAL, "invalid Log level %d", &logLevel);
        }   
	csc_log_printf(log, 7,  "Now serving on IP \"%s\" on port number\"%d\".\n"
                       , IP,&Port
                       );
	fprintf( stdout
                       , "Now serving on IP\"%s\" on port number\"%d\".\n"
                       , IP,&Port
                       );
	// Release log resources.
	csc_log_free(log);
}

int main()
{  
	// Create the iniFile object. 
    char *iniFilePath;
    csc_ini_t *ini;
    int errLineNo;
    char *section;
 	// Read in the iniFile.
    iniFilePath = "config.ini";
    ini = csc_ini_new();
    errLineNo = csc_ini_read(ini, iniFilePath);
 
    if (errLineNo == -1)
    {   fprintf(stderr, "Error opening in ini file \"%s\"\n", iniFilePath);
    }
    else if (errLineNo > 0)
    {   fprintf(stderr, "Error at line number %d in ini file \"%s\"\n",
                        errLineNo, iniFilePath);
    }
    else
    {   section = "Server"; 
        testLookup(ini,section,"IP");
        testLookup(ini,section,"Port");
 
        section = "Logging";   
        testLookup(ini,section,"Level");
    } 
    // Release file resources.   
    csc_ini_free(ini);
    exit(0);
}

    

