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

bool quit = false;
SDL_Window * window = NULL;
SDL_Renderer * la_rend = NULL;
TTF_Font * font = NULL;
const int WIDTH = 800;
const int HEIGHT = 600;
int frameCount = 0; int lastFrame = 0; int timerFPS = 0;
std::vector<SDL_Rect *> boundary_objects;

enum class tetrinimotype: unsigned char{
	looong,
	tbone,
	three_and_one,
	box,
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
			rectOne.w = 20;
			rectOne.h = 120;
			
			rectTwo.x= _x;
			rectTwo.y = _y;
			rectTwo.w = 0;
			rectTwo.h = 0;
		}
		
		if(_type == tetrinimotype::box){
			int wid = 60; int hei = 30;
			rectOne.x = _x;
			rectOne.y = _y;
			rectTwo.x = _x;
			rectTwo.y = _y + hei;
			
			rectOne.w = rectTwo.w = wid;
			rectOne.h = rectTwo.h = hei;
		}
		
		color = _color;
	}
	
	tetrinimocolor get_color(){
		return this->color;
	}
	
	bool is_rotateable(){
		if(this->type == tetrinimotype::box)
			return false;
		return true;
	}
	
};//tetrinomo class.

std::vector<Tetrinomo *> the_tetrinomos;

class Game{
private:
	std::vector<Tetrinomo *> top_layer;
	
	size_t index_of_active_tetrinimo = 0;
	SDL_Rect * bottom_boundary = boundary_objects[2];
	SDL_Rect * right_boundary = boundary_objects[1];
	
	SDL_Rect * left_boun = boundary_objects[0];
	
	Tetrinomo * act = the_tetrinomos[index_of_active_tetrinimo];
	
	std::pair<bool, int> intersects_left();
	std::pair<bool, int> intersects_right();
	bool intersects_bottom();
	bool has_reached_bottom();
	
	
	void move_down(){
		
		if(intersects_bottom()) return;
		
		act->rectOne.y += 10;
		act->rectTwo.y += 10;
		
	}
	
	void move_right(){
		auto p = intersects_right();
		if(p.first == true)
			return;
		else{
			act->rectOne.x += p.second;
			act->rectTwo.x += p.second;
		}
	}
	
	void move_left(){
		auto p = intersects_left();
		if(p.first) return;
		else{
			act->rectOne.x -= p.second;
			act->rectTwo.x -= p.second;
		}
	}
	
	
	
	void drop(){
		
	}
	
	void rotate_right(){
		
	}
	
	void rotate_left(){
		
	}
public:
	
	void update(SDL_Scancode a);
	
	
};

bool Game::has_reached_bottom(){
	
	
	
}

std::pair<bool,int> Game::intersects_left(){
	
	if(SDL_HasIntersection(left_boun, &act->rectOne) || SDL_HasIntersection(left_boun, &act->rectTwo))
		return std::make_pair(true,0);
	
	auto temp_r_one = act->rectOne;
	auto temp_r_two = act->rectTwo;
	temp_r_one.x -= 20; temp_r_two.x -= 20;
	
	if(not SDL_HasIntersection(left_boun, &temp_r_one) &&  not SDL_HasIntersection(left_boun, &temp_r_two))
		return std::make_pair(false, 20);
	
	temp_r_one.x += 20; temp_r_two.x += 20;
	
	int max_movement_to_left = 0;
	for(int i = 1; i < 20; ++i){
		temp_r_one.x -= i; temp_r_two.x -= i;
		if(SDL_HasIntersection(&temp_r_one, left_boun) || SDL_HasIntersection(&temp_r_two, left_boun)){
			max_movement_to_left = i-1; break;
		}
	}
	
	
	return std::make_pair(false, max_movement_to_left);
}

std::pair<bool,int> Game::intersects_right(){
	
	if(SDL_HasIntersection(right_boundary, &act->rectOne) || SDL_HasIntersection(right_boundary, &act->rectTwo)){
		return std::make_pair(true, 0);
	}
	
	auto temp_r_one = act->rectOne;
	auto temp_r_two = act->rectTwo;
	temp_r_one.x += 20; temp_r_two.x += 20;
	
	if(not SDL_HasIntersection(right_boundary, &temp_r_one) &&  not SDL_HasIntersection(right_boundary, &temp_r_two))
		return std::make_pair(false, 20);
	
	temp_r_one.x -= 20;
	temp_r_two.x -= 20;
	
	int max_movement_to_right = 0;
	for(int i = 1; i < 20; ++i){
		temp_r_one.x += i;
		temp_r_two.x+= i;
		if(SDL_HasIntersection(&temp_r_one, right_boundary) || SDL_HasIntersection(&temp_r_two, right_boundary)){
			max_movement_to_right = i-1; break;
		}
	}
	
	return std::make_pair(false, max_movement_to_right);
}

bool Game::intersects_bottom(){
	
	SDL_Rect temp_r_one = act->rectOne;
	SDL_Rect temp_r_two  = act->rectTwo;
	
	temp_r_one.y += 10; temp_r_two.y += 10;
	
	if(SDL_HasIntersection(&temp_r_one, bottom_boundary) || SDL_HasIntersection(&temp_r_two, bottom_boundary))
		return true;
	return false;
}


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
			quit = true; break;
		}
		
		if(keyStates[SDL_SCANCODE_ESCAPE]){
			quit = true; break;
		}
		
		if(keyStates[SDL_SCANCODE_SPACE]){
			x = SDL_SCANCODE_SPACE; break;
		}
		
		if(keyStates[SDL_SCANCODE_RIGHT]){
			x = SDL_SCANCODE_RIGHT; break;
		}
		
		if(keyStates[SDL_SCANCODE_LEFT]){
			x = SDL_SCANCODE_LEFT; break;
		}
		
		if(keyStates[SDL_SCANCODE_DOWN]){
			x = SDL_SCANCODE_DOWN; break;
		}
		
	}//while sdl poll event
	
	return x;
	
}


void Game::update(SDL_Scancode _key){
	
	switch (_key) {
		case SDL_SCANCODE_DOWN:
			move_down(); break;
	
		case SDL_SCANCODE_RIGHT:
			move_right(); break;
		
		case SDL_SCANCODE_D: break;
			
		case SDL_SCANCODE_LEFT:
			move_left(); break;
			
		default:
			break;
	}
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
		auto obj = the_tetrinomos[i];
		
		if(obj->get_color() == tetrinimocolor::green)
			SDL_SetRenderDrawColor(la_rend, 0, 255, 0, 255);
		
		if(obj->get_color() == tetrinimocolor::blue)
			SDL_SetRenderDrawColor(la_rend, 0, 0, 205, 255);
		
		SDL_RenderFillRect(la_rend, &obj->rectOne);
		SDL_RenderFillRect(la_rend, &obj->rectTwo);
			
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
	
	Tetrinomo * box = new Tetrinomo(tetrinimotype::box, 350, 150,
									tetrinimocolor::blue);
	the_tetrinomos.push_back(a);
	the_tetrinomos.push_back(box);
	
	Game g;
	
	
	static int lastTime = 0; int fps = 0;
	
	while(quit == false){
		
		lastFrame = SDL_GetTicks();
		if(lastFrame >= lastTime + 1000){
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
		}//if
		
		auto _input = input();
		g.update(_input);
		render();
		
	}//while not quit
	
	//TODO: create a destructor for the tetrinimo object.
	
	//TODO: add an active tetrinimo state
	//this is the tetrinimo that we will move
	
	//TODO: add a rotate function for each tetrinimo.
	
	//TODO: add a tetrinimobuilder class that spawns
	//...tetrinimos and displays the next tetrinimo
	//...that will be spawned so that the user can
	//...make their move.
	
	/*TODO: when user pressed space, the tetrinimo,
	 should drop and it should drop properly. */
	
	//TODO: draw lines on the shapes to make them look cooler.
	closeAndFreeSystems();
}
