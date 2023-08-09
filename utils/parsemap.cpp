#ifndef PARSEMAP_T
#define PARSEMAP_T

#include <map>
#include <array>
#include <string>
#include "../headers/pacman.hpp"
#include "../headers/cell.hpp"
#include "../headers/global.hpp"
#include "../headers/ghost.hpp"

template <std::size_t Width, std::size_t Height>
struct ParseMap{
    GameMap<Width, Height> operator()( std::array<std::string, Height>& sketch, class Pacman& pacman, std::map<Cell, std::reference_wrapper<Ghost>> ghosts)
	{
		static const std::map<char, Cell> sign2cell{
			{' ', Cell::Empty},
			{'#', Cell::Wall},
			{'=', Cell::Door},
			{'.', Cell::Pellet},
			{'o', Cell::Energizer},
			{'P', Cell::Pacman},
			{'0', Cell::RedGhost},
			{'1', Cell::BlueGhost},
			{'2', Cell::PinkGhost},
			{'3', Cell::OrangeGhost},
		};

        std::array< std::array<Cell, Width>, Height> res;
        for (int row = 0; row < sketch.size(); row++) {
            auto& line = sketch[row];
            for (int col = 0; col <  line.size() ; col++) {
				res[row][col] = Cell::Empty; 

                char* ch = &line[col];
                Cell cell = sign2cell.at(*ch);

                if(cell == Cell::Pacman){
                    pacman.position = Position(col* settings::scaledCell, row* settings::scaledCell);
				} else if (cell >= Cell::RedGhost){
					Ghost& ghost = ghosts.at(cell).get();
					ghost.position = Position(col* settings::scaledCell, row* settings::scaledCell);
                } else {
                    res[row][col] = sign2cell.at(*ch);
                }

            };
        };

        return res;
    }
};


#endif


