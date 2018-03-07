
#ifndef SUBMIT_MAZE_SOLN_H
#define SUBMIT_MAZE_SOLN_H

// ------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <iostream>			// cout, endl
#include <pthread.h>		// pthread_t

#include "Path.h"
#include "Maze.h"
#include "ProgramLog.h"
#include "Assignm3_Utils.h"

// ------------------------------------------------------------------------------------
//
// How to compile : 
//
// E.g. Assume your main program is called "MyProg.cpp", and you want your executable to be called "MyProg" ...
//
// g++ MyProg.cpp SubmitMazeSoln.o Path.o Maze.o -o MyProg -lpthread
//
// Note : the compiled versions of Path.o, Maze.o and SubmitMazeSoln.o are made available to you!
//		  In addition, the headers Path.h, Maze.h and SubmitMazeSoln.h are also made available to you!
//
// Your task is to concentrate on implementing a multi-threaded program to discover paths
// to barrier, danger area and a solution to travel from Start to End locations !!!
//
// ------------------------------------------------------------------------------------

namespace Assignm3
{

// ------------------------------------------------------------------------------------

	const std::string DefaultSolutionFilename = "mazesoln.txt";

	struct ThreadStatisticsInfo
	{
		pthread_t								threadID;

		VectorOfPointStructType					solutionPath;

		// A vector of ALL UNIQUE paths to barriers discovered by this thread
		VectorOfVectorOfPointStructType			submittedPathsToBarriers;

		// To check if student follows instructions. By right a thread should only discover
		// ONE path to danger area before being destroyed !
		VectorOfVectorOfPointStructType			submittedPathsToDangerAreas;

		// Below var will be incremented, if same path to barrier is submitted again and again
		// by this thread!
		int										noOfDuplicatedPathsToBarrier;

		// Below var will be incremented, if same path to danger area is submitted again and again
		// by this thread!
		int										noOfDuplicatedPathsToDangerArea;

		ThreadStatisticsInfo (void)
		{
			noOfDuplicatedPathsToBarrier	= 0;
			noOfDuplicatedPathsToDangerArea	= 0;

			solutionPath					= VectorOfPointStructType ();
			submittedPathsToBarriers		= VectorOfVectorOfPointStructType ();
			submittedPathsToDangerAreas		= VectorOfVectorOfPointStructType ();
		}

		~ThreadStatisticsInfo (void)
		{
			solutionPath.clear ();
			submittedPathsToBarriers.clear ();
			submittedPathsToDangerAreas.clear ();
		}

	};

	// type define a vector of 'ThreadStatisticsInfo' ...
	typedef std::vector<ThreadStatisticsInfo> VectorOfThreadStatisticsInfoType;

// ------------------------------------------------------------------------------------

	class SubmitMazeSoln
	{
		public:
			// Constructor ...
			SubmitMazeSoln (void);

			// Destructor ...			
			~SubmitMazeSoln (void);


			// Student should call this method to 'submit' an EXPLORED PATH to danger area 'X'
			// discovered by one of his/her generated thread!
			//
			// Parameter 1 : pthread_t => refers to the id of the thread that explored the path to discover the Danger Area
			//
			// Parameter 2 : VectorOfPointStructType => refers to a vector of (x, y) Points, containing the path explored by the thread
			// 				 						    from start location to Danger Area's location!
			//
			// Note : Parameter 2 must contain start location Point at the beginning of the vector (i.e. pathToDangerArea [0]) , AND 
			//		  the Danger Area Point at the end of the vector (i.e. pathToDangerArea [size-1] !!)
			//
			// returns 'true'  if the submitted discovery is UNIQUE
			// returns 'false' if : 
			// 		a) first Point in 'pathToDangerArea' does NOT contain the start location of the maze				OR
			//		b) submitted Danger Area's location is wrong														OR
			//		c) same path has been submitted before																OR
			//		d) points in the path are NOT CONNECTED, or CONTAINS OTHER DANGER / BARRIER or DUPLICATED POINTS    OR
			//      e) threadID is invalid																				OR
			//		f) 'pathToDangerArea' is empty
			//
			bool submitPathToDangerArea (pthread_t threadID, VectorOfPointStructType & pathToDangerArea);
			

			// Student should call this method to 'submit' an EXPLORED PATH to barrier '#'
			// discovered by one of his/her generated thread!
			//
			// Parameter 1 : pthread_t => refers to the id of the thread that explored the path to discover the Barrier
			//
			// Parameter 2 : VectorOfPointStructType => refers to a vector of (x, y) Points, containing the path explored by the thread
			// 				 						    from start location to Barrier's location!
			//
			// Note : Parameter 2 must contain start location Point at the beginning of the vector (i.e. pathToBarrier [0]) , AND 
			//		  the Barrier Point at the end of the vector (i.e. pathToBarrier [size-1] !!)
			//
			// returns 'true'  if the submitted discovery is UNIQUE
			// returns 'false' if : 
			// 		a) first Point in 'pathToBarrier' does NOT contain the start location of the maze					OR
			//		b) submitted Barrier's location is wrong															OR
			//		c) same path has been submitted before																OR
			//		d) points in the path are NOT CONNECTED, or CONTAINS OTHER DANGER / BARRIER or DUPLICATED POINTS
			//      e) threadID is invalid																				OR
			//		f) 'pathToBarrier' is empty
			//
			bool submitPathToBarrier (pthread_t threadID, VectorOfPointStructType & pathToBarrier);


			// Student should call this method to 'submit' a SOLUTION PATH to end location 'E'
			// discovered by one of his/her generated thread!
			//
			// Parameter 1 : pthread_t => refers to the id of the thread that explored the path to discover the solution
			//
			// Parameter 2 : VectorOfPointStructType => refers to a vector of (x, y) Points, containing the path explored by the thread
			// 				 						    from start location to end location!
			//
			// returns 'true'  if the submitted solution is VALID
			// returns 'false' if the submitted solution is INVALID ...
			//
			// A student's solution is valid if his submitted solution : 	
			//
			// i)   beginning of the 'pathToEndLocation' vector contains the Start Location
			// ii)  end of the 'pathToEndLocation' vector contains the End Location
			// iii) contains a series of 'connected' points from start to end location
			// iv)  there are no 'barriers' or 'danger' locations in his submitted path
			// v)   each point in his submitted path is unique (i.e. the path must not travel in loops/circles) ...
			// vi)  the BEGINNING point must be 'Start Location' and the END point must be 'End Location' ...
			
			bool submitSolutionPath (pthread_t threadID, VectorOfPointStructType & pathToEndLocation);

			
			// Once ALL explored paths and solution are submitted, student MUST call below method, 
			// to print the details of their solution on screen!
			//
			// Parameter 1 : 'studentName' => the full name of the student, (without spaces)
			//				 E.g. 'Tan Ah Beng' should be changed to 'TanAhBeng' (without spaces) !!
			//
			// Parameter 2 : 'studentID'   => the admin. or matric. no. of the student
			//				 E.g. '1234567'
			//
			void printSubmittedSolution (std::string studentName, std::string studentID, std::ostream &outputStream=std::cout);


			// Once ALL explored paths and solution are submitted, student MUST call below method, 
			// to save the details of their solution to a text file!
			//
			// Parameter 1 : 'studentName' => the full name of the student, (without spaces)
			//				 E.g. 'Tan Ah Beng' should be changed to 'TanAhBeng' (without spaces) !!
			//
			// Parameter 2 : 'studentID'   => the admin. or matric. no. of the student
			//				 E.g. '1234567'
			//
			void saveSubmittedSolution (std::string studentName, std::string studentID);

	
		private:
			Maze * _problemMaze;
			Maze * _studentSolnMaze;

			int _noOfDuplicatedPathsToBarrierSubmitted;
			int _noOfDuplicatedPathsToDangerAreaSubmitted;

			VectorOfPointStructType _finalSolutionPath;
			VectorOfVectorOfPointStructType _discoveredPathsToBarrier;
			VectorOfVectorOfPointStructType _discoveredPathsToDangerArea;
			VectorOfThreadStatisticsInfoType _generatedThreadsVector;

			int findThreadIndex (pthread_t threadID) const;

			// a helper function invoked by other functions (e.g. 'isSolutionValid ()') to check path validity ...
			bool checkPathValidity (VectorOfPointStructType aPath) const;

			bool isPathUnique (VectorOfPointStructType submittedNewPath, VectorOfVectorOfPointStructType discoveredPaths) const;

			void storeThreadInfo ( pthread_t threadID, VectorOfPointStructType & aPath, int typeOfPath );


	};	// end class SubmitMazeSoln ...

}	// end namespace Assignm3 ...

#endif // SUBMIT_MAZE_SOLN_H


// Statistics to report ...
//
// 1) No. of unique threads generated
//
// 2) No. of danger areas discovered
//
// 3) Count of duplicated paths (to danger areas) submitted
//
// E.g. of threadID ... '3079273328'



