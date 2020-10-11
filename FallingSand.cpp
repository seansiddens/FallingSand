#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>


enum Elements {
	AIR,
	SAND,
	WATER,
	DIRT
};

// Initialize global variables
// Window we'll be rendering too
SDL_Window* gWindow = NULL;

// Surface contained by the window
SDL_Surface* gScreenSurface = NULL;

// image we will load onto screen
SDL_Surface* gHelloWorld = NULL;

// window renderer
SDL_Renderer* gRenderer = NULL;

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const unsigned int ELEMENT_LEN = 4;

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

Color colors[ELEMENT_LEN];

int densities[] = { -1, 99, 1, 999};

struct Pixel {
	unsigned char type;
	bool moved;
	Color color;
};

// Initialize map array 
struct Pixel map[SCREEN_HEIGHT][SCREEN_WIDTH];


// Function declarations

// Starts up SDL and creates window
bool init();

// Initialize colors
void initColors();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// Update map logic for each time step
void updateMap();

// Draw particles to screen
void drawParticles(int xpos, int ypos, int radius, Elements element);

// Clear screen
void clearMap();

bool init()
{
	// Initialization flag
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		// Create window
		gWindow = SDL_CreateWindow("Falling Sand", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				// Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				// Initialize colors
				initColors();

				// Initialize map
				for (int y = 0; y < SCREEN_HEIGHT; y++)
				{
					for (int x = 0; x < SCREEN_WIDTH; x++)
					{
						map[y][x].type = AIR;
						map[y][x].color = colors[AIR];
						map[y][x].moved = false;
					}
				}
			}
			//// Get window surface
			//gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}
	return success;
}

void initColors() 
{
	colors[AIR] = { 0, 0, 0, 255 };
	colors[SAND] = { 255, 255, 0, 255 };
	colors[WATER] = { 0, 0, 255, 255 };
	colors[DIRT] = { 139, 69, 19, 255 };
}


bool loadMedia()
{
	// Loading success flag
	bool success = true;

	//// Load splash image
	//gHelloWorld = SDL_LoadBMP("sample.bmp");
	//if (gHelloWorld == NULL)
	//{
	//	printf("Unable to load image %s! SDL Error: %s\n", "sample.bmp", SDL_GetError());
	//	bool success = false;
	//}

	return success;
}


void close()
{
	// Deallocate the surface
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	// Destroy the window
	SDL_DestroyWindow(gWindow);

	// Quit SDL Subsystem
	SDL_Quit();
}

void updateMap()
{
	unsigned char moveDir;
	unsigned char temp;
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			// Check for sand
			if (map[y][x].type == SAND && map[y][x].moved == false)
			{
				// Check bounds
				if (y + 1 < SCREEN_HEIGHT)
				{
					// Try to move to empty spot below
					if (densities[map[y + 1][x].type] < densities[map[y][x].type])
					{
						temp = map[y + 1][x].type;
						map[y + 1][x].type = SAND;
						map[y + 1][x].color = colors[SAND];
						map[y + 1][x].moved = true;
						map[y][x].type = temp;
					}
					else if (x + 1 < SCREEN_WIDTH && x - 1 >= 0)
					{
						if (map[y + 1][x + 1].type == AIR && map[y + 1][x - 1].type == AIR)
						{
							moveDir = SDL_GetTicks() % 2;
							if (moveDir == 0)
							{
								map[y + 1][x + 1].type = SAND;
								map[y + 1][x].color = colors[SAND];
								map[y + 1][x + 1].moved = true;
								map[y][x].type = AIR;
							}
							else
							{
								map[y + 1][x - 1].type = SAND;
								map[y + 1][x].color = colors[SAND];
								map[y + 1][x - 1].moved = true;
								map[y][x].type = AIR;
							}
						}
						else if (map[y + 1][x + 1].type == AIR && map[y + 1][x - 1].type != AIR)
						{
							map[y + 1][x + 1].type = SAND;
							map[y + 1][x].color = colors[SAND];
							map[y + 1][x + 1].moved = true;
							map[y][x].type = AIR;
						}
						else if (map[y + 1][x - 1].type == AIR)
						{
							map[y + 1][x - 1].type = SAND;
							map[y + 1][x].color = colors[SAND];
							map[y + 1][x - 1].moved = true;
							map[y][x].type = AIR;
						}
					}
				}
			}
			// End sand check

			// Check for water
			if (map[y][x].type == WATER && map[y][x].moved == false)
			{
				// Check bounds
				if (y + 1 < SCREEN_HEIGHT)
				{
					// Try to move to empty spot below
					if (densities[map[y + 1][x].type] < densities[map[y][x].type])
					{
						map[y + 1][x].type = WATER;
						map[y + 1][x].color = colors[WATER];
						map[y + 1][x].moved = true;
						map[y][x].type = AIR;
					}
					// Not out of bounds to left and right
					else if (x + 1 < SCREEN_WIDTH && x - 1 >= 0)
					{
						// If both bottom diag spaces are open
						if (map[y + 1][x + 1].type == AIR && map[y + 1][x - 1].type == AIR)
						{
							moveDir = SDL_GetTicks() % 2;
							if (moveDir == 0)
							{
								map[y + 1][x + 1].type = WATER;
								map[y + 1][x + 1].color = colors[WATER];
								map[y + 1][x + 1].moved = true;
								map[y][x].type = AIR;
							}
							else
							{
								map[y + 1][x - 1].type = WATER;
								map[y + 1][x - 1].color = colors[WATER];
								map[y + 1][x - 1].moved = true;
								map[y][x].type = AIR;
							}
						}
						// If only one of the bottom diag spaces are open
						else if (map[y + 1][x + 1].type == AIR && map[y + 1][x - 1].type != AIR)
						{
							map[y + 1][x + 1].type = WATER;
							map[y + 1][x + 1].color = colors[WATER];
							map[y + 1][x + 1].moved = true;
							map[y][x].type = AIR;
						}
						else if (map[y + 1][x - 1].type == AIR)
						{
							map[y + 1][x - 1].type = WATER;
							map[y + 1][x - 1].color = colors[WATER];
							map[y + 1][x - 1].moved = true;
							map[y][x].type = AIR;
						}
						// Check for something above
						else if (map[y + 1][x].type != AIR)
						{
							// Both spaces to left and right are open
							if (map[y][x + 1].type == AIR && map[y][x - 1].type == AIR)
							{
								moveDir = SDL_GetTicks() % 2;
								if (moveDir == 0)
								{
									map[y][x + 1].type = WATER;
									map[y][x + 1].color = colors[WATER];
									map[y][x + 1].moved = true;
									map[y][x].type = AIR;
								}
								else
								{
									map[y][x - 1].type = WATER;
									map[y][x - 1].color = colors[WATER];
									map[y][x - 1].moved = true;
									map[y][x].type = AIR;
								}

							}
							// Only one of the spaces to left or right are open
							else if (map[y][x + 1].type == AIR && map[y][x - 1].type != AIR)
							{
								map[y][x + 1].type = WATER;
								map[y][x + 1].color = colors[WATER];
								map[y][x + 1].moved = true;
								map[y][x].type = AIR;
							}
							else if (map[y][x - 1].type == AIR && map[y][x + 1].type != AIR)
							{
								map[y][x - 1].type = WATER;
								map[y][x - 1].color = colors[WATER];
								map[y][x - 1].moved = true;
								map[y][x].type = AIR;
							}
						}
					}
				}
			}
			// End water check
		}
	}
}


//Drawing a filled circle at a given position with a given radius and a given partice type
void drawParticles(int xpos, int ypos, int radius, Elements type)
{
	for (int x = ((xpos - radius - 1) < 0) ? 0 : (xpos - radius - 1); x <= xpos + radius && x < SCREEN_WIDTH; x++)
		for (int y = ((ypos - radius - 1) < 0) ? 0 : (ypos - radius - 1); y <= ypos + radius && y < SCREEN_HEIGHT; y++)
		{
			if ((x - xpos) * (x - xpos) + (y - ypos) * (y - ypos) <= radius * radius)
			{
				map[y][x].type = type;
				map[y][x].color = colors[type];
			}
		};
}

void clearMap()
{
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			map[y][x].type = AIR;
			map[y][x].moved = false;
		}
	}
}


int main(int argc, char* args[])
{
	// Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		// Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			// Main loop flag
			bool quit = false;

			// Event handler
			SDL_Event e;
			bool mousePressed = false;
			Elements currentElementType = SAND;
			unsigned int brushSize = 1;

			// While application is running
			while (!quit)
			{
				// Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					switch (e.type)
					{ 
					case SDL_QUIT:
						quit = true;
						break;

					case SDL_KEYDOWN:
						switch (e.key.keysym.sym)
						{
						case SDLK_ESCAPE:
							quit = true;
							break;

						case SDLK_0:
							currentElementType = AIR;
							break;

						case SDLK_1:
							currentElementType = SAND;
							break;

						case SDLK_2:
							currentElementType = WATER;
							break;

						case SDLK_3:
							currentElementType = DIRT;
							break;

						case SDLK_LEFTBRACKET:
							if (brushSize > 1) brushSize--;
							break;

						case SDLK_RIGHTBRACKET:
							brushSize++;
							break;
							
						case SDLK_DELETE:
							clearMap();
							break;
						}

					case SDL_MOUSEBUTTONUP:
						if (e.button.button == SDL_BUTTON_LEFT)
						{
							mousePressed = false;
						}
						break;

					case SDL_MOUSEBUTTONDOWN:
						if (e.button.button == SDL_BUTTON_LEFT)
						{
							mousePressed = true;
						}
						break;

					}
				}
				
				// Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderClear(gRenderer);

				if (mousePressed)
				{
					int x, y;
					SDL_GetMouseState(&x, &y);
					drawParticles(x, y, brushSize, currentElementType);
				}

				// Iterate over map buffer and draw pixels
				for (int y = 0; y < SCREEN_HEIGHT; y++)
				{
					for (int x = 0; x < SCREEN_WIDTH; x++)
					{
						if (map[y][x].type != AIR)
						{
							SDL_SetRenderDrawColor(gRenderer, map[y][x].color.r, 
								                              map[y][x].color.g,
								                              map[y][x].color.b,
								                              map[y][x].color.a);
							SDL_RenderDrawPoint(gRenderer, x, y);
							map[y][x].moved = false;
						}
					}
				}

				// Update screen
				SDL_RenderPresent(gRenderer);

				// Update map buffer
				updateMap();
			}
		}
	}
	close();
	return 0;
}
