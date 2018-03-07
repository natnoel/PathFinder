#include <pthread.h>
#include <algorithm>
#include "Path.h"
#include "Maze.h"
#include "SubmitMazeSoln.h"
#include "Assignm3_Utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace Assignm3;

const string MYNAME = "Tan Shi Terng Leon";
const string MYSTUDID = "4000602";
const int TOTAL_THREADS = 25;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//Global Maze Resources
const string threadName[] = {"Mickey", "Elmo", "Donald", "Goofey", "Minnie",
			"Daisy", "Bugs Bunny", "Kobe Bryant", "John", "Peter",
			"Paul", "Mary", "Simon", "Jeremy Lin", "Garfunkel",
			"Bear Grylls", "Mary Poppins", "Tom", "Jerry", "Dave",
			"Twinky Winky", "Dipsy", "La La", "Po", "Bart"};
pthread_t thread[TOTAL_THREADS];
pthread_t finalThread;
int threadIdx = 0;
SubmitMazeSoln *sms;
Maze *maze;
Maze *exploredMaze;
int elapseTime = 0;
int numOfDeadEndPaths = 0;
int barriersDiscovered = 0;
int dangerAreasDiscovered = 0;
VectorOfVectorOfPointStructType deadEndPaths;

Point north (Point);	//Returns the point north of a given point
Point south (Point);	//Returns the point south of a given point
Point east (Point);	//Returns the point east of a given point
Point west (Point);	//Returns the point west of a given point

//Returns true if 2 paths are similar false otherwise
bool arePathsSimilar (VectorOfPointStructType, VectorOfPointStructType);

//Returns true of a path reaches a dead end after traversing a point
bool meetsDeadEnd (Point, VectorOfPointStructType);

//Checks if 1st parameter(Point) exists in the second parameter(vector of Points)
bool beenExplored (Point, VectorOfPointStructType);

//Explores and updates a point in the maze
void explore (Point&, Point&, VectorOfPointStructType&,
		VectorOfPointStructType&, int);

//Checks if a path reaches a dead end
bool isDeadEnd (Point, VectorOfPointStructType);

void *traverse1 (void *);	//1st traversing algorithm
void *traverse2 (void *);	//2nd traversing algorithm
void *traverse3 (void *);	//3rd traversing algorithm

void *summary (void *);		//Prints the solution
void elapseTimeUpdate ();	//Displays progress after elapse time


//g++ main.cpp Path.o Maze.o SubmitMazeSoln.o -o a3 -lpthread
int main ()
{
	//Initializes SubmitMazeSoln and Maze object
	sms = new SubmitMazeSoln ();
	maze = new Maze ();
	sleep (1);
	
	//Loads and displays given maze
	maze->LoadMaze ();
	maze->DisplayMaze ();

	//Initializes and displays unexplored maze
	exploredMaze = new Maze (maze->getLength (), maze->getBreadth (), 
				maze->getStartLocation (), maze->getEndLocation ());
	exploredMaze->DisplayMaze ();

	sleep (1);

	//Summary thread waits for signal
	pthread_create (&finalThread, NULL, summary, NULL);

	//Traverse the maze
	pthread_create (&thread[threadIdx], NULL, traverse1, (void *) threadIdx);

	//Ends the program after summary thread ends
	pthread_join (finalThread, NULL);
}

Point north (Point p)
{
	Point a (p.x, p.y - 1);
	return a;
}

Point south (Point p)
{
	Point a (p.x, p.y + 1);
	return a;
}

Point west (Point p)
{
	Point a (p.x - 1, p.y);
	return a;
}

Point east (Point p)
{
	Point a (p.x + 1, p.y);
	return a;
}

bool arePathsSimilar (VectorOfPointStructType p1, VectorOfPointStructType p2)
{
	if (p1.size () != p2.size ())	//If paths are of different length
		return false;

	VectorOfPointStructType::iterator it1 = p1.begin ();
	VectorOfPointStructType::iterator it2 = p2.begin ();

	while (it1 != p1.end ())	//Checks each point of the path
	{
		if (*it1 != *it2)
			return false;

		++it1;
		++it2;
	}

	return true;
}

//Checks if moving forward (curr) will meet a discovered dead end
bool meetsDeadEnd (Point curr, VectorOfPointStructType path)
{
	path.push_back (curr);

	VectorOfVectorOfPointStructType::iterator it = deadEndPaths.begin ();
	bool exist = false;
	while (it != deadEndPaths.end () && exist == false)
	{
		if (arePathsSimilar (path, *it))
			exist = true;
		++it;
	}

	path.pop_back ();
	return exist;
}

//Checks if vector e contains Point p
bool beenExplored (Point p, VectorOfPointStructType e)
{
	VectorOfPointStructType::iterator it;
	it = find (e.begin (), e.end (), p);
	if (it == e.end ())
		return false;
	return true;
}

//Explores a point and updates
void explore (Point &current, Point &dir, VectorOfPointStructType &path, VectorOfPointStructType &exploredAreas, int tIdx)
{
	if (maze->IsThereBarrier (dir))		//If dir is a barrier
	{
		//Updates exploredMaze
		exploredMaze->updateMaze (dir, BARRIER_INT);

		//Inserts point to path and submits the path to barrier
		path.push_back (dir);
		sms->submitPathToBarrier (thread[tIdx], path);

		//Returns one position back (since it is a barrier)
		path.pop_back ();

		//Updates total barriers discovered
		barriersDiscovered++;

		//Pauses for one second to let user see update on screen
		sleep(1);
	}
	else if (maze->IsThereDanger (dir))	//If dir is a danger area
	{
		//Updates exploredMaze
		exploredMaze->updateMaze (dir, DANGER_INT);
		path.push_back (dir);	//Inserts dir into path
		current = dir;		//Assigns dir to current point

		//Submits path to Danger Area
		sms->submitPathToDangerArea (thread[tIdx], path);

		//Updates total Danger Areas Discovered
		dangerAreasDiscovered++;

		//Displays update one screen
		cout<<"Thread '"<<threadName[tIdx]<<"' stepped into DANGER at ";
		current.display ();
		cout<<"!"<<endl<<endl<<endl;
		elapseTimeUpdate ();
		cout<<endl;
		cout<<"Thread '"<<threadName[tIdx]<<"' is dead! Its sacrifice "
			<<"will not be in vain!"<<endl<<endl
			<<"Creating new thread '"<<threadName[threadIdx + 1]<<"'"
			<<endl<<endl;
		threadIdx = threadIdx + 1;

		//Creates a new thread in place of the exiting thread
		//Using a the same traversing algorithm as the exiting thread
		if (tIdx % 3 == 0)
			pthread_create (&thread[threadIdx], NULL, traverse1, (void *) threadIdx);
		else if (tIdx % 3 == 1)
			pthread_create (&thread[threadIdx], NULL, traverse2, (void *) threadIdx);
		else
			pthread_create (&thread[threadIdx], NULL, traverse3, (void *) threadIdx);

		sleep(3);	//Pauses for 3 seconds so user can see update
	}
	else if (dir == maze->getEndLocation ())//If dir is the end location
	{
		//Inserts dir into path
		path.push_back (dir);

		//Displays the path to end location graphically
		exploredMaze->ShowPathGraphically (path);

		//Displays message
		cout<<"Thread '"<<threadName[tIdx]<<"' just found a solution! "
			<<"Well done!!"<<endl;

		//Submits solution path
		sms->submitSolutionPath (thread[tIdx], path);
		
		cout<<"Finished Finding a SAFE PATH !!"<<endl
			<<"Printing submitted maze solution ..."<<endl;

		//Signals to wake up summary thread (finalThread)
		pthread_cond_signal (&cond);

		//Assigns dir to current point
		current = dir;
	}
	else	//Moves to Point dir
	{
		path.push_back (dir);		//Inserts dir into path
		exploredAreas.push_back (dir);	//Inserts dir to exploredAreas
		current = dir;			//Assigns dir to current
	}
}

//Checks if location is a dead end
bool isDeadEnd (Point curr, VectorOfPointStructType path)
{
	Path p;
	bool deadend = false;
	int barCount = 0;
	int explored = 0;

	//Number of surrounding barriers and danger areas
	if (exploredMaze->IsThereBarrier(north (curr)) || exploredMaze->IsThereDanger(north (curr)))
		barCount++;
	if (exploredMaze->IsThereBarrier (south (curr)) || exploredMaze->IsThereDanger(north (curr)))
		barCount++;
	if (exploredMaze->IsThereBarrier (east (curr)) || exploredMaze->IsThereDanger(north (curr)))
		barCount++;
	if (exploredMaze->IsThereBarrier (west (curr)) || exploredMaze->IsThereDanger(north (curr)))
		barCount++;

	//Number of surrounding path areas (points travelled in the current path)
	if (find (path.begin(), path.end(), north(curr)) != path.end())
		explored++;
	if (find (path.begin(), path.end(), south(curr)) != path.end())
		explored++;
	if (find (path.begin(), path.end(), east(curr)) != path.end())
		explored++;
	if (find (path.begin(), path.end(), west(curr)) != path.end())
		explored++;

	//If all 4 surroundings points are barriers, danger areas or path 
	//travelled
	if ((barCount + explored) == 4)
		deadend = true;

	return deadend;
}

//Depth-first traverse in order North South East West
void *traverse1 (void *threadArrayIdx)
{
	int tIdx = (int) threadArrayIdx;
	cout<<"Thread '"<<threadName[tIdx]<<"' has been created!!"<<endl;
	sleep (1);
	Point current = maze->getStartLocation ();
	Point above, below, left, right;
	VectorOfPointStructType path, exploredAreas;

	path.push_back (maze->getStartLocation ());
	exploredAreas.push_back (maze->getStartLocation ());

	while (!maze->IsThereDanger (current) && 
		(current != maze->getEndLocation ()))
	{
		above = north (current);
		below =  south (current);
		left = west (current);
		right = east (current);

		//Locks mutex
		pthread_mutex_lock (&mutex);

		if (!beenExplored (above, exploredAreas) &&
			!exploredMaze->IsThereDanger (above) &&
			!exploredMaze->IsThereBarrier (above) &&
			!meetsDeadEnd(above, path))
		{
			explore (current, above, path, exploredAreas, tIdx);
		}
		else if (!beenExplored (below, exploredAreas) && 
			!exploredMaze->IsThereDanger (below) && 
			!exploredMaze->IsThereBarrier (below) &&
			!meetsDeadEnd(below, path))
		{
			explore (current, below, path, exploredAreas, tIdx);
		}
		else if (!beenExplored (left, exploredAreas) && 
			!exploredMaze->IsThereDanger (left) && 
			!exploredMaze->IsThereBarrier (left) &&
			!meetsDeadEnd(left, path))
		{
			explore (current, left, path, exploredAreas, tIdx);
		}
		else if (!beenExplored (right, exploredAreas) && 
			!exploredMaze->IsThereDanger (right) && 
			!exploredMaze->IsThereBarrier (right) &&
			!meetsDeadEnd(right, path))
		{
			explore (current, right, path, exploredAreas, tIdx);
		}
		else //Deadend
		{
			if (isDeadEnd (current, path))
			{
				cout<<"Thread '"<<threadName[tIdx]
					<<"' hits a dead end near ";
				current.display ();
				cout<<"!"<<endl<<endl<<endl;
				numOfDeadEndPaths++;
				//Creates 2 new threads after the first thread
				//meets a dead end
				if (tIdx == 0 && numOfDeadEndPaths == 1)
				{
					++threadIdx;
					pthread_create (&thread[threadIdx], NULL,
						traverse2, (void *) threadIdx);
					++threadIdx;
					pthread_create (&thread[threadIdx], NULL,
						traverse3, (void *) threadIdx);
				}
				elapseTimeUpdate ();
				sleep (3);
				deadEndPaths.push_back (path);
			}

			path.pop_back ();
			current = *(path.end () - 1);
		}

		//Unlocks mutex
		pthread_mutex_unlock (&mutex);
	}

	pthread_exit (NULL);
}

//Depth-first traverse in order South West East North
void *traverse2 (void *threadArrayIdx)
{
	int tIdx = (int) threadArrayIdx;
	cout<<"Thread '"<<threadName[tIdx]<<"' has been created!!"<<endl;
	sleep (1);
	Point current = maze->getStartLocation ();
	Point above, below, left, right;
	VectorOfPointStructType path, exploredAreas;

	path.push_back (maze->getStartLocation ());
	exploredAreas.push_back (maze->getStartLocation ());

	while (!maze->IsThereDanger (current) && 
		(current != maze->getEndLocation ()))
	{
		above = north (current);
		below =  south (current);
		left = west (current);
		right = east (current);
		
		//Locks mutex
		pthread_mutex_lock (&mutex);

		if (!beenExplored (below, exploredAreas) &&
			!exploredMaze->IsThereDanger (below) &&
			!exploredMaze->IsThereBarrier (below) &&
			!meetsDeadEnd(below, path))
		{
			explore (current, below, path, exploredAreas, tIdx);
		}
		else if (!beenExplored (left, exploredAreas) && 
			!exploredMaze->IsThereDanger (left) && 
			!exploredMaze->IsThereBarrier (left) &&
			!meetsDeadEnd(left, path))
		{
			explore (current, left, path, exploredAreas, tIdx);
		}
		else if (!beenExplored (right, exploredAreas) && 
			!exploredMaze->IsThereDanger (right) && 
			!exploredMaze->IsThereBarrier (right) &&
			!meetsDeadEnd(right, path))
		{
			explore (current, right, path, exploredAreas, tIdx);
		}
		else if (!beenExplored (above, exploredAreas) && 
			!exploredMaze->IsThereDanger (above) && 
			!exploredMaze->IsThereBarrier (above) &&
			!meetsDeadEnd(above, path))
		{
			explore (current, above, path, exploredAreas, tIdx);
		}
		else //Deadend
		{
			if (isDeadEnd (current, path))
			{
				cout<<"Thread '"<<threadName[tIdx]
					<<"' hits a dead end near ";
				current.display ();
				cout<<"!"<<endl<<endl<<endl;
				numOfDeadEndPaths++;
				elapseTimeUpdate ();
				sleep (3);
				deadEndPaths.push_back (path);
			}

			path.pop_back ();
			current = *(path.end () - 1);
		}

		//Unlocks mutex
		pthread_mutex_unlock (&mutex);
	}

	pthread_exit (NULL);
}

//Depth-first traverse in order West East North South
void *traverse3 (void *threadArrayIdx)
{
	int tIdx = (int) threadArrayIdx;
	cout<<"Thread '"<<threadName[tIdx]<<"' has been created!!"<<endl;
	sleep (1);
	Point current = maze->getStartLocation ();
	Point above, below, left, right;
	VectorOfPointStructType path, exploredAreas;

	path.push_back (maze->getStartLocation ());
	exploredAreas.push_back (maze->getStartLocation ());

	while (!maze->IsThereDanger (current) && 
		(current != maze->getEndLocation ()))
	{
		above = north (current);
		below =  south (current);
		left = west (current);
		right = east (current);

		//Locks mutex
		pthread_mutex_lock (&mutex);

		if (!beenExplored (left, exploredAreas) &&
			!exploredMaze->IsThereDanger (left) &&
			!exploredMaze->IsThereBarrier (left) &&
			!meetsDeadEnd(left, path))
		{
			explore (current, left, path, exploredAreas, tIdx);
		}
		else if (!beenExplored (right, exploredAreas) && 
			!exploredMaze->IsThereDanger (right) && 
			!exploredMaze->IsThereBarrier (right) &&
			!meetsDeadEnd(right, path))
		{
			explore (current, right, path, exploredAreas, tIdx);
		}
		else if (!beenExplored (above, exploredAreas) && 
			!exploredMaze->IsThereDanger (above) && 
			!exploredMaze->IsThereBarrier (above) &&
			!meetsDeadEnd(above, path))
		{
			explore (current, above, path, exploredAreas, tIdx);
		}
		else if (!beenExplored (below, exploredAreas) && 
			!exploredMaze->IsThereDanger (below) && 
			!exploredMaze->IsThereBarrier (below) &&
			!meetsDeadEnd(below, path))
		{
			explore (current, below, path, exploredAreas, tIdx);
		}
		else //Deadend
		{
			if (isDeadEnd (current, path))
			{
				cout<<"Thread '"<<threadName[tIdx]
					<<"' hits a dead end near ";
				current.display ();
				cout<<"!"<<endl<<endl;
				numOfDeadEndPaths++;
				elapseTimeUpdate ();
				sleep (3);
				deadEndPaths.push_back (path);
			}

			path.pop_back ();
			current = *(path.end () - 1);
		}

		//Unlocks mutex
		pthread_mutex_unlock (&mutex);
	}

	pthread_exit (NULL);
}

//Prints waits for signal and prints and saves the solution
void *summary (void *t)
{
	pthread_cond_wait (&cond, &mutex);
	sms->printSubmittedSolution (MYNAME, MYSTUDID);
	sms->saveSubmittedSolution (MYNAME, MYSTUDID);
}

//Displays the update upon each dead end path or danger area discovered
void elapseTimeUpdate ()
{
	cout<<"==================================================="<<endl
	<<" Elapse Time : "<<elapseTime<<endl
	<<" Latest Update ..."<<endl
	<<"==================================================="<<endl<<endl
	<<"Dead End Paths Found		: "<<numOfDeadEndPaths<<endl
	<<"Barriers Discovered		: "<<barriersDiscovered<<endl
	<<"Danger Area Discovered	: "<<dangerAreasDiscovered<<endl<<endl;
	elapseTime++;
}


