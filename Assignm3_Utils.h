

#ifndef Assignm3_UTILS_H
#define Assignm3_UTILS_H

#include <vector>
#include <string>
#include <iostream>		// cout, endl
#include <sstream>		// ostringstream
#include <cstdlib>		// srand, rand

// ------------------------------------------------------------------------------------

    struct Point
    {
		int x;
		int y;

		Point ()				{	x = NULL;	y = NULL;	}
		Point (int x1, int y1)	{	x = x1;		y = y1;		}
		~Point (void)			{	}


		Point & operator= (const Point &p)
		{	x = p.x;	y = p.y;	return (*this);		}

		bool operator== (const Point &p)
		{	return ( (x == p.x) && (y == p.y) );		} 

		bool operator!= (const Point &p)
		{	return ( (x != p.x) || (y != p.y) );		} 

		// 2 points are 'connected' but 'different' if they : 
		// i)  share the same 'x' but adjacent 'y' values, OR
		// ii) share the same 'y' but adjacent 'x' values!!
		bool isConnected (Point &p)
		{
			return (	((x == p.x) && ( ((y-1) == p.y) || ((y+1) == p.y) )) || 
						((y == p.y) && ( ((x-1) == p.x) || ((x+1) == p.x) ))
				   );
		}

		void display (std::ostream &outputStream=std::cout)		
		{	outputStream << "[" << x << ", " << y << "]";	}
    };

// ------------------------------------------------------------------------------------

	// type define a vector of Point structs ...
	typedef std::vector<Point> VectorOfPointStructType;

	// type define a vector of VectorOfPointStructType ...
	typedef std::vector<VectorOfPointStructType> VectorOfVectorOfPointStructType;

// ------------------------------------------------------------------------------------

	static std::string IntToString (const int intValue)
	{
		std::ostringstream oss (std::ostringstream::out);
		oss << intValue;
		return ( oss.str() );
	
	}	// end intToString () ...

// ------------------------------------------------------------------------------------

	static int GenerateRandomInteger (const int lowerLimit, const int upperLimit)
	{
		time_t secs;
		time (&secs);
		std::srand ( (unsigned int) secs );
		return ( std::rand () % (upperLimit - lowerLimit + 1) + lowerLimit );

	}	// end GenerateRandomInteger () ...

// ------------------------------------------------------------------------------------

#endif // Assignm3_UTILS_H


