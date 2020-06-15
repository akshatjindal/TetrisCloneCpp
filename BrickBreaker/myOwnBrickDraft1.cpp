#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>

#define WIDTH 620
#define HEIGHT 720
#define SPEED 40
#define BrickWidth 40
#define BrickHeight 8
#define SPACING 20
#define BALLSPEED 12

enum class Color:unsigned char{green, brown, red};//enum class.
class Brick{
	Color color = Color::green; //green by default
	int strength = 50; //50 if green, 100 if brown, 200 if red
	
	
public:
	SDL_Rect rect;
	Brick()
	{
		rect.w = BrickWidth;
		rect.h = BrickHeight;
	}//default constructor
	Brick(Color _color)
	{
		this->color = _color;
		if(_color == Color::brown){strength = 100;}
		if(_color == Color::red){strength = 200;}
		rect.w = BrickWidth;
		rect.h = BrickHeight;
	}
	Color getColor()
	{
		return this->color;
	}
	int getStrength()
	{
		return this->strength;
	}
	void decreaseStrength(int _in)
	{
		this->strength -= _in;
	}
	void updateColor()
	{
		if(strength > 0 && strength <= 50)
			this->color = Color::green;
		else if(strength <= 100)
			this->color = Color::brown;
		else
			this->color = Color::red;
	}//.update.color.
	
};//brick class

SDL_Window * window = NULL;
SDL_Renderer * la_rend = NULL;
TTF_Font * font = NULL;
SDL_Rect paddle, lives, ball;
std::vector<Brick *> bricks;
int frameCount, timerFPS, lastFrame, fps;
float velX, velY;
bool quit = false;
int count = 0;
void writeCount();
void freeAndCloseSystems(){
	TTF_CloseFont(font);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(la_rend);
	SDL_Quit();
}//freeAndCloseSystems func

void setUpSystems(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		std::cout << SDL_GetError() << std::endl;
	}
	
	if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &la_rend) < 0){
		std::cout << SDL_GetError() << std::endl;
	}
	
	SDL_SetWindowTitle(window, "Brick Breaker");
	if(TTF_Init() < 0)
		std::cout << "TTF_Init didnt work" << std::endl;
	
	font = TTF_OpenFont("./resources/font.ttf", 15);
	
}//setUpSystems func

void setUpBricks(){
	bricks.reserve(15);

	int temp = 30; int tempTwo = 30; int tempThree = 30; int tempFour = 30;
	for(int i = 0; i < 30; ++i){
		Brick * a = NULL;
		if(i % 2 == 0){
			a = new Brick(Color::red);
		}
		
		else
			a = new Brick(Color::green);
		
		if(i < 10){
			a->rect.x = temp + SPACING*(i+1);
			temp = a->rect.x;
			a->rect.y = 50;
		}
		
		if(i >= 10 && i < 20){
			a->rect.x = tempTwo + SPACING * (i+1);
			tempTwo = a->rect.x;
			a->rect.y = 100;
		}
		
		if(i>= 20 && i < 25){
			a->rect.x = tempThree + SPACING * (25+1-i);
			tempThree = a->rect.x;
			a->rect.y = 150;
		}
		
		if(i>= 25){
			a->rect.x = tempFour + SPACING * (30+1-i);
			tempFour = a->rect.x;
			a->rect.y = 200;
		}
		
		if(i)
		
		bricks.push_back(a);
		
	}//forloop
	
	
}

void renderBricks(){
	for(int i = 0; i < bricks.size(); ++i){
		if(bricks[i] == nullptr)
			continue;
		if(bricks[i]->getColor() == Color::green){
			SDL_SetRenderDrawColor(la_rend, 0, 128, 0, 255);
		}
		
		else if(bricks[i]->getColor() == Color::brown){
			SDL_SetRenderDrawColor(la_rend, 165, 42, 42, 255);
		}
		
		else if (bricks[i]->getColor() == Color::red){
			SDL_SetRenderDrawColor(la_rend, 255, 99, 71, 255);
		}
		
		SDL_RenderFillRect(la_rend, &(bricks[i]->rect));
	}//forloop
	
	
}

void freeBricks(){
	for(int i = 0; i < bricks.size(); ++i){
		if(bricks[i] != nullptr)
			delete bricks[i];
	}//forloop
}//freeBricks func

void update()
{
	bool ballAndPaddleIntersection = false;
	if(SDL_HasIntersection(&ball, &paddle)) {
		double rel=(paddle.x+(paddle.w/2))-(ball.x+(12/2));
		double norm=rel/(paddle.w/2);
		double bounce = norm* (5*3.1415926/12);
		velY=-BALLSPEED*cos(bounce);
		velX=BALLSPEED*-sin(bounce);
		ballAndPaddleIntersection = 1;
	}//if..
	
	if(ball.y + 12 >=  HEIGHT)
		velY = -velY;
	
	if(ball.y <= 0)
		velY = -velY;
	
	if(ball.x <= 0 or ball.x + 12 >= WIDTH)
		velX = -velX;
	
	ball.x += velX; ball.y += velY;
	
	if(paddle.x < 0)
		paddle.x = 0;
	
	if(paddle.x + paddle.w > WIDTH)
		paddle.x = WIDTH - paddle.w;
	
	if(not ballAndPaddleIntersection){
		for(int i = 0; i < bricks.size(); ++i){
			if(bricks[i] == nullptr) continue;
			if(SDL_HasIntersection(&ball, &(bricks[i]->rect))){
				bricks[i]->decreaseStrength(50);
				if(bricks[i]->getStrength() <= 0){
					delete bricks[i];
					bricks[i] = nullptr;
				}//if getstrength
			}//if sdl intersec
		}//for...
	}//if not
	
}//updateball func

void input(){
	SDL_Event e;
	//input stuff
	const Uint8 * keyStates = SDL_GetKeyboardState(NULL);
	
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
			std::cout << "kosi" << "\n";
			paddle.x += SPEED;
			++count;
			break;
		}
		
		if(keyStates[SDL_SCANCODE_LEFT]){
			std::cout << "losi" << "\n";
			
			paddle.x -= SPEED;
			++count;
			break;
		}
		
	}//while sdl poll event
}

void render(){
	
	SDL_SetRenderDrawColor(la_rend, 0, 0, 0, 255);
	SDL_RenderClear(la_rend);
	
	frameCount++;
	timerFPS = SDL_GetTicks() - lastFrame;
	if(timerFPS < (1000/60)){
		SDL_Delay((1000/60) - timerFPS);
	}
	
	SDL_SetRenderDrawColor(la_rend, 255, 255, 255, 255);
	SDL_RenderFillRect(la_rend, &ball);
	SDL_RenderFillRect(la_rend, &paddle);
	writeCount();
	
	renderBricks();
	
	SDL_RenderPresent(la_rend);
}

void writeCount(){
	SDL_Surface * text_surface = NULL;
	SDL_Texture * background_texture = NULL;
	SDL_Color color = {255,255,255};
	std::string text = std::to_string(count);
	text_surface = TTF_RenderText_Solid(font,text.c_str(), color);
	
	background_texture = SDL_CreateTextureFromSurface(la_rend, text_surface);
	lives.w = text_surface->w;
	lives.h = text_surface->h;
	lives.x = 200;
	lives.y = HEIGHT/2;
	SDL_FreeSurface(text_surface);
	SDL_RenderCopy(la_rend, background_texture, NULL, &lives);
	SDL_DestroyTexture(background_texture);
}

int main(){
	setUpSystems();
	
	paddle.h = 15; paddle.w = WIDTH/4;
	paddle.x = WIDTH/2 - paddle.w/2;
	paddle.y = HEIGHT - paddle.h - 32;
	ball.x = WIDTH/2; ball.y = HEIGHT/2;
	ball.w = 12; ball.h = 12;
	
	setUpBricks();
	velY = BALLSPEED/2;
	velX = 0;
	
	static int lastTime = 0;
	
	while(quit == false){
		
		lastFrame = SDL_GetTicks();
		if(lastFrame >= lastTime + 1000){
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
		}//if
		
		update();
		input();
		render();
		
	}//while not quit
	
	freeBricks();
	freeAndCloseSystems();
	
	
}
