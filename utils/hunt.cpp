#include <cmath>

#include "../headers/hunt.hpp"
#include "../headers/global.hpp"
#include "../headers/position.hpp"
#include "../headers/direction.hpp"

namespace hunt {

	bool isPositionOutsideMap( const Position p ){
		return (p.x < 0 || p.y < 0 || p.x >= settings::mapWidthPixels || p.y >= settings::mapHeightPixels);
	}

	int getDistance( const int fromX, const int fromY, const int toX, const int toY ){
		int a = fromY - toY;
		int b = fromX - toX;

		int d = std::sqrt(a*a + b*b);

		return d;
	};

	int getLowestGapPossible( int from, int over ){
		int n1 = (int)( over / from );
		int n2 = n1 + 1;

		int a = std::abs( over - (from * n1 ) );
		int b = std::abs( over - (from * n2 ) );

		if(a < b){
			return a;
		} else {
			return b;
		}

	};

	Position tileToItsPosition( int tileX, int tileY ){
		Position p( tileX * settings::scaledCell, tileY * settings::scaledCell);
		return p;
	};

	Position tileToItsCenterPosition( int tileX, int tileY ){
		Position p = tileToItsPosition(tileX, tileY);
		p += settings::halfScaledCell;
		return p;
	};

};


