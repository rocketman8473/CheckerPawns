#include "Board.h"

Board::Board( int width, int height )
{

	settings = Settings::CreateInstance();

	this->width = width;
	this->height = height;

	board_tiles.resize( height );
	piece_tiles.resize( height );
	possible_moves.resize( height );

	this->reset();

	cout << "Board Size:        " << settings->retrieve("BOARD_WIDTH") << "x" << settings->retrieve("BOARD_HEIGHT") << "\n";
	cout << "Player Rows:       " << settings->retrieve("PIECE_ROWS") << "\n";

}

void Board::reset()
{

	for ( int i = 0 ; i < this->height ; i++ )
	{

		int board_tile_type;
		int piece_tile_type;

		board_tile_type = (i%2) ? TILE_BLACK : TILE_WHITE;

		if ( i < settings->retrieve("PIECE_ROWS") ) piece_tile_type = TILE_GREEN;
		else if ( i > this->get_height()-settings->retrieve("PIECE_ROWS")-1 ) piece_tile_type = TILE_RED;
		else piece_tile_type = TILE_NONE;

		board_tiles[i]    = new Row( i, width, board_tile_type );
		piece_tiles[i]    = new Row( i, width, piece_tile_type );
		possible_moves[i] = new Row( i, width, TILE_NONE );

	}

	select_tile(0,0);
	choose_tile(-1,-1);

}

void Board::clear_possible_moves()
{
	for ( int i = 0 ; i < this->height ; i++ )
		possible_moves[i]->clear();
}

void Board::clean()
{
	for ( int i = 0 ; i < this->width ; i++ )
	{
		delete board_tiles[i];
		delete piece_tiles[i];
		delete possible_moves[i];
	}
	delete selected_tile;
	delete chosen_tile;
}

void Board::show()
{
	for ( int i = 0 ; i < this->height ; i++ )
	{
		board_tiles[i]->show();
		piece_tiles[i]->show();
		possible_moves[i]->show();
	}
	selected_tile->show();
	chosen_tile->show();
}

void Board::capture_piece( Tile* &old_piece, Tile* &new_piece )
{

    int old_type = old_piece->get_type();

    int old_xcoord = old_piece->get_xcoord();
    int old_ycoord = old_piece->get_ycoord();

    int new_xcoord = new_piece->get_xcoord();
    int new_ycoord = new_piece->get_ycoord();

    int delta_xcoord = old_xcoord - new_xcoord;
    int delta_ycoord = old_ycoord - new_ycoord;

    for ( int j = -1 ; j < 2 ; j+=2 )
    {

        for ( int i = -1 ; i < 2 ; i+= 2 )
        {

            int x = ( i < 0 ) ? i + 3 : i - 3;
            int y = ( j < 0 ) ? j + 3 : j - 3;

            int captured_yindex = old_ycoord + j;
			int captured_xindex = old_xcoord + i;

			if ( captured_xindex < 0 || captured_yindex < 0 ) continue;
			if ( captured_xindex > settings->retrieve("BOARD_WIDTH")-1 ||
				captured_yindex > settings->retrieve("BOARD_HEIGHT")-1 ) continue;

            int captured_type = get_piece_tile(captured_xindex, captured_yindex)->get_type();

            if ( ( delta_xcoord == x ) && ( delta_ycoord == y) )
            {

                if ( old_type == TILE_RED || old_type == TILE_RED_KING )
                    if ( ( captured_type != TILE_RED ) && ( captured_type != TILE_RED_KING ) )
                        if ( j == -1 || old_type == TILE_RED_KING )
                            get_piece_tile(captured_xindex, captured_yindex)->set_type( TILE_NONE );

                if ( old_type == TILE_GREEN || old_type == TILE_GREEN_KING )
                    if ( ( captured_type != TILE_GREEN ) && ( captured_type != TILE_GREEN_KING ) )
                        if ( j == 1 || old_type == TILE_GREEN_KING )
                            get_piece_tile(captured_xindex, captured_yindex)->set_type( TILE_NONE );

            }

        }

    }

}

Tile* Board::get_board_tile(int x, int y)
{
	if ( ( x >= 0 && x < width ) && ( y >= 0 && y < height ) )
		return board_tiles[y]->get_tile(x);
	else return NULL;
}

Tile* Board::get_piece_tile(int x, int y)
{
	if ( ( x >= 0 && x < width ) && ( y >= 0 && y < height ) )
		return piece_tiles[y]->get_tile(x);
	else return NULL;
}

Tile* Board::get_possible_moves(int x, int y)
{
	if ( ( x >= 0 && x < width ) && ( y >= 0 && y < height ) )
		return possible_moves[y]->get_tile(x);
	else return NULL;
}

Tile* Board::get_selected_tile()
{
	return selected_tile;
}

Tile* Board::get_chosen_tile()
{
	return chosen_tile;
}

int Board::get_width()
{
	return this->width;
}

int Board::get_height()
{
	return this->height;
}

void Board::select_tile(int x, int y)
{

	switch ( get_board_tile( x, y )->get_type() )
	{
		case TILE_BLACK:
			selected_tile = new Tile( x, y, TILE_BLACK_SELECTED );
			break;
		case TILE_WHITE:
			selected_tile = new Tile( x, y, TILE_WHITE_SELECTED );
			break;
	}

}

void Board::choose_tile(int x, int y)
{
    chosen_tile = new Tile( x, y, TILE_NONE );
}

int Board::get_tile_count(int type)
{

	int count = 0;

	for ( int i = 0 ; i < this->width ; i++ )
		for ( int j = 0 ; j < this->height ; j++ )
			if ( this->get_piece_tile(i,j)->get_type() == type ) count ++;

	return count;

}

bool Board::check_winner()
{

	if ( get_tile_count( TILE_RED ) + get_tile_count( TILE_RED_KING ) == 0 )
	{ cout << "Green Wins!" << "\n"; return true; }

	if ( get_tile_count( TILE_GREEN ) + get_tile_count( TILE_GREEN_KING ) == 0 )
	{ cout << "Red Wins!" << "\n"; return true; }

	return false;

}

void Board::move_cursor( int direction )
{

    get_selected_tile()->set_type( TILE_NONE );

    int xcoord = get_selected_tile()->get_xcoord();
    int ycoord = get_selected_tile()->get_ycoord();

    switch ( direction )
    {

        case DIRECTION_UP:    if ( ycoord > 0 )					ycoord--; break;
        case DIRECTION_DOWN:  if ( ycoord < get_height()-1 )	ycoord++; break;
        case DIRECTION_LEFT:  if ( xcoord > 0 )					xcoord--; break;
        case DIRECTION_RIGHT: if ( xcoord < get_width()-1 )		xcoord++; break;

    }

	get_selected_tile()->set_coords( xcoord, ycoord );

	Tile* selected_board = get_board_tile( xcoord, ycoord );

    if ( selected_board->get_type() == TILE_BLACK )
        get_selected_tile()->set_type( TILE_BLACK_SELECTED );
    else if ( selected_board->get_type() == TILE_WHITE )
        get_selected_tile()->set_type( TILE_WHITE_SELECTED );

}

void Board::choose_tile()
{

    int old_type;
    int new_type;

	Tile* old_chosen_piece = get_piece_tile( get_chosen_tile()->get_xcoord(), get_chosen_tile()->get_ycoord() );
	Tile* new_chosen_piece = get_piece_tile( get_selected_tile()->get_xcoord(), get_selected_tile()->get_ycoord() );

    if ( old_chosen_piece ) old_type = old_chosen_piece->get_type();
    if ( new_chosen_piece ) new_type = new_chosen_piece->get_type();

    get_chosen_tile()->set_coords( new_chosen_piece->get_xcoord(), new_chosen_piece->get_ycoord() );

	if ( new_type != TILE_NONE )
	{

		clear_possible_moves();
        if ( old_chosen_piece && old_chosen_piece->equals( new_chosen_piece ) )
            get_chosen_tile()->unset();
        else
            set_possible_moves();

	}
	else
	{

		if ( get_possible_moves( new_chosen_piece->get_xcoord(), new_chosen_piece->get_ycoord() )->get_type() != TILE_NONE )
		{

			if ( old_chosen_piece )
			{
		
				clear_possible_moves();

				new_chosen_piece->set_type( old_type );

				capture_piece( old_chosen_piece, new_chosen_piece );

				if ( old_type == TILE_RED && get_chosen_tile()->get_ycoord() == 0 )
					new_chosen_piece->set_type( TILE_RED_KING );

				if ( old_type == TILE_GREEN && get_chosen_tile()->get_ycoord() == get_height()-1 )
					new_chosen_piece->set_type( TILE_GREEN_KING );

				old_chosen_piece->set_type( TILE_NONE );

			}

		}
		else clear_possible_moves();

		get_chosen_tile()->unset();

	}

}

void Board::set_possible_moves()
{

	int width  = get_width();
	int height = get_height();
    int xcoord = chosen_tile->get_xcoord();
    int ycoord = chosen_tile->get_ycoord();
    int type   = get_piece_tile( xcoord, ycoord )->get_type();

	Tile* topleft     = get_piece_tile( xcoord-1, ycoord-1 );
	Tile* topright    = get_piece_tile( xcoord+1, ycoord-1 );
	Tile* bottomleft  = get_piece_tile( xcoord-1, ycoord+1 );
	Tile* bottomright = get_piece_tile( xcoord+1, ycoord+1 );

	int toplefttype     = ( topleft )     ? topleft->get_type()     : TILE_INVALID;
	int toprighttype    = ( topright )    ? topright->get_type()    : TILE_INVALID;
	int bottomlefttype  = ( bottomleft )  ? bottomleft->get_type()  : TILE_INVALID;
	int bottomrighttype = ( bottomright ) ? bottomright->get_type() : TILE_INVALID;

	switch ( type )
	{
		case TILE_RED_KING:
			if ( bottomlefttype == TILE_NONE )
				get_possible_moves( bottomleft->get_xcoord(), bottomleft->get_ycoord() )->set_type( TILE_RED_POSSIBLE );
			if ( bottomrighttype == TILE_NONE )
				get_possible_moves( bottomright->get_xcoord(), bottomright->get_ycoord() )->set_type( TILE_RED_POSSIBLE );
		case TILE_RED:
			get_chosen_tile()->set_type( TILE_RED_SELECTED );
			if ( toplefttype == TILE_NONE )
				get_possible_moves( topleft->get_xcoord(), topleft->get_ycoord() )->set_type( TILE_RED_POSSIBLE );
			if ( toprighttype == TILE_NONE )
				get_possible_moves( topright->get_xcoord(), topright->get_ycoord() )->set_type( TILE_RED_POSSIBLE );
			break;
		case TILE_GREEN_KING:
			if ( toplefttype == TILE_NONE )
				get_possible_moves( topleft->get_xcoord(), topleft->get_ycoord() )->set_type( TILE_GREEN_POSSIBLE );
			if ( toprighttype == TILE_NONE )
				get_possible_moves( topright->get_xcoord(), topright->get_ycoord() )->set_type( TILE_GREEN_POSSIBLE );
		case TILE_GREEN:
			get_chosen_tile()->set_type( TILE_GREEN_SELECTED );
			if ( bottomlefttype == TILE_NONE )
				get_possible_moves( bottomleft->get_xcoord(), bottomleft->get_ycoord() )->set_type( TILE_GREEN_POSSIBLE );
			if ( bottomrighttype == TILE_NONE )
				get_possible_moves( bottomright->get_xcoord(), bottomright->get_ycoord() )->set_type( TILE_GREEN_POSSIBLE );
			break;
	}

	set_jump_moves( xcoord, ycoord, type );

}

void Board::set_jump_moves( int xcoord, int ycoord, int type )
{

	Tile* tile = NULL;

	Tile* topleft     = get_piece_tile( xcoord-1, ycoord-1 );
	Tile* topright    = get_piece_tile( xcoord+1, ycoord-1 );
	Tile* bottomleft  = get_piece_tile( xcoord-1, ycoord+1 );
	Tile* bottomright = get_piece_tile( xcoord+1, ycoord+1 );

	Tile* topleft2     = get_piece_tile( xcoord-2, ycoord-2 );
	Tile* topright2    = get_piece_tile( xcoord+2, ycoord-2 );
	Tile* bottomleft2  = get_piece_tile( xcoord-2, ycoord+2 );
	Tile* bottomright2 = get_piece_tile( xcoord+2, ycoord+2 );

	int toplefttype     = ( topleft )     ? topleft->get_type()     : TILE_INVALID;
	int toprighttype    = ( topright )    ? topright->get_type()    : TILE_INVALID;
	int bottomlefttype  = ( bottomleft )  ? bottomleft->get_type()  : TILE_INVALID;
	int bottomrighttype = ( bottomright ) ? bottomright->get_type() : TILE_INVALID;

	int topleft2type     = ( topleft2 )     ? topleft2->get_type()     : TILE_INVALID;
	int topright2type    = ( topright2 )    ? topright2->get_type()    : TILE_INVALID;
	int bottomleft2type  = ( bottomleft2 )  ? bottomleft2->get_type()  : TILE_INVALID;
	int bottomright2type = ( bottomright2 ) ? bottomright2->get_type() : TILE_INVALID;

	switch ( type )
	{

		case TILE_RED_KING:
			if ( ( bottomlefttype == TILE_GREEN || bottomlefttype == TILE_GREEN_KING ) && bottomleft2type == TILE_NONE )
			{
				tile = get_possible_moves( xcoord-2, ycoord+2 );
				if ( tile )
				{
					if ( tile->get_type() != TILE_RED_POSSIBLE )
					{
						tile->set_type( TILE_RED_POSSIBLE );
						set_jump_moves( xcoord-2, ycoord+2, type );
					}
				}
			}
			if ( ( bottomrighttype == TILE_GREEN || bottomrighttype == TILE_GREEN_KING ) && bottomright2type == TILE_NONE )
			{
				tile = get_possible_moves( xcoord+2, ycoord+2 );
				if ( tile )
				{
					if ( tile->get_type() != TILE_RED_POSSIBLE )
					{
						tile->set_type( TILE_RED_POSSIBLE );
						set_jump_moves( xcoord+2, ycoord+2, type );
					}
				}
			}
		case TILE_RED:
			if ( ( toplefttype == TILE_GREEN || toplefttype == TILE_GREEN_KING ) && topleft2type == TILE_NONE )
			{
				tile = get_possible_moves( xcoord-2, ycoord-2 );
				if ( tile )
				{
					if ( tile->get_type() != TILE_RED_POSSIBLE )
					{
						tile->set_type( TILE_RED_POSSIBLE );
						set_jump_moves( xcoord-2, ycoord-2, type );
					}
				}
			}
			if ( ( toprighttype == TILE_GREEN || toprighttype == TILE_GREEN_KING ) && topright2type == TILE_NONE )
			{
				tile = get_possible_moves( xcoord+2, ycoord-2 );
				if ( tile )
				{
					if ( tile->get_type() != TILE_RED_POSSIBLE )
					{
						tile->set_type( TILE_RED_POSSIBLE );
						set_jump_moves( xcoord+2, ycoord-2, type );
					}
				}
			}
			break;
		case TILE_GREEN_KING:
			if ( ( toplefttype == TILE_RED || toplefttype == TILE_RED_KING ) && topleft2type == TILE_NONE )
			{
				tile = get_possible_moves( xcoord-2, ycoord-2 );
				if ( tile )
				{
					if ( tile->get_type() != TILE_GREEN_POSSIBLE )
					{
						tile->set_type( TILE_GREEN_POSSIBLE );
						set_jump_moves( xcoord-2, ycoord-2, type );
					}
				}
			}
			if ( ( toprighttype == TILE_RED || toprighttype == TILE_RED_KING ) && topright2type == TILE_NONE )
			{
				tile = get_possible_moves( xcoord+2, ycoord-2 );
				if ( tile )
				{
					if ( tile->get_type() != TILE_GREEN_POSSIBLE )
					{
						tile->set_type( TILE_GREEN_POSSIBLE );
						set_jump_moves( xcoord+2, ycoord-2, type );
					}
				}
			}
		case TILE_GREEN:
			if ( ( bottomlefttype == TILE_RED || bottomlefttype == TILE_RED_KING ) && bottomleft2type == TILE_NONE )
			{
				tile = get_possible_moves( xcoord-2, ycoord+2 );
				if ( tile )
				{
					if ( tile->get_type() != TILE_GREEN_POSSIBLE )
					{
						tile->set_type( TILE_GREEN_POSSIBLE );
						set_jump_moves( xcoord-2, ycoord+2, type );
					}
				}	
			}
			if ( ( bottomrighttype == TILE_RED || bottomrighttype == TILE_RED_KING ) && bottomright2type == TILE_NONE )
			{
				tile = get_possible_moves( xcoord+2, ycoord+2 );
				if ( tile )
				{
					if ( tile->get_type() != TILE_GREEN_POSSIBLE )
					{
						tile->set_type( TILE_GREEN_POSSIBLE );
						set_jump_moves( xcoord+2, ycoord+2, type );
					}
				}
			}
			break;

	}

}
