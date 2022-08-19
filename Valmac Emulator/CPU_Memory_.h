
#include<string>

#ifndef M_MEMORY_H
#define M_MEMORY_H
 
#define MAX_MEMORY (4096) // The valmac cpu has 4k memory so we will use that as a macro here

class CPU_Memory_
{
private:
	/*
	Note: Although an accurate depiction of a Valmac Emmulator would store registers, the stack amongst 
	other things within the interpreter memory. Its not being done here due to it making the code more difficult to
	understand and would require extra error handeling. Needless to say it could be done with little effort
	*/

	//Privatising the constructor as this class is a singleton 
	CPU_Memory_() = default;

	//Defining the 4k memory
	uint8_t memory[MAX_MEMORY];

	//Defining an array to hold the addresses of sprites in memory
	uint16_t sprite_memory_addr[20];

public:
	~CPU_Memory_() = default;

	/*
	Getter Functions
	*/

	//Gets the one instance of the memory class
	static inline CPU_Memory_& Get_Instance_Of_Memory()
	{
		static CPU_Memory_ ins_;
		return ins_;
	}

	//Gets a pointer to the memory
	constexpr inline uint8_t* Get_Memory_()
	{
		return memory;
	}

	//Gets a pointer to the array of sprite addresses
	constexpr inline uint16_t* Get_Sprite_Memory_Addr()
	{
		return sprite_memory_addr;
	}

	//Gets the size of the memory
	constexpr inline int Get_Memory_Size()
	{
		return MAX_MEMORY;
	}

	/*
	Setter Functions
	*/

	//clears the memory
	inline void Reset_Memory_()
	{
		memset(memory, 0x00, MAX_MEMORY);
	}


	/*
	Utility Functions
	*/

	void Load_Rom_Into_Memory(std::string s_romName = "")
	{
		//Big-Endian formatting 

		//Array to hold opcodes
		uint16_t ROM_[] =
		{
		0x6AF0,
		0xFA18,

		//register E is reserved for left paddle pos Y
		0x6E0E,
		//register D is reserved for right paddle pos Y
		0x6D0E,


		//xpos for the ball
		0x6C0F,

		//ypos for the ball
		0x6B0F,

		// AXIS RULES when 1 increment axis when 0 decrement axis
		//Xdir 
		0x6901,
		//Ydir
		0x6A01,


		0x00E0,//clear the screen




		//check left paddle at top
		0x6000,
		0x4E02,
		0x121E,
		0x6101,
		0xE0A1,
		0x8E15,

		//check left paddle at bottom
		0x80E0,
		0x7005,
		0x4019,
		0x122E,
		0x6101,
		0x6204,
		0xE2A1,
		0x8E14,







		//check right paddle at top
		0x6000,
		0x4D02,
		0x123C,
		0x6101,
		0x6201,
		0xE2A1,
		0x8D15,

		//check right paddle at bottom 
		0x80D0,
		0x7005,
		0x4019,
		0x124C,
		0x0000,
		0x6105,
		0xE1A1,
		0x7D01,
		0x0000,

		//check is ball is colliding with bottom of the screen					
		0x3B1D,
		0x1258,
		0x3A01,
		0x1262,
		0x6A00,

		//check is ball is colliding with bottom of the screen					
		0x3B02,
		0x1262,
		0x3A00,
		0x1262,
		0x6A01,



		//check is ball is colliding with left of the screen					
		0x3C02,
		0x126E,
		0x3900,
		0x126E,
		0x6901,
		0x0000,


		//check is ball is colliding with left of the screen					
		0x3C3D,
		0x127A,
		0x4900,
		0x127A,
		0x6900,
		0x0000,


		//set collision flag in V6
		0x6600,//V0 = 0

		//jump to check collsion
		0x1286,

		//collision check subroutine
		0x5B00,//if VB and V1 are equal
		0x00EE,//exit
		0x6601,//set V6 to 1
		0x00EE,//exit

		//check collision with ball and left paddle
		0x3C02,
		0x12A8,
		0x80E0,

		0x227E,
		0x7001,

		0x227E,
		0x7001,

		0x227E,
		0x7001,

		0x227E,
		0x7001,

		0x227E,

		// if collision flag is not set then add points to right player
		0x3600,
		0x12A8,
		0x7801,

		//if right player has max points then end game
		0x4809,
		0x00FE,




		//check collision with the ball and the right paddle

		//reset collsiion flag
		0x6600,

		0x3C3D,
		0x12CC,
		0x80D0,

		0x227E,
		0x7001,

		0x227E,
		0x7001,

		0x227E,
		0x7001,

		0x227E,
		0x7001,

		0x227E,

		//if collision flag is not set then add points to left player
		0x3600,
		0x12CC,
		0x7701,

		//if the left paddle has max points then end game
		0x4709,
		0x00FE,


		//update ball X
				0x3900,
				0x12D6,
				0x6001,
				0x8C05,
				0x12D8,
				0x7C01,

				//update ball y
							0x3A00,
							0x12E0,
							0x6001,
							0x8B05,
							0x12E2,
							0x7B01,





							//draw all sprites 



							//left paddle
										0x600B,
										0xF029,
										0x6001,
										0xD0E5,

										//right paddle
													0x600B,//set VF to 4
													0xF029,//setting I to sp4
													0x603E,//setting xPOS to 0
													0xD0D5,

													//ball
																0x600C,//set VF to 7
																0xF029,//setting I to sp4
																0xDCB5,

																//scores

																			//left score
																			0xF729,//setting I to sp4
																			0x6017,//set VF to 7
																			0x6103,//set VF to 7
																			0xD015,

																			//right score
																			0xF829,//setting I to sp4
																			0x6022,//set VF to 7
																			0x6103,//set VF to 7
																			0xD015,

																			//arbitary walls
																						0x600D,
																						0xF029,
																						0x6000,
																						0x6100,
																						0xD012,
																						0x6008,
																						0xD012,
																						0x6010,
																						0xD012,
																						0x6018,
																						0xD012,
																						0x6020,
																						0xD012,
																						0x6028,
																						0xD012,
																						0x6030,
																						0xD012,
																						0x6038,
																						0xD012,


																						0x611E,
																						0x6000,
																						0xD012,
																						0x6008,
																						0xD012,
																						0x6010,
																						0xD012,
																						0x6018,
																						0xD012,
																						0x6020,
																						0xD012,
																						0x6028,
																						0xD012,
																						0x6030,
																						0xD012,
																						0x6038,
																						0xD012,

																						0x600E,
																						0xF029,
																						0x603E,
																						0x6102,
																						0xD015,
																						0x6107,
																						0xD015,
																						0x610C,
																						0xD015,
																						0x6111,
																						0xD015,
																						0x6116,
																						0xD015,
																						0x611B,
																						0xD013,

																						0x6000,
																						0x6102,
																						0xD015,
																						0x6107,
																						0xD015,
																						0x610C,
																						0xD015,
																						0x6111,
																						0xD015,
																						0x6116,
																						0xD015,
																						0x611B,
																						0xD013,

																						//dotted line
																									0x600A,
																									0xF029,
																									0x601C,
																									0x6103,
																									0xD014,
																									0x6107,
																									0xD015,
																									0x610D,
																									0xD015,
																									0x6113,
																									0xD015,
																									0x6119,
																									0xD014,

																									//render all drawn
																												0x00A0,

																												//jump to top to redo
																															0x1210,

																															//final exit code
																																		0x00FE



		};
		

		//If the rom is too big to load in memory then throw an exception
		uint16_t rom_size_ = sizeof(ROM_);
		if((rom_size_ + 0x0200) >= static_cast<uint16_t>(MAX_MEMORY))
			throw std::string("Rom is too large to load into memory") ;
		
		/*
		SPRITE LOADING
		*/

		//SPRITE: 0
		sprite_memory_addr[0] = 0x0000;
		memory[0] = 0b11111100;
		memory[1] = 0b10000100;
		memory[2] = 0b10000100;
		memory[3] = 0b10000100;
		memory[4] = 0b11111100;

		//SPRITE: 1
		sprite_memory_addr[1] = 0x0005;
		memory[5] = 0b00100000;
		memory[6] = 0b01100000;
		memory[7] = 0b00100000;
		memory[8] = 0b00100000;
		memory[9] = 0b11111000;

		//SPRITE: 2
		sprite_memory_addr[2] = 0x000A;
		memory[10] = 0b11111000;
		memory[11] = 0b00001000;
		memory[12] = 0b11111000;
		memory[13] = 0b10000000;
		memory[14] = 0b11111000;

		//SPRITE: 3
		sprite_memory_addr[3] = 0x000F;
		memory[15] = 0b11111000;
		memory[16] = 0b00001000;
		memory[17] = 0b11111000;
		memory[18] = 0b00001000;
		memory[19] = 0b11111000;

		//SPRITE: 4
		sprite_memory_addr[4] = 0x0014;
		memory[20] = 0b10001000;
		memory[21] = 0b10001000;
		memory[22] = 0b11111000;
		memory[23] = 0b00001000;
		memory[24] = 0b00001000;

		//SPRITE: 5
		sprite_memory_addr[5] = 0x0019;
		memory[25] = 0b11111000;
		memory[26] = 0b10000000;
		memory[27] = 0b11111000;
		memory[28] = 0b00001000;
		memory[29] = 0b11111000;

		//SPRITE: 6
		sprite_memory_addr[6] = 0x001E;
		memory[30] = 0b11111000;
		memory[31] = 0b10000000;
		memory[32] = 0b11111000;
		memory[33] = 0b10001000;
		memory[34] = 0b11111000;

		//SPRITE: 7
		sprite_memory_addr[7] = 0x0023;
		memory[35] = 0b11110000;
		memory[36] = 0b00010000;
		memory[37] = 0b00010000;
		memory[38] = 0b00010000;
		memory[39] = 0b00010000;

		//SPRITE: 8
		sprite_memory_addr[8] = 0x0028;
		memory[40] = 0b11111000;
		memory[41] = 0b10001000;
		memory[42] = 0b11111000;
		memory[43] = 0b10001000;
		memory[44] = 0b11111000;

		//SPRITE: 9
		sprite_memory_addr[9] = 0x002D;
		memory[45] = 0b11111000;
		memory[46] = 0b10001000;
		memory[47] = 0b11111000;
		memory[48] = 0b00001000;
		memory[49] = 0b11111000;



		//////////

		//SPRITE: DOTTED LINE
		sprite_memory_addr[10] = 0x0032;
		memory[50] = 0b00010000;
		memory[51] = 0b00000000;
		memory[52] = 0b00010000;
		memory[53] = 0b00000000;
		memory[54] = 0b00010000;

		//SPRITE: LEFT PADDLE //could load into V and shift for left and right
		sprite_memory_addr[11] = 0x0037;
		memory[55] = 0b10000000;
		memory[56] = 0b10000000;
		memory[57] = 0b10000000;
		memory[58] = 0b10000000;
		memory[59] = 0b10000000;


	

		//SPRITE: ball
		sprite_memory_addr[12] = 0x003C;
		memory[60] = 0b10000000;
		memory[61] = 0b00000000;
		memory[62] = 0b00000000;
		memory[63] = 0b00000000;
		memory[64] = 0b00000000;

		//wall
		sprite_memory_addr[13] = 0x0041;
		memory[65] = 0b11111111;
		memory[66] = 0b11111111;
		memory[67] = 0b00000000;
		memory[68] = 0b00000000;
		memory[69] = 0b00000000;


		//horizontal wall
		sprite_memory_addr[14] = 0x0046;
		memory[70] = 0b11000000;
		memory[71] = 0b11000000;
		memory[72] = 0b11000000;
		memory[73] = 0b11000000;
		memory[74] = 0b11000000;

//#include<iostream>

		/*std::cout << sizeof(ROM_) / << "\n";
		std::cin.ignore();
		std::cout << sizeof(ROM_) / 2;
		std::cin.ignore();*/

		//Loading rom into memory
		for (size_t i = 0; i < sizeof(ROM_) / 2; ++i)
		{
			//load first 8 bits of opcode into memory
			memory[0x0200 + 2*i] = static_cast<uint8_t>(ROM_ [i] >> 8);

			//load seccond 8 bits of opcode into memory
			memory[0x0200 + 2*i + 1] = static_cast<uint8_t>(ROM_[i]);
		}
	}
};

#endif