#include <cmath> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "Random.h"
#include "Board.h"
#include "TextureManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
using namespace std;

int main()
{
	/* ==== READ FILE =====*/
	string fileName = "boards/config.cfg";
	int width = 0;
	int height = 0;
	int mineCount = 0;
	int tileCount = 0;

	//Gather the width, height, and number of mines in the file
	ifstream inFile(fileName);
	if (inFile.is_open()) {
		string lineFromFile;
		getline(inFile, lineFromFile);
		width = stoi(lineFromFile);
		getline(inFile, lineFromFile);
		height = stoi(lineFromFile);
		getline(inFile, lineFromFile);
		mineCount = stoi(lineFromFile);
		tileCount = width * height;
	}
	 
	//Create the window and board
	sf::RenderWindow window(sf::VideoMode(32*width, 100+32*height), "Minesweeper");
	Board* board = new Board(width, height, mineCount);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {	//If user EXITS
				window.close();
			}
			if (event.type == sf::Event::MouseButtonPressed) { 
				if (event.mouseButton.button == sf::Mouse::Left) {  //If user LEFT clicks
					//Get mouse position
					auto mousePosition = sf::Mouse::getPosition(window);
					
					//If mouse is on a TILE and the game is NOT LOST
					if (!board->gameLost && 0 <= mousePosition.x && mousePosition.x <= 32 * width && 0 <= mousePosition.y && mousePosition.y <= 32 * height) {
						board->makeTileMove_LeftClick(mousePosition.x, mousePosition.y);
					}

					//If  mouse is in SMILEY FACE
					if ((16*(width-2) <= mousePosition.x) && (mousePosition.x <= 16 * width + 32) && (32 * height <= mousePosition.y) && (mousePosition.y <= 32 * (height + 2))) {
						board->GameReset();
					}

					//If mouse is on DEBUG button
					if (((16 * (width - 2) + 64 * 2) <= mousePosition.x) && (mousePosition.x <= (16 * (width - 2) + 64 * 3)) && (32 * height <= mousePosition.y) && (mousePosition.y <= 32 * (height + 2))) {
						if (!board->debugMode) {
							board->debugMode = true;
						}
						else {
							board->debugMode = false;
						}
					}

					//If mouse is on TEST_1/2/3 BUTTONS
					if (((16 * (width - 2) + 64 * 3) <= mousePosition.x) && (mousePosition.x < (16 * (width - 2) + 64 * 4)) && (32 * height <= mousePosition.y) && (mousePosition.y <= 32 * (height + 2))) {
						board->LoadTestBoard("boards/testboard1.brd");
					}
					if (((16 * (width - 2) + 64 * 4) <= mousePosition.x) && (mousePosition.x < (16 * (width - 2) + 64 * 5)) && (32 * height <= mousePosition.y) && (mousePosition.y <= 32 * (height + 2))) {
						board->LoadTestBoard("boards/testboard2.brd");
					}
					if (((16 * (width - 2) + 64 * 5) <= mousePosition.x) && (mousePosition.x <= (16 * (width - 2) + 64 * 6)) && (32 * height <= mousePosition.y) && (mousePosition.y <= 32 * (height + 2))) {
						board->LoadTestBoard("boards/testboard3.brd");
					}
				}

				if (event.mouseButton.button == sf::Mouse::Right) {	//If user RIGHT clicks
					//Get mouse position
					auto mousePosition = sf::Mouse::getPosition(window);
					cout << mousePosition.x << "  " << mousePosition.y << endl;

					//If mouse is on a TILE and game is NOT LOST
					if (!board->gameLost && 0 <= mousePosition.x && mousePosition.x <= 32 * width && 0 <= mousePosition.y && mousePosition.y <= 32 * height) {
						board->makeTileMove_RightClick(mousePosition.x, mousePosition.y);
					}
				}
			}
		}
		//Sets the board's background to white
		window.clear(sf::Color(255,255,255));
		//Checks if the game is lost
		board->DefeatBoard();
		//Checks if the game is won
		board->VictoryReveal();
		//Calculates the number of adjacent mines at each tile
		board->CalculateAdjacentMines();

		/* ==== DRAW BOARD TILES ==== */
		for (unsigned int i = 0; i < height; i++) {
			for (unsigned int j = 0; j < width; j++) {
				//If tile is a flag and not revealed
				if (board->getTileFlag(i, j) && !board->getTileRevealed(i, j)) {
					sf::Sprite tempTile_Hidden(TextureManager::GetTexture("tile_hidden"));
					tempTile_Hidden.setPosition(32 * j, 32 * i);
					window.draw(tempTile_Hidden);

					sf::Sprite tempFlag(TextureManager::GetTexture("flag"));
					tempFlag.setPosition(32 * j, 32 * i);
					window.draw(tempFlag);
				}
				//If tile is not revealed
				else if (!board->getTileRevealed(i, j)) {
					sf::Sprite tempTile_Hidden(TextureManager::GetTexture("tile_hidden"));
					tempTile_Hidden.setPosition(32 * j, 32 * i);
					window.draw(tempTile_Hidden);
				}
				//If tile is revealed and not a mine
				else if (board->getTileRevealed(i, j) && !board->getTileMine(i,j)) {
					sf::Sprite tempTile_Revealed(TextureManager::GetTexture("tile_revealed"));
					tempTile_Revealed.setPosition(32 * j, 32 * i);
					window.draw(tempTile_Revealed);
				}
				//If tile is mine and revealed
				else if (board->getTileMine(i, j) && board->getTileRevealed(i, j)) {
					sf::Sprite tempTile_Revealed(TextureManager::GetTexture("tile_revealed"));
					tempTile_Revealed.setPosition(32 * j, 32 * i);
					window.draw(tempTile_Revealed);

					sf::Sprite tempMine(TextureManager::GetTexture("mine"));
					tempMine.setPosition(32 * j, 32 * i);
					window.draw(tempMine);
				}
				//If DEBUG MODE and tile is NOT REVEALED and MINE
				if (board->debugMode && board->getTileMine(i, j) && !board->getTileRevealed(i, j)) {
					sf::Sprite tempTile_Hidden(TextureManager::GetTexture("tile_hidden"));
					tempTile_Hidden.setPosition(32 * j, 32 * i);
					window.draw(tempTile_Hidden);

					if (board->getTileFlag(i, j)) {
						sf::Sprite tempFlag(TextureManager::GetTexture("flag"));
						tempFlag.setPosition(32 * j, 32 * i);
						window.draw(tempFlag);
					}

					sf::Sprite tempMine(TextureManager::GetTexture("mine"));
					tempMine.setPosition(32 * j, 32 * i);
					window.draw(tempMine);
				}
				//If tile is REVEALED, NOT a MINE, NOT a FLAG, and ADJACENT MINE COUNT > 0
				if (board->getTileRevealed(i, j) && !board->getTileMine(i, j) && !board->getTileFlag(i, j) && board->getAdjacentMineCount(i, j) > 0) {
					int adjacentMines = board->getAdjacentMineCount(i, j);
					sf::Sprite tempNumMines(TextureManager::GetTexture("number_" + to_string(adjacentMines)));
					tempNumMines.setPosition(32 * j, 32 * i);
					window.draw(tempNumMines);
				}
			}
		}
	
		/* ==== DRAW MINE COUNTER ==== */
		{
			int counterNumber = board->getMineCounter();
			int first, second, third;
			first = abs(counterNumber) / 100;
			second = (abs(counterNumber) % 100) / 10;
			third = abs(counterNumber) % 10;

			sf::Sprite firstDigit(TextureManager::GetTexture("digits", first));
			firstDigit.setPosition(21, 32 * height);
			window.draw(firstDigit);

			sf::Sprite secondDigit(TextureManager::GetTexture("digits", second));
			secondDigit.setPosition(42, 32 * height);
			window.draw(secondDigit);

			sf::Sprite thirdDigit(TextureManager::GetTexture("digits", third));
			thirdDigit.setPosition(63, 32 * height);
			window.draw(thirdDigit);

			if (counterNumber < 0) {
				sf::Sprite negativeDigit(TextureManager::GetTexture("digits", 10));
				negativeDigit.setPosition(0, 32 * height);
				window.draw(negativeDigit);
			}
		}

		/* ==== DRAW SMILEY FACE ===== */
		if (board->gameWon) {
			sf::Sprite face_win(TextureManager::GetTexture("face_win"));
			face_win.setPosition((16 * (width - 2)), 32 * height);
			window.draw(face_win);
		}
		else if (board->gameLost) {
			sf::Sprite face_lose(TextureManager::GetTexture("face_lose"));
			face_lose.setPosition((16 * (width - 2)), 32 * height);
			window.draw(face_lose);
		}
		else {
			sf::Sprite face_happy(TextureManager::GetTexture("face_happy"));
			face_happy.setPosition((16 * (width - 2)), 32 * height);
			window.draw(face_happy);
		}

		/* ==== DRAW DEBUG AND TEST BUTTONS ==== */ 
		{
			sf::Sprite debug(TextureManager::GetTexture("debug"));
			debug.setPosition((16 * (width - 2) + 64 * 2), 32 * height);
			window.draw(debug);

			sf::Sprite test_1(TextureManager::GetTexture("test_1"));
			test_1.setPosition((16 * (width - 2) + 64 * 3), 32 * height);
			window.draw(test_1);

			sf::Sprite test_2(TextureManager::GetTexture("test_2"));
			test_2.setPosition((16 * (width - 2) + 64 * 4), 32 * height);
			window.draw(test_2);

			sf::Sprite test_3(TextureManager::GetTexture("test_3"));
			test_3.setPosition((16 * (width - 2) + 64 * 5), 32 * height);
			window.draw(test_3);
		}

		//Display the window
		window.display();
		}
		return 0;
	}