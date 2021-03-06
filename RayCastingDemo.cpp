#include <iostream>
#include <SFML/Graphics.hpp>
#include "Board.h"

int main()
{
	cout << "Press Space to turn on the light, left click to place ocstacles and G to turn it off" << endl;
	Board gameBoard;

	int xQuads = gameBoard.getXQuads();
	int yQuads = gameBoard.getYQuads();
	float width = gameBoard.getWidth();
	float space = gameBoard.getSpace();

	sf::RenderWindow window(sf::VideoMode(xQuads * (width + space), yQuads * (width + space)), "Raytracing");

	sf::VertexArray Grid(sf::Quads, xQuads * yQuads * 4);
	sf::VertexArray Edges(sf::Lines, 2);
	sf::VertexArray Rays(sf::Lines, 2);
	sf::VertexArray Light(sf::Triangles, 3);
	bool LightOn = false;
	bool toggleRays = false;

	while (window.isOpen()) {
		sf::Event evnt;
		float mouseX = sf::Mouse::getPosition(window).x / (width + space);
		float mouseY = sf::Mouse::getPosition(window).y / (width + space);
		while (window.pollEvent(evnt)) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && mouseX < xQuads && mouseX >= 0 && mouseY < yQuads && mouseY >= 0 && !gameBoard.isOn(mouseX, mouseY)) {
				gameBoard.turnOn(mouseX, mouseY);
				gameBoard.updateEdgePool();
				//resize edge pool
				Edges.resize(gameBoard.getEdgesNumber() * 2);

				//Set Edge vertices, to draw edges, uncomment this code
				for (int i = 0; i < gameBoard.getEdgesNumber(); i++) {
					Edges[2 * i].position = sf::Vector2f(gameBoard.getStartEdgex(i), gameBoard.getStartEdgey(i));
					Edges[2 * i + 1].position = sf::Vector2f(gameBoard.getEndEdgex(i), gameBoard.getEndEdgey(i));
					Edges[2 * i].color = sf::Color::Red;
					Edges[2 * i + 1].color = sf::Color::Red;
				}

			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && mouseX < xQuads && mouseX >= 0 && mouseY < yQuads && mouseY >= 0 && gameBoard.isOn(mouseX, mouseY)) {
				gameBoard.turnOff(mouseX, mouseY);
				gameBoard.updateEdgePool();
				//resize edge pool
				Edges.resize(gameBoard.getEdgesNumber() * 2);

				//Set Edge vertices
				for (int i = 0; i < gameBoard.getEdgesNumber(); i++) {
					Edges[2 * i].position = sf::Vector2f(gameBoard.getStartEdgex(i), gameBoard.getStartEdgey(i));
					Edges[2 * i + 1].position = sf::Vector2f(gameBoard.getEndEdgex(i), gameBoard.getEndEdgey(i));
				}
			}
		}


		//resize Light
		Light.resize(gameBoard.getRayNumber() * 3);
		//resize ray pool
		Rays.resize(gameBoard.getRayNumber() * 2);
		//set Ray and light verices
		//Light drawing needs work
		for (int i = 0; i < gameBoard.getRayNumber() - 1; i++) {
			//To draw rays, uncomment this and the code below
			Rays[2 * i].position = sf::Vector2f(gameBoard.getRayX(i), gameBoard.getRayY(i));
			Rays[2 * i + 1].position = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
			Light[3 * i].position = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);				Light[3 * i + 1].position = sf::Vector2f(gameBoard.getRayX(i), gameBoard.getRayY(i));
			Light[3 * i + 2].position = sf::Vector2f(gameBoard.getRayX(i + 1), gameBoard.getRayY(i + 1));
		}
		if (gameBoard.getRayNumber() > 0) {
			int rayNum = gameBoard.getRayNumber();
			Rays[2 * rayNum-2].position = sf::Vector2f(gameBoard.getRayX(rayNum - 1), gameBoard.getRayY(rayNum - 1));
			Rays[2 * rayNum-1].position = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
			Light[3 * rayNum - 3].position = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
			Light[3 * rayNum - 2].position = sf::Vector2f(gameBoard.getRayX(rayNum - 1), gameBoard.getRayY(rayNum -1));
			Light[3 * rayNum - 1].position = sf::Vector2f(gameBoard.getRayX(0), gameBoard.getRayY(0));
		}
		//Set Grid vertices
		for (int i = 0; i < yQuads; i++) {
			for (int j = 0; j < xQuads; j++) {
				Grid[4 * (i*xQuads + j)].position = sf::Vector2f(j * (width + space), i * (width + space));
				Grid[4 * (i*xQuads + j) + 1].position = sf::Vector2f(j * (width + space) + width, i * (width + space));
				Grid[4 * (i*xQuads + j) + 2].position = sf::Vector2f(j * (width + space) + width, i * (width + space) + width);
				Grid[4 * (i*xQuads + j) + 3].position = sf::Vector2f(j * (width + space), i * (width + space) + width);

				sf::Color cellColor = sf::Color::Black;
				Grid[4 * (i*xQuads + j)].color = cellColor;
				Grid[4 * (i*xQuads + j) + 1].color = cellColor;
				Grid[4 * (i*xQuads + j) + 2].color = cellColor;
				Grid[4 * (i*xQuads + j) + 3].color = cellColor;

				if (gameBoard.isOn(j, i)) {
					cellColor = sf::Color::Blue;
					Grid[4 * (i*xQuads + j)].color = cellColor;
					Grid[4 * (i*xQuads + j) + 1].color = cellColor;
					Grid[4 * (i*xQuads + j) + 2].color = cellColor;
					Grid[4 * (i*xQuads + j) + 3].color = cellColor;
				}
			}
		}

		window.clear();
		window.draw(Grid);
		//window.draw(Edges);// No need to draw edges or rays
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			LightOn = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
			LightOn = false;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
			toggleRays = !toggleRays;
		}

		if (LightOn) {
			if  (mouseX >= 0 && mouseX < xQuads && mouseY >= 0 && mouseY < yQuads && !gameBoard.isOn(mouseX, mouseY)) {
				gameBoard.updateRayPool(mouseX, mouseY);
				if(toggleRays)
					window.draw(Rays);
				else
					window.draw(Light);
			}
		}
		window.display();
	}
	return 0;
}
