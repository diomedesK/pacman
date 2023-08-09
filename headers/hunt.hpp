#ifndef HUNT_H
#define HUNT_H

#include "../headers/global.hpp"
#include "../headers/position.hpp"
#include "../headers/direction.hpp"

namespace hunt{
	int getDistance( const int fromX, const int fromY, const int toX, const int toY );
	int getLowestGapPossible( int from, int over );
	bool isPositionOutsideMap( const Position p );
	Position tileToItsCenterPosition( int tileX, int tileY );
	Position tileToItsPosition( int tileX, int tileY );
};

#endif
