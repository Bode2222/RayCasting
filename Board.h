#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

class Board
{
public:
	Board();
	int getXQuads();
	int getYQuads();
	float getWidth();
	float getSpace();
	int getEdgesNumber();
	float getStartEdgex(int i);
	float getStartEdgey(int i);
	float getEndEdgex(int i);
	float getEndEdgey(int i);
	void updateEdgePool();
	void updateRayPool(float mouseX, float mouseY);
	float getRayX(int i);
	float getRayY(int i);
	int getRayNumber();
	bool isOn(int x, int y);
	void turnOn(int x, int y);
	void turnOff(int x, int y);
	~Board();

private:
	static const int xQuads = 40;
	static const int yQuads = 32;
	float width = 16;
	float space = 0;
	const int NORTH = 0;
	const int EAST = 1;
	const int SOUTH = 2;
	const int WEST = 3;

	struct Cell {
		bool on = false;
		bool edge[4];
		int edgeID[4];
	};
	Cell cellGrid[xQuads][yQuads];

	struct Edge {
		float sx, sy, ex, ey;
	};
	std::vector<Edge> edgePool;

	struct Ray {
		float theta = 0.0f;
		float x = 0;//ending x position value of the ray
		float y = 0;//ending y position value of the ray
		//Note: the starting x and y is the mouse position
		float slope = 0;
		float distance = 0;
	};
	std::vector<Ray> rayPool;

};

