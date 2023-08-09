#ifndef PACMAN_H
#define PACMAN_H

#include <SFML/Graphics.hpp>

#include <cmath>
#include <iostream>

#include "./global.hpp"
#include "./position.hpp"
#include "./direction.hpp"
#include "./movable.hpp"

class Pacman: public Movable {
	public:
		Pacman();

		sf::Sprite deathSprite;
		sf::Texture deathTexture;

        bool update(GameMap<settings::mapTileWidth, settings::mapTileHeight>&) override;
		bool canTurn(GameMap<settings::mapTileWidth, settings::mapTileHeight>& gamemap, Direction targetDirection) override;
		bool draw( sf::RenderWindow& targetwin ) override;
		bool drawDisappearance( sf::RenderWindow& targetwin, bool& concluded );

        ~Pacman();
};

#endif
