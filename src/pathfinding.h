#include <iostream>
#include <cstring>
#include "raycaster.h"

const int INF = 9999999;

struct Coordinate 
{
	int x;
	int y;
	Coordinate():x(0), y(0){}
	Coordinate(int _x, int _y) : x(_x), y(_y) {}
};

struct Node {
	Coordinate pos;
	int g_score; // cost from start node
	int h_score; // estimated cost to goal
	int f_score; // total cost
	int parentIndex; // Index of the parent node in the closed list
	Node(){};
	Node(const Coordinate& _pos, int _g_score, int _h_score, int _parentIndex)
	: pos(_pos), g_score(_g_score), h_score(_h_score), parentIndex(_parentIndex) {
	f_score = g_score + h_score;
	}
};

// Heuristic function (Chebyshev distance for diagonal movement)
int heuristic(const Coordinate& current, const Coordinate& goal) 
{
	int dx = abs(current.x - goal.x);
	int dy = abs(current.y - goal.y);
	return std::max(dx, dy);
}

// Check if a coordinate is within the map boundaries and is passable (value of 0 in MAPDATA)
bool isValid(const Coordinate& pos, const u8* mapData) 
{
	return pos.x >= 0 && pos.x < MAP_SIZE && pos.y >= 0 && pos.y < MAP_SIZE && mapData[pos.y * MAP_SIZE + pos.x] == 0;
}

void findPath(const Coordinate& start, const Coordinate& goal, const u8* mapData, Coordinate* path, int& pathLength) {
	return;
	if (!isValid(start, mapData) || !isValid(goal, mapData)) {
		std::cout << "Invalid start or goal position." << std::endl;
		return;
	}

	Node openList[MAP_SIZE * MAP_SIZE];
	Node closedList[MAP_SIZE * MAP_SIZE];
	int openCount = 0;
	int closedCount = 0;

	openList[openCount++] = Node(start, 0, heuristic(start, goal), -1);

	while (openCount > 0) 
	{
		// Find the node with the lowest f_score in the openList
		int current = 0;
		for (int i = 1; i < openCount; ++i) {
			if (openList[i].f_score < openList[current].f_score ||
				(openList[i].f_score == openList[current].f_score && openList[i].h_score < openList[current].h_score)) {
				current = i;
			}
		}

		if (openList[current].pos.x == goal.x && openList[current].pos.y == goal.y) {
			// Reconstruct the path
			pathLength = 0;
			int currentIndex = current;
			while (currentIndex != -1) {
				path[pathLength++] = openList[currentIndex].pos;
				currentIndex = openList[currentIndex].parentIndex;
			}

			// Reverse the path in place
			for (int i = 0, j = pathLength - 1; i < j; ++i, --j) {
				Coordinate temp = path[i];
				path[i] = path[j];
				path[j] = temp;
			}

			return;
		}

		// Move the current node from openList to closedList
		closedList[closedCount++] = openList[current];
		openList[current] = openList[--openCount];

		const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
		const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

		for (int i = 0; i < 8; ++i) { // 8 possible directions (diagonals included)
			Coordinate neighbor(openList[current].pos.x + dx[i], openList[current].pos.y + dy[i]);
			if (!isValid(neighbor, mapData))
				continue;

			int tentative_g_score = openList[current].g_score + ((i % 2 == 0) ? 14 : 10); // 14 for diagonal, 10 for straight movement
			int tentative_f_score = tentative_g_score + heuristic(neighbor, goal);

			int openIndex = -1;
			for (int j = 0; j < openCount; ++j) {
				if (openList[j].pos.x == neighbor.x && openList[j].pos.y == neighbor.y) {
					openIndex = j;
					break;
				}
			}

			int closedIndex = -1;
			for (int j = 0; j < closedCount; ++j) {
				if (closedList[j].pos.x == neighbor.x && closedList[j].pos.y == neighbor.y) {
					closedIndex = j;
					break;
				}
			}

			if (openIndex != -1 && tentative_f_score >= openList[openIndex].f_score)
				continue;

			if (closedIndex != -1 && tentative_f_score >= closedList[closedIndex].f_score)
				continue;

			if (openIndex != -1)
				openList[openIndex] = openList[--openCount];

			if (closedIndex != -1)
				closedList[closedIndex] = closedList[--closedCount];

			openList[openCount++] = Node(neighbor, tentative_g_score, heuristic(neighbor, goal), closedCount - 1);
		}
	}

	std::cout << "No path found from start to goal." << std::endl;
	return;
}