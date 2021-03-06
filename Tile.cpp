#include "Tile.h"

Tile::Tile( int x, int y, int tile_type )
{

	settings = Settings::CreateInstance();
	window = Window::CreateInstance();
	xcoord = x; ycoord = y;
    set_type(tile_type);

}

void Tile::show()
{

	SDL_Surface* tileset   = window->get_tileset();
	SDL_Surface* screen    = window->get_screen();
	vector<SDL_Rect> clips = window->get_sprites();

	SDL_Rect offset;
	offset.x = xcoord * settings->retrieve("TILE_WIDTH");
	offset.y = ycoord * settings->retrieve("TILE_HEIGHT");

	SDL_BlitSurface( tileset, &clips[get_type()], screen, &offset );

}

bool Tile::equals(Tile* tile)
{
	return ( ( this->xcoord == tile->get_xcoord() )
		&& ( this->ycoord == tile->get_ycoord() ) );
}

void Tile::set_type( int type ) { this->type = type; }
int Tile::get_type() { return this->type; }

void Tile::set_xcoord( int xcoord ) { this->xcoord = xcoord; }
void Tile::set_ycoord( int ycoord ) { this->ycoord = ycoord; }

void Tile::set_coords( int xcoord, int ycoord )
{
	set_xcoord( xcoord );
	set_ycoord( ycoord );
}

int Tile::get_xcoord() { return this->xcoord; }
int Tile::get_ycoord() { return this->ycoord; }

void Tile::unset() { xcoord = -1; ycoord = -1; type = TILE_NONE; }
