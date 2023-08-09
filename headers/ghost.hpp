#ifndef GHOST_H
#define GHOST_H

#include "./movable.hpp"
#include <SFML/Audio.hpp>

class Ghost: public Movable {
	private:
		sf::Color color;
		sf::Color originalSpriteColor;

		Position chasePosition;
		int baseSpeed;

		sf::Music escapingSound;

	public:
		Position scatterPosition = Position::null();
		bool isFrightened = false;
		bool isEscaping = false;
		bool forceWhiteColor = false;

		Ghost( sf::Color color );
		Ghost( sf::Color color, Position scatterPosition );

		void chase( Position p );;

		void setBaseSpeed( int baseSpeed );

		void toggleEscaping();

		void toggleFrightening();

		void toggleNormal();

		Direction getDirectionAfter( GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, const Position& targetPos );

		bool canTurn(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, Direction targetDirection) override;

		bool update(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap) override;;

		bool draw( sf::RenderWindow& targetwin ) override;

		virtual ~Ghost ();
};

#endif
