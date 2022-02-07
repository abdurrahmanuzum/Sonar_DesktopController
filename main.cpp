#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <iomanip>
#include <string>

#include "TCPClient.h"
#include "sonar_gui.h"
#include "globals.h"


const SDL_Rect GUI_Box = { 768, 0, GUI_WIDTH, GUI_HEIGHT };









/*
	Function geteHardwareInput

	Takes keystrokes from keyboard and mause wheel event and writes to buffer


	--------output buffer--------

	[7][6]  [5][4]  [3][2]  [1][0]
	  |      |  |     |      |   |            
	  |		 |	|     |		 |	 ------------> ___ ___ ___ ___
	  |		 |	|	  |		 |					|	|	|	|
	  |		 |	|	  |		 |					|	|	|	---> w 
	  |		 |	|	  v 	 |					|	|	-------> s 
	  |		 |	|	speed	 |					|	-----------> a 
	  |		 |	v			 |					---------------> d 
	  |		 | *sonar cw	 |
	  |		 |				 |---------------> ___ ___ ___ ___ 	
	  |		 v								    |   |   |   |
	  |		*sonar ccw							|	|	|	---> q [test]
	  |											|	|	-------> e [test]						
	  v											|   -----------> [unused]
	  sonar scan speed							---------------> [unused]



	  0th byte is responsible for basic control keyboard inputs
	  1th byte is currently unused, can be used for more complex keyboard inputs
	  2th and 3th bytes control speed of the vehicle
	  4th byte controls if sonar scan direction is forced to be cw
	  5th byte controls if sonar scan direction is forced to be ccw
	  *4th and 5th bytes are 0 while LCTRL is not pressed, 1 while pressed, and F accordingly to the mouse wheel spin direction, if spinning.
	  6th and 7th bytes are responsible to set sonar scan speed. This can be reduced to one byte if needed. No great resolution required.


	  Note that one hexadecimal digit is 4 bits, however, in order to be able to store it as a char, 8 bits are used. Since there are no 4 bit data type. ( I guess )

	  Controls:
		W, A, S, D for basic movement
		Mouse wheel up/down for speed control
		Hold LSHIFT to lock current angle
		Hold LSHIFT + mouse wheel up/down to manually set angle

		*Hold LCTRL + mouse wheel up/down to zoom [handled by Sonar::setzoom]

*/
void getHardwareInput2( SDL_Event& e, std::string& buffer, GUI& gui )
{
	static int	keystroke	= 0;
	static int	speed		= 0;
	static int	shift		= 0;
	int			force_ccw	= 0;
	int			force_cw	= 0;
	bool		changed		= false;



//==================================================================
//|						Key Press Events                           |
//==================================================================
	if ( e.type == SDL_KEYDOWN )
	{
		switch( e.key.keysym.sym )
		{
			case SDLK_w:	keystroke |= 0b00000001;	break;
			case SDLK_s:	keystroke |= 0b00000010;	break;
			case SDLK_a:	keystroke |= 0b00000100;	break;
			case SDLK_d:	keystroke |= 0b00001000;	break;
				
			case SDLK_LSHIFT: shift = 0x1; break;
		}
		
		changed = true;
	}
	else if ( e.type == SDL_KEYUP )
	{
		switch( e.key.keysym.sym )
		{
			case SDLK_w:	keystroke &= ~0b00000001;	break;
			case SDLK_s:	keystroke &= ~0b00000010;	break;
			case SDLK_a:	keystroke &= ~0b00000100;	break;
			case SDLK_d:	keystroke &= ~0b00001000;	break;

			case SDLK_LSHIFT: shift = 0x0; break;
		}

		changed = true;
	}



//==================================================================
//|						Mouse Wheel Events                         |
//==================================================================
	if ( shift && e.type == SDL_MOUSEWHEEL )
	{
		if		( e.wheel.y > 0 )	{ force_ccw = 0xF; }
		else if ( e.wheel.y < 0 )	{ force_cw  = 0xF; }

		changed = true;
	}
	else if ( e.type == SDL_MOUSEWHEEL )
	{
		if		( e.wheel.y > 0 ) { speed += 25; }
		else if ( e.wheel.y < 0 ) { speed -= 25; }
		
		if		( speed > 255 ) { speed = 255; }
		else if ( speed < 0 )	{ speed = 0; }

		changed = true;
	}




//==================================================================
//|						    Update				                   |
//==================================================================
	if ( changed )
	{
		buffer.erase( buffer.begin(), buffer.end() );

		uint32_t output = 0;

		//push scan speed
		//---//

		//push enforced rotation
		output |= force_ccw | shift;

		output <<= 4;
		output |= force_cw | shift;

		output <<= 8;
		output |= speed;

		output <<= 8;
		output |= keystroke;


		char holder[9];
		_itoa( output, holder, 16 );
		buffer = holder;

		gui.updateTextBox( "speed", "Speed: " + std::to_string( (int)speed ), true );
	}
}





void handleHardwareInput( SDL_Event& e, std::string& buffer, GUI& gui )
{
	static int	keystroke	= 0;
	static int	speed		= 0;
	static int	shift		= 0;
	static int	force_ccw	= 0;
	static int	force_cw	= 0;
	bool		changed		= false;



//==================================================================
//|						KEY PRESS EVENTS                           |
//==================================================================
	if ( e.type == SDL_KEYDOWN )
	{
		switch( e.key.keysym.sym )
		{
			case SDLK_w:	keystroke |= 0b00000001;	break;
			case SDLK_s:	keystroke |= 0b00000010;	break;
			case SDLK_a:	keystroke |= 0b00000100;	break;
			case SDLK_d:	keystroke |= 0b00001000;	break;
				
			case SDLK_LEFT:		force_ccw = 0xF;	break;
			case SDLK_RIGHT:	force_cw  = 0xF;	break;			
		}
		
		changed = true;
	}
	else if ( e.type == SDL_KEYUP )
	{
		switch( e.key.keysym.sym )
		{
			case SDLK_w:	keystroke &= ~0b00000001;	break;
			case SDLK_s:	keystroke &= ~0b00000010;	break;
			case SDLK_a:	keystroke &= ~0b00000100;	break;
			case SDLK_d:	keystroke &= ~0b00001000;	break;

			case SDLK_LEFT:		force_ccw = 0x1;	break;
			case SDLK_RIGHT:	force_cw  = 0x1;	break;

			case SDLK_UP:		force_ccw = 0x0; 
								force_cw  = 0x0;	break;
		}

		changed = true;
	}



//==================================================================
//|						  MOUSE WHEEL EVENTS                       |
//==================================================================
	if ( shift && e.type == SDL_MOUSEWHEEL )
	{
		if		( e.wheel.y > 0 )	{ force_ccw = 0xF; }
		else if ( e.wheel.y < 0 )	{ force_cw  = 0xF; }

		changed = true;
	}
	else if ( e.type == SDL_MOUSEWHEEL )
	{
		if		( e.wheel.y > 0 ) { speed += 25; }
		else if ( e.wheel.y < 0 ) { speed -= 25; }
		
		if		( speed > 255 ) { speed = 255; }
		else if ( speed < 0 )	{ speed = 0; }

		changed = true;
	}	




//==================================================================
//|							   UPDATE			                   |
//==================================================================
	if ( changed )
	{
		buffer.erase( buffer.begin(), buffer.end() );

		uint32_t output = 0;

		//push scan speed
		//---//

		//push enforced rotation
		output |= force_ccw;// | shift;

		output <<= 4;
		output |= force_cw;// | shift;

		output <<= 8;
		output |= speed;

		output <<= 8;
		output |= keystroke;


		char holder[9];
		_itoa( output, holder, 16 );
		buffer = holder;

		gui.updateTextBox( "speed", "Speed: " + std::to_string( (int)speed ), true );
	}
}







int main( int argc, char* argv[] )
{
	/// Initialize communication ///
	TCPClient radio;

	if ( !radio.init() || !radio.connectto("192.168.4.1", 80) )
	{
		return -1;
	}
	std::cout << "Connected." << std::endl;


	/// Initialize SDL ///
	if ( !init() )
	{
		return -2;
	}


	/// Declerations ///
	
	GUI gui;

	Sonar sonar;

	gui.init();
	sonar.init( gui );

	gui.createTextBox( "speed", "Speed:   ", GUI_BOX.x + 2, 100 );	// Controlled by getHardwareInput

	gui.createTextBox( "inst0", "Move:  W A S D", GUI_BOX.x + 2, 648 );	
	gui.createTextBox( "inst1", "Speed: Mouse wheel", GUI_BOX.x + 2, 678 );
	gui.createTextBox( "inst2", "Zoom:  LCTRL + m. wheel", GUI_BOX.x + 2, 708 );
	gui.createTextBox( "inst3", "Angle: LSHIFT + m. wheel", GUI_BOX.x + 2, 738 );
	

	SDL_Event e;
	
	std::string input_buffer = { 0 };
	std::string output_buffer = { 0 };



	

	/// Main loop ///

	for(;;)
	{
		//input_buffer.clear();
		//output_buffer.clear();

		while ( SDL_PollEvent( &e ) )
		{
			if ( e.type == SDL_QUIT ) goto Shutdown;

			handleHardwareInput( e, output_buffer, gui );
			sonar.setzoom( e );

		}/// YOU MAY WANT TO ADD A BREAK !



		radio.senddata( output_buffer );
		radio.getdata( input_buffer );
		
		sonar.update( input_buffer );
		sonar.updateTextboxes( gui );
		sonar.display();	

		gui.renderTextBoxes();


		SDL_RenderPresent( renderer );
	}

	




	Shutdown:
	close();
	return 0;
}


