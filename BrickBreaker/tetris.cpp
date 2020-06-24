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

enum class rotateType: unsigned char{
	left,
	right,
};

class Tetrinomo{
	
private:
	tetrinimotype type;
	tetrinimocolor color;
	bool can_go_further_down = true;
	bool tetrinimo_locked = false;
	bool standing = true;
public:
	SDL_Rect rectOne;
	SDL_Rect rectTwo;
	
	Tetrinomo(){}
	
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
		type = _type;
	}
	
	tetrinimocolor get_color(){
		return this->color;
	}
	
	void exchange_width_height_helper(){
		int tempWidth = this->rectOne.w;
		
		this->rectOne.w = this->rectOne.h;
		this->rectOne.h = tempWidth;
		this->rectTwo.w = this->rectTwo.w;
		this->rectTwo.h = tempWidth;
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
	
	void rotateLeftLogic();
	void rotateRightLogic();

	void set_standing(bool _standing){
		this->standing = _standing;
	}
	
	bool get_standing(){
		return this->standing;
	}
	
	void rotate(rotateType _rotate){
		
		if(_rotate == rotateType::left){
			rotateLeftLogic();
		}
		
		else if(_rotate == rotateType::right){
			rotateRightLogic();
		}
	
		
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
	
	bool currently_intersecting_another_tetrinimo(const SDL_Rect & r1, const SDL_Rect &r2);
	void rect_manipulator_helper(movetype _move, SDL_Rect &r1, SDL_Rect &r2, bool manipulate);
	
	bool tetrinimo_intersection_helper(const SDL_Rect & r1, const SDL_Rect &r2, const Tetrinomo * _tet);
	
	bool currently_intersects_a_boundary();
	std::pair<bool, int> intersects_down();
	std::pair<bool, int> intersects_another_tetrinimo(movetype _move);
	std::pair<bool, int> intersects_left();
	std::pair<bool, int> intersects_right();
	bool tetrinimo_right_under_active();
	
	void spawn_tetrinimo(){
		
		Tetrinomo * a = new Tetrinomo(tetrinimotype::three_and_one,300,200, tetrinimocolor::pink);
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
		
//	SDL_Rect r1 = act->rectOne; SDL_Rect r2 = act->rectTwo;
//	assert(not currently_intersecting_another_tetrinimo(r1, r2));
		
		auto p = intersects_down();
		bool intersects_down = p.first;
		if(not intersects_down){
			auto c = intersects_another_tetrinimo(movetype::down);
			bool intersects_another_tet = c.first;
			if(intersects_another_tet){
				act->rectOne.y += c.second;
				act->rectTwo.y += c.second;
				
			}
			
			else if(not intersects_another_tet){
				act->rectOne.y += p.second;
				act->rectTwo.y += p.second;
			}
		}
		
//		r1 = act->rectOne; r2 = act->rectTwo;
//		assert(not currently_intersecting_another_tetrinimo(r1, r2));
	}
	
	void move_right(){
		
//		SDL_Rect & r1 = act->rectOne; SDL_Rect & r2 = act->rectTwo;
//		assert(not currently_intersecting_another_tetrinimo(r1, r2));
		
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
		
//		assert(not currently_intersecting_another_tetrinimo(r1, r2));
	}
	
	void move_left(){
//		SDL_Rect r1 = act->rectOne; SDL_Rect r2 = act->rectTwo;
//		assert(not currently_intersecting_another_tetrinimo(r1, r2));
		
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
		
//		r1 = act->rectOne; r2 = act->rectTwo;
//		assert(not currently_intersecting_another_tetrinimo(r1, r2));
	}//move_left func
	
	
	
	void drop(){
		
	}
	
	void rotate(rotateType _r){
		
		act->rotate(_r);
		
		//TODO: once you rotate, you wanna check if this is collidi
		//..ing w/ some other tetrinimo.
		
		if(currently_intersecting_another_tetrinimo(act->rectOne, act->rectTwo) || currently_intersects_a_boundary()){

			std::cout << "awena\n";
			
			if(_r == rotateType::right)
				{
					std::cout << "losi\n";
					act->rotate(rotateType::left);
				}
			else
				{
					act->rotate(rotateType::right);
				}
		}
		
		else{
			act->set_standing(!act->get_standing());
		}

	}
	
public:
	
	void update(SDL_Scancode a);
};

bool Game::currently_intersecting_another_tetrinimo(const SDL_Rect & r1, const SDL_Rect &r2){
	
	for(size_t i = 0; i < index_of_active_tetrinimo; ++i){
		if(tetrinimo_intersection_helper(r1, r2, the_tetrinomos[i]))
			return true;
	}
	
	return false;
}

void Tetrinomo::rotateLeftLogic(){
	
	if(this->type == tetrinimotype::box)
		return;
	
	
	if(not this->standing)
		{
			this->rectOne.y -= (this->rectOne.w - 20);
			this->rectTwo.y -= (this->rectTwo.w - 20);
		}
	else if(this->standing){
		this->rectOne.y += (this->rectOne.h - 20);
		this->rectOne.x -= (this->rectOne.h - 20);
		
		this->rectTwo.y += (this->rectTwo.h - 20);
		this->rectTwo.x -= (this->rectTwo.h - 20);
	}
	
	exchange_width_height_helper();
	
}

void Tetrinomo::rotateRightLogic(){
	
	if(this->type == tetrinimotype::box) return;
	
	if(this->type == tetrinimotype::looong){
		
		std::cout << standing << "\n";
		std::cout << "trois\n" ;

		this->rectOne.y += (this->rectOne.h - 20);
		this->rectTwo.y += (this->rectOne.h - 20);
		exchange_width_height_helper();
	}
	
	if(this->type == tetrinimotype::three_and_one){
		std::cout << "five\n";
		
		this->rectOne.y += (this->rectOne.h - 20);
		this->rectTwo.y = this->rectOne.y - 30;
		
		int tempWidthOne = this->rectOne.w; int tempWidthTwo = this->rectTwo.w;
		this->rectOne.w = this->rectOne.h;
		this->rectOne.h = tempWidthOne;
		
		this->rectTwo.w = this->rectTwo.h;
		this->rectTwo.h = tempWidthTwo;
		this->rectTwo.x = this->rectOne.x + 30;
		this->rectTwo.y = this->rectOne.y + this->rectOne.h - 30;
		
	}

	

}

bool Game::currently_intersects_a_boundary(){
	
	bool flag = false;
	
	auto temp_r1 = act->rectOne; auto temp_r2 = act->rectTwo;
	if(SDL_HasIntersection(&temp_r1, bottom_boundary)
	   || SDL_HasIntersection(&temp_r2, bottom_boundary)){
//		act->set_cant_go_further_down();
		flag = 1;
	}
	
	else if(SDL_HasIntersection(left_boun, &temp_r1) || SDL_HasIntersection(left_boun, &temp_r2))
		flag = 1;
	
	else if(SDL_HasIntersection(right_boundary, &act->rectOne) || SDL_HasIntersection(right_boundary, &act->rectTwo))
		flag = 1;
	
	return flag;
}


std::pair<bool, int> Game::intersects_down(){
	
	auto temp_r1 = act->rectOne; auto temp_r2 = act->rectTwo;
	temp_r1.y += 10; temp_r2.y += 10;
	
	if(SDL_HasIntersection(&temp_r1, bottom_boundary)
	   || SDL_HasIntersection(&temp_r2, bottom_boundary)){
		act->set_cant_go_further_down();
		return std::make_pair(true, 0);
	}
	
	if(not SDL_HasIntersection(&temp_r1, bottom_boundary) && not SDL_HasIntersection(&temp_r2, bottom_boundary))
		return std::make_pair(false, 10);
	
	temp_r1.y += 10; temp_r2.y +=10;
	
	int move = 0;
	for(int i = 1; i < 10; ++i){
		temp_r1.y += i; temp_r2.y += i;
		if(SDL_HasIntersection(&temp_r1, bottom_boundary) || SDL_HasIntersection(&temp_r2, bottom_boundary)){
			move = i-1; break;
		}
	}
	
	return std::make_pair(false, move);
}

bool Game::tetrinimo_intersection_helper(const SDL_Rect & r1, const SDL_Rect &r2, const Tetrinomo * _tet){
	if(
	   SDL_HasIntersection(&r1, &_tet->rectOne) ||
	   SDL_HasIntersection(&r1, &_tet->rectTwo) ||
	   SDL_HasIntersection(&r2, &_tet->rectOne) ||
	   SDL_HasIntersection(&r2, &_tet->rectTwo)
	   ){
		return true;
	}
	
		return false;
}

void Game::rect_manipulator_helper(movetype _move, SDL_Rect &r1, SDL_Rect &r2, bool manipulate){
	switch (_move) {
		case movetype::right :
			if(manipulate)
				{r1.x += 20; r2.x += 20; break; }
			if(not manipulate)
				{r1.x -= 20; r2.x -= 20; break; }
			break;
			
		case movetype::left :
			if(manipulate)
				{r1.x -= 20; r2.x -= 20; break; }
			if(not manipulate)
				{r1.x += 20; r2.x += 20; break; }
			
		case movetype::down :
			if(manipulate)
				{r1.y += 10; r2.y += 10; break; }
			if(not manipulate)
				{r1.y -= 10; r2.y -= 10; break; }
			
		default:
			break;
	}
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
				{temp_r1.x += i; temp_r2.x += i;
					break;}
				
			case movetype::left :
				{temp_r1.x -= i; temp_r2.x -= i;
					break;}
				
			case movetype::down :
				{temp_r1.y += i; temp_r2.y += i;
					break;}
				
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
		{
			std::cout << "right\n";
			rotate(rotateType::right);
			break;
		}
			
		case SDL_SCANCODE_A:
		{
			std::cout << "kosi\n";
			rotate(rotateType::left);
			break;
		}
		case SDL_SCANCODE_LEFT:
			
			move_left(); break;
		
		case SDL_SCANCODE_LSHIFT:
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
//
//	SDL_Rect * four = new SDL_Rect();
//	four->x = 390; four->y = 470;
//	four->w = 90;
//	four->h = 30;
//
//
//	auto five = new SDL_Rect();
//	five->x = 440; five->y = 490;
//	five->w = 60;
//	five->h = 30;
	
	boundary_objects.push_back(one);
	boundary_objects.push_back(two);
	boundary_objects.push_back(three);
//	boundary_objects.push_back(four);
//	boundary_objects.push_back(five);


}

void drawBoundary()
{
	SDL_SetRenderDrawColor(la_rend, 139, 0, 0, 255);
	for(auto i = 0; i < boundary_objects.size(); ++i){
		if(i == 4)
		{SDL_SetRenderDrawColor(la_rend, 0, 0, 205, 255);}
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
		
		if(obj->get_color() == tetrinimocolor::pink)
			SDL_SetRenderDrawColor(la_rend, 75, 0, 100, 255);
		
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
