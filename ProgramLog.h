
#ifndef PROGRAM_LOG_H
#define PROGRAM_LOG_H

#include <string>
#include <iostream>

#include <errno.h>

// ------------------------------------------------------------------------------------

	static void WriteLogMessage (std::ostream & outputStream, std::string message)
	{
		if (outputStream.good())
			outputStream << message << std::endl;
		else
		{
			std::cout << std::endl;
			std::cout << "WriteLogMessage Error! outputStream.good() fails! Re-direct Log Msg to screen!" << std::endl;
			std::cout << "Log Msg : " << message << std::endl;
			std::cout << std::endl;
		}

	}	// end WriteLogMessage () ...

// ------------------------------------------------------------------------------------

#endif	// PROGRAM_LOG_H ...

