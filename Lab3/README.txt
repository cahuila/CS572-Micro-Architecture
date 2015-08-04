Author:			Joaquin Aguirre
Class:			CS572, Fall 2013
Assignment:		Lab #3
Filename:		README

File manifest:		pipeSim.cpp - contains the source code for the general purpose register pipeline simulator program.

			Memory.cpp - contains the memory modual for the program

			stdafx.h - contains method, header and variable declarations 

			lab3c.s - contains the assembly source code for the simulator.

			results3.txt - contains the results for the lab3c.s test case.
	
			README.txt - contains the README information for the lab.

Compile
Instructions:		

			GPR Simulator:
				To compile the program. Load into Visual C++ and press the compile.
			
Operating
Instructions:

			GPR Simulator:
				To run the GPR program just start the executable. This would default to the file 
				located on the desktop. To run a custom file, execute under command prompt.


Design
Decisions:		I decided to create a 64 bit machine. Since it more efficient for a machine to split up 64 bits instructions, instead of 40 bit.
 			instructions. This allows for 32 bits for the memory address and also 32 bits for the opcode.
			The registers are 32 bits to allow for the storage of address locations. Also, since I was dealing with different instruction that took
			different types of arguments, I decided to create more than one instruction type. Even though the instructions took different arguemnts
			by keeping the arguments referenced at the same locations in the instruction, helped with not duplicating unecessary work.
			
Lessons
Learned:		One of the main things I learns, was how much the added buffers are REQUIRED for this machine to run correctly. With the forwarding was a little diffiult as well. You had to keep track of which variables were being used in the pipeline. With the nops being allowed after each branch helped with keeping track of the variables. 