#include <iostream>
#include <math.h>
#include <vector>
#include <SDL2/SDL.h>
#pragma comment (lib, "SDL2.lib")

#undef main

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;
const int SCREEN_FPS = 60;

//global window and renderer
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
//SDL_Surface *windowSurf = NULL;
//Uint32 *pixels = NULL;

class Complex
{
public:
	int x;
	int y;
	void rotateCW();
	void rotateCCW();

	Complex operator+(const Complex &c)
	{
		Complex sum{ this->x + c.x, this->y + c.y };
		return sum;
	}
	Complex operator-(const Complex &c)
	{
		Complex sum{ this->x - c.x, this->y - c.y };
		return sum;
	}
	Complex operator+=(const Complex &c)
	{
		this->x += c.x;
		this->y += c.y;
		return *this;
	}
	Complex operator*(int n)
	{
		Complex product{ this->x * n, this->y * n };
		return product;
	}
};

void Complex::rotateCW()
{
	if (y == -1) { x = 1; y = 0; }
	else if (x == 1) { x = 0; y = 1; }
	else if (y == 1) { x = -1; y = 0; }
	else if (x == -1) { x = 0; y = -1; }
}

void Complex::rotateCCW()
{
	if (y == -1) { x = -1; y = 0; }
	else if (x == -1) { x = 0; y = 1; }
	else if (y == 1) { x = 1; y = 0; }
	else if (x == 1) { x = 0; y = -1; }
}

std::vector<Complex> fullSeq;

class Sequence
{
public:
	Complex c1;
	Complex c2;
	Complex c3;
	Complex c4;

	Sequence(Complex _c1, Complex _c2, Complex _c3, Complex _c4);
	Sequence flipNE();
	Sequence flipNW();
};

Sequence::Sequence(Complex _c1, Complex _c2, Complex _c3, Complex _c4)
{
	c1 = _c1;
	c2 = _c2;
	c3 = _c3;
	c4 = _c4;
}

Sequence Sequence::flipNE()
{
	Sequence res{ c1, c4, c3, c2 };
	return res;
}

Sequence Sequence::flipNW()
{
	Sequence res{ c3, c2, c1, c4 };
	return res;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Gravity simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			}
		}
	}

	return success;
}

void close()
{
	//Destroy global things	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void sequentize(Sequence seq, Complex pos, unsigned iter)
{
	if (iter == 0)
	{
		fullSeq.push_back(pos + seq.c1);
		fullSeq.push_back(pos + seq.c2);
		fullSeq.push_back(pos + seq.c3);
		fullSeq.push_back(pos + seq.c4);
	}
	else
	{
		int multi = 2 << (iter - 1);

		sequentize(seq.flipNE(), pos + seq.c1 * multi, iter - 1);
		sequentize(seq, pos + seq.c2 * multi, iter - 1);
		sequentize(seq, pos + seq.c3 * multi, iter - 1);
		sequentize(seq.flipNW(), pos + seq.c4 * multi, iter - 1);

	}
}

int main()
{
	//Start up SDL and create window
	printf("Initializing SDL... ");
	if (init()) printf("success\n");

	//Event handler
	SDL_Event e;
	//Main loop flag
	bool quit = false;
	bool update = true;

	Sequence seq{ { 0,0 },{ 0,1 },{ 1,1 },{ 1,0 } };

	Complex position{ 0,0 };

	int scale = 4;
	int offsetX = 1;
	int offsetY = 1;

	sequentize(seq, position, 7);

	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);

	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	for (unsigned i = 0; i < fullSeq.size(); i++)
	{
		if (i > 0)
		{
			SDL_RenderDrawLine(
				gRenderer,
				fullSeq[i - 1].x*scale + offsetX,
				fullSeq[i - 1].y*scale + offsetY,
				fullSeq[i].x*scale + offsetX,
				fullSeq[i].y*scale + offsetY
			);
		}

		//pixels[(fullSeq[i].y* windowSurf->w) + fullSeq[i].x] = 0xffffffff;
		//std::cout << fullSeq[i].x << ", " << fullSeq[i].y << std::endl;
	}
	//SDL_RenderDrawLines(gRenderer, fullSeq, fullSeq.size());


	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		//Update screen
		SDL_RenderPresent(gRenderer);
	}

	close();
}