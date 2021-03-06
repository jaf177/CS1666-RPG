#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <cmath>
#include <fstream>
#include "../Headers/Globals.h"
#include <SDL2/SDL_net.h>
#include "../Headers/mainGame.h"
#include "../Headers/Player.h"
#include "../Headers/Button.h"
#include "../Headers/Tile.h"
#include "../Headers/CombatManager.h"
#include "../Headers/Cluster.h"
#include "../Headers/LoadTexture.h"
#include "../Headers/ResourceManager/ResourceManager.h"
#include "../Headers/Levelmap.h"

// Function declarations

bool init();//Starts up SDL, creates window, and initializes OpenGL

void close();//Frees media and shuts down SDL
void ThankYouTransition();

SDL_Texture* loadImage(std::string fname);

SDL_Renderer* gRenderer = nullptr;
// Globals

SDL_Window* gWindow = nullptr;//The window rendering to

SDL_GLContext gContext;//OpenGL context

std::vector<SDL_Texture*> gTex;
void handleMain();

/*
OPTIONS MENU
*/
const int OPTION_max_volume = 10;
const int OPTION_min_volume = 0;
int OPTION_volume = 6;
const int OPTION_max_difficulty = 10;
const int OPTION_min_difficulty = 1;
int OPTION_difficulty = 5;
bool OPTION_randomMaps = true;
/*
OPTIONS MENU
*/
int MAP_INDEX = 0;


enum SCENE_CHANGE
{
	GOTO_MAIN,
	GOTO_SOLO,
	GOTO_COOP,
	GOTO_CREDITS,
	GOTO_EXIT,
	GOTO_INGAME,
	GOTO_OPTIONS,
};

Mix_Music *gMusic = NULL;
Mix_Chunk *gBSound = NULL;
Mix_Chunk *soundButtonSuccess_Up = NULL; 
Mix_Chunk *soundButtonSuccess_Down = NULL;
Mix_Chunk *soundButtonFailure = NULL; 
Mix_Chunk *soundMenuSelect = NULL;
TTF_Font* font;

//Player ONE
Player* player1;
std::vector<int> attr;

bool init()
{
    
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	
	// Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
	}

	//set all the required Options for GLFW, Use OpenGL 2.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);//Double-buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//Create window
	gWindow = SDL_CreateWindow("CS1666-RPG", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (gWindow == nullptr)
	{
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//Create rendering context for OpenGL
	gContext = SDL_GL_CreateContext(gWindow);

	if (gContext == NULL)
	{
		std::cout << "OpenGL context could not be created! SDL Error: %s\n" << SDL_GetError() << std::endl;
		return false;
	}

	//Initialize GLEW
	glewExperimental = GL_TRUE; //use the new OpenGL functions and extensions
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
	}
    
	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	/* Create a renderer for our window
	 * Use hardware acceleration (last arg)
	 * Choose first driver that can provide hardware acceleration
	 *   (second arg, -1)
	 */
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == nullptr)
	{
		std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
	}

	// Set renderer draw/clear color
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	
	// Initialize PNG loading via SDL_image extension library
	int imgFlags = IMG_INIT_PNG;
	imgFlags = imgFlags | IMG_INIT_JPG;//add jpg support
	int retFlags = IMG_Init(imgFlags);
	if (retFlags != imgFlags)
	{
		std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return false;
	}
	//initialize the sdl mixer  
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
		//return false;
	}

	if (TTF_Init() == -1)
	{
		std::cout << "TTF could not initialize. Error: %s\n", TTF_GetError();
		return false;
	}
	font = TTF_OpenFont("Fonts/Gameplay.ttf", 20);
	if (font == NULL)
	{
		std::cout << "font was null";
	}

	SDLNet_Init();

	return true;
}

bool check_collision(SDL_Rect a, SDL_Rect b)
{
	// Check vertical overlap
	if (a.y - a.h >= b.y)
		return false;
	if (a.y <= b.y - b.h)
		return false;

	// Check horizontal overlap
	if (a.x >= b.x + b.w)
		return false;
	if (a.x + a.w <= b.x)
		return false;

	return true;
}


SDL_Rect * loadMap(Tile* tiles[MAX_HORIZONTAL_TILES][MAX_VERTICAL_TILES],string mapToLoad)
{
	Tile::loadTiles();
	bool tilesLoaded = true;
	int x = 0, y = 0;
	std::vector<SDL_Rect> blockedTiles;
	std::ifstream map("Maps/"+mapToLoad);
	if (!map.is_open())
	{
		printf("Unable to load map file!\n");
		tilesLoaded = false;
	}
	else
	{
		for (int tempy = 0; tempy < MAX_VERTICAL_TILES; tempy++)
		{
			for (int tempx = 0; tempx < MAX_HORIZONTAL_TILES; tempx++)
			{
				//Determines what kind of tile will be made
				int tileType = -1;

				//Read tile from map file
				map >> tileType;

				//If the was a problem in reading the map
				if (map.fail())
				{
					//Stop loading map
					printf("Error loading map: Unexpected end of file!\n");
					tilesLoaded = false;
					break;
				}

				//If the number is a valid tile number
				if (tileType >= 0)
				{
					tiles[tempx][tempy] = new Tile(x, y, tileType);
					blockedTiles.push_back(tiles[tempx][tempy]->getBox());
				}
				//If we don't recognize the tile type
				else
				{
					//Stop loading map
					printf("Error loading map: Invalid tile type at (%d,%d)!\n", tempx,tempy);
					tilesLoaded = false;
					break;
				}
				x += TILE_WIDTH;

				//If we've gone too far
				if (x >= LEVEL_WIDTH)
				{
					//Move back
					x = 0;

					//Move to the next row
					y += TILE_HEIGHT;
				}
			}
		}
	}
	return &blockedTiles[0];
}


SDL_Texture* loadImage(std::string fname)
{
	SDL_Texture* newText = nullptr;

	SDL_Surface* startSurf = IMG_Load(fname.c_str());
	if (startSurf == nullptr) {
		std::cout << "Unable to load image " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}

	SDL_SetColorKey(startSurf, SDL_TRUE, SDL_MapRGB(startSurf->format, 0, 0xFF, 0xFF));//if the color is 0, 0xFF, 0xFF, it should be cleaned

	newText = SDL_CreateTextureFromSurface(gRenderer, startSurf);
	if (newText == nullptr) {
		std::cout << "Unable to create texture from " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
	}

	SDL_FreeSurface(startSurf);

	return newText;
}

void close()
{
	for (auto i : gTex) {
		SDL_DestroyTexture(i);
		i = nullptr;
	}
	
	SDL_GL_DeleteContext(gContext);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;
	TTF_CloseFont(font);
	font = NULL;
	//Free music
	Mix_FreeMusic(gMusic);
	Mix_FreeChunk(gBSound);
	Mix_FreeChunk(soundButtonSuccess_Up);
	Mix_FreeChunk(soundButtonSuccess_Down);
	Mix_FreeChunk(soundButtonFailure);
	gMusic = NULL;
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}
int adjustVolume(int initial)
{
	return initial * OPTION_volume / 6;
}
void startMusic(string song, int volume)
{
	gMusic = Mix_LoadMUS(song.c_str());
	if (gMusic == NULL)
		std::cout << "Failed to load music" << std::endl;
	Mix_PlayMusic(gMusic, -1);
	Mix_VolumeMusic(adjustVolume(volume));
}
void animateCharacter(Character* i, SDL_Rect camera)
{
	if (SDL_GetTicks() - i->timeSinceLastAnimation > i->getTimeBetweenAnimations())
	{
		i->currentFrame = (i->currentFrame + 1) % i->getNumAnimationFrames();
		i->timeSinceLastAnimation = SDL_GetTicks();
	}

	i->drawRectangle.x = i->currentFrame * i->getImageWidth();
	i->rectangle.x = (int)i->xPosition - camera.x;
	i->rectangle.y = (int)i->yPosition - camera.y;
	SDL_RenderCopyEx(gRenderer, i->getSpriteTexture(), &i->drawRectangle, &i->rectangle, 0.0, nullptr, i->flip);
}

int playCredits() {
	
	string imagePath = "Images/Credits/";
	gTex.push_back(loadImage(imagePath+"dsgCredits.jpg"));
	gTex.push_back(loadImage(imagePath + "RyanKillenCreditImage.jpg")); //Ryan Killen - rek77
 	gTex.push_back(loadImage(imagePath + "bmbCredits.jpg"));
	gTex.push_back(loadImage(imagePath + "dank_farnan_meme.jpg")); //Austin Himler - arh121
	gTex.push_back(loadImage(imagePath + "Kexin Wang.jpg"));
	gTex.push_back(loadImage(imagePath + "justin.jpg"));
	gTex.push_back(loadImage(imagePath + "my_greatest_creation.png")); // jake
	gTex.push_back(loadImage(imagePath + "ilum.jpg")); // James Finkel
	gTex.push_back(loadImage(imagePath + "SankethKolliCredit.jpg")); //Sanketh Kolli - ssk38
	gTex.push_back(loadImage(imagePath + "mjl159Credits.png")); //Mitchell Leng - mjl159

	startMusic("Audio/Song_Credits.wav", MIX_MAX_VOLUME / 8);

//This is for the actual credits
	SDL_Event e;
	int j = 0;
	for (auto i : gTex)
	{
		while (j < 101)
		{
			SDL_PollEvent(&e);
			if (e.type == SDL_QUIT)
			{
				Mix_HaltMusic();
				return GOTO_EXIT;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					Mix_HaltMusic();
					return GOTO_MAIN;
				}
			}
			if (j == 0)
			{
				SDL_RenderCopy(gRenderer, i, NULL, NULL);
				SDL_RenderPresent(gRenderer);

			}
			SDL_Delay(20);
			j++;
		}
		j = 0;
	}
	ThankYouTransition();
	Mix_HaltMusic();
	return GOTO_MAIN;
}


void renderText(const char* text, SDL_Rect* rect, SDL_Color* color)
{
	SDL_Surface* surface;
	SDL_Texture* texture;

	surface = TTF_RenderText_Solid(font, text, *color);
	texture = SDL_CreateTextureFromSurface(gRenderer, surface);
	rect->w = surface->w;
	rect->h = surface->h;
	SDL_FreeSurface(surface);
	SDL_RenderCopy(gRenderer, texture, NULL, rect);
	SDL_DestroyTexture(texture);
}



void gameStartTransition()
{
	SDL_Rect wipe = { 0,0,72,72 };
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255); 
	vector<SDL_Rect> allRectangles;
	int direction = 0; // 0 -> Down , 1 -> Right , 2 -> Up , 3 -> Left
	int bound_left = 0, bound_up = 0, bound_right = 648, bound_down = 648;
	while (bound_left < bound_right || bound_up < bound_down)
	{			
		SDL_RenderFillRect(gRenderer, &wipe);
		allRectangles.push_back(wipe);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(10);
		switch (direction)
		{
		case 0:
			if (wipe.y == bound_down)
			{
				direction++;
				bound_left += 72;
				wipe.x += 72;
			}
			else wipe.y += 72;
			break;
		case 1:
			if (wipe.x == bound_right)
			{
				direction++;
				bound_down -= 72;
				wipe.y -= 72;
			}
			else wipe.x += 72;
			break;
		case 2:	
			if (wipe.y == bound_up)
			{
				direction++;
				bound_right -= 72;
				wipe.x -= 72;
			}
			else wipe.y -= 72;
			break;
		case 3:
			if (wipe.x == bound_left)
			{
				direction = 0;
				bound_up += 72;
				wipe.y += 72;
			}
			else wipe.x -= 72;
			break;
		}
	}
	SDL_RenderFillRect(gRenderer, &wipe);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(10);
	allRectangles.push_back(wipe);
	
	for (int rectangles = allRectangles.size() - 1; rectangles >= 0; rectangles--)
	{
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderClear(gRenderer);
		SDL_RenderPresent(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		for (int current = 0; current <= rectangles; current++)
		{
			//cout << allRectangles[current].x << " " << allRectangles[current].y << " " << allRectangles[current].h << " " << allRectangles[current].w << endl;
			SDL_RenderFillRect(gRenderer, &allRectangles[current]);	
		}
		SDL_RenderPresent(gRenderer);
		SDL_Delay(10);
	}
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(10);
}

void combatTransition()
{
	startMusic("Audio/Song_CombatTransition.wav", MIX_MAX_VOLUME / 8);

	SDL_Rect wipe = { 0,0,72,72 };
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	for (; wipe.x < 720; wipe.x += 72)
	{
		for (wipe.y = 0; wipe.y < 720; wipe.y += 72)
		{
			SDL_RenderFillRect(gRenderer, &wipe);
			SDL_RenderPresent(gRenderer);
			SDL_Delay(10);
		}
	}
	Mix_PauseMusic();
}
void levelTransition()
{
	SDL_Rect wholeS = { 0,0,720,720 };
	SDL_Rect words = { 220,200,120,60 };
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &wholeS);
	SDL_RenderPresent(gRenderer);
	string level = "Loading Next Level";
	SDL_Color TextColor = { 255, 255, 255, 0 };
	renderText(level.c_str(), &words, &TextColor);
	SDL_RenderPresent(gRenderer);
	SDL_Rect wipe = { 180,240,20,20 };
	SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
	for (; wipe.x < 540; wipe.x += 20)
	{
		SDL_RenderFillRect(gRenderer, &wipe);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(100);
	}
}

void EndTransition()
{
	startMusic("Audio/Song_PlayerVictory.wav", MIX_MAX_VOLUME);
	SDL_Rect wholeS = { 0,0,720,720 };
	SDL_Rect word1 = { 220,200,120,60 };
	SDL_Rect word2 = { 30, 240,120,60};
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &wholeS);
	SDL_RenderPresent(gRenderer);
	string line1 = "Congratulations! You Win!";
	string line2 = "You Defeated the Forces of Evil!";
	SDL_Color TextColor = { 255, 255, 255, 0 };
	renderText(line1.c_str(), &word1, &TextColor);
	renderText(line2.c_str(), &word2, &TextColor);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(200);
}

void ThankYouTransition()
{
	SDL_Rect wholeS = { 0,0,720,720 };
	SDL_Rect word1 = { 220,200,120,60 };
	SDL_Rect word2 = { 220,280,120,60 };
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &wholeS);
	SDL_RenderPresent(gRenderer);
	string line1 = "Thank you for Playing!";
	string line2 = "Returning to Main Menu!";
	SDL_Color TextColor = { 255, 255, 255, 0 };
	renderText(line1.c_str(), &word1, &TextColor);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(5000);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &wholeS);
	renderText(line1.c_str(), &word1, &TextColor);
	renderText(line2.c_str(), &word2, &TextColor);
	SDL_RenderPresent(gRenderer);
	SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
	SDL_Delay(5000);

}

void GameOverTransition()
{
	startMusic("Audio/Song_PlayerDefeat.wav", MIX_MAX_VOLUME);
	SDL_Rect wholeS = { 0,0,720,720 };
	SDL_Rect word1 = { 280,200,120,60 };
	SDL_Rect word2 = { 180, 240,120,60 };
	SDL_Rect word3 = { 110, 280,120,60 };
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &wholeS);
	SDL_RenderPresent(gRenderer);
	string line1 = "Game Over!";
	string line2 = "You have been defeated!";
	string line3 = "You were not up to the challenge!";
	SDL_Color TextColor = { 255, 255, 255, 0 };
	renderText(line1.c_str(), &word1, &TextColor);
	renderText(line2.c_str(), &word2, &TextColor);
	renderText(line3.c_str(), &word3, &TextColor);
	SDL_RenderPresent(gRenderer);
	SDL_Rect wipe = { 180,240,20,20 };
	SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
	SDL_Delay(200);
}
void deleteAll(vector<Button *> buttons)
{
	for (auto i : buttons)
	{
		delete(i);
	}
}
void flashButton(SDL_Texture* one, SDL_Texture* two, Button* i, Mix_Chunk* sound)
{
	int flashTimer = 210;
	Mix_PlayChannel(-1, sound, 0);
	Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
	SDL_RenderCopy(gRenderer, one, NULL, &i->rect);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(flashTimer);
	SDL_RenderCopy(gRenderer, two, NULL, &i->rect);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(flashTimer);
	SDL_RenderCopy(gRenderer, one, NULL, &i->rect);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(flashTimer);
	SDL_RenderCopy(gRenderer, two, NULL, &i->rect);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(flashTimer);
	SDL_RenderCopy(gRenderer, one, NULL, &i->rect);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(flashTimer);
}
int characterCreateScreen()
{
	soundMenuSelect = Mix_LoadWAV("Audio/SoundEffect_MenuSelect.wav");
	if (soundMenuSelect == NULL)
	{
		printf("Failed to load Button sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}
	// Initialize Variables
	bool onCharacterCreate = true;
	int pointsToAllocate = 5;
	int maxStat = 10;
	int minStat = 1;
	int strength = 1;
	int intelligence = 1;
	int dexterity = 1;
	int constitution = 1;
	int faith = 1;
	int charImageX = 0;
	int charImageY = 0;
	int charImageW = 96;
	int charImageH = 132;

	int attribute_summary_x = 0;
	int attribute_summary_y = 0;
	int attribute_summary_width = 300;
	int attribute_summary_height = 200;

	int mouseX = -1;
	int mouseY = -1;


	int delaysPerFrame = 0;
	int frame = 0;
	int attributeX = 245;

	SDL_Rect characterBox = { 490, 225, charImageW, charImageH };
	SDL_Rect pointsAllocatedRectangle = { 227, 32, 0, 0 };
	SDL_Rect strengthTextRectangle = { attributeX, 115, 0, 0 };
	SDL_Rect intelligenceTextRectangle = { attributeX, 205, 0, 0 };
	SDL_Rect dexterityTextRectangle = { attributeX, 302, 0, 0 };
	SDL_Rect constitutionTextRectangle = { attributeX, 395, 0, 0 };
	SDL_Rect faithTextRectangle = { attributeX, 490, 0, 0 };
	SDL_Rect nameTextRectangle = { 150, 640, 0,0 };
	SDL_Rect errorTextRectangle = { 115, 580, 0, 0 };
	SDL_Rect errorTextRectangleLong = { 25, 580, 0, 0 };

	/* these rectangles are for when the player hovers over the ability words */
		/* Strength */
			SDL_Rect strBox = { 200, 100, attribute_summary_width, attribute_summary_height };
			SDL_Texture* strSum = loadImage("Images/UI/CreateScreen/STR_Sum.png");
			SDL_Rect strRectangle = { attribute_summary_x, attribute_summary_y, attribute_summary_width, attribute_summary_height };
		/* End */
		/* Intelligence */
			SDL_Rect intBox = { 200, 160, attribute_summary_width, attribute_summary_height };
			SDL_Texture* intSum = loadImage("Images/UI/CreateScreen/INT_Sum.png");
			SDL_Rect intRectangle = { attribute_summary_x, attribute_summary_y, attribute_summary_width, attribute_summary_height };
		/* End */
		/* Dexterity */
			SDL_Rect dexBox = { 200, 220, attribute_summary_width, attribute_summary_height };
			SDL_Texture* dexSum = loadImage("Images/UI/CreateScreen/DEX_Sum.png");
			SDL_Rect dexRectangle = { attribute_summary_x, attribute_summary_y, attribute_summary_width, attribute_summary_height };
		/* End */
		/* Constitution */
			SDL_Rect conBox = { 200, 280, attribute_summary_width, attribute_summary_height };
			SDL_Texture* conSum = loadImage("Images/UI/CreateScreen/CON_Sum.png");
			SDL_Rect conRectangle = { attribute_summary_x, attribute_summary_y, attribute_summary_width, attribute_summary_height };
		/* End */
		/* Faith */
			SDL_Rect faiBox = { 200, 340, attribute_summary_width, attribute_summary_height };
			SDL_Texture* faiSum = loadImage("Images/UI/CreateScreen/FAI_Sum.png");
			SDL_Rect faiRectangle = { attribute_summary_x, attribute_summary_y, attribute_summary_width, attribute_summary_height };
		/* End */
	/* End */

	SDL_Color textColor = { 112, 96, 80, 0 };
	SDL_Color errorColor = { 255, 0, 0, 0 };
	std::string nameInputText;
	std::string errorInputText;

	std::vector<Button*> buttons;
	SDL_Texture* upPress = loadImage("Images/UI/CreateScreen/pointUpArrow_Pressed.png");
	SDL_Texture* downPress = loadImage("Images/UI/CreateScreen/pointDownArrow_Pressed.png");
	SDL_Texture* upLocked = loadImage("Images/UI/CreateScreen/pointUpArrow_Locked.png");
	SDL_Texture* downLocked = loadImage("Images/UI/CreateScreen/pointDownArrow_Locked.png");

	SDL_Texture* character = loadImage("Images/Player/Idle_Down.png");


	//Establish Visuals
	buttons.push_back(new Button("up", 340, 80, 35, 45, "Images/UI/CreateScreen/pointUpArrow.png", "strength", gRenderer));
	buttons.push_back(new Button("down", 340, 130, 35, 42, "Images/UI/CreateScreen/pointDownArrow.png", "strength", gRenderer));
	buttons.push_back(new Button("up", 340, 175, 35, 45, "Images/UI/CreateScreen/pointUpArrow.png", "intelligence", gRenderer));
	buttons.push_back(new Button("down", 340, 225, 35, 42, "Images/UI/CreateScreen/pointDownArrow.png", "intelligence", gRenderer));
	buttons.push_back(new Button("up", 340, 270, 35, 45, "Images/UI/CreateScreen/pointUpArrow.png", "dexterity", gRenderer));
	buttons.push_back(new Button("down", 340, 320, 35, 42, "Images/UI/CreateScreen/pointDownArrow.png", "dexterity", gRenderer));
	buttons.push_back(new Button("up", 340, 365, 35, 45, "Images/UI/CreateScreen/pointUpArrow.png", "constitution", gRenderer));
	buttons.push_back(new Button("down", 340, 415, 35, 42, "Images/UI/CreateScreen/pointDownArrow.png", "constitution", gRenderer));
	buttons.push_back(new Button("up", 340, 460, 35, 45, "Images/UI/CreateScreen/pointUpArrow.png", "faith", gRenderer));
	buttons.push_back(new Button("down", 340, 510, 35, 42, "Images/UI/CreateScreen/pointDownArrow.png", "faith", gRenderer));
	buttons.push_back(new Button("start", 450, 575, 230, 56, "Images/UI/CreateScreen/StartButton.png", "", gRenderer));
	buttons.push_back(new Button("back", 450, 650, 230, 56, "Images/UI/CreateScreen/BackButton.png", "", gRenderer));

	SDL_Texture* back = loadImage("Images/UI/CreateScreen/BackButton.png");
	SDL_Texture* back_selected = loadImage("Images/UI/CreateScreen/BackButton_Selected.png");
	SDL_Texture* start = loadImage("Images/UI/CreateScreen/StartButton.png");
	SDL_Texture* start_selected = loadImage("Images/UI/CreateScreen/StartButton_Selected.png");

	// Load Background
	LoadTexture background;
	background.loadFromFile("Images/UI/CreateScreen/characterCreateV2NoButtons.png", gRenderer);
	background.renderBackground(gRenderer);

	startMusic("Audio/Song_CharacterCreate_New.wav", MIX_MAX_VOLUME / 8);
	gBSound = Mix_LoadWAV("Audio/BSound.wav");
	soundButtonFailure = Mix_LoadWAV("Audio/SoundEffect_ButtonFailure.wav");
	soundButtonSuccess_Up =  Mix_LoadWAV("Audio/SoundEffect_ButtonSuccess_Up.wav");
	soundButtonSuccess_Down = Mix_LoadWAV("Audio/SoundEffect_ButtonSuccess_Down.wav");
	if (gBSound == NULL || soundButtonFailure == NULL || soundButtonSuccess_Up == NULL || soundButtonSuccess_Down == NULL)
	{
		printf("Failed to load Button sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}


	
	SDL_Event e;
	while (onCharacterCreate)
	{
		while (SDL_PollEvent(&e))
		{

			if (e.type == SDL_QUIT)
			{
				Mix_HaltMusic();
				return GOTO_EXIT; //end game
			}

			if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_KEYDOWN)
			{
				errorInputText = "";
			}
			if (e.button.button == (SDL_BUTTON_LEFT) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				int deltaAttribute;
				for (auto i : buttons)
				{
					//if mouse is clicked inside a button
					if (((mouseX >= i->x) && (mouseX <= (i->x + i->w))) &&
						((mouseY >= i->y) && (mouseY <= (i->y + i->h))))
					{
					if (i->type == "back")
					{
						flashButton(back_selected, back, i, soundMenuSelect);
						deleteAll(buttons);
							
							background.free();
							Mix_HaltMusic();
							return GOTO_MAIN;
					}
					i->pressed = 5;
					if (i->type == "start")
					{
						if (nameInputText == "nlf4" || pointsToAllocate == 0)
							{
								if (nameInputText == "nlf4" || nameInputText != "")
								{
									flashButton(start_selected, start, i, soundMenuSelect);
									onCharacterCreate = false;
									if (nameInputText == "nfl4" || nameInputText == "nlf4")
										player1 = new Player(nameInputText, 10, 10, 10, 10, 10);
									else
										player1 = new Player(nameInputText, strength, intelligence, dexterity, constitution, faith);
									std::cout << std::string(*player1); //displays player 1
									//make Character Object, validate, return to main
									deleteAll(buttons);
									background.free();
									Mix_HaltMusic();
									gameStartTransition();
									return GOTO_INGAME;
								}
								else
								{
									Mix_PlayChannel(-1, soundButtonFailure, 0);
									Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
									errorInputText = "Enter Your Name!";
								}
							}
							else
							{
								Mix_PlayChannel(-1, soundButtonFailure, 0);
								Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
								errorInputText = "Points Remaining!";
								break; //not valid to start, break out of for loop
							}
						}
						else if (i->type == "up")
						{
							if (pointsToAllocate > 0)
							{
								Mix_PlayChannel(-1, soundButtonSuccess_Up, 0);
								Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
								deltaAttribute = 1;
							}
							else
							{
								Mix_PlayChannel(-1, soundButtonFailure, 0);
								Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
								errorInputText = "No Points Remaining!";
								deltaAttribute = 0;
								
							}
						}
						else if (i->type == "down")
						{
							if (i->locked)
							{
								Mix_PlayChannel(-1, soundButtonFailure, 0);
								Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
								deltaAttribute = 0;
							}
							else
							{
								Mix_PlayChannel(-1, soundButtonSuccess_Down, 0);
								Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
								deltaAttribute = -1;
							}					
						}

						if (i->attribute == "strength") {
							if ((deltaAttribute + strength) <= maxStat && (deltaAttribute + strength) >= minStat)
							{
								strength += deltaAttribute;
								pointsToAllocate -= deltaAttribute;
							}
							else if ((deltaAttribute + strength) > maxStat) {
		
								errorInputText = "Max Strength!";
								
							}
							else if ((deltaAttribute + strength) < minStat) {
								errorInputText = "Min Strength!";
								
							}
						}
						else if (i->attribute == "intelligence") {
							if ((deltaAttribute + intelligence) <= maxStat && (deltaAttribute + intelligence) >= minStat) {
								intelligence += deltaAttribute;
								pointsToAllocate -= deltaAttribute;

						     
							}
							else if ((deltaAttribute + intelligence) > maxStat) {
								errorInputText = "Max Intelligence!";
							}
							else if ((deltaAttribute + intelligence) < minStat) {
								errorInputText = "Min Intelligence!";
							}
						}
						else if (i->attribute == "dexterity") {
							if ((deltaAttribute + dexterity) <= maxStat && (deltaAttribute + dexterity) >= minStat) {
								dexterity += deltaAttribute;
								pointsToAllocate -= deltaAttribute;
							}
							else if ((deltaAttribute + dexterity) > maxStat) {
								errorInputText = "Max Dexterity!";
							}
							else if ((deltaAttribute + dexterity) < minStat) {
								errorInputText = "Min Dexterity!";
							}
						}
						else if (i->attribute == "constitution") {
							if ((deltaAttribute + constitution) <= maxStat && (deltaAttribute + constitution) >= minStat) {
								constitution += deltaAttribute;
								pointsToAllocate -= deltaAttribute;
							}
							else if ((deltaAttribute + constitution) > maxStat) {
								errorInputText = "Max Constitution!";
							}
							else if ((deltaAttribute + constitution) < minStat) {
								errorInputText = "Min Constitution!";
							}
						}
						else if (i->attribute == "faith") {
							if ((deltaAttribute + faith) <= maxStat && (deltaAttribute + faith) >= minStat) {
								faith += deltaAttribute;
								pointsToAllocate -= deltaAttribute;
							}
							else if ((deltaAttribute + faith) > maxStat) {
								errorInputText = "Max Faith!";
							}
							else if ((deltaAttribute + faith) < minStat) {
								errorInputText = "Min Faith!";
							}
						}
						break;
					}
				}
			}

			else if (e.type == SDL_KEYDOWN)
			{
				//remove char if backspace
				if (e.key.keysym.sym == SDLK_BACKSPACE && nameInputText.length() > 0)
				{
					Mix_PlayChannel(-1, gBSound, 0);
					Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
					nameInputText.pop_back();
				}
			}
			else if (e.type == SDL_TEXTINPUT) {
				//add char
				//set length limit to arbitrariy be 11 (fits textbox about right, depends on what user enters)
				if (nameInputText.length() < 11) {
					Mix_PlayChannel(-1, gBSound, 0);
					Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
					nameInputText += e.text.text;
				}
			}

		    
		}

		background.renderBackground(gRenderer);
		//Renders buttons and shows pressed image if pressed
		for (auto i : buttons) {
			if (i->attribute == "strength") {
				if (i->type == "up") {
					if (strength == maxStat || pointsToAllocate == 0) {
						i->locked = true;
					}
					else i->locked = false;
				}
				else {
					if (strength == minStat) {
						i->locked = true;
					}
					else i->locked = false;
				}
			}
			else if (i->attribute == "intelligence") {
				if (i->type == "up") {
					if (intelligence == maxStat || pointsToAllocate == 0) {
						i->locked = true;
					}
					else i->locked = false;
				}
				else {
					if (intelligence == minStat) {
						i->locked = true;
					}
					else i->locked = false;
				}
			}
			else if (i->attribute == "dexterity") {
				if (i->type == "up") {
					if (dexterity == maxStat || pointsToAllocate == 0) {
						i->locked = true;
					}
					else i->locked = false;
				}
				else {
					if (dexterity == minStat) {
						i->locked = true;
					}
					else i->locked = false;
				}
			}
			else if (i->attribute == "constitution") {
				if (i->type == "up") {
					if (constitution == maxStat || pointsToAllocate == 0) {
						i->locked = true;
					}
					else i->locked = false;
				}
				else {
					if (constitution == minStat) {
						i->locked = true;
					}
					else i->locked = false;
				}
			}
			else if (i->attribute == "faith") {
				if (i->type == "up") {
					if (faith == maxStat || pointsToAllocate == 0) {
						i->locked = true;
					}
					else i->locked = false;
				}
				else {
					if (faith == minStat) {
						i->locked = true;
					}
					else i->locked = false;
				}
			}
			if (!i->locked) {
				if (i->pressed <= 0 || i->attribute == "")
					SDL_RenderCopy(gRenderer, i->texture, NULL, &i->rect);
				else
				{
					if (i->type == "up")
						SDL_RenderCopy(gRenderer, upPress, NULL, &i->rect);
					else
						SDL_RenderCopy(gRenderer, downPress, NULL, &i->rect);
					i->pressed--;
				}
		
			}
			else {
				
				if (i->type == "up")
					SDL_RenderCopy(gRenderer, upLocked, NULL, &i->rect);
				else
					SDL_RenderCopy(gRenderer, downLocked, NULL, &i->rect);
			}
		}

		charImageX = frame * charImageW;


		SDL_Rect charactersRectangle = { charImageX, charImageY, charImageW, charImageH };
		SDL_RenderCopy(gRenderer, character, &charactersRectangle, &characterBox);



		//to add more frames per image to make it more fluid
		//definitely not the best way to do this, need to sync to a consistent gametime
		delaysPerFrame++;
		if (delaysPerFrame >= 6)
		{
			frame = (frame + 1) % 6;
			delaysPerFrame = 0;
		}

		std::string strengthString = std::to_string(strength);
		std::string intelligenceString = std::to_string(intelligence);
		std::string dexterityString = std::to_string(dexterity);
		std::string constitutionString = std::to_string(constitution);
		std::string faithString = std::to_string(faith);
		std::string pointsLeftToAllocateString = std::to_string(pointsToAllocate);

		renderText(strengthString.c_str(), &strengthTextRectangle, &textColor);
		renderText(intelligenceString.c_str(), &intelligenceTextRectangle, &textColor);
		renderText(dexterityString.c_str(), &dexterityTextRectangle, &textColor);
		renderText(constitutionString.c_str(), &constitutionTextRectangle, &textColor);
		renderText(faithString.c_str(), &faithTextRectangle, &textColor);
		renderText(pointsLeftToAllocateString.c_str(), &pointsAllocatedRectangle, &textColor);
		if (nameInputText.length() > 0) {
			renderText(nameInputText.c_str(), &nameTextRectangle, &textColor);
		}
		if (errorInputText.length() > 13) {
			renderText(errorInputText.c_str(), &errorTextRectangleLong, &errorColor);
		}
		else if (errorInputText.length() > 0) {
			renderText(errorInputText.c_str(), &errorTextRectangle, &errorColor);
		}
		
		/* Check if mouse is hovering over ability word*/
		SDL_GetMouseState(&mouseX, &mouseY);
		if (mouseX >= 35 && mouseX < 180)
		{
			if (mouseY > 125 && mouseY < 165)
			{
				SDL_RenderCopy(gRenderer, strSum, &strRectangle, &strBox);
			}
			else if (mouseY > 215 && mouseY < 255)
			{
				SDL_RenderCopy(gRenderer, intSum, &intRectangle, &intBox);
			}
			else if (mouseY > 305 && mouseY < 345)
			{
				SDL_RenderCopy(gRenderer, dexSum, &dexRectangle, &dexBox);
			}
			else if (mouseY > 395 && mouseY < 435)
			{
				SDL_RenderCopy(gRenderer, conSum, &conRectangle, &conBox);
			}
			else if (mouseY > 485 && mouseY < 525)
			{
				SDL_RenderCopy(gRenderer, faiSum, &faiRectangle, &faiBox);
			}
		}
		/* END */
		
		SDL_RenderPresent(gRenderer);
		SDL_Delay(16);
	}
	return GOTO_CREDITS;
}

int handlePauseMenu(bool inPauseMenu, std::vector<Player*> allPlayers, std::vector<Cluster*> allEnemies, Tile *tiles[MAX_HORIZONTAL_TILES][MAX_VERTICAL_TILES], SDL_Rect camera)
{
	std::vector<Button*> buttons;
	buttons.push_back(new Button("continue", 240, 200, 260, 64, "Images/UI/PauseMenu/ContinueButton.png", "", gRenderer));
	buttons.push_back(new Button("exit", 240, 300, 260, 64, "Images/UI/PauseMenu/ExitButton.png", "", gRenderer));
	SDL_Texture* background = loadImage("Images/UI/PauseMenu/PauseMenuNoButtons.png"); 
	SDL_Texture* continuebutton = loadImage("Images/UI/PauseMenu/ContinueButton.png");
	SDL_Texture* continuebutton_selected = loadImage("Images/UI/PauseMenu/ContinueButton_Selected.png");
	SDL_Texture* exitbutton = loadImage("Images/UI/PauseMenu/ExitButton.png");
	SDL_Texture* exitbutton_selected = loadImage("Images/UI/PauseMenu/ExitButton_Selected.png");
	SDL_Rect background_rectangle = { 0, 0, 720, 720 };

	soundMenuSelect = Mix_LoadWAV("Audio/SoundEffect_MenuSelect.wav");
	if (soundMenuSelect == NULL)
	{
		printf("Failed to load Button sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}

	SDL_Event e;
	bool justPaused = true;
	while (inPauseMenu)
	{
		while (SDL_PollEvent(&e))
		{
			const Uint8* key = SDL_GetKeyboardState(nullptr);
			if (e.type == SDL_QUIT)
			{
				inPauseMenu = false;
				return GOTO_EXIT;
			}
			if (key[SDL_SCANCODE_ESCAPE])
			{
				if (!justPaused)
					{
						deleteAll(buttons);
						SDL_DestroyTexture(background);
						inPauseMenu = false;
						return GOTO_INGAME;
					}
			}
			else justPaused = false;
			if (e.button.button == (SDL_BUTTON_LEFT) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				for (auto i : buttons)
				{
					//if mouse is clicked inside a button
					if (((mouseX >= i->x) && (mouseX <= (i->x + i->w))) && ((mouseY >= i->y) && (mouseY <= (i->y + i->h))))
					{
						if (i->type == "continue")
						{
							flashButton(continuebutton_selected, continuebutton, i, soundMenuSelect);
							deleteAll(buttons);
							SDL_DestroyTexture(background);
							inPauseMenu = false;
							return GOTO_INGAME;
						}
						else if (i->type == "exit")
						{
							flashButton(exitbutton_selected, exitbutton, i, soundMenuSelect);
							deleteAll(buttons);
							SDL_DestroyTexture(background);
							Mix_HaltMusic();
							inPauseMenu = false;
							return GOTO_MAIN;
						}

					}

				}

			}
			
		}
		//Set Black
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);
		
		for (int tempx = 0; tempx < MAX_HORIZONTAL_TILES; tempx++)
		{
			for (int tempy = 0; tempy < MAX_VERTICAL_TILES; tempy++)
			{
				tiles[tempx][tempy]->render(&camera);
			}
		}

		for (auto &i : allPlayers)
		{
			animateCharacter(i, camera);
		}
		for (auto &i : allEnemies)
		{
			animateCharacter(i, camera);
		}
		SDL_RenderCopy(gRenderer, background, NULL, &background_rectangle);
		for (auto &i : buttons)
		{
			SDL_RenderCopy(gRenderer, i->texture, NULL, &i->rect);
		}
		
		SDL_RenderPresent(gRenderer);
		SDL_Delay(16);
	}
	return GOTO_EXIT;
}

double getDistance(Character* a, Character* b)
{
	return sqrt(pow(a->xPosition-b->xPosition,2)+pow(a->yPosition-b->yPosition,2));
}

int xToTile(Character* c)
{
	int newX = (int)((c->xPosition + c->getImageWidth()/ 2) / TILE_WIDTH);
	return newX;
}

int yToTile(Character* c)
{
	int newY = (int)((c->yPosition + c->getImageHeight()) / TILE_HEIGHT);
	return newY;
}

int tileToX(Character* c)
{
	int newX = (int)(TILE_WIDTH * c->xTile)- (c->rectangle.w / 2);
	return newX;
}

int tileToY(Character* c)
{
	int newY = (TILE_HEIGHT * c->yTile);
	return newY;
}

void getTilePosition(Character* c)
{
	cout << c->getName() << "(" << xToTile(c) << "," << yToTile(c) << ")" << endl;
}

int moveCluster(std::vector<Cluster*> c, std::vector<Player*> p, double time, Tile* map[MAX_HORIZONTAL_TILES][MAX_VERTICAL_TILES])
{

	for (auto cl : c)
	{
		if (cl->combatReady)
		{
			if (cl->targetPlayer == NULL)
			{
				for (auto play_num : p)
				{
					if (getDistance(play_num, cl) <= cl->pursuitRange)
					{
						cl->setTarget(play_num);
						break;
					}
				}
			}
			if (cl->targetPlayer != NULL)
			{
				if (cl->currentPath.size() == 0)
				{
					cl->findPath(map);
				}
				else if ((getDistance(cl->targetPlayer, cl) <= cl->pursuitRange) && (cl->targetPlayer->xTile != cl->targetTileX || cl->targetPlayer->yTile != cl->targetTileY))
				{
					cl->setTarget(cl->targetPlayer);
					cl->findPath(map);
				}	
			}
			else
			{
				if (cl->currentPath.size() == 0)
				cl->findRandom(map);
			}
			cl->moveSteps(time);
		}
	}
	return 0;
}


int playGame()
{
	
	

	std::vector<Player*> allPlayers;
	std::vector<Cluster*> allEnemies;

	std::vector<string> HUD_HPStrings;
	std::vector<string> HUD_NameStrings;
	std::vector<string> HUD_LevelStrings;

	std::vector<SDL_Rect> HUD_HealthRect;
	std::vector<SDL_Rect> HUD_NameRect;
	std::vector<SDL_Rect> HUD_LevelRect;

	Cluster* CollidingCluster = NULL;
	Uint32 timeSinceLastMovement = SDL_GetTicks();
	Uint32 timeSinceLastAnimation = SDL_GetTicks();
	Uint32 lastSync = SDL_GetTicks();
	

	allPlayers.push_back(player1);

	
	int numberOfLevels;
	if (OPTION_randomMaps)
		numberOfLevels = ALL_CUSTOM_MAPS.size();
	else numberOfLevels = ALL_MAPS.size();
	for (MAP_INDEX = 0; MAP_INDEX < numberOfLevels; MAP_INDEX++)
	{
		Tile* tiles[MAX_HORIZONTAL_TILES][MAX_VERTICAL_TILES];
		SDL_Rect* BlockedTiles;
		if (OPTION_randomMaps)
		{
			LevelMap levelcreator(MAX_HORIZONTAL_TILES, MAX_VERTICAL_TILES, MAP_INDEX + 1);
			levelcreator.CreateLevel();
			BlockedTiles = loadMap(tiles, ALL_CUSTOM_MAPS.at(MAP_INDEX));
		}
		else BlockedTiles = loadMap(tiles, ALL_MAPS.at(MAP_INDEX));
		int numEnemies = STARTING_ENEMIES * (MAP_INDEX + 1);
		allEnemies = vector<Cluster*>();
		
		for (auto player : allPlayers)
		{
			while (true)
			{
				int x_to_tile = xToTile(player);
				int y_to_tile = yToTile(player);
				if (tiles[x_to_tile][y_to_tile]->solid)
					break;
				player->xPosition = 1+(rand() % (LEVEL_WIDTH - player->getImageWidth()));
				player->xTile = xToTile(player);
				player->yPosition = 1+(rand() % (LEVEL_HEIGHT - player->getImageHeight()));
				player->yTile = yToTile(player);
			}
			player->setSpriteSheetNumber(IDLE_DOWN);
			player->timeSinceLastMovement = timeSinceLastMovement;
			player->timeSinceLastAnimation = timeSinceLastAnimation;
		}

		for (int num_enemy = 0; num_enemy < numEnemies; num_enemy++)
		{
			int length = sizeof(int);
			int enemiesInCluster = (rand() % (ENEMIES_PER_CLUSTER + MAP_INDEX)) + 1;
			Cluster* enemy = new Cluster(enemiesInCluster, OPTION_difficulty);
			cout << "Enemy " << num_enemy + 1 << " Cluster Size: " << enemy->clusterSize << endl;
			cout << enemy->isAlive() << endl;
			allEnemies.push_back(enemy);
		}

		for (auto i : allEnemies)
		{
			i->setSpriteSheetNumber(IDLE_DOWN);
			i->timeSinceLastAnimation = timeSinceLastAnimation;
			while (true)
			{
				i->xTile = 1+(rand() % (MAX_HORIZONTAL_TILES-1));
				i->yTile = 1 + (rand() % (MAX_VERTICAL_TILES - 1));
				i->xPosition = tileToX(i);
				i->yPosition = tileToY(i);
				if (tiles[i->xTile][i->yTile]->solid)
					break;
			}
		}

		int hudArea = 0; // 0 is top left	1 is top right	2 is bottom left	3 is bottom right
		for (auto player : allPlayers)
		{
			HUD_HPStrings.push_back("Health: " + to_string(player->getHPCurrent()));
			HUD_LevelStrings.push_back("Level: " + to_string(player->getLevel()));
			HUD_NameStrings.push_back("Name: " + player->getName());

			switch (hudArea)
			{
			case 0:
				HUD_NameRect.push_back({ 10, 10, 0, 0 });
				HUD_HealthRect.push_back({ 10, 35, 0, 0 });
				HUD_LevelRect.push_back({ 10, 60, 0, 0 });
				break;
			case 1:
				HUD_NameRect.push_back({ SCREEN_WIDTH-10, 10, 0, 0 });
				HUD_HealthRect.push_back({ SCREEN_WIDTH-10, 35, 0, 0 });
				HUD_LevelRect.push_back({ SCREEN_WIDTH-10, 60, 0, 0 });
				break;
			case 2:
				HUD_NameRect.push_back({ 10, SCREEN_HEIGHT-10, 0, 0 });
				HUD_HealthRect.push_back({ 10, SCREEN_HEIGHT - 35, 0, 0 });
				HUD_LevelRect.push_back({ 10, SCREEN_HEIGHT - 60, 0, 0 });
				break;
			case 3:
				HUD_NameRect.push_back({ SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10, 0, 0 });
				HUD_HealthRect.push_back({ SCREEN_WIDTH - 10, SCREEN_HEIGHT - 35, 0, 0 });
				HUD_LevelRect.push_back({ SCREEN_WIDTH - 10, SCREEN_HEIGHT - 60, 0, 0 });
				break;
			}
			hudArea++;
		}
		
		SDL_Color hudTextColor = { 0, 0, 0, 0 };

		double timePassed = 0;
		int response = 0;

		SDL_Event e;
		SDL_Rect camera = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };
		bool inOverworld = true;
		bool combatStarted = false;
		bool inPauseMenu = false;
		bool keepPlaying = true;
		bool justUnpaused = false;
		startMusic("Audio/Song_Overworld.wav", MIX_MAX_VOLUME / 8);
		while (keepPlaying)
		{
			while (inOverworld) 
			{
				while (SDL_PollEvent(&e))
				{
					if (e.type == SDL_QUIT)
					{
						inOverworld = false;
						return GOTO_EXIT;
					}
				}
				// figure out how much of a second has passed
				timePassed = (SDL_GetTicks() - timeSinceLastMovement) / 1000.0;
				player1->xDeltaVelocity = 0;
				player1->yDeltaVelocity = 0;
				double runningAddSpeed = 0;

				const Uint8* keyState = SDL_GetKeyboardState(nullptr);
				if (keyState[SDL_SCANCODE_W])
					player1->yDeltaVelocity -= (player1->getAcceleration() * timePassed);
				if (keyState[SDL_SCANCODE_A])
					player1->xDeltaVelocity -= (player1->getAcceleration() * timePassed);
				if (keyState[SDL_SCANCODE_S])
					player1->yDeltaVelocity += (player1->getAcceleration() * timePassed);
				if (keyState[SDL_SCANCODE_D])
					player1->xDeltaVelocity += (player1->getAcceleration() * timePassed);
				if (keyState[SDL_SCANCODE_LSHIFT])
					runningAddSpeed = 200;


				if (player1->xDeltaVelocity == 0)
				{
					if (player1->xVelocity > 0)
					{
						if (player1->xVelocity < (player1->getAcceleration() * timePassed))
							player1->xVelocity = 0;
						else player1->xVelocity -= (player1->getAcceleration() * timePassed);
					}
					else if (player1->xVelocity < 0)
					{
						if (-player1->xVelocity < (player1->getAcceleration() * timePassed))
							player1->xVelocity = 0;
						else player1->xVelocity += (player1->getAcceleration() * timePassed);
					}
				}
				else player1->xVelocity += player1->xDeltaVelocity;

				if (player1->yDeltaVelocity == 0)
				{
					if (player1->yVelocity > 0)
					{
						if (player1->yVelocity < (player1->getAcceleration() * timePassed))
							player1->yVelocity = 0;
						else player1->yVelocity -= (player1->getAcceleration() * timePassed);
					}
					else if (player1->yVelocity < 0)
					{
						if (-player1->yVelocity < (player1->getAcceleration() * timePassed))
							player1->yVelocity = 0;
						else player1->yVelocity += (player1->getAcceleration() * timePassed);
					}
				}
				else player1->yVelocity += player1->yDeltaVelocity;

				//bound within Max Speed
				if (player1->xVelocity < -(player1->getSpeedMax() + runningAddSpeed))
					player1->xVelocity = -(player1->getSpeedMax() + runningAddSpeed);
				else if (player1->xVelocity > (player1->getSpeedMax() + runningAddSpeed))
					player1->xVelocity = (player1->getSpeedMax() + runningAddSpeed);
				//bound within Max Speed
				if (player1->yVelocity < -(player1->getSpeedMax() + runningAddSpeed))
					player1->yVelocity = -(player1->getSpeedMax() + runningAddSpeed);
				else if (player1->yVelocity > (player1->getSpeedMax() + runningAddSpeed))
					player1->yVelocity = (player1->getSpeedMax() + runningAddSpeed);

				//Change sprite if character is in motion
				int beforeMoveX = (int)player1->xPosition;
				int beforeMoveY = (int)player1->yPosition;
				for (auto &i : allPlayers)
				{
					if (i->xVelocity == 0 && i->yVelocity == 0)
					{
						if (i->getSpriteSheetNumber() == RUN_UP || i->getSpriteSheetNumber() == RUN_UP_RIGHT || i->getSpriteSheetNumber() == RUN_UP_LEFT )
							i->changeTexture(IDLE_UP);
						else if (i->getSpriteSheetNumber() == RUN_DOWN || i->getSpriteSheetNumber() == RUN_DOWN_RIGHT || i->getSpriteSheetNumber() == RUN_DOWN_LEFT)
							i->changeTexture(IDLE_DOWN);
						else if (i->getSpriteSheetNumber() == RUN_LEFT)
							i->changeTexture(IDLE_LEFT);
						else if (i->getSpriteSheetNumber() == RUN_RIGHT)
							i->changeTexture(IDLE_RIGHT);
					}
					else
					{
						if (i->yVelocity == 0 && i->xVelocity > 0 && i->getSpriteSheetNumber()!= RUN_RIGHT)
							i->changeTexture(RUN_RIGHT);
						else if (i->yVelocity == 0 && i->xVelocity < 0 && i->getSpriteSheetNumber() != RUN_LEFT)
							i->changeTexture(RUN_LEFT);
						else if (i->xVelocity == 0 && i->yVelocity > 0 && i->getSpriteSheetNumber() != RUN_DOWN)
							i->changeTexture(RUN_DOWN);
						else if (i->xVelocity == 0 && i->yVelocity < 0 && i->getSpriteSheetNumber() != RUN_UP)
							i->changeTexture(RUN_UP);
						else if (i->xVelocity > 0 && i->yVelocity > 0 && i->getSpriteSheetNumber() != RUN_DOWN_RIGHT)
							i->changeTexture(RUN_DOWN_RIGHT);
						else if (i->xVelocity < 0 && i->yVelocity > 0 && i->getSpriteSheetNumber() != RUN_DOWN_LEFT)
							i->changeTexture(RUN_DOWN_LEFT);
						else if (i->xVelocity > 0 && i->yVelocity < 0 && i->getSpriteSheetNumber() != RUN_UP_RIGHT)
							i->changeTexture(RUN_UP_RIGHT);
						else if (i->xVelocity < 0 && i->yVelocity < 0 && i->getSpriteSheetNumber() != RUN_UP_LEFT)
							i->changeTexture(RUN_UP_LEFT);
					}
						
					//Move vertically
					i->yPosition += (i->yVelocity * timePassed);
					if (i->yPosition  < 0 || ((i->yPosition + i->getImageHeight())  >= LEVEL_HEIGHT))
					{
						//go back into window
						i->yPosition = beforeMoveY;
					}
					else
					{
						int x_to_tile = xToTile(i);
						int y_to_tile = yToTile(i);

						if (!tiles[x_to_tile][y_to_tile]->solid)
						{
							i->yPosition = beforeMoveY;
						}
					}

					//Move horizontally
					i->xPosition += (i->xVelocity * timePassed);
					if (i->xPosition < 0 || (i->xPosition + i->getImageWidth() >= LEVEL_WIDTH))
					{
						//go back into window
						i->xPosition = beforeMoveX;
					}
					else
					{
						int x_to_tile = xToTile(i);
						int y_to_tile = yToTile(i);

						if (!tiles[x_to_tile][y_to_tile]->solid)
						{
							i->xPosition = beforeMoveX;
						}
					}
				}
				//calculate tile player is currently standing on
				player1->xTile = xToTile(player1);
				player1->yTile = yToTile(player1);

				camera.x = (int)((player1->xPosition + player1->rectangle.w / 2) - SCREEN_WIDTH / 2);
				camera.y = (int)((player1->yPosition + player1->rectangle.h / 2) - SCREEN_HEIGHT / 2);
				if (camera.x < 0)
				{
					camera.x = 0;
				}
				if (camera.y < 0)
				{
					camera.y = 0;
				}
				if (camera.x > LEVEL_WIDTH - camera.w)
				{
					camera.x = LEVEL_WIDTH - camera.w;
				}
				if (camera.y > LEVEL_HEIGHT - camera.h)
				{
					camera.y = LEVEL_HEIGHT - camera.h;
				}

				timeSinceLastMovement = SDL_GetTicks();


				//Set Black
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				for (int tempx = 0; tempx < MAX_HORIZONTAL_TILES; tempx++)
				{
					for (int tempy = 0; tempy < MAX_VERTICAL_TILES; tempy++)
					{
						tiles[tempx][tempy]->render(&camera);
					}
				}

				int moveResult = moveCluster(allEnemies, allPlayers, timePassed, tiles);
				if (moveResult == -1)
					return GOTO_EXIT;

				for (auto &i : allPlayers)
				{
					animateCharacter(i, camera);
				}
				for (auto &i : allEnemies)
				{
					animateCharacter(i, camera);
				}
				
				int hudArea = 0; // 0 is top left	1 is top right	2 is bottom left	3 is bottom right
				for (auto player : allPlayers)
				{
					HUD_HPStrings.at(hudArea) = "Health: " + to_string(player->getHPCurrent());
					HUD_LevelStrings.at(hudArea) = "Level: " + to_string(player->getLevel());
					HUD_NameStrings.at(hudArea) = "Name: " + player->getName();
					renderText(HUD_NameStrings.at(hudArea).c_str(), &HUD_NameRect.at(hudArea), &hudTextColor);
					renderText(HUD_HPStrings.at(hudArea).c_str(), &HUD_HealthRect.at(hudArea), &hudTextColor);
					renderText(HUD_LevelStrings.at(hudArea).c_str(), &HUD_LevelRect.at(hudArea), &hudTextColor);
					hudArea++;
				}
				SDL_RenderPresent(gRenderer);

				if (keyState[SDL_SCANCODE_ESCAPE])
				{
					if(!justUnpaused)
					{
						inOverworld = false;
						inPauseMenu = true;
					}
				}
				else justUnpaused = false;

				int enemyToRemove = 0;
				for (auto z : allEnemies)
				{
					
					if (check_collision(player1->rectangle, z->rectangle) && z->combatReady)
					{
						z->combatReady = false;
						z->readyTimeLeft = 12000;
						z->setSpriteSheetNumber(NOT_READY);
						CollidingCluster = z;
						allEnemies.erase(allEnemies.begin() + enemyToRemove);
						inOverworld = false;
						combatStarted = true;
						break;
					}
					if (z->readyTimeLeft > -1)
						z->readyTimeLeft -= 1;
					if (z->readyTimeLeft == 0)
					{
						z->combatReady = true;
						z->setSpriteSheetNumber(IDLE_DOWN);
					}
					enemyToRemove++;
				}
			}

			if (inPauseMenu)
			{
				response = handlePauseMenu(inPauseMenu, allPlayers, allEnemies, tiles, camera);
				switch (response)
				{
				case GOTO_INGAME:
					inOverworld = true;
					justUnpaused = true;
					break;
				case GOTO_MAIN:
					return GOTO_MAIN;
				default:
					return GOTO_EXIT;
				}
				inPauseMenu = false;
				timeSinceLastMovement = SDL_GetTicks();
			}

			while (combatStarted)
			{
				combatTransition();
				CombatManager cm = CombatManager(player1,CollidingCluster);
				startMusic("Audio/Song_InCombat.wav", MIX_MAX_VOLUME / 8);
				Mix_ResumeMusic();
				int combatResult = cm.combatMain();
				startMusic("Audio/Song_Overworld.wav", MIX_MAX_VOLUME / 8);
				Mix_ResumeMusic();
				timeSinceLastMovement = SDL_GetTicks();
				std::cout << combatResult << std::endl;
				if (combatResult == ENEMY_WINS)
				{
					GameOverTransition();
					SDL_Delay(8000);
					return GOTO_MAIN;
				}
				else if (combatResult == PLAYER_WINS)
				{
					if (allEnemies.size() == 0)
					{
						keepPlaying = false;
						if (MAP_INDEX + 1 == ALL_MAPS.size())
						{
							EndTransition();
							SDL_Delay(4000);
							return GOTO_CREDITS;
						}
						else
						{
							player1->xPosition = 0;
							player1->xTile = xToTile(player1);
							player1->yPosition = 0;
							player1->yTile = yToTile(player1);
							levelTransition();
						}
					}
				}
				else if (combatResult == PLAYER_ESCAPES)
				{
					allEnemies.push_back(CollidingCluster);
				}
				else if (combatResult == PLAYER_EXIT)
				{
					return GOTO_EXIT;
				}
				combatStarted = false;
				inOverworld = true;
			}
			int backToMainMenu = 1;
			if (response == backToMainMenu)
			{
				handleMain();
			}
		}
	}
	return GOTO_CREDITS;
}

void printProgramLog(GLuint program)
{
	//Make sure name is shader
	if (glIsProgram(program))
	{
		//Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//Allocate string
		char* infoLog = new char[maxLength];

		//Get info log
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			//Print Log
			printf("%s\n", infoLog);
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf("Name %d is not a program\n", program);
	}
}

void printShaderLog(GLuint shader)
{
	//Make sure name is shader
	if (glIsShader(shader))
	{
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//Allocate string
		char* infoLog = new char[maxLength];

		//Get info log
		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			//Print Log
			printf("%s\n", infoLog);
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf("Name %d is not a shader\n", shader);
	}
}
int optionsScreen()
{
	soundMenuSelect = Mix_LoadWAV("Audio/SoundEffect_MenuSelect.wav");
	if (soundMenuSelect == NULL)
	{
		printf("Failed to load Button sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}
	std::vector<Button*> buttons;

	SDL_Texture* title = loadImage("Images/UI/OptionsScreen/title.png");
	SDL_Rect space = { 160, 50, 400, 72 };
	

	// Initialize Variables
	bool onOptions = true;

	int mouseX = -1;
	int mouseY = -1;



	SDL_Rect volumeTextRectangle = { 305, 150, 0, 0 };
	SDL_Rect difficultyTextRectangle = { 285, 250, 0, 0 };
	SDL_Rect mapsTextRectangle = { 285, 350, 0, 0 };

	SDL_Color textColor = { 112, 96, 80, 0 };
	SDL_Texture* leftPress = loadImage("Images/UI/OptionsScreen/pointLeftArrow_Pressed.png");
	SDL_Texture* rightPress = loadImage("Images/UI/OptionsScreen/pointRightArrow_Pressed.png");
	SDL_Texture* leftLocked = loadImage("Images/UI/OptionsScreen/pointLeftArrow_Locked.png");
	SDL_Texture* rightLocked = loadImage("Images/UI/OptionsScreen/pointRightArrow_Locked.png");
	SDL_Texture* onLocked = loadImage("Images/UI/OptionsScreen/OnButton_Locked.png");
	SDL_Texture* offLocked = loadImage("Images/UI/OptionsScreen/OffButton_Locked.png");
	

	//Establish Visuals
	buttons.push_back(new Button("left", 225, 140, 35, 45, "Images/UI/OptionsScreen/pointLeftArrow.png", "volume", gRenderer));
	buttons.push_back(new Button("right", 465, 140, 35, 45, "Images/UI/OptionsScreen/pointRightArrow.png", "volume", gRenderer));
	buttons.push_back(new Button("left", 225, 240, 35, 45, "Images/UI/OptionsScreen/pointLeftArrow.png", "difficulty", gRenderer));
	buttons.push_back(new Button("right", 465, 240, 35, 45, "Images/UI/OptionsScreen/pointRightArrow.png", "difficulty", gRenderer));
	buttons.push_back(new Button("on", 125, 340, 140, 45, "Images/UI/OptionsScreen/OnButton.png", "maps", gRenderer));
	buttons.push_back(new Button("off", 465, 340, 140, 45, "Images/UI/OptionsScreen/OffButton.png", "maps", gRenderer));
	buttons.push_back(new Button("back", 450, 650, 230, 56, "Images/UI/OptionsScreen/BackButton.png", "", gRenderer));

	SDL_Texture* back = loadImage("Images/UI/CreateScreen/BackButton.png");
	SDL_Texture* back_selected = loadImage("Images/UI/CreateScreen/BackButton_Selected.png");
	// Load Background
	LoadTexture background;
	background.loadFromFile("Images/UI/OptionsScreen/OptionsNoButtons.png", gRenderer);
	background.renderBackground(gRenderer);

	gBSound = Mix_LoadWAV("Audio/BSound.wav");
	soundButtonFailure = Mix_LoadWAV("Audio/SoundEffect_ButtonFailure.wav");
	soundButtonSuccess_Up = Mix_LoadWAV("Audio/SoundEffect_ButtonSuccess_Up.wav");
	soundButtonSuccess_Down = Mix_LoadWAV("Audio/SoundEffect_ButtonSuccess_Down.wav");
	if (gBSound == NULL || soundButtonFailure == NULL || soundButtonSuccess_Up == NULL || soundButtonSuccess_Down == NULL)
	{
		printf("Failed to load Button sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}

	SDL_Event e;
	while (onOptions)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				Mix_HaltMusic();
				return GOTO_EXIT; //end game
			}
			if (e.button.button == (SDL_BUTTON_LEFT) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				int deltaAttribute = 0;
				for (auto i : buttons)
				{
					//if mouse is clicked inside a button
					if (((mouseX >= i->x) && (mouseX <= (i->x + i->w))) &&
						((mouseY >= i->y) && (mouseY <= (i->y + i->h))))
					{
						if (i->type == "back")
						{
							flashButton(back_selected, back, i, soundMenuSelect);
							deleteAll(buttons);
							background.free();
							return GOTO_MAIN;
						}
						i->pressed = 5;
						
						if (i->type == "right")
						{
							if (i->locked)
							{
								Mix_PlayChannel(-1, soundButtonFailure, 0);
								Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
							}
							else
							{
								Mix_PlayChannel(-1, soundButtonSuccess_Up, 0);
								Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
								deltaAttribute = 1;
							}
						}
						else if (i->type == "left")
						{
							if (i->locked)
							{
								Mix_PlayChannel(-1, soundButtonFailure, 0);
								Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
							}
							else
							{
								Mix_PlayChannel(-1, soundButtonSuccess_Down, 0);
								Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
								deltaAttribute = -1;
							}
						}

						if (i->attribute == "volume")
						{
							if ((deltaAttribute + OPTION_volume) <= OPTION_max_volume && (deltaAttribute + OPTION_volume) >= OPTION_min_volume)
							{
								OPTION_volume += (2 * deltaAttribute);
								Mix_VolumeMusic(adjustVolume(MIX_MAX_VOLUME/8));
							}
						}
						else if (i->attribute == "difficulty")
						{
							if ((deltaAttribute + OPTION_difficulty) <= OPTION_max_difficulty && (deltaAttribute + OPTION_difficulty) >= OPTION_min_difficulty)
							{
								OPTION_difficulty += deltaAttribute;
							}
						}
						else if (i->attribute == "maps")
						{
							if(i->type == "on")
							{
								if (i->locked)
								{
									Mix_PlayChannel(-1, soundButtonSuccess_Down, 0);
									Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
									OPTION_randomMaps = true;
								}
								else
								{
									Mix_PlayChannel(-1, soundButtonFailure, 0);
									Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
								}
							}
							else if (i->type == "off")
							{
								if (i->locked)
								{
									Mix_PlayChannel(-1, soundButtonSuccess_Down, 0);
									Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
									OPTION_randomMaps = false;
								}
								else
								{
									Mix_PlayChannel(-1, soundButtonFailure, 0);
									Mix_Volume(-1, adjustVolume(MIX_MAX_VOLUME));
								}
							}
						}
						break;
					}
				}
			}


		}

		background.renderBackground(gRenderer);
		//Renders buttons and shows pressed image if pressed
		for (auto i : buttons)
		{
			if (i->attribute == "volume")
			{
				if (i->type == "left")
				{
					if (OPTION_volume == OPTION_min_volume)
					{
						i->locked = true;
					}
					else i->locked = false;
				}
				else
				{
					if (OPTION_volume == OPTION_max_volume)
					{
						i->locked = true;
					}
					else i->locked = false;
				}
			}
			else if (i->attribute == "difficulty")
			{
				if (i->type == "left")
				{
					if (OPTION_difficulty == OPTION_min_difficulty)
					{
						i->locked = true;
					}
					else i->locked = false;
				}
				else
				{
					if (OPTION_difficulty == OPTION_max_difficulty)
					{
						i->locked = true;
					}
					else i->locked = false;
				}
			}
			else if (i->attribute == "maps")
			{
				if (i->type == "on")
				{
					if (OPTION_randomMaps)
						i->locked = false;
					else i->locked = true;
				}
				else
				{
					if (OPTION_randomMaps)
						i->locked = true;
					else i->locked = false;
				}
			}
			if (!i->locked)
			{
				if (i->pressed <= 0 || i->attribute == "")
					SDL_RenderCopy(gRenderer, i->texture, NULL, &i->rect);
				else
				{
					if (i->type == "left")
						SDL_RenderCopy(gRenderer, leftPress, NULL, &i->rect);
					else if(i->type == "right")
						SDL_RenderCopy(gRenderer, rightPress, NULL, &i->rect);
					else SDL_RenderCopy(gRenderer, i->texture, NULL, &i->rect);
					i->pressed--;
				}
			}
			else
			{
				if (i->type == "left")
					SDL_RenderCopy(gRenderer, leftLocked, NULL, &i->rect);
				else if (i->type == "right")
					SDL_RenderCopy(gRenderer, rightLocked, NULL, &i->rect);
				else if (i->type == "on")
					SDL_RenderCopy(gRenderer, onLocked, NULL, &i->rect);
				else if (i->type == "off")
					SDL_RenderCopy(gRenderer, offLocked, NULL, &i->rect);
			}
		}

		std::string volumeString = "Volume : " + std::to_string(OPTION_volume);
		std::string difficultyString = "Difficulty : " + std::to_string(OPTION_difficulty);
		std::string mapsString = "Random Maps";

		SDL_RenderCopy(gRenderer, title, NULL, &space);
		renderText(volumeString.c_str(), &volumeTextRectangle, &textColor);
		renderText(difficultyString.c_str(), &difficultyTextRectangle, &textColor);
		renderText(mapsString.c_str(), &mapsTextRectangle, &textColor);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(16);
	}

	return GOTO_MAIN;
}
int mainMenu()
{
	if(!Mix_PlayingMusic())
		startMusic("Audio/Song_MainMenu_New.wav", MIX_MAX_VOLUME / 5);
	soundMenuSelect = Mix_LoadWAV("Audio/SoundEffect_MenuSelect.wav");
	if (soundMenuSelect == NULL)
	{
		printf("Failed to load Button sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}
	bool run = true;
	std::vector<Button*> buttons;

	SDL_Texture* singleplayer = loadImage("Images/UI/MainMenu/SingleplayerButton.png");
	SDL_Texture* singleplayer_selected = loadImage("Images/UI/MainMenu/SingleplayerButton_Selected.png");
	SDL_Texture* options = loadImage("Images/UI/MainMenu/OptionsButton.png");
	SDL_Texture* options_selected = loadImage("Images/UI/MainMenu/OptionsButton_Selected.png");
	SDL_Texture* credits = loadImage("Images/UI/MainMenu/CreditsButton.png");
	SDL_Texture* credits_selected = loadImage("Images/UI/MainMenu/CreditsButton_Selected.png");
	SDL_Texture* exit = loadImage("Images/UI/MainMenu/ExitButton.png");
	SDL_Texture* exit_selected = loadImage("Images/UI/MainMenu/ExitButton_Selected.png");
	SDL_Texture* title = loadImage("Images/UI/MainMenu/title.png");
	SDL_Rect space = { 100, 50, 526, 72 };
	
	// Add all buttons to a vector
	buttons.push_back(new Button("singleplayer", 240, 275, 240, 64, "Images/UI/MainMenu/BlankButton.png", "", gRenderer));
	buttons.push_back(new Button("options", 240, 350, 240, 64, "Images/UI/MainMenu/BlankButton.png", "", gRenderer));
	buttons.push_back(new Button("credits", 240, 425, 240, 64, "Images/UI/MainMenu/BlankButton.png", "", gRenderer));
	buttons.push_back(new Button("exit", 240, 500, 240, 64, "Images/UI/MainMenu/BlankButton.png", "", gRenderer));

	SDL_Texture* background = loadImage("Images/UI/MainMenu/MainMenuNoButtons.png");

	SDL_Event e;
	SDL_RenderCopy(gRenderer, background, NULL, NULL);
	SDL_RenderCopy(gRenderer, title, NULL, &space);
	for (auto i : buttons)
	{
		if (i->type == "singleplayer")
			SDL_RenderCopy(gRenderer, singleplayer, NULL, &i->rect);
		else if (i->type == "options")
			SDL_RenderCopy(gRenderer, options, NULL, &i->rect);
		else if (i->type == "credits")
			SDL_RenderCopy(gRenderer, credits, NULL, &i->rect);
		else if (i->type == "exit") 
			SDL_RenderCopy(gRenderer, exit, NULL, &i->rect);
	}
	SDL_RenderPresent(gRenderer);
	while (run)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				close();
				return GOTO_EXIT;
			}
			if (e.button.button == (SDL_BUTTON_LEFT) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				for (auto i : buttons) {
					//if mouse is clicked inside a button
					if (((mouseX >= i->x) && (mouseX <= (i->x + i->w))) && ((mouseY >= i->y) && (mouseY <= (i->y + i->h))))
					{
						if (i->type == "singleplayer")
						{
							flashButton(singleplayer_selected, singleplayer, i, soundMenuSelect);
							deleteAll(buttons);
							SDL_DestroyTexture(background);
							run = false;
							return GOTO_SOLO; // GO TO CHARACTER SELECT
						}
						else if (i->type == "options")
						{
							flashButton(options_selected, options, i, soundMenuSelect);
							deleteAll(buttons);
							SDL_DestroyTexture(background);
							run = false;
							return GOTO_OPTIONS; // GO TO OPTIONS
						}
						else if (i->type == "credits")
						{
							flashButton(credits_selected, credits, i, soundMenuSelect);
							deleteAll(buttons);
							SDL_DestroyTexture(background);
							run = false;
							return GOTO_CREDITS; // GO TO CREDITS
						}
						else if (i->type == "exit")
						{
							flashButton(exit_selected, exit, i, soundMenuSelect);
							deleteAll(buttons);
							SDL_DestroyTexture(background);
							run = false;
							return GOTO_EXIT; // EXIT
						}
						break;
					}
				}
			}
		}
	}
	return GOTO_CREDITS;
}

int main(int argc, char *argv[])
{
	srand((unsigned int)time(NULL));
	if (!init())
	{
		std::cout << "Failed to initialize!" << std::endl;
		close();
		exit(1);
	}
	handleMain();
	close();
	exit(1);
}
void handleMain()
{
	player1 = new Player("Default", 1, 1, 1, 1, 1);
	int currentMode = GOTO_MAIN;
	while (true)
	{
		switch (currentMode)
		{
		case GOTO_MAIN:
			currentMode = mainMenu();
			break;
		case GOTO_SOLO:
			currentMode = characterCreateScreen();
			break;
		case GOTO_INGAME:
			currentMode = playGame();
			break;
		case GOTO_CREDITS:
			currentMode = playCredits();
			break;
		case GOTO_EXIT:
			return;
		case GOTO_OPTIONS:
			currentMode = optionsScreen();
			break;
		default:
			cout << "Option not set" << endl;
			return;
		}
	}	
}
