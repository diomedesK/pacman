#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string.h>

#include "../headers/hunt.hpp"
#include "../headers/ghost.hpp"

Ghost::Ghost( sf::Color color ): color(color){
	this->toggleNormal();

	if(!this->texture.loadFromFile("./resources/textures/Ghost" + std::to_string(settings::cellSize) + ".png")){
		throw EXIT_FAILURE;
	}

	if (!escapingSound.openFromFile("./resources/audio/cartoon-running-fx.wav")){
		throw EXIT_FAILURE;
	}

	this->sprite.setTexture(this->texture);
	this->sprite.setScale(settings::mapScaleFactor, settings::mapScaleFactor);

	originalSpriteColor = this->sprite.getColor();
};


Ghost::Ghost( sf::Color color, Position scatterPosition ): Ghost(color){
	this->scatterPosition = scatterPosition;
};

void Ghost::chase( Position p ){
	this->chasePosition = p;
};

void Ghost::setBaseSpeed( int baseSpeed ){
	this->baseSpeed = baseSpeed;
}

void Ghost::toggleEscaping(){
	this->escapingSound.play();
	this->isEscaping = true;
	this->isFrightened = false;
};

void Ghost::toggleFrightening(){
	this->isFrightened = true;
	this->isEscaping = false;
};

void Ghost::toggleNormal(){
	this->isEscaping = false;
	this->isFrightened = false;
	this->forceWhiteColor = false;
};

Direction Ghost::getDirectionAfter( GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, const Position& targetPos ){
	static const std::map<Direction, Direction> opposingDirections{
		{ Direction::RIGHT, Direction::LEFT },
			{ Direction::LEFT, Direction::RIGHT },
			{ Direction::UP, Direction::DOWN },
			{ Direction::DOWN, Direction::UP },
	};

	Direction electedDir;

	int shorterDistanceYet = INT_LEAST32_MAX;
	auto possibleDirections = this->getPossibleDirections(gamemap);

	Position currentTile = this->getCurrentTile();
	for( Direction possibleDir : possibleDirections  ){
		Position possibleNextPos;
		Position nextTile;

		switch(possibleDir){
			case UP:
				nextTile = Position( currentTile.x, currentTile.y - 1 );
				break;
			case DOWN:
				nextTile = Position( currentTile.x, currentTile.y + 1 );
				break;
			case LEFT:
				nextTile = Position( currentTile.x - 1, currentTile.y);
				break;
			case RIGHT:
				nextTile = Position( currentTile.x + 1, currentTile.y);
				break;
		};
		possibleNextPos = hunt::tileToItsCenterPosition(nextTile.x, nextTile.y);

		if(hunt::isPositionOutsideMap(possibleNextPos)){
			return opposingDirections.at(this->direction);
		}

		if( gamemap[nextTile.y][nextTile.x] == Cell::Door ){
			/*Always escape by the door if possible*/
			return possibleDir;
		}

		if( possibleDirections.size() > 1 && opposingDirections.at(this->direction) == possibleDir ){
			/*Make sure to allow turning back only if it is the only direction left*/
			continue;
		};

		int distance = hunt::getDistance(possibleNextPos.x, possibleNextPos.y, targetPos.x, targetPos.y);

		if(distance < shorterDistanceYet){
			shorterDistanceYet = distance;
			electedDir = possibleDir;
		};

	};

	return electedDir;
};

bool Ghost::canTurn(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, Direction targetDirection){
	Cell facingCell;
	bool turnable = this->isTurnable(gamemap, targetDirection, facingCell);

	if(turnable && facingCell == Cell::Door){
		/*Allow passing throught doors if it's going on the up direction (necessary for exiting the central square), 
		 *or if in escape mode*/
		if(this->isEscaping || targetDirection == Direction::UP){
			return true;
		}  else {
			return false;
		}
	} else {
		return turnable;
	}

};

bool Ghost::update(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap){
	Position currentTile = this->getCurrentTile();

	if(this->isEscaping && currentTile.y == 9 && ( currentTile.x >= 9 && currentTile.x <= 11 ) ){
		/*These are the mappings for the region where the ghosts should go when they are escaping*/
		this->toggleNormal();
	};

	if(this->isFrightened){
		this->setSpeed(settings::frightenedGhostSpeed);
	} else if(this->isEscaping){
		this->setSpeed(settings::escapingGhostSpeed);
	} else {
		this->setSpeed(this->baseSpeed);
	};

	if(this->position.x % settings::scaledCell <= this->getMovementAllowedMargin() && this->position.y % settings::scaledCell <= this->getMovementAllowedMargin() ){
		Direction newdir = this->getDirectionAfter(gamemap, this->chasePosition);

		if(this->direction != newdir){
			this->turn(gamemap, newdir);
		};
	};

	return this->move(gamemap);
};

bool Ghost::draw( sf::RenderWindow& targetwin ){
	auto dbgcircle = sf::CircleShape(2, 30);

	static unsigned short count = 0;
	static const std::map<Direction, short> direction2eye{
		/*Direction to sprite mapping*/
		{ Direction::RIGHT, 0 },
			{ Direction::UP,    1 },
			{ Direction::LEFT,  2 },
			{ Direction::DOWN,  3 },
	};

	this->sprite.setPosition(this->position.x, this->position.y);

	/* body sprites */
	this->sprite.setTextureRect(sf::IntRect((int)(count/(float)(this->getSpeed())) * settings::cellSize, 0, 16, 16));
	if(this->isFrightened){
		static const sf::Color frightenedColor = sf::Color(0, 0, 200);
		this->sprite.setColor(this->forceWhiteColor? sf::Color::White : frightenedColor);
		this->forceWhiteColor = false;

	} else if(this->isEscaping){
		static const sf::Color escapingColor = sf::Color(0, 0, 0);
		this->sprite.setColor(escapingColor);

	} else {
		this->sprite.setColor(this->color);
	}

	targetwin.draw(this->sprite);

	auto csz = settings::cellSize;

	/* eye sprites */
	this->sprite.setColor(this->originalSpriteColor);

	if(this->isFrightened){
		this->sprite.setTextureRect(sf::IntRect( 4 * csz, 1 * csz, csz, csz));

	} else if( this->isEscaping ){
		this->sprite.setTextureRect(sf::IntRect(direction2eye.at(this->direction) * csz, 2 * csz, csz, csz));

	} else{
		this->sprite.setTextureRect(sf::IntRect(direction2eye.at(this->direction) * csz, 1 * csz, csz, csz));
	}
	targetwin.draw(this->sprite);

	Position facingPos = this->getFacingPosition();
	dbgcircle.setPosition(facingPos.x, facingPos.y);
	targetwin.draw(dbgcircle);

	count = ++count % ( this->getSpeed() * settings::ghostAnimationFrames );
	return true;
};

Ghost::~Ghost (){
};

