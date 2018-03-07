
#ifndef MAZE_H
#define MAZE_H

// ------------------------------------------------------------------------------------

#include <iomanip>			// setw

#include <string>			// for string manipulation ...
#include <iostream>			// cout, endl
#include <fstream>			// for file i/o ...

#include "Assignm3_Utils.h"

// ------------------------------------------------------------------------------------

namespace Assignm3
{
	const char DANGER_CHAR		= 'X';
	const char BARRIER_CHAR		= '#';
	const char END_POS_CHAR		= 'E';
	const char START_POS_CHAR	= 'S';
	
	const int DANGER_INT			= -10;
	const int BARRIER_INT			= -11;
	const int END_POS_INT			= -22;
	const int START_POS_INT			= -33;
	const int UNINITIALIZED_VALUE	= -99;

	const std::string DefaultMazeFilename = "mazedata.txt";

/*
	const std::string DefaultMazeFilename = "High_Casualties.txt";
	const std::string DefaultMazeFilename = "Freedom.txt";
	const std::string DefaultMazeFilename = "My_Only_Way.txt";
	const std::string DefaultMazeFilename = "Horizontals.txt";
	const std::string DefaultMazeFilename = "Verticals.txt";
	const std::string DefaultMazeFilename = "Diagonals.txt";
	const std::string DefaultMazeFilename = "Horizontals1.txt";
	const std::string DefaultMazeFilename = "Verticals1.txt";
	const std::string DefaultMazeFilename = "Frustration.txt";
	const std::string DefaultMazeFilename = "Checkered.txt";
	const std::string DefaultMazeFilename = "Choices.txt";
	const std::string DefaultMazeFilename = "Checkered_Upsize.txt";
*/

	const std::string MAZE_LENGTH_KEYWORD = "Length : ";
	const std::string MAZE_BREADTH_KEYWORD = "Breadth : ";
	
	class Maze
	{
		public:
			// Constructor ...
			Maze (int length=0, int breadth=0);
			Maze (int length, int breadth, Point startLocation, Point endLocation);

			// Destructor ...			
			~Maze (void);
			
			void LoadMaze (const std::string filename=DefaultMazeFilename);
			void SaveMaze (const std::string filename=DefaultMazeFilename) const;

			VectorOfVectorOfPointStructType getPathVector (void) const;
			VectorOfPointStructType getShortestPath (void) const;
			
			int getLength  (void) const;
			int getBreadth (void) const;

			void setStartLocation (Point p);
			void setEndLocation   (Point p);

			Point getStartLocation (void) const;
			Point getEndLocation   (void) const;

			int getNoOfDangerChar  (void) const;
			int getNoOfBarrierChar (void) const;

			void updateMaze (Point p, int data);

			bool IsThereDanger  (int x, int y) const;
			bool IsThereBarrier (int x, int y) const;
			bool IsThereDanger  (Point aLocation) const;
			bool IsThereBarrier (Point aLocation) const;
			void AddNewPath (VectorOfPointStructType newPath);
	
			void DisplayInfo (std::ostream &outputStream=std::cout) const;
			void DisplayMaze (void) const;
			void DisplayMaze (int * mazeArray, std::ostream &outputStream=std::cout, int field_width=1) const;

			// display path as graphically via an array of '2D maze' data
			void ShowPathGraphically (VectorOfPointStructType & pointStructVector, std::ostream &outputStream=std::cout);
	
		private:
			int _length;
			int _breadth;
			Point _startLocation;
			Point _endLocation;
			
			// 1D array representing a 2D maze of chars ...
			int * _mazeArray;
			
			// A 'path' consists of a series of Point structs plotting
			// a course from _startLocation to _endLocation
			//
			// Below data structure is used to store all valid paths
			// discovered in _mazeArray
			VectorOfVectorOfPointStructType _pathVector;
		
			void InitMazeArray (void);
			
			void ReadLengthData (std::string aLine);
			void ReadBreadthData (std::string aLine);
			void ReadMazeData (std::string aLine, int & currRowIndex);
			void SaveMazeArray (int * mazeArray, std::ostream &outputStream) const;

		
		
	};	// end class Maze ...

}	// end namespace Assignm3 ...

#endif // MAZE_H

