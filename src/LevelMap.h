#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Scene;

#define DEADEND_0	'T'
#define DEADEND_90	'E'
#define DEADEND_180 'L'
#define DEADEND_270 'J'

#define CORRIDOR_0	'|'
#define CORRIDOR_90	'-'

#define CORNER_0	'p'
#define CORNER_90	'b'
#define CORNER_180	'd'
#define CORNER_270	'q'

#define SIDEWALL_0		'^'
#define SIDEWALL_90		'<'
#define SIDEWALL_180	'_'
#define SIDEWALL_270	'>'

struct LevelPieceEntry {
	int x;
	int y;
};

class LevelMap
{
private:
	std::unordered_map<char, std::vector<LevelPieceEntry>> pieces_;
public:
	LevelMap(Scene* scene, std::string levelPath);
};