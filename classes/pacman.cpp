#include "../headers/pacman.hpp"

#include <sstream>
#include "../headers/hunt.hpp"

using sfk = sf::Keyboard;
Pacman::Pacman(){
	/* this->speed = settings::pacmanSpeed; */
	this->setSpeed(settings::pacmanSpeed);

	if(!this->texture.loadFromFile("./resources/textures/Pacman" + std::to_string(settings::cellSize) + ".png")){
		throw EXIT_FAILURE;
	}

	this->sprite.setTexture(this->texture);
	this->sprite.setScale(settings::mapScaleFactor, settings::mapScaleFactor);

	if(!this->deathTexture.loadFromFile("./resources/textures/PacmanDeath" + std::to_string(settings::cellSize) + ".png")){
		throw EXIT_FAILURE;
	}

	this->deathSprite.setTexture(this->deathTexture);
	this->deathSprite.setScale(settings::mapScaleFactor, settings::mapScaleFactor);
};

bool Pacman::canTurn(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, Direction targetDirection) {
	Cell facingCell;
	bool turnable = this->isTurnable(gamemap, targetDirection, facingCell);

	if(turnable && facingCell == Cell::Door){
		return false;
	}

	return turnable;
};


bool Pacman::update(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap){
	Direction newdir = this->direction;
	if(sfk::isKeyPressed(sfk::Up)){
		newdir = Direction::UP;
	} 
	else if(sfk::isKeyPressed(sfk::Down)){
		newdir = Direction::DOWN;
	} 
	else if(sfk::isKeyPressed(sfk::Left)){
		newdir = Direction::LEFT;
	} 
	else if(sfk::isKeyPressed(sfk::Right)){
		newdir = Direction::RIGHT;
	};

	if(newdir != this->direction){
		bool p = this->turn(gamemap, newdir);
	};

	return this->move(gamemap);
};

auto dbgcircle = sf::CircleShape(2, 30);
bool Pacman::draw( sf::RenderWindow& targetwin ){
	/* should fix it later; make things independent of each other */
	static unsigned short counter = 0;

	this->sprite.setTextureRect( 
			sf::IntRect(
				floor(counter/(float)(this->getSpeed())) * settings::cellSize,
				settings::cellSize * this->direction,
				settings::cellSize,
				settings::cellSize
				));

	this->sprite.setPosition(this->position.x, this->position.y);
	targetwin.draw(this->sprite);

	dbgcircle.setFillColor(sf::Color(255, 0, 255));

	Position facingPos = this->getFacingPosition();
	dbgcircle.setPosition(facingPos.x, facingPos.y);
	targetwin.draw(dbgcircle);

	dbgcircle.setFillColor(sf::Color(255, 0, 0));
	dbgcircle.setPosition(this->getCenteredPosition().x, getCenteredPosition().y);
	targetwin.draw(dbgcircle);

	counter = ++counter % ( this->getSpeed() * settings::pacmanAnimationFrames );
	return true;
};

bool Pacman::drawDisappearance( sf::RenderWindow& targetwin, bool& concluded ){
	static short counter = 0;
	static int currentFrame = 0;

	static int tempo = settings::pacmanDisappearanceTempo;

	currentFrame = floor(counter/(float)(tempo));
	this->deathSprite.setTextureRect( sf::IntRect(
				currentFrame * settings::cellSize,
				0,
				settings::cellSize,
				settings::cellSize
				));
	this->deathSprite.setPosition(this->position.x, this->position.y);
	targetwin.draw(this->deathSprite);

	if(currentFrame >= settings::pacmanDisappearanceAnimationFrames){
		currentFrame = 0;
		concluded = true;
	} else{
		concluded = false;
	}

	counter = ++counter % ( tempo * settings::pacmanDisappearanceAnimationFrames + 1 );

	return true;
};

Pacman::~Pacman (){
}
