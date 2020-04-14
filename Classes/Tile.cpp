#include "../Headers/Tile.h"

//0: grass 1:water
Tile::Tile(int x, int y, int tileType)
{
	//Get the offsets
	mBox.x = x;
	mBox.y = y;

	//Set the collision box
	mBox.w = TILE_WIDTH;
	mBox.h = TILE_HEIGHT;
	flip = SDL_FLIP_NONE;
	//Get the tile type
	mType = tileType;
	Helper temp = Helper();
	
	switch (tileType)
	{
	case 1://pond WWWW
		texture = pond;
		break;
	case 11://outside top left edge WWWG
		texture = OutsideTopLeft;
		break;
	case 10://outside top right edge WWGW
		texture = OutsideTopRight;
		break;
	case 4://pond bottom edge WWGG
		texture = pondBottom;
		break;
	case 13://outside Bottom Left edge WGWW
		texture = OutsideBottomLeft;
		break;
	case 9://pond right edge WGWG
		texture = pondLeft;
		flip = SDL_FLIP_HORIZONTAL;
		break;
	case 6://pond bottom right corner WGGG
		texture = pondBottomLeft;
		flip = SDL_FLIP_HORIZONTAL;
		break;
	case 12://outside Bottom Right edge GWWW
		texture = OutsideBottomRight;
		break;
	case 16: //water checkerboard WGGW
		texture = waterchecker;
		break;
	case 17: // grass checkerboard GWWG
		texture = grasschecker;
		break;
	case 2://pond left edge GWGW
		texture = pondLeft;
		break;
	case 5://pond bottom left corner GWGG
		texture = pondBottomLeft;
		break;
	case 3://pond top edge GGWW
		texture = pondTop;
		break;
	case 8://pond top right corner GGWG
		texture = pondTopLeft;
		flip = SDL_FLIP_HORIZONTAL;
		break;
	case 7://pond top left corner GGGW
		texture = pondTopLeft;
		break;
	case 15://flower 1 GGGG
		texture = flower1;
		break;
	case 14://single grass 2 GGGG
		texture = singleGrass2;
		solid = true;
		break;
	case 0://grass GGGG
		texture = grass;
		solid = true;
		break;
	}
}
//To render everytile have a loop iterate through the tile array calling .render and pass the camera object
//The tile will be rendered to the screen if it collides with the camera object
//only check for collisions with player for tiles if this function returns true
bool Tile::render(SDL_Rect* camera)
{
	
	//If the tile is on screen
	if (SDL_HasIntersection(camera, &mBox))
	{
		SDL_Rect Temp = {mBox.x-camera->x,mBox.y-camera->y, mBox.w,mBox.h};
		SDL_RenderCopyEx(gRenderer,texture,NULL,&Temp,0.0,nullptr,flip);
		return true;
	}
	return false;
}
int Tile::getType()
{
	return mType;
}


SDL_Rect Tile::getBox()
{
	return mBox;
}

void Tile::loadTiles()
{
	singleGrass2 = Helper::loadImage("Images/Tiles/singleGrass2.png", gRenderer);

	OutsideBottomLeft = Helper::loadImage("Images/Tiles/OutsideBottomLeft.png", gRenderer);

	OutsideBottomRight = Helper::loadImage("Images/Tiles/OutsideBottomRight.png", gRenderer);

	pondTopLeft = Helper::loadImage("Images/Tiles/pondTopLeft.png", gRenderer);

	OutsideTopLeft = Helper::loadImage("Images/Tiles/OutsideTopLeft.png", gRenderer);

	OutsideTopRight = Helper::loadImage("Images/Tiles/OutsideTopRight.png", gRenderer);

	pondLeft = Helper::loadImage("Images/Tiles/pondLeft.png", gRenderer);

	pondBottomLeft = Helper::loadImage("Images/Tiles/pondBottomLeft.png", gRenderer);

	pondBottom = Helper::loadImage("Images/Tiles/pondBottom.png", gRenderer);

	pondTop = Helper::loadImage("Images/Tiles/pondTop.png", gRenderer);

	pond = Helper::loadImage("Images/Tiles/pond.png", gRenderer);

	grass = Helper::loadImage("Images/Tiles/singleGrass.png", gRenderer);

	flower1 = Helper::loadImage("Images/Tiles/flower1.png", gRenderer);

	specialflower = Helper::loadImage("Images/Tiles/specialflower.png", gRenderer);

	waterchecker = Helper::loadImage("Images/Tiles/waterchecker.png", gRenderer);

	grasschecker = Helper::loadImage("Images/Tiles/grasschecker.png", gRenderer);
}






