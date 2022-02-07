#define _CRT_SECURE_NO_WARNINGS

#include <sstream>
#include <iomanip>
#include "sonar_gui.h"
#include "globals.h"
#include "rules.h"



SDL_Window*		window	 = nullptr;
SDL_Renderer*	renderer = nullptr;
TTF_Font*		ttffont  = nullptr;




GUI::GUI()
{
	m_textbox = { 0, 0, 0, 0 };
	m_texture = nullptr;
	m_ttffont = nullptr;

}



GUI::~GUI()
{
	erase();

	TTF_CloseFont( m_ttffont );
	m_ttffont = nullptr;

	for ( auto i : m_textTextures )
	{	
		if ( i.second != nullptr )
		{												///I======================================================I///
			SDL_DestroyTexture( i.second );				///  IMPORTANT FIX: write access vialation at shutdown.    ///
			i.second = nullptr;							///I======================================================I///
		}
	}
}



bool GUI::init()
{
	m_ttffont = TTF_OpenFont( "media/Orkney Regular.ttf", 18 );
	if ( m_ttffont == nullptr )
	{
		std::cerr << "Failed to laod font. Error: " << TTF_GetError() << std::endl;
		return false;
	}
	return true;
}



void GUI::erase()
{
	SDL_DestroyTexture( m_texture );
	m_texture = nullptr;
}



bool GUI::loadText( std::string text )
{
	SDL_Color color = { 0xff, 0xff, 0xff };
	erase();

	SDL_Surface* surface = TTF_RenderText_Solid( m_ttffont, text.c_str(), color );
	if ( surface == nullptr ) { return false; }

	m_texture = SDL_CreateTextureFromSurface( renderer, surface );
	if ( m_texture == nullptr ) { return false; }

	m_textbox.w = surface->w;
	m_textbox.h = surface->h;

	SDL_FreeSurface( surface );

	return true;
}



SDL_Texture* GUI::createTextTexture( std::string text, SDL_Rect* box )
{
	SDL_Color color = { 0xFF, 0xFF, 0xFF };

	SDL_Surface* surface = TTF_RenderText_Solid( m_ttffont, text.c_str(), color );
	if ( surface == nullptr ) { return nullptr; }

	SDL_Texture* new_texture = SDL_CreateTextureFromSurface( renderer, surface );

	if ( box != nullptr )
	{
		box->w = surface->w;
		box->h = surface->h;
	}
	
	SDL_FreeSurface( surface );

	return new_texture;
}



void GUI::popup( std::string message, Textpos textpos, int x, int y, bool clear )
{
	if ( loadText( message ) )
	{
		switch ( textpos )
		{
			case Textpos::TOP_LEFT:
				m_textbox.x = 0;
				m_textbox.y = 0;
			break;

			case Textpos::TOP_RIGHT:
				m_textbox.x = SCREEN_WIDTH - m_textbox.w;
				m_textbox.y = 0;
			break;

			case Textpos::CENTER_LEFT:
				m_textbox.x = 0;
				m_textbox.y = ( SCREEN_HEIGHT - m_textbox.h ) / 2;
			break;

			case Textpos::CENTER_RIGHT:
				m_textbox.x = SCREEN_WIDTH - m_textbox.w;
				m_textbox.y = ( SCREEN_HEIGHT - m_textbox.h ) / 2;
			break;

			case Textpos::BOTTOM_LEFT:
				m_textbox.x = 0;
				m_textbox.y = SCREEN_HEIGHT - m_textbox.h;
			break;

			case Textpos::BOTTOM_RIGHT:
				m_textbox.x = SCREEN_WIDTH - m_textbox.w;
				m_textbox.y = SCREEN_HEIGHT - m_textbox.h;
			break;

			case Textpos::CENTER_TOP:
				m_textbox.x = ( SCREEN_WIDTH - m_textbox.w ) / 2;
				m_textbox.y = 0;
			break;

			case Textpos::CENTER_BOTTOM:
				m_textbox.x = ( SCREEN_WIDTH - m_textbox.w ) / 2;
				m_textbox.y = SCREEN_HEIGHT - m_textbox.h;
			break;

			case Textpos::CENTER:
				m_textbox.x = ( SCREEN_WIDTH - m_textbox.w ) / 2;
				m_textbox.y = ( SCREEN_HEIGHT - m_textbox.h ) / 2;
			break;

			case Textpos::CUSTOM:
				m_textbox.x = x;
				m_textbox.y = y;
			break;
		}

		if ( clear )
		{
			SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );
			SDL_RenderClear( renderer );
		}

		SDL_RenderCopy( renderer, m_texture, NULL, &m_textbox );

		SDL_RenderPresent( renderer );

		standby();

		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );
		SDL_RenderClear( renderer );
	}
	else
	{
		std::cerr << "An error ocurred displaying text." << std::endl;
	}
}



void GUI::createTextBox( std::string label, std::string text, int x, int y, int w, int h )
{
	SDL_Rect textbox;

	SDL_Texture* new_texture = createTextTexture( text, &textbox );

	textbox.x = x;
	textbox.y = y;

	if ( w != 0 && h != 0 ) { textbox.w = w; textbox.h = h; }

	if ( new_texture == nullptr )
	{
		std::cerr << "GUI::createTextBox(...): Failed to create texture." << std::endl;
		return;
	}

	m_textTextures.insert( std::pair<std::string, SDL_Texture*>( label, new_texture ) );
	m_textBoxes.insert( std::pair<std::string, SDL_Rect>( label, textbox ) );	
}



void GUI::updateTextBox( std::string label, std::string new_text, bool wrap_content )
{
	auto i = m_textTextures.find( label );

	if ( i != m_textTextures.end() )
	{
		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );
		SDL_RenderFillRect( renderer, &(m_textBoxes[label]) );

		SDL_DestroyTexture( i->second );

		SDL_Rect textbox;

		SDL_Texture* new_texture = createTextTexture( new_text, &textbox );
		i->second = new_texture;

		if ( wrap_content )
		{
			m_textBoxes[ label ].w = textbox.w;
			m_textBoxes[ label ].h = textbox.h;

		}
	}
}



void GUI::renderTextBoxes()
{
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );
	for ( auto i : m_textBoxes )
	{
		SDL_RenderFillRect( renderer, &(i.second) );
	}

	for ( auto i : m_textTextures )
	{
		SDL_RenderCopy( renderer, i.second, NULL, &(m_textBoxes[i.first]) );
	}

#ifdef DEBUG_SHOW_TEXT_OUTLINE
	SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
	for ( auto i : m_textBoxes )
	{
		SDL_RenderDrawRect( renderer, &(i.second) );
	}
#endif
}



void GUI::standby()
{
	SDL_Event e;

	for(;;)
	{
		SDL_PollEvent( &e );

		if ( e.type == SDL_QUIT )
		{
			close();
			exit(1);
		}
		else if ( e.type == SDL_KEYUP )
		{
			break;
		}
	}
}



void GUI::writeEvent( SDL_Event& e, std::string& buffer )
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

		updateTextBox( "speed", "Speed: " + std::to_string( (int)speed ), true );
	}
}




/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ///




Sonar::Sonar()
{
	m_origin				= { DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 };
	m_background			= nullptr;
	m_cursor				= nullptr;
	m_angle_rad				= 0;
	m_distance_front		= 0;
	m_distance_rear			= 0;
	m_max_render_distance	= 100;
}



Sonar::~Sonar()
{
	close();
}



void Sonar::init( GUI& gui )
{
	loadTexture();
	
	//m_gui.init();

	gui.createTextBox( "angle", "Angle:    ", GUI_BOX.x + 2, 10 );
	gui.createTextBox( "zoom", "Max Distance:    ", GUI_BOX.x + 2, 40 );

}



void Sonar::update( std::string input_buffer )
{
	parseData( input_buffer );

	//double angle_r = m_angle * ( M_PI / 180 ); 
	double m = tan( m_angle_rad );

	double x = 0;
	double y = 0;

	double distance;
	bool improper = false;


	/// For x > 0 ///

	SDL_SetRenderDrawColor( renderer, SDL_BLACK );
	
	for ( x = 0; x < MAX_RADIUS, !improper; x += 0.01 )
	{
		y = m*x;

		distance = y>=0 ? m_distance_front : m_distance_rear;


		if ( y*y + x*x > distance*distance ) 
		{
			SDL_SetRenderDrawColor( renderer, SDL_GREEN );	
		}

		if ( y*y + x*x > MAX_RADIUS_SQUARED )
		{
			y = ( y/abs(y) ) * sqrt( MAX_RADIUS_SQUARED - x*x );
			improper = true;
		}
			
			
		int temp_x = x;// - (int)x % TILE_LENGTH;


		SDL_Rect temp = { temp_x+m_origin.x, -y+m_origin.y, TILE_LENGTH, TILE_LENGTH };
		SDL_RenderFillRect( renderer, &temp );	

	}


	/// For x < 0 ///

	SDL_SetRenderDrawColor( renderer, SDL_BLACK );
	improper = false;

	for ( x = -0.1; x > -MAX_RADIUS, !improper; x -= 0.01 )
	{
		y = m*x;

		distance = y>=0 ? m_distance_front : m_distance_rear;


		if ( y*y + x*x > distance*distance )
		{
			SDL_SetRenderDrawColor( renderer, SDL_GREEN );
		}
			
		if ( y*y + x*x > MAX_RADIUS_SQUARED )
		{
			y = ( y/abs(y) ) * sqrt( MAX_RADIUS_SQUARED - x*x );
			improper = true;
		}
			
			
			
		int temp_x = x;// - (int)x % TILE_LENGTH;

		SDL_Rect temp = { temp_x+m_origin.x, -y+m_origin.y, TILE_LENGTH, TILE_LENGTH };
		SDL_RenderFillRect( renderer, &temp );
	}

}



void Sonar::updateTextboxes( GUI& gui )
{
	gui.updateTextBox( "angle", "Angle: " + std::to_string( (int)( m_angle_rad*(180/M_PI) ) ) + "°" , true );

	gui.updateTextBox( "zoom", "Max Distance: " + std::to_string( m_max_render_distance ), true );
}



void Sonar::setzoom( SDL_Event& e )
{
	static bool ctrl = false;

	if ( e.type == SDL_KEYDOWN  && e.key.keysym.sym == SDLK_LCTRL )
	{
		ctrl = true;
	}
	else if ( e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_LCTRL )
	{
		ctrl = false;
	}

	else if ( ctrl && e.type == SDL_MOUSEWHEEL )
	{
		if		( e.wheel.y > 0 && m_max_render_distance <= 195 )	{ m_max_render_distance += 5; }
		else if ( e.wheel.y < 0 && m_max_render_distance >= 35  )	{ m_max_render_distance -= 5; }
	}
}



void Sonar::display()
{
	//Draw grid
	#ifdef DEBUG_SONAR_SHOW_GRID

		SDL_SetRenderDrawColor( renderer, SDL_GREY );
	
		for ( int i=0; i < SCREEN_WIDTH; i += TILE_LENGTH )
		{
			SDL_RenderDrawLine( renderer, i, 0, i, SCREEN_HEIGHT );
		}

		for ( int i=0; i < SCREEN_WIDTH; i += TILE_LENGTH )
		{
			SDL_RenderDrawLine( renderer, 0, i, SCREEN_WIDTH, i );
		}

	#endif

	/*
	SDL_SetRenderDrawColor( renderer, SDL_WHITE );

	//Draw boundary circle
	SDL_RenderDrawCircle( renderer, m_origin.x, m_origin.y, MAX_RADIUS );

	//Draw milestone circles
	//---//

	//Draw x axis
	SDL_RenderDrawLine( renderer, 0, DISPLAY_HEIGHT/2, DISPLAY_WIDTH, DISPLAY_HEIGHT/2 );

	//Draw y axis
	SDL_RenderDrawLine( renderer, DISPLAY_WIDTH/2, 0, DISPLAY_WIDTH/2, DISPLAY_HEIGHT );

	//Draw current scan line
	//SDL_RenderDrawLine( renderer, m_origin.x, m_origin.y, cos( m_angle_rad )*MAX_RADIUS + m_origin.x, -sin(m_angle_rad )*MAX_RADIUS + m_origin.y );
	*/

	SDL_Rect renderRect = { 0, 0, 768, 768 };
	SDL_RenderCopy( renderer, m_background, NULL, &renderRect );

	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );
	SDL_RenderFillCircle( renderer, m_origin.x, m_origin.y, 20 );

	renderRect = { m_origin.x - 20, m_origin.y - 20, 40, 40 };
	SDL_Point centre = { 20, 20 };
	SDL_RenderCopyEx( renderer, m_cursor, NULL, &renderRect, 180-(m_angle_rad*(180/M_PI)), &centre, SDL_FLIP_NONE );

	SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderDrawLine( renderer, GUI_BOX.x, 0, 772, 768 );


	//Render everything
	//SDL_RenderPresent( renderer );

}



void Sonar::parseData( std::string data )
{
	uint32_t input = strtoull( data.c_str(), nullptr, 16 );
		
	m_angle_rad = ( input & 0x000000FF ) * (M_PI/180);
	input >>= 8;

	m_distance_front = input & 0x000000FF;
	input >>= 8;

	m_distance_rear = input & 0x000000FF;

	m_distance_front = map( m_distance_front, 0, m_max_render_distance, 0, MAX_RADIUS );
	m_distance_rear	 = map( m_distance_rear,  0, m_max_render_distance, 0, MAX_RADIUS );
}



bool Sonar::loadTexture()
{
	SDL_Surface* surface = IMG_Load( "media/background.png" );
	if ( surface == nullptr ) 
	{
		std::cerr << "Sonar::loadTexture(): Failed to load surface. Error: " << IMG_GetError() << std::endl;
		return false; 
	}

	SDL_SetColorKey( surface, SDL_TRUE, SDL_MapRGB( surface->format, 0, 0xFF, 0 ) );

	m_background = SDL_CreateTextureFromSurface( renderer, surface );
	if ( m_background == nullptr ) 
	{
		std::cerr << "Sonar::loadTexture(): Failed to create texture. Error: " << SDL_GetError() << std::endl;	
		return false; 
	}

	

	surface = IMG_Load( "media/cursor.png" );
	if ( surface == nullptr )
	{
		std::cerr << "Sonar::loadTexture(): Failed to load cursor.png. Error: " << IMG_GetError << std::endl;
		return false;
	}

	SDL_SetColorKey( surface, SDL_TRUE, SDL_MapRGB( surface->format, 0, 0xFF, 0 ) );

	m_cursor = SDL_CreateTextureFromSurface( renderer, surface );
	if ( m_cursor == nullptr )
	{
		std::cerr << "Sonar::loadTexture(): Failed to create cursor texture. Error: " << SDL_GetError() << std::endl;
		return false;
	}


	
	SDL_FreeSurface( surface );
	return true;
}




/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ///



Button::Button( int x, int y, int w, int h, std::string label ) : box( {x,y,w,h} ), label_box( { x, y+h, 0, 0 } )
{
	texture = nullptr;
	this->label = nullptr;
	pressed = false;

	loadTextTexture( label );
}



Button::~Button()
{
	SDL_DestroyTexture( texture );
	texture = nullptr;
	pressed = false;
}



void Button::update( SDL_Event& e )
{
	std::cout << "Base version of update" << std::endl;

	if ( isButtonHover()      )	onButtonHover();
	if ( isButtonPress( e )   )	onButtonPress();
	if ( isButtonRelease( e ) )	onButtonRelease();
}



bool Button::isButtonHover()
{
	///You may want to check if event is SDL_MOUSEMOTION

	int mouse_x;
	int mouse_y;

	SDL_GetMouseState( &mouse_x, &mouse_y );

	if ( ( mouse_x > box.x && mouse_x < box.x + box.w ) && ( mouse_y > box.y && mouse_y < box.y + box.h ) )
	{
		return true;
	}

	return false;
}



bool Button::isButtonPress( SDL_Event& e )
{
	if ( isButtonHover() )
	{
		if ( e.type == SDL_MOUSEBUTTONDOWN )
		{
			pressed = true;
		}
		
	}

	return pressed;
}



bool Button::isButtonRelease( SDL_Event& e )
{
	if ( pressed )
	{
		return !( pressed = !(e.type == SDL_MOUSEBUTTONUP) );
	}

	return false;
}



bool Button::loadTexture( std::string path )
{
	SDL_Texture* new_texture = nullptr;

	SDL_Surface* surface = IMG_Load( path.c_str() );
	
	if ( surface == nullptr )
	{
		std::cerr << "Failed to load media from: " << path << "Error: " << IMG_GetError() << std::endl;
		return false;
	}

	SDL_SetColorKey ( surface, SDL_TRUE, SDL_MapRGB( surface->format, 0, 0xFF, 0 ) );

	new_texture = SDL_CreateTextureFromSurface( renderer, surface );
	if( new_texture == nullptr )
	{
		std::cerr << "Failed to create texture from surface. Error: " << SDL_GetError() << std::endl;
		return false;
	}

	/// get widh and height here??

	SDL_FreeSurface( surface );

	texture = new_texture;

	return true;
}



bool Button::loadTextTexture( std::string text )
{
	SDL_Color text_color = { 0xFF, 0xFF, 0xFF };
	SDL_Surface* text_surface = TTF_RenderText_Solid( ttffont, text.c_str(), text_color );
	if ( text_surface == nullptr )
	{
		std::cerr << "Failed to create text from surface. Error: " << TTF_GetError() << std::endl;
		return false;
	}

	label = SDL_CreateTextureFromSurface( renderer, text_surface );
	if ( label == nullptr )
	{
		std::cerr << "Failed to create text texture from surface. Error: " << SDL_GetError() << std::endl;
		return false;
	}

	label_box.w = text_surface->w;
	label_box.h = text_surface->h;

	SDL_FreeSurface( text_surface );

	return true;
}



void Button::render()
{
	SDL_RenderCopy( renderer, texture, NULL, &box );

	//SDL_SetRenderDrawColor( renderer, 0xFF,0xFF,0xFF,0xFF );
	//SDL_RenderFillRect( renderer, &box );
}




/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ///




DynamicButton::DynamicButton( int* bind_value, int x, int y, int fw, int fh, int bw, int bh, Direction direction, std::string label ) : Button( x, y, fw, fh, label )
{
	value = bind_value;
	this->direction = direction;
	background_box = { x, y, bw, bh };
	background_texture = nullptr;
}



void DynamicButton::onButtonHover()
{
	std::cout << "hover" << std::endl;
}



void DynamicButton::onButtonPress()
{
	std::cout << "press";

	int x, y;
	SDL_GetMouseState( &x, &y );

	int dx = x - (box.x + box.w/2);
	box.x += dx;

	if ( box.x < background_box.x || box.x + box.w > background_box.x + background_box.w ) 
	{
		box.x -= dx;
	}

	*value = box.x - background_box.x + box.w/2;

}



void DynamicButton::onButtonRelease()
{
	std::cout << "release" << std::endl;
}



bool DynamicButton::loadTexture( std::string path_foregroundTexture, std::string path_backgroundTexture )
{
	SDL_Surface* surface = IMG_Load( path_backgroundTexture.c_str() );
	
	if ( surface == nullptr )
	{
		std::cerr << "Failed to load media from: " << path_backgroundTexture << "Error: " << IMG_GetError() << std::endl;
		return false;
	}

	SDL_SetColorKey ( surface, SDL_TRUE, SDL_MapRGB( surface->format, 0, 0xFF, 0 ) );

	background_texture = SDL_CreateTextureFromSurface( renderer, surface );
	if( background_texture == nullptr )
	{
		std::cerr << "Failed to create texture from surface. Error: " << SDL_GetError() << std::endl;
		return false;
	}





	surface = IMG_Load( path_foregroundTexture.c_str() );

	if ( surface == nullptr )
	{
		std::cerr << "Failed to load media from: " << path_foregroundTexture << "Error: " << IMG_GetError() << std::endl;
		return false;
	}

	SDL_SetColorKey ( surface, SDL_TRUE, SDL_MapRGB( surface->format, 0, 0xFF, 0 ) );

	texture = SDL_CreateTextureFromSurface( renderer, surface );
	
	if( texture == nullptr )
	{
		std::cerr << "Failed to create texture from surface. Error: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_FreeSurface( surface );



	return true;
}
	


void DynamicButton::render()
{
	SDL_RenderCopy( renderer, background_texture, NULL, &background_box );
	SDL_RenderCopy( renderer, texture, NULL, &box );

	SDL_RenderCopy( renderer, label, NULL, &label_box );
}




/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ///




bool init()
{
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		std::cerr << "Failed to initialize SDL. Error: " << SDL_GetError() << std::endl;
		return false;
	}


	window = SDL_CreateWindow( "Sonar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if ( window == nullptr )
	{
		std::cerr << "Failed to create window. Error: " << SDL_GetError() << std::endl;
		return false;
	}


	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	if ( renderer == nullptr )
	{
		std::cerr << "Failed to create renderer. Error: " << SDL_GetError() << std::endl;
		return false;
	}
	SDL_SetRenderDrawColor( renderer, SDL_BLACK );


	if( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) )
	{
		std::cerr << "Failed to initialize SDL_image. Error: " << IMG_GetError() << std::endl;
		return false;
	}

	if ( TTF_Init() == -1 )
	{
		std::cerr << "Failed to initalize SDL_ttf. Error: " << TTF_GetError() << std::endl;
		return false;
	}


	//ttffont = TTF_OpenFont( "media/font/Orkney Regular.ttf", 16 );
	//if ( ttffont == nullptr )
	//{
	//	std::cerr << "Failed to open font. Error: " << TTF_GetError() << std::endl;
	//	return false;
	//}



	return true;
}



void close()
{
	SDL_DestroyRenderer( renderer );
	renderer = nullptr;

	SDL_DestroyWindow( window );
	window = nullptr;

	SDL_Quit();
}



void SDL_RenderDrawCircle( SDL_Renderer* renderer, int x, int y, double r )
{
	double angle = 0;

	while ( angle <= 2*M_PI )
	{
		SDL_RenderDrawPoint( renderer, r*cos(angle)+x, r*sin(angle)+y );
		angle += 0.001*M_PI;
	}
}



void SDL_RenderFillCircle( SDL_Renderer* renderer, int x, int y, double r )
{
	double angle = 0;

	while ( angle <= 2*M_PI )
	{
		SDL_RenderDrawLine( renderer, x, y, r*cos(angle)+x, r*sin(angle)+y );
		angle += 0.001*M_PI;		
	}
}




































































