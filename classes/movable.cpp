#include "../headers/movable.hpp"
#include <iostream>

#include "../headers/hunt.hpp"

#include <cmath>

Movable::Movable(){
}

Position Movable::getCenteredPosition(){
	short cx = this->position.x + settings::halfScaledCell;
	short cy = this->position.y + settings::halfScaledCell;

	Position p;
	p.x = cx;
	p.y = cy;

	return p;
};

void Movable::setSpeed(int newSpeed){
	this->speed = newSpeed;
	this->movementAllowedMargin = (hunt::getLowestGapPossible(this->speed, settings::scaledCell));
};

int Movable::getSpeed(){
	return this->speed;
};

void Movable::setMovementAllowedMargin(int newMargin){
	this->movementAllowedMargin = newMargin;
};

int Movable::getMovementAllowedMargin(){
	return this->movementAllowedMargin;
};

Position Movable::getFacingPosition(){
	Position res = this->getCenteredPosition();
	switch( this->direction ){
		case Direction::UP:
			res.y -= settings::halfScaledCell + 1;
			break;
		case Direction::DOWN:
			res.y += settings::halfScaledCell + 0;
			break;
		case Direction::LEFT:
			res.x -= settings::halfScaledCell + 1;
			break;
		case Direction::RIGHT:
			res.x += settings::halfScaledCell + 0;
			break;
	};

	return res;
}

Position Movable::getCurrentTile(){
	Position center = this->getCenteredPosition();
	Position p((int)(center.x / settings::scaledCell), (int)(center.y / settings::scaledCell));
	return p;
}

void Movable::recenter(){
	Position currentTileCenterPosition = this->getCurrentTile();
	this->position = hunt::tileToItsPosition(currentTileCenterPosition.x, currentTileCenterPosition.y);
};

bool Movable::isTurnable(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, Direction targetDirection, Cell& facingCellGift ){

	Position currentTile = this->getCurrentTile();
	Position currentTileCenterPosition = hunt::tileToItsCenterPosition(currentTile.x, currentTile.y);

	Position possiblepos(currentTileCenterPosition);

	/* Check if input direction leads to facing a wall. If it doesn't, then allow the direction change */
	switch( targetDirection ){
		case Direction::LEFT:
			possiblepos.x -= settings::scaledCell;
			break;
		case Direction::RIGHT:
			possiblepos.x += settings::scaledCell;
			break;
		case Direction::UP:
			possiblepos.y -= settings::scaledCell;
			break;
		case Direction::DOWN:
			possiblepos.y += settings::scaledCell;
			break;
	};

	auto currentCenter = this->getCenteredPosition();

	const Cell facingCell = gamemap[possiblepos.y / settings::scaledCell][possiblepos.x / settings::scaledCell];
	facingCellGift = facingCell;

	if( facingCell != Cell::Wall ){
		/* I chose this slightly more complicated approach because it allows the usage of any speed value for the movable.
		 * Briefly, a margin is calculated based on the movable current speed. If the distance from the 
		 * object center position and the current tile center position is less or equal to the margin,
		 * a direction change is allowed.
		 * That is because we want objects to move only if they are in the mid of a tile, but not al speed increments
		 * will lead the object to the center of the tile. 
		 */

		int margin = this->getMovementAllowedMargin();

		if( ( ( targetDirection == Direction::UP || targetDirection == Direction::DOWN ) && std::abs( currentCenter.x - currentTileCenterPosition.x ) <= margin) || 
			( ( targetDirection == Direction::LEFT || targetDirection == Direction::RIGHT ) && std::abs( currentCenter.y - currentTileCenterPosition.y ) <= margin)
		  ) {
			if(margin != 0){ 
				/*I.e. the current speed is not a factor of the cell size. 
				  Recentering is a must, otherwise may not get aligned/ghost wont find the right directions */
				this->recenter(); 
			};

			return true;
		}

	}

	return false;
};

bool Movable::turn(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, Direction targetDirection ){
	if( this->canTurn(gamemap, targetDirection) && !hunt::isPositionOutsideMap(this->position) ){
		this->direction = targetDirection;
		return true;
	} else {
		return false;
	}
};


std::vector<Direction> Movable::getPossibleDirections(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap){
	std::vector<Direction> res;
	static const std::array<Direction, 4> possibleDirections{ Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT };

	for ( Direction d: possibleDirections ) {
		if(this->canTurn(gamemap, d)){
			res.push_back(d);
		};
	};

	return res;
};

bool Movable::move(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap){
	using sfk = sf::Keyboard;

	std::map<Direction, std::tuple<short, short> > direction2move{
		{ Direction::RIGHT,    {this->speed,  0,} },
		{ Direction::UP,       {0, -this->speed,} },
		{ Direction::LEFT,     {-this->speed, 0,} },
		{ Direction::DOWN,     {0,  this->speed,} },
	};

	Position facingPos = this->getFacingPosition();

	if(hunt::isPositionOutsideMap(facingPos)){
		this->position += direction2move.at(this->direction);

		if(this->position.x > (signed)(settings::mapWidthPixels)){
			this->position.x = -settings::scaledCell;
		} 
		else if ( this->position.x < -settings::scaledCell ){
			this->position.x = settings::mapWidthPixels;
		}
		else if ( this->position.y < -settings::scaledCell ){
			this->position.y = settings::mapHeightPixels;
		}
		else if ( this->position.y > (signed)(settings::mapHeightPixels) ){
			this->position.y = -settings::scaledCell;
		}

		return true;
	} else if( gamemap[(int)(facingPos.y / settings::scaledCell)][(int)(facingPos.x / settings::scaledCell)] != Cell::Wall ){ 
		this->position += direction2move.at(this->direction);
		return true;
	}

	if(this->position.x % settings::scaledCell != 0 || this->position.y % settings::scaledCell != 0 ){
		/*If movable is stuck, then recenter it*/
		this->recenter();
	}

	return false;
};

Movable::~Movable(){
}
