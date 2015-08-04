Author:			Joaquin Aguirre
Class:			CS572, Fall 2013
Assignment:		Lab #2
Filename:		README

File manifest:		gprSim.c - contains the source code for the general purpose register simulator program.

			palindrome.s - contains the assembly source code for the simulator.

			restuls2.txt - contains the test results of three four test cases. Also contains the file program sizes.
	
			README.txt - contains the README information for the lab.

Compile
Instructions:		

			GPR Simulator:
				To compile the program. Load into Visual C++ and press the compile.
			
Operating
Instructions:

			GPR Simulator:
				To run the GPR program just start the executable. This would default to the the palindrome file
				located on the desktop. To run a custom file, execute under command prompt.


Design
Decisions:		I decided to create a 64 bit machine. Since it more efficient for a machine to split up 64 bits instructions, instead of 40 bit.
 			instructions. This allows for 32 bits for the memory address and also 32 bits for the opcode.
			The registers are 32 bits to allow for the storage of address locations. Also, since I was dealing with different instruction that took
			different types of arguments, I decided to create more than one instruction type. Even though the instructions took different arguemnts
			by keeping the arguments referenced at the same locations in the instruction, helped with not duplicating unecessary work.
			
Lessons
Learned:		One of the main issues that I was having was parseing the file. Since there were a couple different variations to the insturcitons
			each instruciton needed to be parsed differently. This lead me to have to use more than one insturction format. 
			Also, there was a difference in the way my simulator was holding addresses and the way that the assembly code was handling addressing.
			So, I have to account for when an address is being added as bytes or bits. Since, I had to store bytes in an array, it was difficult
			because of the way I had initually structured my memory (in chunks of 64 bits). I didnt change the structure of my memory, I ended up
			having to calculate addresses manually with pointers to get the same effect. For the next lab, I am probably going to redesign my memory
			to be as low level as possible. This will definenlty help in the long run.
				
