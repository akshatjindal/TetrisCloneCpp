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

enum class movetype: unsigned char{
	left,
	down,
	right,
};

class Tetrinomo{
	
private:
	tetrinimotype type;
	tetrinimocolor color;
	bool can_go_further_down = true;
	bool tetrinimo_locked = false;
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
		
		if(_type == tetrinimotype::three_and_one){
			int wid = 90; int hei = 30;
			rectOne.x = _x;
			rectOne.y = _y;
			
			rectTwo.x = _x+60;
			rectTwo.y = _y - 30;
			
			rectOne.w = wid; rectOne.h = hei;
			rectTwo.w = 30; rectTwo.h = hei;
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
	
	bool can_go_furth_down(){
		return this->can_go_further_down;
	}
	
	void set_cant_go_further_down(){
		this->can_go_further_down = false;
	}
	
	bool is_locked(){
		return this->tetrinimo_locked;
	}
	
	void lock_tetrinimo(){
		this->tetrinimo_locked = true;
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
	
	
	void rect_manipulator_helper(movetype _move, SDL_Rect &r1, SDL_Rect &r2, bool manipulate);
	
	bool tetrinimo_intersection_helper(const SDL_Rect & r1, const SDL_Rect &r2, const Tetrinomo * _tet, movetype _move);
	

	std::pair<bool, int> intersects_another_tetrinimo(movetype _move);
	std::pair<bool, int> intersects_left();
	std::pair<bool, int> intersects_right();
	bool intersects_bottom();
	bool has_reached_bottom();
	
	void spawn_tetrinimo(){
		Tetrinomo * a = new Tetrinomo(tetrinimotype::three_and_one,300,200, tetrinimocolor::green);
		the_tetrinomos.push_back(a);
	}
	
	void spawn_and_change_active_tetrinimo(){
		++index_of_active_tetrinimo;
		spawn_tetrinimo();
		act = the_tetrinomos[index_of_active_tetrinimo];
		std::cout << "sd\n";

	}
	
	void lock_logic(){
		
		if(act->is_locked()) return;
		
		if(act->can_go_furth_down() == false){
			act->lock_tetrinimo();
			spawn_and_change_active_tetrinimo();
		}
		
			
	}
	
	void move_down(){
		
		if(has_reached_bottom()) return;
		
		auto p = intersects_another_tetrinimo(movetype::down);
		
		if(p.first == true){
			act->rectOne.y += p.second;
			act->rectTwo.y += p.second;
		}
		
		//TODO: implement a function that tells us how close
		//...a rect is from the bottom boundary.
		//...this func will serve the same function as intersect_left() and intersect_right()
		else{
			act->rectOne.y += 10;
			act->rectTwo.y += 10;
		}
	}
	
	void move_right(){
		auto p = intersects_right();
		bool intersects_right_boundary = p.first;
		if(not intersects_right_boundary){
			auto c = intersects_another_tetrinimo(movetype::right);
			bool intersects_another_tet = c.first;
			if(not intersects_another_tet){
				act->rectOne.x += p.second;
				act->rectTwo.x += p.second;
			}
			
			else if(intersects_another_tet){
				act->rectOne.x += c.second;
				act->rectTwo.x += c.second;
			}
		}
	}
	
	void move_left(){
		auto p = intersects_left();
		bool intersects_left_boundary = p.first;
		if(not intersects_left_boundary){
			auto c = intersects_another_tetrinimo(movetype::left);
			bool intersects_another_tet = c.first;
			if(intersects_another_tet){
				act->rectOne.x -= c.second;
				act->rectTwo.x -= c.second;
			}
			
			else if(not intersects_another_tet){
				act->rectOne.x -= p.second;
				act->rectTwo.x -= p.second;
			}
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

bool Game::tetrinimo_intersection_helper(const SDL_Rect & r1, const SDL_Rect &r2, const Tetrinomo * _tet, movetype _move){
	
	if(_move == movetype::down){
		if(
		   SDL_HasIntersection(&r1, &_tet->rectOne) ||
		   SDL_HasIntersection(&r1, &_tet->rectTwo) ||
		   SDL_HasIntersection(&r2, &_tet->rectOne) ||
		   SDL_HasIntersection(&r2, &_tet->rectTwo)
		   ){
			return true;
		}
	}
	
	if(_move == movetype::left){
		
	}
	
	
		return false;
}

void Game::rect_manipulator_helper(movetype _move, SDL_Rect &r1, SDL_Rect &r2, bool manipulate){
	switch (_move) {
		case movetype::right :
			if(manipulate)
				{r1.x += 20; r2.x += 20;}
			if(not manipulate)
				{r1.x -= 20; r2.x -= 20;}
			break;
			
		case movetype::left :
			if(manipulate)
				{r1.x -= 20; r2.x -= 20;}
			if(not manipulate)
				{r1.x += 20; r2.x += 20;}
			
		case movetype::down :
			if(manipulate)
				{r1.y += 10; r2.y += 10;}
			if(not manipulate)
				{r1.y -= 10; r2.y -= 10;}
			
		default:
			break;
	}
}

bool Game::has_reached_bottom(){
	
	//TODO: we need to check if the tetrinimo has reached as far down as it can go
	//...for this particular set of x values (think about dividing the playing field
	//...in grid lines).
	
	if(intersects_bottom()){
		
		act->set_cant_go_further_down();
		return true;
	}
	
	return false;
	
}


/*returns false if the tetrinimo can move its full 20 or 10 units in
 the corresponding direction without intersecting w/ another tetrinimo.
 
 returns true along with x units that the tetrinimo can move if it
 does intersect with another tetrinimo. */
std::pair<bool, int> Game::intersects_another_tetrinimo(movetype _move){

	SDL_Rect temp_r1 = act->rectOne;
	SDL_Rect temp_r2 = act->rectTwo;
	
	rect_manipulator_helper(_move, temp_r1, temp_r2, true);
	
	Tetrinomo * intersecting_tetrinimo = nullptr;
	
	for(auto i = 0; i < index_of_active_tetrinimo; ++i){
		if(tetrinimo_intersection_helper(temp_r1, temp_r2, the_tetrinomos[i])){
			intersecting_tetrinimo = the_tetrinomos[i];
			break;
		}
	}
	

	if(intersecting_tetrinimo == nullptr){
		switch (_move) {
			case movetype::down :
//				std::cout << "awe\n";
				return std::make_pair(false, 10);
				break;
				
			default:
				return std::make_pair(false, 20);
				break;
		}
	}
	
	rect_manipulator_helper(_move, temp_r1, temp_r2, false);
	
	int max_movement_allowed = 0;
	if(_move == movetype::right || _move == movetype::left)
		max_movement_allowed = 20;
	else if (_move == movetype::down)
		max_movement_allowed = 10;
	
	int move = 0;

	for(int i = 1; i < max_movement_allowed; ++i){
		switch (_move) {
			case movetype::right :
				temp_r1.x += i; temp_r2.x += i;
				break;
			case movetype::left :
				temp_r1.x -= i; temp_r2.x -= i;
				break;
			case movetype::down :
				temp_r1.y += i; temp_r2.y += i;
				break;
			default:
				break;
		}
		
		if(tetrinimo_intersection_helper(temp_r1, temp_r2, intersecting_tetrinimo)){
			move = i-1;
			break;
		}
		
	}
	
	return std::make_pair(true, move);
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
	auto x = SDL_SCANCODE_UNKNOWN;
	
	while(SDL_PollEvent(&e)){
		auto keyPressed = e.key.keysym.scancode;
		
		if(e.type == SDL_QUIT){
			quit = true; break;
		}
		
		switch (keyPressed) {
			case SDL_SCANCODE_ESCAPE:
				quit = true;
				break;
				
			default:
				x = keyPressed;
				break;
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
		
		case SDL_SCANCODE_D:
			//TODO: add rotate functionality.
			break;
			
		case SDL_SCANCODE_LEFT:
			
			move_left(); break;
		
		case SDL_SCANCODE_LSHIFT:
			std::cout << "kosi\n";
			lock_logic(); break;
			
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
