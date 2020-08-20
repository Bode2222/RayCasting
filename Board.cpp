#include "Board.h"

//sets borders of the grid by turnining cells at the edges on
Board::Board()
{
	for (int i = 0; i < yQuads; i++) {
		for (int j = 0; j < xQuads; j++) {
			if (j == 1 || j == xQuads - 2) {
				cellGrid[j][i].on = true;
			}
			if (i == 1 || i == yQuads - 2) {
				cellGrid[j][i].on = true;
			}
			if (j == 1 && i == 0) {
				cellGrid[j][i].on = false;
			}
			if (j == xQuads - 2 && i == 0) {
				cellGrid[j][i].on = false;
			}
			if (j == 0 && i == 1) {
				cellGrid[j][i].on = false;
			}
			if (j == xQuads - 1 && i == 1) {
				cellGrid[j][i].on = false;
			}
			if (j == 0 && i == yQuads - 2) {
				cellGrid[j][i].on = false;
			}
			if (j == xQuads -1  && i == yQuads - 2) {
				cellGrid[j][i].on = false;
			}
			if (j == 1 && i == yQuads - 1) {
				cellGrid[j][i].on = false;
			}
			if (j == xQuads - 2 && i == yQuads - 1) {
				cellGrid[j][i].on = false;
			}
		}
	}
}

//sets specified cells' on bool to true
void Board::turnOn(int x, int y) {
	cellGrid[x][y].on = true;
}

//sets specified cells' on bool to false
void Board::turnOff(int x, int y) {
	cellGrid[x][y].on = false;
}

//returns the number of horizontal squares in the grid
int Board::getXQuads() {
	return xQuads;
}

//returs the number of vertical squares in the grid
int Board::getYQuads() {
	return yQuads;
}

//returns the width of each square
float Board::getWidth() {
	return width;
}

//returns the space between each square
float Board::getSpace() {
	return space;
}

//returns the size of the edgePool
int Board::getEdgesNumber() {
	return edgePool.size();
}

//returns the starting x value of the specified edge
float Board::getStartEdgex(int i) {
	return edgePool[i].sx;
}

//returns the starting y value of the specified edge
float Board::getStartEdgey(int i) {
	return edgePool[i].sy;
}

//returns the ending x value of the specified edge
float Board::getEndEdgex(int i) {
	return edgePool[i].ex;
}

//returns the ending y value of the specified edge
float Board::getEndEdgey(int i) {
	return edgePool[i].ey;
}

//returns the size of the ray pool
int Board::getRayNumber() {
	return rayPool.size();
}

//returns the ray x value
float Board::getRayX(int i) {
	return rayPool[i].x;
}

//returns the ray y value
float Board::getRayY(int i) {
	return rayPool[i].y;
}

/*Instead of shooting rays in random directions, I decided to only shot rays at the edges of obstacles, ie,
*only in the directions that will cast shadows. to do this, i created 3 rays, one pointing to each edge, one a little
*to the left, and one a little to the right. then, for each ray i iterate through each edge. if the edge is not in the 
*quadrant the ray is pointing in, the i skip over it. else, i check to see if the ray intersects with the edge
*then I sort all the intersections and select the closest intersection if any were found, and reassing that to be the 
*new end of the ray
*The resulst is rays that extend from the mouse to every edge, and stop at the first obstacle encountered,
*as well as 2 additional rays that extend to either side of the edge, making the rays look shadow like.
*/
void Board::updateRayPool(float mouseX, float mouseY) {
	const float pi = 3.141592654;
	const float difAngle = 0.01;

	struct distance {
		Edge curEdge;
		float distance;//distance to mouse/source of light
		float x = -1.f, y = -1.f;
	};

	//empty ray pool
	rayPool.clear();

	//for every edge, create 3 rays, calculate their slopes and angles
	//Important note: theta is the angle from the edge to the mouse, not the other way around
	for (int i = 0; i < edgePool.size(); i++) {
		rayPool.push_back(Ray());
		rayPool[3 * i].slope = (mouseY - edgePool[i].sy / (width + space)) / (mouseX - edgePool[i].sx / (width + space));
		rayPool[3 * i].theta = atan2((edgePool[i].sy / (width + space) - mouseY), (mouseX - edgePool[i].sx / (width + space)));
		if (rayPool[3 * i].theta < 0) {//note origin is at the edge
			rayPool[3 * i].theta = pi + (pi + rayPool[3 * i].theta);
		}

		//ray less than orig ray
		rayPool.push_back(Ray());
		if (rayPool[3 * i].theta >= difAngle) {
			rayPool[3 * i + 1].theta = rayPool[3 * i].theta - difAngle;
		}
		rayPool[3 * i + 1].slope = -1 * tan(rayPool[3 * i + 1].theta);

		//ray greater than orig ray
		rayPool.push_back(Ray());
		if (rayPool[3 *i].theta <= 360 - difAngle) {
			rayPool[3 * i + 2].theta = rayPool[3 * i].theta + difAngle;
		}
		rayPool[3 * i + 2].slope = -1 * tan(rayPool[3 * i + 2].theta);
	}
	for (int i = 0; i < rayPool.size(); i++) {//for every ray 
		float slope = rayPool[i].slope;
		vector<distance> distPool;
		distPool.clear();
		for (int j = 0; j < edgePool.size(); j++) {//for every edge
			distPool.push_back(distance());//pushback the edges distance from the ray to be calculated later
			distPool[distPool.size() - 1].curEdge = edgePool[j];
			if (rayPool[i].theta < 1.5 * pi + difAngle || rayPool[i].theta > 1.5 * pi + difAngle) {//deal with this
				/*rayPool.erase(rayPool.begin() + i-1);
				rayPool.erase(rayPool.begin() + i-2);
				rayPool.erase(rayPool.begin() + i-3);*/
				rayPool[i].x = edgePool[j].sx;
				rayPool[i].y = edgePool[j].sy;
			}
			//if line seg is to the left and ray is pointing to the right//theta is to the left
			if (edgePool[j].sx / (width + space) < mouseX && edgePool[j].ex / (width + space) < mouseX && (rayPool[i].theta >= (pi / 2) && rayPool[i].theta < (1.5 * pi))) {
				//doesnt intersect
				//cout << "Edge " << j << " was too left" << endl;
				distPool[distPool.size() - 1].distance = 99999;
			}

			//if line seg is to the right and ray(angle) is pointing to the left//theta is to the right
			else if ((mouseX < (edgePool[j].sx / (width + space)) && (mouseX < edgePool[j].ex / (width + space))) && ((rayPool[i].theta < (pi / 2) || rayPool[i].theta >(1.5 * pi)))) {
				//doesnt intersect
				//cout << "Edge " << j << " was too right" << endl;
				distPool[distPool.size() - 1].distance = 99999;
			}

			//if line seg is below and ray is pointing above//thta is at the nottom
			else if ((edgePool[j].sy / (width + space)) > mouseY && ((edgePool[j].ey / (width + space)) > mouseY) && (rayPool[i].theta >= pi || rayPool[i].theta == 0)) {
				//cout << "Edge " << j << " was too low" << endl;
				distPool[distPool.size() - 1].distance = 99999;
			}

			//if line seg is above and ray is pointing below//theta is at the top
			else if (edgePool[j].sy / (width + space) < mouseY && edgePool[j].ey / (width + space) < mouseY && (rayPool[i].theta <= pi )) {
				//doesnt intersect
				//cout << "Edge " << j << " was too high" << endl;
				distPool[distPool.size() - 1].distance = 99999;
			}
			
			//else check to see if it intersects
			else {//y = mx +c
				float sx = edgePool[j].sx / (width + space);
				float ex = edgePool[j].ex / (width + space);
				float sy = edgePool[j].sy / (width + space);
				float ey = edgePool[j].ey / (width + space);

				//calculate the y intercept
				float c = mouseY - slope * mouseX;
				if (sy == ey) {//if y is constant, calculate the x at constant y
					float x = (ey - c)/slope;
					if ((x >= sx && x <= ex) || (x <= sx && x >= ex)) {//if x is in the line seg, it intersects
						distPool[distPool.size() - 1].y = sy;
						distPool[distPool.size() - 1].x = x;
						distPool[distPool.size() - 1].distance = sqrt((mouseX -distPool[distPool.size() - 1].x) * (mouseX - distPool[distPool.size() - 1].x) + (mouseY - distPool[distPool.size() - 1].y) *(mouseY - distPool[distPool.size() - 1].y));
					}
					else {
						distPool[distPool.size() - 1].distance = 99999;
					}
				}
				else {//x is const, calculate the y at constant x
					double y = slope *ex + c;
					if ((y >= sy && y <= ey) || (y <= sy && y >= ey)) {//if y is in range of line seg, it intersects
						distPool[distPool.size() - 1].x = sx;
						distPool[distPool.size() - 1].y = y;
						distPool[distPool.size() - 1].distance = sqrt((mouseX - distPool[distPool.size() - 1].x) * (mouseX - distPool[distPool.size() - 1].x) + (mouseY - distPool[distPool.size() - 1].y) *(mouseY - distPool[distPool.size() - 1].y));
					}
					else {

						distPool[distPool.size() - 1].distance = 99999;
					}

				}
			}
		}
		//for some reason, the first edge refuses to intersect
		//sort the distPool by distance and the shortest dist. x and y is new ray x and y
		sort(distPool.begin(), distPool.end(), [](const distance &left, const distance &right) {return left.distance < right.distance; });
		if (distPool[0].x != -1 && distPool[0].y != -1) {//if the ray hit something
			rayPool[i].x = distPool[0].x * (width + space);
			rayPool[i].y = distPool[0].y * (width + space);
		}
		else {//if the ray didnt hit anything, should not be feasible as long as cursor stays in bounds
			//but in the event that it doesnt, this is to ensure there are no errors
			//rayPool[i].x = 256;
			//rayPool[i].y = 256;
			//rayPool.erase(rayPool.begin() + i);
		}
	}
	//sort rays by their angle to aid light representation in the main function
	sort(rayPool.begin(), rayPool.end(), [](const Ray &left, const Ray &right) {return left.theta < right.theta; });
}

//returns the specified cells on bool
bool Board::isOn(int x, int y) {
	return cellGrid[x][y].on;
}

/*iterates through the grid structure, checking if cells are on. If they are on and have neighbouring cells,
*the do not give them edges on that neighbour to prevent overlap of an edge and a cell
*however, if they are not on, check if they have a neighbouring cell who has a similar edge and whose edge
*can be extended to cover the cell in question as well
*The result of this is edges bordering every cell, drastically reducing the number of edges
*that would have been used for calculation if instead I had tested every edge for every cell
*/
void Board::updateEdgePool() {
	//tighten this up l8r
	edgePool.clear();
	for (int i = 0; i < yQuads; i++) {
		for (int j = 0; j < xQuads; j++) {
			//resets all the cell values
			cellGrid[j][i].edge[NORTH] = false;
			cellGrid[j][i].edgeID[NORTH] = -1;
			cellGrid[j][i].edge[SOUTH] = false;
			cellGrid[j][i].edgeID[SOUTH] = -1;
			cellGrid[j][i].edge[WEST] = false;
			cellGrid[j][i].edgeID[WEST] = -1;
			cellGrid[j][i].edge[EAST] = false;
			cellGrid[j][i].edgeID[EAST] = -1;
			if (isOn(j, i)) {
				//if it has a neighbour on a side, doesnt need an edge on that side;
				//northern neighbour
				if (!cellGrid[j][i - 1].on || i - 1 < 0){
					//if the northern neighbour is not on, check if the western neighbour has a northern edge
					//if it does, extend it
					if (cellGrid[j - 1][i].edge[NORTH] && j - 1 >= 0){
						cellGrid[j][i].edge[NORTH] = true;
						cellGrid[j][i].edgeID[NORTH] = cellGrid[j - 1][i].edgeID[NORTH];
						edgePool[cellGrid[j][i].edgeID[NORTH]].ex = j * (width + space) + width;
					}
					else {//else if it doesnt have a western neighbour with a northern edge,
						//create a new edge and add it to the edge pool
						cellGrid[j][i].edge[NORTH] = true;
						edgePool.push_back(Edge());
						edgePool[edgePool.size() - 1].sx = j * (width + space);
						edgePool[edgePool.size() - 1].sy = i * (width + space);
						edgePool[edgePool.size() - 1].ex = j * (width + space) + width;
						edgePool[edgePool.size() - 1].ey = i * (width + space);
						cellGrid[j][i].edgeID[NORTH] = edgePool.size() - 1;
					}
				}
				//southern neighbour
				if (!cellGrid[j][i + 1].on) {//if the southern neighbour is not on, check 
					//if the western neighbour has a southern edge
					if (cellGrid[j - 1][i].edge[SOUTH] && j-1 >= 0) {
						cellGrid[j][i].edge[SOUTH] = true;
						cellGrid[j][i].edgeID[SOUTH] = cellGrid[j - 1][i].edgeID[SOUTH];
						edgePool[cellGrid[j][i].edgeID[SOUTH]].sx = j * (width + space) + width;
					}
					else {//else if it doesnt have a western neighbour with a southern edge,
						//create a new edge and add it to the edge pool
						cellGrid[j][i].edge[SOUTH] = true;
						edgePool.push_back(Edge());
						edgePool[edgePool.size() - 1].sx = j * (width + space) + width;
						edgePool[edgePool.size() - 1].sy = i * (width + space) + width;
						edgePool[edgePool.size() - 1].ex = j * (width + space);
						edgePool[edgePool.size() - 1].ey = i * (width + space) + width;
						cellGrid[j][i].edgeID[SOUTH] = edgePool.size() - 1;
					}
				}

				//eastern neighbour
				if (!cellGrid[j + 1][i].on && j + 1 < xQuads) {//if the eastern neighbour is not on, check 
					//if the Northern neighbour has an eastern edge
					if (cellGrid[j][i - 1].edge[EAST]) {
						cellGrid[j][i].edge[EAST] = true;
						cellGrid[j][i].edgeID[EAST] = cellGrid[j][i - 1].edgeID[EAST];
						edgePool[cellGrid[j][i].edgeID[EAST]].ey = i * (width + space) + width;
					}
					else {//else if it doesnt have a NORTHERN neighbour with an eastern edge,
						//create a new edge and add it to the edge pool
						cellGrid[j][i].edge[EAST] = true;
						edgePool.push_back(Edge());
						edgePool[edgePool.size() - 1].sx = j * (width + space) + width;
						edgePool[edgePool.size() - 1].sy = i * (width + space);
						edgePool[edgePool.size() - 1].ex = j * (width + space) + width;
						edgePool[edgePool.size() - 1].ey = i * (width + space) + width;
						cellGrid[j][i].edgeID[EAST] = edgePool.size() - 1;
					}
				}

				//Western neighbour
				if (!cellGrid[j - 1][i].on || j-1 < 0) {//if the western neighbour is not on, check 
					//if the Northern neighbour has an western edge
					if (cellGrid[j][i - 1].edge[WEST]) {
						cellGrid[j][i].edge[WEST] = true;
						cellGrid[j][i].edgeID[WEST] = cellGrid[j][i - 1].edgeID[WEST];
						edgePool[cellGrid[j][i].edgeID[WEST]].sy = i * (width + space) + width;
					}
					else {//else if it doesnt have a northern neighbour with a western edge,
						//create a new edge and add it to the edge pool
						cellGrid[j][i].edge[WEST] = true;
						edgePool.push_back(Edge());
						edgePool[edgePool.size() - 1].sx = j * (width + space);
						edgePool[edgePool.size() - 1].sy = i * (width + space) + width;
						edgePool[edgePool.size() - 1].ex = j * (width + space);
						edgePool[edgePool.size() - 1].ey = i * (width + space);
						cellGrid[j][i].edgeID[WEST] = edgePool.size() - 1;
					}
				}

			}
		}
	}
}

Board::~Board()
{

}