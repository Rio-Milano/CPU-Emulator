#ifndef VALMAC_H
#define VALMAC_H

//My Files
#include"CPU_Helper_Functions_.h"
#include"CPU_Memory_.h"

//Cpp pre-compiled headders 
#include<chrono>
#include<thread>
#include< iostream>
#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>

//macros for size deffinitions
#define  MAX_STACK_SIZE		(16)
#define  MAX_REGISTERS		(16)
#define  MAX_KEYPAD_SIZE	(16)

//macros for casting types 
#define CAST_8(x) (static_cast<uint8_t>(x))
#define CAST_FLT(x) (static_cast<float>(x))


//Memory Macros
#define _Memory_Instance_ CPU_Memory_::Get_Instance_Of_Memory()
#define _Memory_ CPU_Memory_::Get_Instance_Of_Memory().Get_Memory_()
#define _Memory_Sprtie_Addr_ CPU_Memory_::Get_Instance_Of_Memory().Get_Sprite_Memory_Addr()


//This class represents the Valmac CPU in an OOP model. It is also a singleton.

class CPU_Valmac_
{
public:
	~CPU_Valmac_() = default;

	//Gets the instance of the class
	static CPU_Valmac_& Get_Instance_()
	{
		static CPU_Valmac_ ins_;
		return ins_;
	}


	void Init();

	inline void Step_PC_(){	m_program_counter_u16 += 2;}//increments the PC by 2 to go to the next opcode 

	inline uint16_t Get_Opcode_()
	{
		//testing for an odd PC
		(m_program_counter_u16 & 0x0001) ? throw std::string("The Program Counter can not be an odd number") : void();

		//if the memory is outside the data space thow exeption ca 
		(m_program_counter_u16 < 0x0200) ?
			throw std::string("PC was set to before 0x0200 which out of range of data space!")
			: void();

		
		//opcodes are stored as little endian
		return	static_cast<uint16_t>(_Memory_[m_program_counter_u16]) << 8 |//get the first half of the opcode and shift to the left by 8 then or it with the seccond half of the opcode
				static_cast<uint16_t>(_Memory_[m_program_counter_u16 + 1]);
	}

	void Update_Timers_();

	void Update_Window_();

	void DE_();




	void FDE_()
	{
		//get the current time
		m_start_time_tp = std::chrono::high_resolution_clock::now();
	

		//while the program is running
		while (m_window_rnwin->isOpen())
		{
			//update the sound and delay timers
			Update_Timers_();

			//get window events
			Update_Window_();

			//if the cpu is not finished running code
			if (m_running_b)
			{
				//if <= 500 opcodes have been executed  and there is no delay
				if ((m_runtime_flt < 1.f) && (m_delay_timer_u8 == 0x00))
				{
					//clear the set values of the keypad
					memset(m_keypad_u8, 0x00, sizeof(m_keypad_u8));

					//set the pressed key
					Get_Key_Pressed_(m_keypad_u8);

					//FETCH: the opcode pointed to by the program counter
					 m_opcode_u16 = Get_Opcode_();

					//DECODE->EXECUTE the opcode by interpreting it through a seris of switch statements 
					DE_();
					

					m_runtime_flt += m_speed_flt;
					
				}
			}
			else
			{
				m_window_rnwin->close();
			}
		}
	}


private:
	CPU_Valmac_();

	//Boolean flag that shows if the FDE cycle is in progress
	bool m_running_b = true;

	//Hold the current fetched opcode
	uint16_t m_opcode_u16;



	/*
	SIMULATED MEMORY
	*/

	//Holds 16 u8 bit general purpouse registers
	uint8_t m_registers_u8 [MAX_REGISTERS];

	//16 level stack for subroutine calls
	uint16_t m_stack_u16 [MAX_STACK_SIZE]{};

	//stores the next free position in the stack
	uint8_t m_stack_pointer_u8 {0x00};

	//used to play sounds and make delays
	uint8_t m_delay_timer_u8{0x00};
	uint8_t m_sound_timer_u8{ 0x00 };

	//the gfx 2d array to hold binary coded pixels
	uint8_t  m_display_u8[32][64]{ {} };

	//array of keys (0-F) to hold 0x00 or 0x01 if key is pressed or key is not pressed 
	uint8_t m_keypad_u8[16]{};




	/*
	SIMULATED CONTROL UNIT
	*/

	//a special address register 
	uint16_t m_index_register_u16;

	//Stores the address of the current instruction
	uint16_t m_program_counter_u16{ 0x0200 };




	/*
	SIMULATED CLOCK
	*/

	//holds time between secconds.... used for time managment 
	std::chrono::steady_clock::time_point m_start_time_tp;
	std::chrono::steady_clock::time_point m_end_time_tp;

	//used to ensure only 500 opcodes get FDE every seccond 
	const double m_speed_flt { 1.f / 500.f };//we can only do 500 instructions per seccond 
	double m_runtime_flt { 0.f };//here we keep track of instryuctions executed and once = to 1 500 instructions have been done




	/*
	Other things
	*/

	//holds loaded audio file and sound obj to play buffer
	sf::Sound m_speaker_snd;
	sf::SoundBuffer m_audio_sndbf;


	//stores result of window polled event
	sf::Event m_event_ev;

	//window used fror visual output
	std::unique_ptr<sf::RenderWindow> m_window_rnwin;

	//window context data
	std::unique_ptr<sf::VideoMode> m_video_mode_vm;

	//window size
	sf::Vector2i m_window_size_v2i{640, 320};
	
	//single pool of rectangle instance 
	sf::RectangleShape m_pixel_rgs;

	

};

#endif