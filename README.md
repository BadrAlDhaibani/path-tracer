# path-tracer
creates nodes based on selected max nodes, and their distance (growth), within a selected environment from 1-5, then traces a path to the closest node of a selected point

author: Badr Al-Dhaibani

source files: display.c rrtMaker.c rrtTester.c
header files: display.h obstacles.h

instructions for compiling and running:
first, in the directory terminal, simply type, 'make' to compile and create an executable titled 'rrtTester', 	 		
then to run, type, './rrtTester growthAmount maximumNodes environmentNumber'
	
	growthAmount: a desired value from 5-100 inclusive
	maximumNodes: a desired value from 5-5000 inclusive
	environmentNumber: a desired value from 1-5 inclusive
	

