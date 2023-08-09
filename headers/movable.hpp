#ifndef MOVABLE_H
#define MOVABLE_H

#include <SFML/Graphics.hpp>

#include "./global.hpp"
#include "./direction.hpp"
#include "./position.hpp"

class Movable {
	private:
		int speed;
		int movementAllowedMargin;

    protected:
        sf::Texture texture;
        sf::Sprite sprite;

    public:
		Movable();

        Position position;
        Direction direction = Direction::RIGHT;

        Position getCenteredPosition();
		Position getFacingPosition();
		Position getCurrentTile();

		void setSpeed(int);
		int getSpeed();

		void setMovementAllowedMargin(int);
		int getMovementAllowedMargin();

		virtual bool isTurnable(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, Direction targetDirection, Cell &facingCellGift );
		virtual bool update(GameMap<settings::mapTileWidth, settings::mapTileHeight>&) = 0;

		virtual bool canTurn(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, Direction targetDirection) = 0;
		virtual bool draw( sf::RenderWindow& targetwin ) = 0;

		bool turn(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, Direction targetDirection );
		bool move(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap);

		void recenter();

		std::vector<Direction> getPossibleDirections(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap);

		std::string getDirectionName(Direction dir){
			std::string res = "";
			switch (dir) {
				case RIGHT:
					res = "RIGHT";
					break;
				case UP:
					res =  "UP";
					break;
				case LEFT:
					res =  "LEFT";
					break;
				case DOWN:
					res =  "DOWN";
					break;
			}
			return res;
		};

        virtual ~Movable();
};

#endif
