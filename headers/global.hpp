#ifndef GLOBAL_H
#define GLOBAL_H

#include <array>
#include "./cell.hpp"

namespace settings{
	constexpr unsigned short frameRate = 60;
	constexpr unsigned int frameDuration = 1'000'000 / frameRate;

	constexpr unsigned short levelDisplayFontSize = 16;
	constexpr unsigned short levelDisplayPadding = 8;

	constexpr unsigned short scoreDisplayFontSize = 16;
	constexpr unsigned short scoreDisplayPadding = 8;

	constexpr unsigned short mapTileWidth = 21;
	constexpr unsigned short mapTileHeight = 21;
	constexpr unsigned short mapScaleFactor = 2;

	constexpr unsigned short cellSize = 16;
	constexpr unsigned short scaledCell = settings::mapScaleFactor * settings::cellSize;
	constexpr unsigned short halfScaledCell = scaledCell / 2;
	constexpr unsigned short mapWidthPixels = mapTileWidth * cellSize * mapScaleFactor;
	constexpr unsigned short mapHeightPixels = mapTileHeight * cellSize * mapScaleFactor;

	constexpr unsigned short pelletRadius = scaledCell / 8;
	constexpr unsigned short energizerRadius = 8;

	constexpr unsigned short pacmanAnimationFrames = 6;
	constexpr unsigned short pacmanSpeed = 4;
	constexpr unsigned short ghostAnimationFrames = 6;
	constexpr unsigned short ghostSpeed = 2; 
	constexpr unsigned short escapingGhostSpeed = 8; 
	constexpr unsigned short frightenedGhostSpeed = 1; 

	constexpr unsigned short pacmanDisappearanceAnimationFrames = 11;
	constexpr unsigned short pacmanDisappearanceMilliSeconds = 1500;
	constexpr unsigned short pacmanDisappearanceTempo = pacmanDisappearanceMilliSeconds * (frameRate/ pacmanDisappearanceAnimationFrames) / 1000 ;


	constexpr unsigned short frighteningTimeMilliseconds = 4 * 1'000;
	constexpr unsigned short maximumFrighteningTimeMilliseconds = 6 * 1'000;
	constexpr unsigned int ghostModeSwitchMilliseconds = 6 * 1000;

	constexpr unsigned int ghostBlinkStartThresholdMilliseconds = 1000;
	constexpr unsigned int ghostBlinkDurationMilliseconds = 100;

	constexpr unsigned short ghostCapturePoints = 50;
	constexpr unsigned short pelletEatingPoints = 1;
};


template <std::size_t Width, std::size_t Height>
using GameMap = std::array<std::array<Cell, Width>, Height>;

#endif
