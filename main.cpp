#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <array>
#include <cstring>

#include "./headers/global.hpp"
#include "./headers/pacman.hpp"
#include "./utils/parsemap.cpp"

#include "./headers/ghost.hpp"

#include <SFML/Audio.hpp>

class Sound{
	private:
		sf::Music waka;
		sf::Music startup;
		sf::Music eatingGhost;
		sf::Music frightenedGhosts;
		sf::Music pacmanDeath;
	public:
		Sound(){
			if(!waka.openFromFile("./resources/audio/waka.wav")){
					throw EXIT_FAILURE;
			};
			
			if(!eatingGhost.openFromFile("./resources/audio/eating-ghost.wav")){
					throw EXIT_FAILURE;
			};

			if(!frightenedGhosts.openFromFile("./resources/audio/frightened-ghosts.wav")){
					throw EXIT_FAILURE;

			};
			if(!pacmanDeath.openFromFile("./resources/audio/pacman-death.wav")){
					throw EXIT_FAILURE;
			};

		};

		void playWaka(){
			if(this->waka.getStatus() != sf::Music::Playing){
				this->waka.play();
			}
		};

		void playFrightenedGhosts(){
			if(this->frightenedGhosts.getStatus() != sf::Music::Playing){
				this->frightenedGhosts.play();
			}
		};

		void stopFrightenedGhosts(){
			this->frightenedGhosts.stop();
		};

		void playEatingGhost(){
			this->eatingGhost.play();
		};

		void playPacmanDeath(){
			if(this->pacmanDeath.getStatus() != sf::Music::Playing){
				this->pacmanDeath.play();
			}
		};
};


class Game{
	using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

	private:
		sf::RenderWindow window{
				sf::VideoMode(settings::mapWidthPixels, settings::mapHeightPixels + settings::levelDisplayFontSize + settings::levelDisplayPadding * 2),
				"Pacman \\O/",
				sf::Style::Titlebar | sf::Style::Close
		};

		sf::Font monoF;
		sf::Text levelDisplayText;
		sf::Text scoreDisplayText;

		std::string levelTemplate = "Level: ";
		std::string scoreTemplate = "Score: ";

		class Pacman pacman;
		unsigned int score = 0;
		int currentLevel = 0;

		SteadyTimePoint gameStartTimepoint;
		int frighteningMillisecondsLeft;
		bool shouldGhostsScatter;
		bool isGameLost;
		bool isGameWon;
		bool hasPacmanDeathBeenDrawn;
		bool hasPacmanWonBeenDrawn;

		Sound soundManager;

		/* std::array<std::string, settings::mapTileHeight> mapSketch = { */
		/* 	" ################### ", */
		/* 	" #........#........# ", */
		/* 	" #o##.###.#.###.##o# ", */
		/* 	" #.................# ", */
		/* 	" #.##.#.#####.#.##.# ", */
		/* 	" #....#...#...#....# ", */
		/* 	" ####.### # ###.#### ", */
		/* 	"    #.#   0   #.#    ", */
		/* 	"#####.# ##=## #.#####", */
		/* 	"     .  #123#  .     ", */
		/* 	"#####.# ##### #.#####", */
		/* 	"    #.#       #.#    ", */
		/* 	" ####.# ##### #.#### ", */
		/* 	" #........#........# ", */
		/* 	" #.##.###.#.###.##.# ", */
		/* 	" #o.#.....P.....#.o# ", */
		/* 	" ##.#.#.#####.#.#.## ", */ 
		/* 	" #....#...#...#....# ", */ 
		/* 	" #.######.#.######.# ", */
		/* 	" #.................# ", */
		/* 	" ################### " */
		/* }; */

		std::array<std::string, settings::mapTileHeight> mapSketch = {
			" ################### ",
			" #........#........# ",
			" #o##.###.#.###.##o# ",
			" #.................# ",
			" #.##.#.#####.#.##.# ",
			" #....#...#...#....# ",
			" ####.### # ###.#### ",
			"    #.#   0   #.#    ",
			"#####.# ##=## #.#####",
			"     .  #123#  .     ",
			"#####.# ##### #.#####",
			"    #.#       #.#    ",
			" ####.# ##### #.#### ",
			" #........#........# ",
			" #.##.###.#.###.##.# ",
			" #o.#.....P.....#.o# ",
			" ##.#.#.#####.#.#.## ", 
			" #....#...#...#....# ", 
			" #.######.#.######.# ",
			" #.................# ",
			" ################### "
		};


        GameMap<settings::mapTileWidth, settings::mapTileHeight> gameMap;

		/*Pinky should go to top right, inky to bottom left, clyde to top right, while blinky doesn't scatter, just chases*/
		Ghost blinky{sf::Color(208,62,25)}; 
		Ghost pinky{sf::Color(234,130,229), Position(settings::mapWidthPixels, 0)}; 
		Ghost inky{sf::Color(70,191,238), Position(0, 0)}; 
		Ghost clyde{sf::Color(219,133,28), Position(settings::mapWidthPixels, settings::mapHeightPixels)}; 

		std::map<Cell, std::reference_wrapper<Ghost>> ghosts = {
			{ Cell::RedGhost, std::ref(blinky) },
			{ Cell::PinkGhost, std::ref(pinky) },
			{ Cell::BlueGhost, std::ref(inky) },
			{ Cell::OrangeGhost, std::ref(clyde)}
		};

		struct shapeSetting{
			sf::Shape* shape;
			sf::Color color;
		};

		sf::RectangleShape* wall = new sf::RectangleShape(sf::Vector2f(settings::scaledCell, settings::scaledCell));
		sf::RectangleShape* door = new sf::RectangleShape(sf::Vector2f(settings::scaledCell, (int)( settings::scaledCell / 4 )));
		sf::CircleShape* pellet = new sf::CircleShape(settings::pelletRadius, 30);
		sf::CircleShape* energizer = new sf::CircleShape(settings::energizerRadius, 30);

		std::map<Cell, shapeSetting> shapeSettings{
			{Cell::Wall, {wall, sf::Color(0, 0 , 215)}},
			{Cell::Pellet, {pellet, sf::Color(250, 250, 0)}},
			{Cell::Energizer, {energizer, sf::Color(250, 250, 0)} },  
			{Cell::Door, {door, sf::Color(200, 250, 255)} },
		};

		sf::RectangleShape* horizontalLine = new sf::RectangleShape( sf::Vector2f(settings::scaledCell * settings::mapTileWidth, 1) );
		sf::RectangleShape* verticalLine = new sf::RectangleShape( sf::Vector2f(1, settings::scaledCell * settings::mapTileWidth) );

	public:
		void reconfigureGame(){
			this->isGameLost = false;
			this->isGameWon = false;
			this->hasPacmanDeathBeenDrawn = false;
			this->hasPacmanWonBeenDrawn = false;
			this->frighteningMillisecondsLeft = 0;
			this->shouldGhostsScatter = true;
			this->gameStartTimepoint = std::chrono::steady_clock::now();

			this->pacman.direction = Direction::RIGHT;
			this->gameMap = ParseMap<settings::mapTileWidth, settings::mapTileWidth>()(this->mapSketch, this->pacman, this->ghosts );

			int newGhostBaseSpeed = this->currentLevel + settings::ghostSpeed;
			pacman.setSpeed( this->currentLevel + settings::pacmanSpeed );
			for( const auto& pair : this->ghosts ){

				Ghost& ghost = pair.second.get();
				ghost.setBaseSpeed(newGhostBaseSpeed);
				ghost.toggleNormal();
			};
		};

		Game(){
			if(!this->monoF.loadFromFile("./resources/fonts/mono.ttf")){
				throw EXIT_FAILURE;
			}

			this->levelDisplayText = sf::Text(this->scoreTemplate, this->monoF, settings::levelDisplayFontSize);
			this->levelDisplayText.setPosition(settings::levelDisplayPadding, this->window.getSize().y - settings::levelDisplayFontSize - settings::levelDisplayPadding);

			this->scoreDisplayText = sf::Text(this->scoreTemplate, this->monoF, settings::scoreDisplayFontSize);

			for(const auto& pair : this->shapeSettings){
				pair.second.shape->setFillColor(pair.second.color);
			};

			this->verticalLine->setFillColor(sf::Color(100, 100, 100));
			this->horizontalLine->setFillColor(sf::Color(100, 100, 100));

			this->reconfigureGame();
		};

		int main( int argc, char* argv[] ){
			static unsigned deltaTime;
			SteadyTimePoint currentTime, previousTime = std::chrono::steady_clock::now();

			unsigned int accumulatedElapsedTime = 0;

			for (; window.isOpen(); ) {
				sf::Event event;

				currentTime = std::chrono::steady_clock::now();
				deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime).count();
				previousTime += std::chrono::microseconds(deltaTime);

				if(settings::frameDuration > accumulatedElapsedTime){
					accumulatedElapsedTime += deltaTime;
					continue;
				} else {
					accumulatedElapsedTime = 0;
				}

				while( window.pollEvent(event) ){
					if( event.type == sf::Event::Closed ){
						window.close();
					};
				};

				if( this->isGameLost){
					this->displayGameOverScreen();
				} else if ( this->isGameWon ){
					this->displayGameWonScreen();
				} else {
					SteadyTimePoint start = std::chrono::steady_clock::now();
					this->updateGame();
					SteadyTimePoint end = std::chrono::steady_clock::now();
					unsigned execution_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
					/* std::cout << "Game update took " << execution_time << " microseconds" << std::endl; */
				};

			};

			return 0;
		};

		void displayGameWonScreen(){
			window.clear();

			if(this->hasPacmanWonBeenDrawn == false){
				this->pacman.drawDisappearance(this->window, this->hasPacmanWonBeenDrawn);
			} else {
				static sf::Text congratulationText("Congratulations, you won!", this->monoF, window.getSize().x / 16);
				congratulationText.setPosition( 
						(window.getSize().x - congratulationText.getLocalBounds().width) / 2,
						((int)(window.getSize().y / 2) - congratulationText.getLocalBounds().height )
						);

				window.draw(congratulationText);

				static sf::Text pressStartText("Press enter to continue", this->monoF, window.getSize().x / 32);
				pressStartText.setPosition( 
						(window.getSize().x - pressStartText.getLocalBounds().width) / 2,
						congratulationText.getPosition().y + congratulationText.getGlobalBounds().height * 2
						);

				window.draw(pressStartText);

				if( sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ){
					this->currentLevel++;
					this->reconfigureGame();
					return;
				};
			};

			window.display();
		};

		void displayGameOverScreen(){
			window.clear();

			if(this->hasPacmanDeathBeenDrawn == false){
				this->soundManager.playPacmanDeath();
				this->pacman.drawDisappearance(this->window, this->hasPacmanDeathBeenDrawn);
			} else {
				static sf::Text gameOverText("GAME OVER", this->monoF, window.getSize().x / 16);
				gameOverText.setPosition( 
						(window.getSize().x - gameOverText.getLocalBounds().width) / 2,
						((int)(window.getSize().y / 2) - gameOverText.getLocalBounds().height )
						);

				static sf::Text scoreText("Your score is " + std::to_string(this->score), this->monoF, window.getSize().x / 32);
				scoreText.setPosition(
						(window.getSize().x - scoreText.getLocalBounds().width) / 2,
						gameOverText.getPosition().y + gameOverText.getGlobalBounds().height * 2
						);

				static sf::Text pressStartText("Press enter to continue", this->monoF, window.getSize().x / 32);
				pressStartText.setPosition( 
						(window.getSize().x - pressStartText.getLocalBounds().width) / 2,
						scoreText.getPosition().y + scoreText.getGlobalBounds().height * 2
						);

				window.draw(gameOverText);
				window.draw(scoreText);
				window.draw(pressStartText);

				if( sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ){
					this->currentLevel = 0;
					this->score = 0;
					this->reconfigureGame();
					return;
				} 
			}

			window.display();

		}

		void updateGame(){
            static const short gridThickness = 1;
			static const Position _nullpos = Position::null();

			static SteadyTimePoint lastGhostModeSwitchTimepoint = std::chrono::steady_clock::now();
			static SteadyTimePoint lastFrighteningUpdateTimepoint = std::chrono::steady_clock::now();

			SteadyTimePoint currentTimePoint = std::chrono::steady_clock::now();
			Position currentPacmanTile = pacman.getCurrentTile();

			window.clear();

			this->isGameWon = true;
            for(int col = 0; col < this->gameMap.size(); col++){
                auto& line = this->gameMap[col];
                for(int row = 0; row < line.size(); row++){
					const Cell& cell = this->gameMap[row][col];

					if(cell == Cell::Empty) continue;
					auto* targetShape = this->shapeSettings[cell].shape;

                    if(cell == Cell::Pellet){
                        targetShape->setPosition( 
								col * settings::scaledCell + settings::halfScaledCell - settings::pelletRadius ,
								row * settings::scaledCell + settings::halfScaledCell - settings::pelletRadius 
								);
						this->isGameWon = false;
					} else if( cell == Cell::Energizer ){
                        targetShape->setPosition( 
								col * settings::scaledCell + settings::halfScaledCell - settings::energizerRadius,
								row * settings::scaledCell + settings::halfScaledCell - settings::energizerRadius
								);
					} else if ( cell == Cell::Door ){
                        targetShape->setPosition( 
								col * settings::scaledCell,
								row * settings::scaledCell + settings::halfScaledCell - door->getSize().y / 2
								);
					} else {
						targetShape->setPosition( col*settings::scaledCell, row*settings::scaledCell);
                    };

					window.draw(*targetShape);
                };
            };

			if( std::chrono::duration_cast<std::chrono::milliseconds>( currentTimePoint - lastGhostModeSwitchTimepoint ).count() > settings::ghostModeSwitchMilliseconds  ){
				this->shouldGhostsScatter = !this->shouldGhostsScatter;
				lastGhostModeSwitchTimepoint = currentTimePoint;
			};

			if( this->frighteningMillisecondsLeft > 0 ){
				unsigned timeBetweenFrighteningUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - lastFrighteningUpdateTimepoint).count();
				this->frighteningMillisecondsLeft -= timeBetweenFrighteningUpdate;

				soundManager.playFrightenedGhosts();

				lastFrighteningUpdateTimepoint = std::chrono::steady_clock::now();

				if(this->frighteningMillisecondsLeft <= settings::ghostBlinkStartThresholdMilliseconds){
					static short blinkTimerAcc;
					constexpr unsigned f = settings::ghostBlinkDurationMilliseconds * 2;
					blinkTimerAcc = (blinkTimerAcc + timeBetweenFrighteningUpdate) % f;
					for(auto& pair : this->ghosts){
						Ghost& ghost = pair.second.get();
						if(blinkTimerAcc > settings::ghostBlinkDurationMilliseconds){
							ghost.forceWhiteColor = true;
						} else {
							ghost.forceWhiteColor = false;
						}
					};
				}

				if(this->frighteningMillisecondsLeft <= 0){
					for(auto& pair : this->ghosts){
						Ghost& ghost = pair.second.get();
						if(!ghost.isEscaping){
							ghost.toggleNormal();
						};
					};

					this->frighteningMillisecondsLeft = 0;
					soundManager.stopFrightenedGhosts();
				};

			};

			for (const auto& pair : ghosts) {
				Ghost& ghost = pair.second;

				if(ghost.getCurrentTile() == currentPacmanTile && !ghost.isEscaping){
					if(ghost.isFrightened){
						this->score += settings::ghostCapturePoints;
						soundManager.playEatingGhost();
						ghost.toggleEscaping();
					} else {
						this->isGameLost = true;
					};
				};

				if(ghost.isEscaping){
					static const Position mapCenter = Position(settings::mapWidthPixels/2, settings::mapHeightPixels/2);
					ghost.chase(mapCenter);
				} else {
					ghost.chase( ghost.scatterPosition != _nullpos && shouldGhostsScatter ? ghost.scatterPosition : pacman.position);
				};

				ghost.update(this->gameMap);
				ghost.draw(this->window);
			};

			Cell& pacmanSteppingCell = this->gameMap[currentPacmanTile.y][currentPacmanTile.x];
			if(pacmanSteppingCell == Cell::Pellet ){
				pacmanSteppingCell = Cell::Empty;
				this->score += settings::pelletEatingPoints;
				this->soundManager.playWaka();

			} else if(pacmanSteppingCell == Cell::Energizer ){
				if(this->frighteningMillisecondsLeft < settings::maximumFrighteningTimeMilliseconds){
					this->frighteningMillisecondsLeft += settings::frighteningTimeMilliseconds;

					if(this->frighteningMillisecondsLeft > settings::maximumFrighteningTimeMilliseconds){
						this->frighteningMillisecondsLeft = settings::maximumFrighteningTimeMilliseconds;
					}
				}

				lastFrighteningUpdateTimepoint = std::chrono::steady_clock::now();

				for( const auto& pair : this->ghosts ){
					auto& ghost = pair.second.get();

					if(!ghost.isEscaping){
						ghost.toggleFrightening();
					};

				}

				pacmanSteppingCell = Cell::Empty;
			}
			
			/* draw grid */
            for (int i = 0; i < settings::mapTileWidth; ++i) {
                horizontalLine->setPosition(sf::Vector2f(0, i * settings::scaledCell));
                window.draw(*horizontalLine);
            }
            for (int i = 0; i < settings::mapTileWidth; ++i) {
                verticalLine->setPosition(sf::Vector2f(i * settings::scaledCell, 0));
                window.draw(*verticalLine);
            }

            pacman.update(this->gameMap);
			pacman.draw(this->window);

			this->levelDisplayText.setString(levelTemplate + std::to_string(this->currentLevel));
			window.draw(this->levelDisplayText);

			this->scoreDisplayText.setString(scoreTemplate + std::to_string(this->score));
			this->scoreDisplayText.setPosition(
					this->window.getSize().x - this->scoreDisplayText.getLocalBounds().width - settings::scoreDisplayPadding,
					this->window.getSize().y - settings::scoreDisplayFontSize - settings::scoreDisplayPadding
					);

			window.draw(this->scoreDisplayText);
			window.display();
		};

		~Game(){
			delete this->wall;
			delete this->pellet;
			delete this->energizer;
			delete this->door;
		};
};

int main(int argc, char *argv[]){
	return Game{}.main(argc, argv);
}
