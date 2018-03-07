
#ifndef PATH_H
#define PATH_H

// ------------------------------------------------------------------------------------

#include <iostream>			// cout, endl

#include "Assignm3_Utils.h"

// ------------------------------------------------------------------------------------

namespace Assignm3
{
	class Path
	{
		public:
			// Constructor ...
			Path (void);

			// Destructor ...			
			~Path (void);
			
			// returns true of a Point is found in a vector of Points ...
			bool isLocationInPath (Point & location, VectorOfPointStructType & pointStructVector) const;

			// display path as series of [x, y] coordinates
			void displayPath (VectorOfPointStructType & pointStructVector, 
							  std::ostream &outputStream=std::cout, 
							  int maxDataPerLine=8) const;

			// check if 2 paths are identical ...
			bool arePathsIdentical (VectorOfPointStructType & firstPath, VectorOfPointStructType & secondPath) const;

		private:
		
	};	// end class Path ...

}	// end namespace Assignm3 ...

#endif // PATH_H

