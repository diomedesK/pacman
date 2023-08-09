#ifndef POSITION_H
#define POSITION_H

#include <ostream>
#include <stdint.h>

struct Position{
	short x = 0;
	short y = 0;


    Position(){ };

	Position(short tx, short ty): x(tx), y(ty){ };

    Position(const Position& p2): x(p2.x), y(p2.y){ };

	static Position null(){
		return Position(INT16_MIN, INT16_MIN);
	}

	bool operator==(const Position& p2){
		return p2.x == this->x && p2.y == this->y;
	};

	bool operator!=(const Position& p2){
		return !this->operator==(p2);
	};

	friend std::ostream& operator<<(std::ostream& co, const Position& pos){
		auto const BUFFSZ = 50;
		char* buff = (char*) malloc(sizeof(char) * BUFFSZ);
		std::snprintf(buff, BUFFSZ, "{x: %d, y: %d }", pos.x, pos.y);
		co << buff;

		delete[] buff;
		return co;
	};

	Position operator+( std::tuple<short, short> pos ){
        Position p;
        p.x = this->x + std::get<0>(pos);
        p.y = this->y + std::get<1>(pos);

		return p;
	};

	Position operator+( const Position posb ){
        Position p;
        p.x = this->x + posb.x;
        p.y = this->y + posb.y;

		return p;
	};

	Position operator*( double d ){
        Position p;
        p.x = this->x * d;
        p.y = this->y * d;

		return p;
	};

	void operator+=( std::tuple<short, short> pos ){
		this->x += std::get<0>(pos);
		this->y += std::get<1>(pos);
	};

	void operator-=( std::tuple<short, short> pos ){
		this->x -= std::get<0>(pos);
		this->y -= std::get<1>(pos);
	};

	void operator+=( int inc ){
		this->x += inc;
		this->y += inc;
	}

	void operator-=( int inc ){
		this->x -= inc;
		this->y -= inc;
	}

	void operator=( std::tuple<short, short> pos ){
		this->x = std::get<0>(pos);
		this->y = std::get<1>(pos);
	};

};


#endif
