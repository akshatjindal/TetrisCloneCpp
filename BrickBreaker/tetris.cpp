//
//  tetris.cpp
//  BrickBreaker
//
//  Created by Akshat Jindal on 6/8/20.
//  Copyright © 2020 Akshat Jindal. All rights reserved.
//

#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>

enum class tetrinimotype: unsigned char{
	looong,
	tbone,
	three_and_one,
};

enum class tetrinimocolor : unsigned char{
	green,
	red,
	blue,
	yellow,
	pink,
};

class Tetrinomo{
	
private:
	tetrinimotype type;
	tetrinimocolor color;
public:
	SDL_Rect rectOne;
	SDL_Rect rectTwo;
	
	Tetrinomo(){
		
	}
	
	Tetrinomo(tetrinimotype _type, int _x, int _y, tetrinimocolor _color){
		if(_type == tetrinimotype::looong){
			rectOne.x = _x;
			rectOne.y = _y;
			rectOne.w = 18;
			rectOne.h = 115;
		}
		
		color = _color;
	}
	
	tetrinimocolor get_color(){
		return this->color;
	}
	
};//tetrinomo class.

bool quit = false;
SDL_Window * window = NULL;
SDL_Renderer * la_rend = NULL;
TTF_Font * font = NULL;
const int WIDTH = 800;
const int HEIGHT = 600;
int frameCount = 0; int lastFrame = 0; int timerFPS = 0;
std::vector<SDL_Rect *> boundary_objects;
std::vector<Tetrinomo *> the_tetrinomos;

void update(SDL_Scancode a);

void printSDLError(){
	std::cout << SDL_GetError() << std::endl;
}

void setUpSystems(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) printSDLError();
	
	if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &la_rend) < 0) printSDLError();
	
	SDL_SetWindowTitle(window, "Tetris");
	
	if(TTF_Init() < 0) printSDLError();
	
	font = TTF_OpenFont("font.ttf", 30);
}

void closeAndFreeSystems(){
	TTF_CloseFont(font);
	font = nullptr;
	SDL_DestroyRenderer(la_rend);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

SDL_Scancode input(){
	SDL_Event e;
	//input stuff
	const Uint8 * keyStates = SDL_GetKeyboardState(NULL);
	
	auto x = SDL_SCANCODE_UNKNOWN;
	
	while(SDL_PollEvent(&e)){
		
		if(e.type == SDL_QUIT){
			quit = true;
			break;
		}
		
		if(keyStates[SDL_SCANCODE_ESCAPE]){
			quit = true;
			break;
		}
		
		if(keyStates[SDL_SCANCODE_RIGHT]){
			
			break;
		}
		
		if(keyStates[SDL_SCANCODE_LEFT]){
			
			break;
		}
		
		if(keyStates[SDL_SCANCODE_DOWN]){
			x = SDL_SCANCODE_DOWN;
		}
		
	}//while sdl poll event
	
	return x;
	
}

void update(SDL_Scancode _key){
	if(_key == SDL_SCANCODE_DOWN)
		the_tetrinomos[0]->rectOne.y += 10;
}

void createBoundaryObjects(){
	
	boundary_objects.reserve(3);
	
	SDL_Rect * one = new SDL_Rect();
	one->x = 250;
	one->y = 200;
	one->w = 30;
	one->h = 350;
	
	SDL_Rect * two = new SDL_Rect();
	two->x = 550;
	two->y = 200;
	two->w = 30;
	two->h = 350;
	
	SDL_Rect * three = new SDL_Rect();
	three->x = 250; three->y = 551;
	three->w = 580-250; three->h = 20;
	
	boundary_objects.push_back(one);
	boundary_objects.push_back(two);
	boundary_objects.push_back(three);
}

void drawBoundary()
{
	SDL_SetRenderDrawColor(la_rend, 139, 0, 0, 255);
	for(auto i = 0; i < boundary_objects.size(); ++i){
		SDL_RenderFillRect(la_rend, boundary_objects.at(i));
	}
}

void drawTetrinimos(){
	for(auto i = 0; i < the_tetrinomos.size(); ++i){
		if(the_tetrinomos[i]->get_color() == tetrinimocolor::green)
			SDL_SetRenderDrawColor(la_rend, 0, 255, 0, 255);
		
		SDL_RenderFillRect(la_rend, &the_tetrinomos[i]->rectOne);
		SDL_RenderFillRect(la_rend, &the_tetrinomos[i]->rectTwo);
			
	}
}

void render(){
	// SDL_GetRenderDrawColor(la_rend, 0, 0, 0, 255);
	SDL_SetRenderDrawColor(la_rend, 100, 100, 100, 255);
	
	SDL_RenderClear(la_rend);
	
	drawBoundary();
	
	drawTetrinimos();
	
	SDL_RenderPresent(la_rend);
	
	frameCount++;
	timerFPS = SDL_GetTicks() - lastFrame;
	if(timerFPS < (1000/60)){
		SDL_Delay((1000/60) - timerFPS);
	}
	
	
}


int main(){
	
	setUpSystems();
	createBoundaryObjects();
	
	Tetrinomo * a = new Tetrinomo(tetrinimotype::looong, 300, 150, tetrinimocolor::green);
	//TODO: create a destructor for the tetrinimo object.
	
	the_tetrinomos.push_back(a);
	
	static int lastTime = 0; int fps = 0;
	
	while(quit == false){
		
		lastFrame = SDL_GetTicks();
		if(lastFrame >= lastTime + 1000){
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
		}//if
		
		auto _input = input();
		update(_input);
		render();
		
	}//while not quit
	
	
	closeAndFreeSystems();
}
