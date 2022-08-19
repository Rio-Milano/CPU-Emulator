	#include "CPU_Helper_Functions_.h"

	/*
	1 2 3 4
	Q W E R
	A S D F
	Z X C V
	*/


	uint8_t Get_Key_Pressed_(uint8_t* p_keypad_u8)
	{
		// Before each and every opcode is FDE
		//The keypad is memset to 0x00
		//This functon is called giving presed values into keypad
		//the opcode is ran

		#pragma region ROW_1
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		{
			p_keypad_u8[0] = 0x01;
			return 0x00;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		{
			p_keypad_u8[1] = 0x01;
			return 0x01;
		}


		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		{
			p_keypad_u8[2] = 0x01;
			return 0x02;
		}


		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
		{
			p_keypad_u8[3] = 0x01;
			return 0x03;
		}

		#pragma endregion 

		#pragma region ROW_2
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			p_keypad_u8[4] = 0x01;
			return 0x04;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			p_keypad_u8[5] = 0x01;
			return 0x05;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		{
			p_keypad_u8[6] = 0x01;
			return 0x06;
		}


		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		{
			p_keypad_u8[7] = 0x01;
			return 0x07;
		}

		#pragma endregion 

		#pragma region ROW_3
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			p_keypad_u8[8] = 0x01;
			return 0x08;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			p_keypad_u8[9] = 0x01;
			return 0x09;
		}


		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			p_keypad_u8[10] = 0x01;
			return 0x0A;
		}


		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
		{
			p_keypad_u8[11] = 0x01;
			return 0x0B;
		}

		#pragma endregion 

		#pragma region ROW_4
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		{
			p_keypad_u8[12] = 0x01;
			return 0x0C;
		}


		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
		{
			p_keypad_u8[13] = 0x01;
			return 0x0D;
		}


		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			p_keypad_u8[14] = 0x01;
			return 0x0E;
		}


		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
		{
			p_keypad_u8[15] = 0x01;
			return 0x0F;
		}
		#pragma endregion


		else
		{
			return 0xFF;
		}

	}


	
