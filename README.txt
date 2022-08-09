Author:		Ofer Greenberg
Date: 		May 31st, 2022
Updated on:	August 8th, 2022

Purpose:	The program takes in a matrix of characters and allows user to shift it, rotate it clockwise, 
            	counterclockwise and perform horizontal and vertical inversions.

Compile:	Compile file using g++

	        g++ program.cpp -o p


Run:		Pass input file-name as a command-line argument (i.e.	p test1.txt)
	    	Or 
	    	Run the program, and follow prompts using the input format below format

    	Once the program loads a matrix, user can type in the following commands:
        - rotate [degree in float]		Rotate clockwise
        - crotate [degree in float]		Rotate counterclockwise
        - vmirror				perform a vertical inversion
        - hmirror				perform a horizontal inversion 
	- up/down/left/right [unsigned int]	shift grid in multiples of 4 chars
        - exit					Exit program
        - help/ any other input			Show this menu on console


Input:	Both the file input and from stdin use the following format
	[rows] [columns]
	c00 c01 c02 … 
	c10 c11 c12 …
	… 
	…	

Files: 		program.pcc
		test[#].txt
