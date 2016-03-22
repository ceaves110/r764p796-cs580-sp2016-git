/*
Christopher Eaves
R764P796
Operating Systems
Program 2 The Collatz sequence with shared memory!
The collatz sequence can reduce any number down to 1 using the following formula
If x is odd then:    (x*3)+1
If x is even then:   x \ 2

Eventually this will reduce a number to 1



*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){    
	const char* name = "collatz"; //Name of the shared memory object
	int uInput = atoi(argv[1]); // This is the number the user enters
	int validInput = 0;  // This will be used to validate that the users entry is positive and less than 500000
	pid_t pid; // Process id
	int status;   //Status of the id  
	const int SIZE = 4096; // Size of shared memor object
	void *ptr; // Pointer to shared memory object
	int shm_fd;// Shared memory descriptor
	   
	
	
	
	while (validInput == 0) //If the user enters a invalid input it will ask them to enter again until a valid input is recieved
	{

		if(uInput < 0)    
		{          
			printf("Please enter a positive number that is greater than 0\n");      
			scanf("%d", &uInput);    
		}
	
		else if (uInput >= 500000)
		{
			printf("Please enter a positive number that is less than 500,000\n");      
			scanf("%d", &uInput); 
		}    
		else
		{
			validInput = 1;
		}
	}  
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); // Open shared memory object
	ftruncate(shm_fd, SIZE); //Cpmfiger the size of the object
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0); // Map the object
	pid = fork();     // Forks the process creating a child process
	
	if(pid < 0)  // If pid is negative then the child failed to be created   
	{      
		printf(" Child creation failed\n");      
		exit(-1);    
	}     
	else if(pid == 0)    //Child creation was a success so run the collatz sequence
	{                 
		shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
		ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);		
		if (uInput == 1) //if the user entered 1 then print the number
		{
			printf("%d \n",uInput);
		}
		while(uInput != 1) // If the input is not one run the collatz sequence to reduce it to 1
		{
			
			sprintf(ptr, "%8d", uInput);
				//Print to the shared memory object.
			ptr += snprintf(NULL, 0, "%8d", uInput); // Increase the pointer by the number of digits in the number
			if(uInput % 2 != 0) // If the number is odd then multiply it by 3 and add 1
			{          
				uInput = (uInput * 3) + 1; 
				
			}                 
			else if(uInput % 2 == 0)        //If the number is even then divide it by 2
			{          
				uInput = uInput / 2;
				
			}         
               
            
		}
		sprintf(ptr, "%8d", uInput); // Add the 1 at the end
		ptr++;
	}
	
	else    
	{           
		wait(&status);      //Have the parent wait until the child is complete
		printf("parent value:\n %s\n", (char*) ptr); // Display the parent value
		shm_unlink(name);
	}               
return 0;   
}