#include "CPU_Valmac_.h"

template<class T> 

void LOG(T& x)
{
	std::cout << x << std::endl;
}



CPU_Valmac_::CPU_Valmac_() : m_pixel_rgs({ 10.f, 10.f })
{
	//init the video mode for the window
	m_video_mode_vm = std::make_unique<sf::VideoMode>(m_window_size_v2i.x, m_window_size_v2i.y);

	//create a new window
	m_window_rnwin = std::make_unique<sf::RenderWindow>(*m_video_mode_vm, "Valmac Emmulator:	By A0040053");

	//load the beep sound into a buffer
	m_audio_sndbf.loadFromFile("BLEEP.wav");

	//set the sound buffer
	m_speaker_snd.setBuffer(m_audio_sndbf);
}



void CPU_Valmac_::Init()
{
	//the FDE has been reset
	m_running_b = true;

	//reset PC
	m_program_counter_u16 = 0x0200;//0x2000;  // Program counter starts at 0x200
	
	//reset current OPCODE
	m_opcode_u16 = 0x0000;      // Reset current opcode	
	
	//reset I
	m_index_register_u16 = 0x0000;      // Reset index register
	
	//reset SP
	m_stack_pointer_u8 = 0x00;      // Reset stack pointer

	//reset display
	memset(m_display_u8, 0x00, 32 * 64);
	
	//reset all registers 0-F
	memset(m_registers_u8, 0x000, MAX_STACK_SIZE);

	//reset stack
	memset(m_stack_u16, 0x0000, MAX_REGISTERS);

	//reset KEYPAD
	memset(m_keypad_u8, 0x00, MAX_KEYPAD_SIZE);

	//unload memory
	_Memory_Instance_.Reset_Memory_();

	//reset timers
	m_delay_timer_u8 = 0x00;
	m_sound_timer_u8 = 0x00;

	//clear run time
	m_runtime_flt = 0.0f;
}

void CPU_Valmac_::Update_Timers_()
{
	//get the time now
	m_end_time_tp = std::chrono::high_resolution_clock::now();

	//subtract the time now from the last time the time was taken
	std::chrono::duration<float> duration_ = m_end_time_tp - m_start_time_tp;

	static int l_time_passed_i{0};

	//if the time passed is >= 1 then 1 seccond has surely passed
	if (duration_.count() >= 1.f)
	{
		//if there is no delay
		if (m_delay_timer_u8 == 0x00)
		{
			//if there is still time on the sound timer
			if (m_sound_timer_u8 != 0x00)
				//make a bleep
				m_speaker_snd.play();

			//if the sound timer is > 60
			if (m_sound_timer_u8 > 0x3C)
				//subtract 60 from the sound timer
				m_sound_timer_u8 -= 0x3C;
			
			//if the sound timer is <= 60
			else if(m_sound_timer_u8 > 0x00)
				//set the sound timer to 0
				m_sound_timer_u8 = 0x00;

			//cleae the run time for the next 500 FDE cycles
			m_runtime_flt = 0.0f;

			//reset clock
		}
		//if the delay is > 60
		if (m_delay_timer_u8 > 0x3C)
			//subtract 60 from the delay
			m_delay_timer_u8 -= 0x3C;
		//if the delay is <= 60
		else if(m_delay_timer_u8 > 0x00)
			//set the delay timer = to zero
			m_delay_timer_u8 = 0x00;

		m_start_time_tp = m_end_time_tp;
	}
}

void CPU_Valmac_::Update_Window_()
{
	//get events from window to get input and titlebar events
	while (m_window_rnwin->pollEvent(m_event_ev))
		//if event is closing window then close the window
		if (m_event_ev.type == sf::Event::Closed) m_window_rnwin->close();

}

void CPU_Valmac_::DE_()
{
	//get first 4 bits of opcode and decode it
	switch (m_opcode_u16 & 0xF000) {

	case 0x0000: {

		//first 4 bits are 0 so take last 4 bits and decode
		switch (m_opcode_u16 & 0x0FFF)
		{

			//0x0000
		case 0x0000: {
			/*
				No operation. (can be for Self-modified code)
			*/
			LOG("Opcode: 0x0000: No operation.");
			Step_PC_();
			break;
		}

				   //0x00A0
		case 0x00A0: {
			/*
				Custom opcode: Used to display all drawn sprites
			*/
			LOG("Opcode: 0x000A: All drawn sprites rendered to the screen");
			m_window_rnwin->display();
			Step_PC_();
			break;
		}

				   //0x00E0
		case 0x00E0: {
			/*
				Clears the screen. Note: no hardware calls in the emulator.
			*/

			LOG("Opcode: 0x00E0: Clear the gfx");
			//reset pixels
			memset(m_display_u8, 0x00, sizeof(m_display_u8));
			//clear window
			m_window_rnwin->clear();

			//go to next opcode
			Step_PC_();

			break;
		}

				   //0x00EE
		case 0x00EE: {
			/*
				Returns from a subroutine.
			*/

			m_stack_pointer_u8 == 0 ? throw std::string("Opcode: 0x00EE: Cant return from no call") : void();

			LOG("Opcode: 0x00EE: return from subroutine");

			//decrease stack pointer
			m_stack_pointer_u8 -= 1;

			//set program counter = to the top of stack
			m_program_counter_u16 = m_stack_u16[m_stack_pointer_u8];

			//go to next instruction
			Step_PC_();
			break;
		}


				   //		   //0x0005
				   //case 0x0005: {
				   //	/*
				   //		Custom opcode: resets the set memory when using paramaters
				   //	*/

				   //	m_stack_pointer_u8 == 0 ? throw std::string("Opcode: 0x0005: Cant return from no call") : void();


				   //	LOG("Opcode: 0x0005: return from augmented subroutine ");

				   //	//decrease the stack pointer
				   //	m_stack_pointer_u8 -= 1;

				   //	//set the program counter to the call of the function
				   //	m_program_counter_u16 = (m_stack_u16[m_stack_pointer_u8] - 2);

				   //	//get the opcode of the call of the function
				   //	uint16_t temp_opcode_ = Get_Opcode_();

				   //	//set the function paramaters of the function back to their default ones set by the user
				   //	_Memory_[temp_opcode_] = _Memory_[m_program_counter_u16 + 4];
				   //	_Memory_[temp_opcode_ + 1] = _Memory_[m_program_counter_u16 + 5];


				   //	//set the pc to beyond the function call, params and default set
				   //	Step_PC_();
				   //	Step_PC_();
				   //	Step_PC_();
				   //	break;
				   //}

				   //0x00FE
		case 0x00FE: {
			/*
			(optional – some systems used hard reset)
			*/

			LOG("Opcode: 0x00FE: Program exited");

			//cpu is not running anymore
			m_running_b = false;

			break;
		}

				   //0x0NNN OR Ox0NNN + params and reset state
		default: {
			/*
			* FOR 0x0NNN with no reset state#

			 Calls machine code routine at address NNN. Not necessary for most ROMs. Note: no ROMs in the emulator.

			* FOR 0x0NNN with reset state
			here I have decided to slightly tweak the opcode of the opcode 0NNN
			now it will only execute if VF is set to zero

			I have done this to extend the functionality of the cpu as now I can have an
			additional subset of opcodes what pass parameters and change opcode values in memory i.e. self modified code that changes memory
			*/

			//0x0NNN /w no reset state

			//if no zero state by checking if VF is 0
			/*if (_Memory_[MAX_MEMORY - 1] == 0x00)
			{*/
			//if address in opcode > 0x0800
			if ((m_opcode_u16 & 0x0FFF) > 0x0800)
			{
				//if the stack is full then throw error
				(m_stack_pointer_u8 == MAX_STACK_SIZE) ? throw std::string("Stack Overflow") : void();

				//put program counter in stack
				m_stack_u16[m_stack_pointer_u8] = m_program_counter_u16;

				//increment stack pointer
				m_stack_pointer_u8 += 1;

				//make program counter = to address 0x0NNN
				m_program_counter_u16 = m_opcode_u16 & 0x0FFF;

				LOG("Opcode: 0x0NNN /w no reset state: Going to address 0x0NNN");
				break;
			}
			else
			{
				LOG("Can not go to subroutine before 0x0800");
			}

			break;
		}
			   ////0x0NNN /w reset state
			   //else if (_Memory_[MAX_MEMORY - 1] == 0x01)
			   //{
			   //	//step pc to point to parama
			   //	Step_PC_();
			   //	//get params from next opcode
			   //	uint16_t seccond_opcode_ = Get_Opcode_();

			   //	//SECCOND OPCODE FORMAT: 0x0XYZ 

			   //	//make a referance to the pc as will need to go back to it after modified function memory
			   //	uint16_t temp_PC_ = m_program_counter_u16;

			   //	//overwrite the PC to the address of the function
			   //	m_program_counter_u16 = m_opcode_u16;

			   //	//get the opcode of the function
			   //	uint16_t temp_opcode_ = Get_Opcode_();

			   //	//or the function with the params of copy of function after moving params into place
			   //	temp_opcode_ |= (seccond_opcode_ >> 4);

			   //	//set the function to the augmented funcion
			   //	_Memory_[m_opcode_u16] = static_cast<uint8_t>(temp_opcode_ >> 8);
			   //	_Memory_[m_opcode_u16 + 1] = static_cast<uint8_t>(temp_opcode_);

			   //	//check if the stack is full
			   //	(m_stack_pointer_u8 == MAX_STACK_SIZE) ? throw std::string("Stack Overflow") : void();

			   //	LOG("Opcode: 0x0NNN /w  reset state: Going to address 0x0NNN");

			   //	//put PC temp onto stack
			   //	m_stack_u16[m_stack_pointer_u8] = temp_PC_;

			   //	//increment the stack pointer
			   //	m_stack_pointer_u8 += 1;

			   //	//set the program counter to the address of the function
			   //	m_program_counter_u16 = m_opcode_u16;

			   //	break;
			   //}
			   //else
			   //{
			   //	throw "Operand not recigonised";
			   //	Step_PC_();
			   //}

			   //break;
		}



		break;
	}

			   //1NNN
	case 0x1000: {

		/*
			Jumps to address NNN.
		*/


		LOG("Opcode: 0x1NNN: PC changed to NNN");


		//set the PC to NNN
		m_program_counter_u16 = (m_opcode_u16 & 0x0FFF);

		break;
	}


			   //0x2NNN
	case 0x2000: {
		LOG("Opcode: 0x2NNN: calling simple function");

		//if the stack is full then throw error
		(m_stack_pointer_u8 == MAX_STACK_SIZE) ? throw std::string("Stack Overflow") : void();

		//put program counter in stack
		m_stack_u16[m_stack_pointer_u8] = m_program_counter_u16;

		//increment stack pointer
		m_stack_pointer_u8 += 1;

		//make program counter = to address NNN
		m_program_counter_u16 = m_opcode_u16 & 0x0FFF;

		break;
	}

			   //3XNN
	case 0x3000: {
		/*
			Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
		*/

		//if the value in VX is == to the 8 bit value in opcode
		if (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] == (m_opcode_u16 & 0x00FF))
		{
			LOG("Opcode: 0x3XNN: Next Instruction skipped as VX was == to NN");

			//skip next instruction if register value is same as NN
			Step_PC_();
		}
		else
			LOG("Opcode: 0x3XNN: Next Instruction not skipped as VX was != to NN");

		//go to next instruction reguardless 
		Step_PC_();
		break;
	}

			   //4XNN
	case 0x4000: {

		/*
		Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
		*/

		//if the value in VX is = to the 8 bit value found in opcode then step the PC
		if (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] != (m_opcode_u16 & 0x00FF))
		{
			LOG("Opcode: 0x4XNN: Next Instruction skipped as VX was != to NN");
			Step_PC_();

		}
		else
			LOG("Opcode: 0x3XNN: Next Instruction not skipped as VX was == to NN");

		//step PC reguardless 
		Step_PC_();

		break;
	}

			   //5XY0
	case 0x5000: {
		/*
			Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
		*/

		//if vx == xy then step pc
		if (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] == m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4])
		{
			LOG("Opcode: 0x5XY0: VX == VY so PC skipped");

			//step pc
			Step_PC_();
		}
		else
		{
			LOG("Opcode: 0x5XY0: VX != VY so PC not skipped");
		}

		Step_PC_();
		break;
	}

	//6XNN
	case 0x6000: {

		/*
			Sets VX to NN. Adjusted if VX is 15 as cant set V15
		*/
		//if ((m_opcode_u16 & 0x0F00) >> 8 != 15)
		//{

			LOG("Opcode: 0x6XNN: VX set to NN");

			//setting VX to NN
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = (m_opcode_u16 & 0x00FF);
		/*}*/
		//else
		//{
		//	//reserving the very very last memory location for a variable holding true or false generally
		//	//not a good approach but sould be fine for now

		//	LOG("Opcode: 0x6XNN: Memory[MAX_MEMORY-1] set to NN (Do this at your own risk, should only use when ROM is below limit and using subroutine params)");
		//	_Memory_[MAX_MEMORY - 1] = (m_opcode_u16 & 0x00FF);
		//}



		//step PC
		Step_PC_();

		break;
	}

			   //7XNN
	case 0x7000: {

		/*
			Adds NN to VX. (Carry flag is not changed)
		*/

		LOG("Opcode: 0x7XNN: VX += NN");

		//vx += nn
		m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] += (m_opcode_u16 & 0x00FF);

		//step pc
		Step_PC_();

		break;
	}

			   // 8XYN
	case 0x8000: {

		//get the last 4 bits of opcode and decode it
		switch (m_opcode_u16 & 0x000F)
		{
			//0x0000
		case 0x0000: {
			/*
			Sets VX to the value of VY
			*/
			LOG("Opcode: 0x8XY0: Sets VX = VY");

			//sets vx= vy
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4];

			//increment PC
			Step_PC_();
			break;
		}

				   //0x8XY1
		case 0x0001: {
			/*
			Sets VX to VX or VY. (Bitwise OR operation)
			*/

			LOG("Opcode: 0x8XY1: Sets VX = VX OR VY");

			//Set vx = vx or vy
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] | m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4]);
			Step_PC_();
			break;
		}

				   //0x8XY2
		case 0x0002: {
			/*
			Sets VX to VX xor VY. (Bitwise AND operation)
			*/

			LOG("Opcode: 0x8XY2: Sets VX = VX AND VY");

			//setting VX = VX AND YX
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] & m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4]);

			//increment PC
			Step_PC_();
			break;
		}

				   //0x8XY3
		case 0x0003: {
			/*
			Sets VX to VX xor VY. (Bitwise AND operation)
			*/

			LOG("Opcode: 0x8XY3: Sets VX = VX AND VY");

			//setting VX = VX XOR VY
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] ^ m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4]);

			Step_PC_();
			break;
		}

				   //0x8XY4
		case 0x0004: {
			/*
				Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
			*/

			LOG("Opcode: 0x8XY4: Sets Vx += VY  takes into account overflow");

			//if vx + vx > the max variable size
			if ((m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] + m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4]) > 255)
				m_registers_u8[0xF] = 0x01;//REGISTER F IS SET TO 1 to set carry to true
			else
				m_registers_u8[0xF] = 0x00;//REGISTER F IS SET TO 0 to set carry t0 false

			//REGISTER X = REGISTER X + REGISTER Y
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] += m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4];

			//step pc
			Step_PC_();

			break;
		}

				   //0x8XY5
		case 0x0005: {
			/*
				VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
			*/

			LOG("Opcode: 0x8XY5: Sets Vx -= VY  takes into account underflow");

			//if VY is greater than VX then the result will be negative 
			if (m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4] > m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8])
				m_registers_u8[0xF] = 0x00;//vf set to zero when there is borrow
			else
				m_registers_u8[0xF] = 0x01;//vf set to 1 when no borrow

			//subtracting VY from VX
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] -= m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4];

			//increment the PC
			Step_PC_();

			break;
		}

		//0x8X06
		case 0x0006: {
			/*
				Shift register VX right, bit 0 goes into register VF
			*/
			LOG("Opcode: 0x8XY6: Sets Vx >>= 1  lsb goes into VF");

			//put bit zero in VF
			m_registers_u8[0xF] = m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] & 0x01;

			//VX set to
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] >> 1;

			//increment PC
			Step_PC_();

			break;
		}

				   //0x8XY7
		case 0x0007: {
			/*
				Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
			*/

			LOG("Opcode: 0x8XY7: Sets Vx = Vy - Vx takes into underflow overflow");

			//if vx greater then vy
			if (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] > m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4])
				m_registers_u8[0xF] = 0;//there will be borrow so set vf to 0
			else
				m_registers_u8[0xF] = 1;//there will not be borrow so set vf to 1

			//set vx to vy - vx
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4] - m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8];

			//incremented PC
			Step_PC_();

			break;
		}

				   //8XY8
		case 0x0008: {
			/*
				Skips the next instruction if VX not equals VY. (Usually the next instruction is a jump to skip a code block)
			*/

			//if vx == xy then step pc
			if (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] != m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4])
			{
				LOG("Opcode: 0x8XY8: VX != VY so PC skipped");

				//step pc
				Step_PC_();
			}
			else
			{
				LOG("Opcode: 0x8XY8: VX == VY so PC not skipped");
			}

			Step_PC_();
			break;
		}

				   //0x8X0E
		case 0x000E: {
			/*
				Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
			*/

			LOG("Opcode: 0x8X0E: Sets Vx <<= 1 puts msb in vf ");

			//get the most significant bit of VX and store in VF
			m_registers_u8[0xF] = m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] >> 7;

			//put left shifted vx in vx
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] << 1;

			//increment PC
			Step_PC_();

			break;
		}
		default: {
			throw std::string("Operand not recigonised");
			Step_PC_();
			break;

		}

			   break;
		}

		//0x9XY0
	case 0x9000: {
		/*
			Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
		*/

		//if Vx != Vy then
		if (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] != m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4])
		{
			LOG("Opcode: 0x9XY0: Vx != Vy so PC skipped");
			Step_PC_();
		}
		else
			LOG("Opcode: 0x9XY0: Vx == Vy so PC not skipped");

		//increment PC reguardles
		Step_PC_();

		break;
	}

			   //0xANNN
	case 0xA000: {
		/*
			Sets I to the address NNN.
		*/

		LOG("Opcode: 0xANNN: I == NNN");

		//set I = to NNN
		m_index_register_u16 = (m_opcode_u16 & 0x0FFF);

		//step pc
		Step_PC_();
		break;
	}


			   //0xBNNN
	case 0xB000: {

		LOG("Opcode: 0xBNNN: PC = NNN + V0");

		//set pc = NNN + V0
		m_program_counter_u16 = ((m_opcode_u16 & 0x0FFF) + m_registers_u8[0]);

		break;
	}

			   //CXNN
	case 0xC000: {
		/*
			Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
		*/

		//generate a random number
		static uint8_t random_number_ = (rand() % 256);

		//set Vx = random(0, 255) & NN
		m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = (random_number_ & (m_opcode_u16 & 0x00FF));

		LOG("Opcode: 0xCXNN: Vx = random(0, 255) & NN");

		//increments PC
		Step_PC_();
		break;
	}

			   //oxDXYN
	case 0xD000: {
		/*
		Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels.
		Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the
		execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset
		when the sprite is drawn, and to 0 if that doesn’t happen. Sprites stored in memory at location in index register (I),
		maximum 8bits wide. Wraps around the screen. If when drawn, clears a pixel, register VF is set to 1 otherwise it is
		zero. All drawing is XOR drawing (e.g. it toggles the screen pixels)
		*/

		LOG("Opcode: 0xDXYN: Draws sprite at location [X,Y], sprite set by I");


		//resets collision to 0 each call
		m_registers_u8[0xF] = 0x00;

		//loop through all rows of pixel
		for (uint8_t row = 0; row < (m_opcode_u16 & 0x000F); ++row)
		{
			//loop through all columns of pixel
			for (uint8_t column = 0; column < 8; ++column)
			{
				//if row of pixel if off screen then continue
				if ((m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4] + CAST_8(row) >= 32))
					continue;

				//if column of pixel if off screen then continue
				if (m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] + CAST_8(column) >= 64)
					continue;

				//if wanting to draw onto screen
				if ((((_Memory_[m_index_register_u16 + CAST_8(row)] << column) >> 7) & 0b00000001) == 0x01)
				{
					//if same location on display is already drawn too
					if (m_display_u8[m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4] + CAST_8(row)][m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] + CAST_8(column)] == 0x01)
						m_registers_u8[0xF] = 0x01;//set collision flag

					//xor pixel into screen
					m_display_u8[m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4] + CAST_8(row)][m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] + CAST_8(column)] ^= 0x01;

					//set pesudo pixel position on window
					m_pixel_rgs.setPosition(CAST_FLT(m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] + CAST_8(column)) * 10.f, CAST_FLT((m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4] + CAST_8(row)) * 10.f));

					//if pixel is set then color is white
					if (m_display_u8[m_registers_u8[(m_opcode_u16 & 0x00F0) >> 4] + CAST_8(row)][m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] + CAST_8(column)] == 0x01)
						m_pixel_rgs.setFillColor(sf::Color::White);
					else
						//else color is not set so color is black
						m_pixel_rgs.setFillColor(sf::Color::Black);

					//draw the pixel onto the screen
					m_window_rnwin->draw(m_pixel_rgs);
				}
			}
		}
		//increment the PC
		Step_PC_();
		break;
	}


	case 0xE000: {
		//get last 8 bits of opcode and decode it
		switch (m_opcode_u16 & 0x00FF)
		{
			//0xEX9E
		case 0x009E: {
			/*
				Skips the next instruction if the key stored in VX is pressed.
				(Usually the next instruction is a jump to skip a code block). The key is a key number, see the
				chip-8 documentation.
			*/
			LOG("Opcode: 0xEX9E: if key in Vx is pressed then skip next instruction");


			//if key stored in Vx is pressed
			if (m_keypad_u8[m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8]] == 0x01)
				//skip next instruction
				Step_PC_();

			//increment the PC
			Step_PC_();

			break;
		}

				   //0xEXA1
		case 0x00A1: {
			/*
				Skips the next instruction if the key stored in VX isn't pressed.
				(Usually the next instruction is a jump to skip a code block) The key is a key number,
				see the chip-8 documentation.
			*/

			LOG("Opcode: 0xEXA1: if key in Vx isnt pressed then skip next instruction");

			//if key in Vx is not pressed then skip the next instruction
			if (m_keypad_u8[m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8]] == 0x00)
				Step_PC_();

			//increment the PC
			Step_PC_();

			break;
		}

		default: {
			throw std::string("Operand not recigonised");
			break;
		}
		}
		break;
	}


	case 0xF000: {
		//get last 8 bits of opcode and decode 
		switch (m_opcode_u16 & 0x00FF)
		{
			//0xFX07
		case 0x0007: {
			/*
				Sets VX to the value of the delay timer.
			*/

			LOG("Opcode: 0xFX07: Sets VX to the value of the delay timer.");

			//setting vx = to value in delay timer
			m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = m_delay_timer_u8;

			//increment pc
			Step_PC_();
			break;
		}

				   //0xFX0A
		case 0x000A: {
			/*
			A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
			*/


			LOG("Opcode: 0xFX0A: A key press is awaited");


			//until a keypressed value is set to 1 then it will not pass this instruction 
			if (Get_Key_Pressed_(m_keypad_u8) != 0xFF)
			{
				//put key pressed in Vx
				m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] = Get_Key_Pressed_(m_keypad_u8);

				//increment PC
				Step_PC_();
			}

			break;
		}

				   //0xFX15
		case 0x0015: {
			/*
			Sets the delay timer to VX.
			*/

			LOG("Opcode: 0xFX15: Sets the delay timer to VX.");

			//puttin vx into delay timer
			m_delay_timer_u8 = m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8];

			//step pc
			Step_PC_();

			break;
		}

				   //0xFX18
		case 0x0018: {
			/*
				Sets the sound timer to VX.
			*/

			LOG("Opcode: 0xFX18: Sound timer set to VX.");

			//putting vx in sound timer
			m_sound_timer_u8 = m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8];

			//increments pc
			Step_PC_();

			break;
		}


				   //0xFX1E
		case 0x001E: {
			/*
			Adds VX to I. VF is not affected. Most interpreters' FX1E instructions do not affect VF,
			with one exception, the interpreter for the Commodore Amiga sets VF to 1 when there is a range
			overflow (I+VX>0xFFF), and to 0 when there isn’t.
			*/

			LOG("Opcode: 0xFX1E: I += Vx.");


			//setting index register += Vx
			m_index_register_u16 += m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8];

			//increments pc
			Step_PC_();

			break;
		}

				   //0xFX29
		case 0x0029: {
			/*
			Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal)
			are represented by a 4x5 font.
			*/

			LOG("Opcode: 0xFX29: I = SpriteAddres[Vx]");

			//sets I to sprite address Vx
			m_index_register_u16 = _Memory_Sprtie_Addr_[m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8]];

			//increments pc
			Step_PC_();

			break;
		}

				   //0xFX33
		case 0x0033: {
			/*
			Stores the binary-coded decimal representation of VX, with the most significant of three
			digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2.
			(In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I,
			the tens digit at location I+1, and the ones digit at location I+2.). Doesn’t change I.
			*/

			LOG("Opcode: 0xFX33: Storing binary coded decimal at I");

			//hundreds go into Mem[I]
			_Memory_[m_index_register_u16] = static_cast<uint8_t>(m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] / 100);

			//tens go into Mem[I + 1]
			_Memory_[m_index_register_u16 + 1] = static_cast<uint8_t>(static_cast<uint8_t>(m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] / 10) % 10);

			//units go into Mem[I+2]
			_Memory_[m_index_register_u16 + 2] = static_cast<uint8_t>(m_registers_u8[(m_opcode_u16 & 0x0F00) >> 8] % 10);

			//increment PC
			Step_PC_();
			break;
		}

				   //0xFX55
		case 0x0055: {
			/*
			Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for
			each value written, but I itself is left unmodified. I could be modified in some systems e.g. I = I + r + 1.
			*/

			LOG("Opcode: 0xF55: Stores V0->VX in memory starting from I");

			//looping through memory addresses I->I+i and setting memory to V[i]
			for (uint8_t i = 0; i <= ((m_opcode_u16 & 0x0F00) >> 8); i++)
			{
				//setting memory to V[i] from I to I->I+i
				_Memory_[m_index_register_u16 + i] = m_registers_u8[i];
			}

			//increment PC
			Step_PC_();

			break;
		}

				   //0xFX65
		case 0x0065: {
			/*
			Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is
			increased by 1 for each value written, but I itself is left unmodified. I could be modified in some systems.
			(Note: I is a terrible name for a variable)
			*/

			LOG("Opcode: 0xF65: Taking X amount of data from memory at I into V0->VX");


			//loop through  0 <= registers <= X 
			for (uint8_t i = 0; i <= ((m_opcode_u16 & 0x0F00) >> 8); i++)
			{
				//set that register to the memory pointed to by the index register coppy
				m_registers_u8[i] = _Memory_[m_index_register_u16 + i];
			}
			m_registers_u8;

			//increment PC
			Step_PC_();

			break;
		}


		default: {
			throw std::string("Operand not recigonised");
			break;
		};

		}
		break;
	}

	default: {
		throw std::string("Operand not recigonised");
		Step_PC_();
		break;
	}
	}
	}





}