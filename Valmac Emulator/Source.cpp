#include"CPU_Valmac_.h"
#include"CPU_Memory_.h"

#include<Windows.h>

#include<iostream>


#define CPU_Valmac_ CPU_Valmac_::Get_Instance_()
#define CPU_Memory_ CPU_Memory_::Get_Instance_Of_Memory()

int main()
{
	try
	{
		//Initialise the registers and memory
		CPU_Valmac_.Init();

		//Load a rom into the memory
		CPU_Memory_.Load_Rom_Into_Memory();
		
		//Start the fetch decode execute cycle
		CPU_Valmac_.FDE_();
	}
	
	//if any exceptions are thrown then print them
	catch (std::string e)
	{
		std::cout << "Error: " << e << std::endl;
	}

	//eof
	return 0;
}