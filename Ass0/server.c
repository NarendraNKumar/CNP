
#include <stdlib.h>
#include <stdio.h>

#include <CscNetLib/std.h>
#include <CscNetLib/iniFile.h>
#include <CscNetLib/isvalid.h>
#include <CscNetLib/logger.h>

#define MaxLineLen 255
#define MaxLineWords 2


int main(int argc, char **argv)
{   int retVal,Port,logLevel;
 char IP;

// Create the iniFile object.
    csc_ini_t *ini = csc_ini_new();

 // Create the logging object.
    csc_log_t *log = csc_log_new("server.log", 2);
    if (log == NULL)
        return csc_FALSE;

// Read in the iniFile.
    retVal = csc_ini_read(ini, "config.ini");
    if (retVal < 0)
    {   csc_log_printf(log, csc_log_FATAL
               , "Error: Failed to open configuration file config.ini!\n"
               );
        exit(1);
    }
    else if (retVal > 0)
    {   csc_log_printf(log, 3
               , "Error reading line %d of configuration file config.ini!\n"
               , retVal
               );
        exit(1);
    }   

// Get property requests from the command line.
    char line[MaxLineLen+1];
    char *words[MaxLineWords+1];
 	while (words != 0)
	{   int nWords = csc_param_quote(words, line, MaxLineWords+1);
        const char *value = csc_ini_getStr(ini, words[0], words[1]);
        if (value == NULL)
        {   csc_log_printf(log, 3
             , "There is no value for key \"%s\" in section \"%s\".\n"
             , words[1], words[0]
               );
        }
        else
         {   csc_log_printf(log, 6
                 , "The key for \"%s\" in section \"%s\" is \"%s\".\n"
                 , words[1], words[0], value
                 );
		  if(words[1] == "IP")
			  IP = words[1];
		  if(words[1] == "Port")
			  Port = words[1];
		  if(words[1] == "logLevel")
			  logLevel = words[1];
         }
	
	 //Validating the Server IP 
	 if (!csc_isValid_ipV4(IP))
		 csc_log_printf(log, csc_log_FATAL, "invalid IP address %s", IP);
	 //Validating target Port to be within 1025 and 65535
	 else if (!csc_isValidRange_int(Port, 1025, 65535, &Port))
		 csc_log_printf(log, csc_log_FATAL, "invalid Port %d", Port);
	 // Check the log level from configurations (2)
	 else if (!csc_isValidRange_int(logLevel, csc_log_TRACE, csc_log_FATAL, &logLevel))
        csc_log_printf(log, csc_log_FATAL, "invalid Log level %d", logLevel);
	 else
		{   fprintf( stdout
                       , "The server is ready to serve\"%s\" on\"%d\".\n"
                       , &IP,&Port
                       );
		} 
	}
 	// Release log resources.
    csc_log_free(log);
 	// Release file resources.
    csc_ini_free(ini);
    exit(0);
}


