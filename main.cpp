#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <string>
#include "Board.h"

SDL_Surface *load_image( std::string filename );
void clip_tiles();
void move_cursor( Board* board, int direction );
void choose_tile( Board* board );
void set_possible_moves( Board* board, Tile* &chosen_piece );
void set_jump_moves( Board* board, int color, int captured_xindex, int captured_yindex, int move_xindex, int move_yindex );

SDL_Surface *screen  = NULL;
SDL_Surface *tileset = NULL;
SDL_Rect sprites[ TILE_SPRITES ];
SDL_Event event;

int main ( int argc, char* args[] )
{

	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		std::cout << "SDL was unable to initialize!" << "\n";
		return 1;
	}
	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	std::cout << "Native Resolution: " << info->current_w << "x" << info->current_h << "\n";
	std::cout << "Window Resolution: " << SCREEN_WIDTH    << "x" << SCREEN_HEIGHT   << "\n";
	std::cout << "Hardware Surfaces: " << ( info->hw_available ? "yes" : "no" ) << "\n";
	std::cout << "Window Manager:    " << ( info->wm_available ? "yes" : "no" ) << "\n";
	if ( SCREEN_WIDTH > info->current_w || SCREEN_HEIGHT > info->current_h )
	{
		std::cout << "Window resolution larger than screen resolution!" << "\n";
		return 2;
	}
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, ( info->hw_available ? SDL_HWSURFACE : SDL_SWSURFACE ) );
	if ( screen == NULL )
	{
		std::cout << "Unable to initialize main window!";
		return 3;
	}
	if ( SDL_EnableKeyRepeat( KEY_REPEAT_DELAY, KEY_REPEAT_INTERVAL ) == -1 || KEY_REPEAT_DELAY == 0 )
	{
		std::cout << "Key Repeat:        no" << "\n";
	}
	else
	{
		std::cout << "Key Repeat:        yes" << "\n";
		std::cout << " - Delay:          " << KEY_REPEAT_DELAY    << "ms" << "\n";
		std::cout << " - Interval:       " << KEY_REPEAT_INTERVAL << "ms" << "\n";
	}
	tileset = load_image( "tiles.png" );
	if ( tileset == NULL )
	{
		std::cout << "Unable to load sprites!" << "\n";
		return 4;
	}
	if ( info->wm_available ) SDL_WM_SetCaption( "CheckerPawns", NULL );

    bool quit = false;

	Board* board = new Board(BOARD_WIDTH, BOARD_HEIGHT);

    clip_tiles();

    while ( !quit )
    {
		int direction = DIRECTION_NONE;
        while ( SDL_PollEvent( &event ) )
        {
            if ( event.type == SDL_QUIT ) quit = true;
            if ( event.type == SDL_KEYDOWN )
            {
                switch( event.key.keysym.sym )
                {
                    case SDLK_UP:     direction = DIRECTION_UP;    break;
                    case SDLK_DOWN:   direction = DIRECTION_DOWN;  break;
                    case SDLK_LEFT:   direction = DIRECTION_LEFT;  break;
                    case SDLK_RIGHT:  direction = DIRECTION_RIGHT; break;
                    case SDLK_RETURN: choose_tile( board );        break;
                    case SDLK_ESCAPE:
                    case SDLK_q:      quit = true;                 break;
                    case SDLK_r:      board->reset();              break;
                }
            }
        }
		if ( direction != DIRECTION_NONE ) move_cursor( board, direction );
        board->show(tileset, screen, sprites);
        board->get_selected_tile()->show(tileset, screen, sprites);
        if ( SDL_Flip( screen ) == -1 ) return 1;
    }

    SDL_FreeSurface( tileset );
    SDL_FreeSurface( screen );

	board->clean();

    SDL_Quit();

	return 0;

}

SDL_Surface* load_image( std::string filename )
{

    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load( filename.c_str() );

    if ( loadedImage != NULL )
    {

        optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
        SDL_FreeSurface( loadedImage );

        if ( optimizedImage != NULL )
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );

    }

    return optimizedImage;

}

void clip_tiles()
{

	for ( int i = 0 ; i < TILE_SPRITES ; i++ )
	{
		sprites[i].x = TILE_WIDTH * ( i / 2 );
		sprites[i].y = ( ( i % 2 ) ? TILE_HEIGHT : 0 );
		sprites[i].w = TILE_WIDTH;
		sprites[i].h = TILE_HEIGHT;
	}

}

void move_cursor( Board* board, int direction )
{

    Tile* selected_board = NULL;

    board->get_selected_tile()->set_type( TILE_NONE );

    int xcoord = board->get_selected_tile()->get_xcoord();
    int ycoord = board->get_selected_tile()->get_ycoord();

    switch ( direction )
    {

        case DIRECTION_UP:    if ( ycoord > 0 )						ycoord--; break;
        case DIRECTION_DOWN:  if ( ycoord < board->get_height()-1 )	ycoord++; break;
        case DIRECTION_LEFT:  if ( xcoord > 0 )						xcoord--; break;
        case DIRECTION_RIGHT: if ( xcoord < board->get_width()-1 )	xcoord++; break;

    }

	board->get_selected_tile()->set_coords( xcoord, ycoord );

    for ( int i = 0 ; i < board->get_width() ; i++ )
	{
		for ( int j = 0 ; j < board->get_height() ; j++ )
		{
			Tile* tile = board->get_board_tile( i, j );
			if ( ( tile->get_xcoord() == xcoord ) && ( tile->get_ycoord() == ycoord ) )
			{
				selected_board = tile;
				break;
			}
		}
	}

    if ( selected_board->get_type() == TILE_BLACK )
        board->get_selected_tile()->set_type( TILE_BLACK_SELECTED );
    else if ( selected_board->get_type() == TILE_WHITE )
        board->get_selected_tile()->set_type( TILE_WHITE_SELECTED );

}

void choose_tile( Board* board )
{

    int old_type;
    int new_type;

	Tile* old_chosen_piece = NULL;
	Tile* new_chosen_piece = NULL;

	for ( int i = 0 ; i < board->get_width() ; i++ )
	{
		for ( int j = 0 ; j < board->get_width() ; j++ )
		{

			Tile* tile = board->get_piece_tile( i, j );
			if ( tile->equals( board->get_chosen_tile() ) )   old_chosen_piece = tile;
			if ( tile->equals( board->get_selected_tile() ) ) new_chosen_piece = tile;

		}
	}

    if ( old_chosen_piece != NULL ) old_type = old_chosen_piece->get_type();
    if ( new_chosen_piece != NULL ) new_type = new_chosen_piece->get_type();

    board->get_chosen_tile()->set_coords( new_chosen_piece->get_xcoord(), new_chosen_piece->get_ycoord() );

	if ( new_type != TILE_NONE )
	{

		board->clear_possible_moves();
        if ( old_chosen_piece != NULL && old_chosen_piece->equals( new_chosen_piece ) )
            board->get_chosen_tile()->unset();
        else
            set_possible_moves( board, new_chosen_piece );

	}
	else
	{

		if ( board->get_possible_moves( new_chosen_piece->get_xcoord(), new_chosen_piece->get_ycoord() )->get_type() != TILE_NONE )
		{

			if ( old_chosen_piece != NULL )
			{
		
				board->clear_possible_moves();

				new_chosen_piece->set_type( old_type );

				board->capture_piece( old_chosen_piece, new_chosen_piece );

				if ( old_type == TILE_RED && board->get_chosen_tile()->get_ycoord() == 0 )
					new_chosen_piece->set_type( TILE_RED_KING );

				if ( old_type == TILE_GREEN && board->get_chosen_tile()->get_ycoord() == board->get_height()-1 )
					new_chosen_piece->set_type( TILE_GREEN_KING );

				old_chosen_piece->set_type( TILE_NONE );

			}

		}
		else board->clear_possible_moves();

		board->get_chosen_tile()->unset();

	}

}

void set_possible_moves( Board* board, Tile* &chosen_piece )
{

    int type   = chosen_piece->get_type();
	int width  = board->get_width();
	int height = board->get_height();
    int xcoord = chosen_piece->get_xcoord();
    int ycoord = chosen_piece->get_ycoord();
	int movement[4][2][2];

	// normal moves
	if ( xcoord > 0 && ycoord > 0 )					{ movement[0][0][0] = xcoord-1; /*left*/	movement[0][0][1] = ycoord-1; /*up*/ }
	else											{ movement[0][0][0] = -1;					movement[0][0][1] = -1; }
	if ( xcoord < width-1 && ycoord > 0 )			{ movement[1][0][0] = xcoord+1; /*right*/	movement[1][0][1] = ycoord-1; /*up*/ }
	else											{ movement[1][0][0] = -1;					movement[1][0][1] = -1; }
	if ( xcoord > 0 && ycoord < height-1 )			{ movement[2][0][0] = xcoord-1; /*left*/	movement[2][0][1] = ycoord+1; /*down*/ }
	else											{ movement[2][0][0] = -1;					movement[2][0][1] = -1; }
	if ( xcoord < width-1 && ycoord < height-1 )	{ movement[3][0][0] = xcoord+1; /*right*/	movement[3][0][1] = ycoord+1; /*down*/ }
	else											{ movement[3][0][0] = -1;					movement[3][0][1] = -1; }

	// jump moves
	if ( xcoord > 1 && ycoord > 1 )					{ movement[0][1][0] = xcoord-2; /*left*/	movement[0][1][1] = ycoord-2; /*up*/ }
	else											{ movement[0][1][0] = -1;					movement[0][1][1] = -1; }
	if ( xcoord < width-2 && ycoord > 1 )			{ movement[1][1][0] = xcoord+2; /*right*/	movement[1][1][1] = ycoord-2; /*up*/ }
	else											{ movement[1][1][0] = -1;					movement[1][1][1] = -1; }
	if ( xcoord > 1 && ycoord < height-2 )			{ movement[2][1][0] = xcoord-2; /*left*/	movement[2][1][1] = ycoord+2; /*down*/ }
	else											{ movement[2][1][0] = -1;					movement[2][1][1] = -1; }
	if ( xcoord < width-2 && ycoord < height-2 )	{ movement[3][1][0] = xcoord+2; /*right*/	movement[3][1][1] = ycoord+2; /*down*/ }
	else											{ movement[3][1][0] = -1;					movement[3][1][1] = -1; }

    if ( ( type == TILE_RED ) || ( type == TILE_RED_KING ) )
    {

        board->get_chosen_tile()->set_type( TILE_RED_SELECTED );

		for ( int i = 0 ; i <= 3 ; i++ )
		{

			int normal_move_x = movement[i][0][0];
			int normal_move_y = movement[i][0][1];
			int jump_move_x   = movement[i][1][0];
			int jump_move_y   = movement[i][1][1];

			if ( ( chosen_piece->get_type() == TILE_RED ) && ( i == 2 ) ) break;

			if ( normal_move_x != -1 && normal_move_y != -1 )
			{

				set_jump_moves( board, TILE_RED, normal_move_x, normal_move_y, jump_move_x, jump_move_y );
				if ( board->get_piece_tile(      normal_move_x, normal_move_y )->get_type() == TILE_NONE )
					 board->get_possible_moves(  normal_move_x, normal_move_y )->set_type( TILE_RED_POSSIBLE );

			}

        }

    }
    else if ( ( type == TILE_GREEN ) || ( type == TILE_GREEN_KING ) )
    {

        board->get_chosen_tile()->set_type( TILE_GREEN_SELECTED );

		for ( int i = 3 ; i >= 0 ; i-- )
		{

			int normal_move_x = movement[i][0][0];
			int normal_move_y = movement[i][0][1];
			int jump_move_x   = movement[i][1][0];
			int jump_move_y   = movement[i][1][1];

			if ( normal_move_x != -1 && normal_move_y != -1 )
			{

				set_jump_moves( board, TILE_GREEN, normal_move_x, normal_move_y, jump_move_x, jump_move_y );
				if ( board->get_piece_tile(        normal_move_x, normal_move_y )->get_type() == TILE_NONE )
					 board->get_possible_moves(    normal_move_x, normal_move_y )->set_type( TILE_GREEN_POSSIBLE );

			}
			
			if ( ( chosen_piece->get_type() == TILE_GREEN ) && ( i == 2 ) ) break;

        }

    }

}

void set_jump_moves( Board* board, int color, int captured_xindex, int captured_yindex, int move_xindex, int move_yindex )
{

	int old_type = board->get_piece_tile( captured_xindex, captured_yindex )->get_type();
	int new_type = board->get_piece_tile( move_xindex, move_yindex )->get_type();

	if ( ( move_xindex != -1 && move_yindex != -1 ) &&
	   ( ( old_type == ( ( color == TILE_RED ) ? TILE_GREEN : TILE_RED ) ) ||
		 ( old_type == ( ( color == TILE_RED ) ? TILE_GREEN_KING : TILE_RED_KING ) ) ) &&
		 ( new_type == TILE_NONE ) )
		board->get_possible_moves(move_xindex,move_yindex)->set_type( ( color == TILE_RED ) ? TILE_RED_POSSIBLE : TILE_GREEN_POSSIBLE );

}
