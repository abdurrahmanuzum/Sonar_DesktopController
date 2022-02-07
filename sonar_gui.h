#pragma once

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <map>

enum class Direction { HORIZONTAL, VERTICAL };




#define SDL_BLACK	 0x00, 0x00, 0x00, 0xFF
#define SDL_GREEN	 0x00, 0x90, 0x00, 0xFF
#define SDL_WHITE	 0xFF, 0xFF, 0xFF, 0xFF
#define SDL_GREY	 0x80, 0x80, 0x80, 0xFF 

const int SCREEN_WIDTH		 = 1024;
const int SCREEN_HEIGHT		 = 768;

const int DISPLAY_WIDTH		 = 768;
const int DISPLAY_HEIGHT	 = 768;

const int GUI_WIDTH			 = 250;
const int GUI_HEIGHT		 = 768;

const int MAX_RADIUS		 = 768/2;
const int MAX_RADIUS_SQUARED = MAX_RADIUS*MAX_RADIUS;

const int TILE_LENGTH = 8;

const SDL_Rect GUI_BOX = { 772, 0, 252, 768 };

extern SDL_Window*		window;
extern SDL_Renderer*	renderer;
extern TTF_Font*		ttffont;

enum class Textpos 
{
	CUSTOM,
	TOP_LEFT,
	TOP_RIGHT,
	CENTER_LEFT,
	CENTER_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	CENTER_TOP,
	CENTER_BOTTOM,
	CENTER
};



bool init();
void close();
void SDL_RenderDrawCircle( SDL_Renderer* renderer, int x, int y, double r );
void SDL_RenderFillCircle( SDL_Renderer* renderer, int x, int y, double r );


class GUI
{
	public:
		GUI();
		~GUI();

		bool init();
		void popup( std::string message, Textpos textpos = Textpos::CENTER, int x = 0, int y = 0, bool clear = true );
		void standby();

		void createTextBox( std::string label, std::string text, int x, int y, int w = 0, int h = 0 );
		void updateTextBox( std::string label, std::string new_text, bool wrap_content );
		void renderTextBoxes();

		void writeEvent( SDL_Event& e, std::string& buffer );


	private:
		void erase();
		bool loadText( std::string text );		
		SDL_Texture* createTextTexture( std::string text, SDL_Rect* box = NULL );

	private:
		SDL_Rect		m_textbox;
		SDL_Texture*	m_texture;
		TTF_Font*		m_ttffont;

		std::map<std::string, SDL_Texture*> m_textTextures;
		std::map<std::string, SDL_Rect>		m_textBoxes;
		
		

};



class Sonar
{
	public:
		Sonar();
		~Sonar();

		void init( GUI& gui );
		void update( std::string input_buffer );
		void updateTextboxes( GUI& gui );
		void setzoom( SDL_Event& e );
		void display();

	private:
		bool loadTexture();
		void parseData( std::string data );

	private:
		SDL_Point		m_origin;
		SDL_Texture*	m_background;
		SDL_Texture*	m_cursor;
		double			m_angle_rad;
		double			m_distance_front;
		double			m_distance_rear;
		int				m_max_render_distance;
		GUI				m_gui;			
};



class Button
{
	public:
		Button( int x, int y, int w, int h, std::string label );
		virtual ~Button();

		void update( SDL_Event& e );

		bool		 isButtonHover();
		bool		 isButtonPress( SDL_Event& e );
		bool		 isButtonRelease( SDL_Event& e );

		virtual void onButtonHover(){}
		virtual void onButtonPress(){}
		virtual void onButtonRelease(){}

		virtual void render();

		virtual bool loadTexture( std::string path );
		bool		 loadTextTexture( std::string text );

	protected:
		bool		 pressed;

		SDL_Rect	 box;
		SDL_Rect	 label_box;

		SDL_Texture* texture;
		SDL_Texture* label;

		
};



class DynamicButton : public Button
{
	public:
		DynamicButton( int* bind_value, int x, int y, int fw, int fh, int bw, int bh, Direction direction, std::string label = ""  );
		~DynamicButton(){}


		void	onButtonHover();
		void	onButtonPress();
		void	onButtonRelease();
		double	getButtonValue();

		bool loadTexture( std::string path_foregroundTexture, std::string path_backgroundTexture );
		void render();

	private:
		int*			value;
		Direction		direction;
		SDL_Texture*	background_texture;
		SDL_Rect		background_box;
};






