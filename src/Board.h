#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "Random.h"
#include "TextureManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
using namespace std;
class Board
{
public:
	struct Tile {
		bool mine = false;				//True if tile is a mine
		bool flag = false;				//True if tile has a flag
		bool revealed = false;		    //True if tile is revealed
		vector<bool> adjacentTiles;     //True for every adjacent tile with a mine
		Tile() {
			mine = false;
			flag = false;
			revealed = false;
		}
	};

	int width,height,numMines,storageNumMines;   //Stores data about the board
	int flagCounter = 0;						 
	bool gameWon = false;						 //True if game is won
	bool gameLost = false;						 //True if game is lost
	bool debugMode = false;						 //True if game is in debug mode
	vector<vector<Tile*>> boardTiles;			 //Vector of tile pointers (the board)

	//Default constructor
	Board(int width, int height, int mineCount) {
		this->width = width;
		this->height = height;
		this->numMines = mineCount;
		this->storageNumMines = mineCount;
		boardTiles.resize(height, vector<Tile*>(width));
		setBoard(height, width);
		MineRandomizer();
	}
	
	//Used to reset and initialize the board
	vector<vector<Tile*>> setBoard(int height, int width) {
		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width; c++) {
				boardTiles[r][c] = new Tile();
			}
		}
		return boardTiles;
	}
	void MineRandomizer() { //randomly places mines
		//Make sure all tiles are initialized
		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width; c++) {
				if (boardTiles[r][c]->mine) {
					boardTiles[r][c]->mine = false;
				}
				if (boardTiles[r][c]->revealed) {
					boardTiles[r][c]->revealed = false;
				}
				if (boardTiles[r][c]->flag) {
					boardTiles[r][c]->flag = false;
				}
			}
		}
		
		int tempMineCount = storageNumMines;
		while (tempMineCount > 0) {
			int tempIndex = Random::Int(0, height * width - 1);
			int tempIndex_X = tempIndex / width;
			int tempIndex_Y = tempIndex % width;
			if (!boardTiles[tempIndex_X][tempIndex_Y]->mine) { //if its not a mine
				boardTiles[tempIndex_X][tempIndex_Y]->mine = true;
				--tempMineCount;
			}
		}
	}
	void GameReset() {
		MineRandomizer();
		gameWon = false;
		gameLost = false;
		flagCounter = 0;
		numMines = storageNumMines;
		debugMode = false;
	}

	//Loads one of the three test boards given the parameter
	void LoadTestBoard(string filename) {
		flagCounter = 0;
		numMines = 0;
		gameLost = false;
		gameWon = false;
		debugMode = false;
		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width; c++) {
				if (boardTiles[r][c]->mine) {
					boardTiles[r][c]->mine = false;
				}
				if (boardTiles[r][c]->revealed) {
					boardTiles[r][c]->revealed = false;
				}
				if (boardTiles[r][c]->flag) {
					boardTiles[r][c]->flag = false;
				}
			}
		}

		ifstream inFile(filename);
		int row = 0;
		if (inFile.is_open()) {
			while (!inFile.eof()) {
				string lineFromFile;
				getline(inFile, lineFromFile);

				for (int i = 0; i < width; i++) {
					if (lineFromFile[i] == '1') {
						++numMines;
						boardTiles[row][i]->mine = true;
					}
				}
				row++;
			}
		}
	}

	//Getter functions
	int getMineCounter() { return numMines - flagCounter; }
	int getAdjacentMineCount(int r, int c) {
		int adjacentMineCount = boardTiles[r][c]->adjacentTiles.size();
		return adjacentMineCount;
	}
	bool getTileFlag(int r, int c) { //If tile is a flag
		if (boardTiles[r][c]->flag == false) {
			return false;
		}
		else {
			return true;
		}
	}
	bool getTileRevealed(int r, int c) { //If tile is revealed
		if (boardTiles[r][c]->revealed == false) {
			return false;
		}
		else {
			return true;
		}
	}
	bool getTileMine(int r, int c) { //If tile is a mine
		if (boardTiles[r][c]->mine == false) {
			return false;
		}
		else {
			return true;
		}
	}

	//If user left clicks a tile
	void makeTileMove_LeftClick(int xPos, int yPos) {
		int rowIndex = yPos / 32;
		int colIndex = xPos / 32;
		//If Tile is NOT REVEALED and NOT a FLAG
		//update: it can be a mine -- FIX THIS
		if ((!boardTiles[rowIndex][colIndex]->revealed) && (!boardTiles[rowIndex][colIndex]->flag)){
			//Reveal it
			boardTiles[rowIndex][colIndex]->revealed = true;
			if (boardTiles[rowIndex][colIndex]->mine) {
				gameLost = true;

			}
			/* ==== TO FIX: ADD THE ALGORITHM TO REVEAL ALL ADJACENT SPACES*/
			else if (getAdjacentMineCount(rowIndex, colIndex) == 0) {
				TileRevealing(rowIndex, colIndex);
			}
		}
	}
	void TileRevealing(int r, int c) {
		boardTiles[r][c]->revealed = true;
		if (getAdjacentMineCount(r, c) == 0 && !boardTiles[r][c]->mine) {
			//Top border
			if (r == 0 && c > 0 && c < width - 1) {
				if (!boardTiles[0][c - 1]->flag && !boardTiles[0][c - 1]->revealed && !boardTiles[0][c - 1]->mine) {
					boardTiles[0][c - 1]->revealed == true;
					TileRevealing(0, c - 1);
				}
				if (!boardTiles[0][c + 1]->flag && !boardTiles[0][c + 1]->revealed && !boardTiles[0][c + 1]->mine) {
					boardTiles[0][c + 1]->revealed == true;
					TileRevealing(0, c + 1);
				}
				if (!boardTiles[1][c - 1]->flag && !boardTiles[1][c - 1]->revealed && !boardTiles[1][c - 1]->mine) {
					boardTiles[1][c - 1]->revealed == true;
					TileRevealing(1, c - 1);
				}
				if (!boardTiles[1][c]->flag && !boardTiles[1][c]->revealed && !boardTiles[1][c]->mine) {
					boardTiles[1][c ]->revealed == true;
					TileRevealing(1, c);
				}
				if (!boardTiles[1][c + 1]->flag && !boardTiles[1][c + 1]->revealed && !boardTiles[1][c + 1]->mine) {
					boardTiles[1][c + 1]->revealed == true;
					TileRevealing(1, c + 1);
				}
			}
			//Bottom border
			else if (r == height - 1 && c > 0 && c < width - 1) {
				if (!boardTiles[height - 1][c - 1]->flag && !boardTiles[height - 1][c - 1]->revealed && !boardTiles[height - 1][c - 1]->mine) {
					boardTiles[height - 1][c - 1]->revealed == true;
					TileRevealing(height - 1, c - 1);
				}
				if (!boardTiles[height - 1][c + 1]->flag && !boardTiles[height - 1][c + 1]->revealed && !boardTiles[height - 1][c + 1]->mine) {
					boardTiles[height - 1][c + 1]->revealed == true;
					TileRevealing(height - 1, c + 1);
				}
				if (!boardTiles[height - 2][c - 1]->flag && !boardTiles[height - 2][c - 1]->revealed && !boardTiles[height - 2][c - 1]->mine) {
					boardTiles[height - 2][c - 1]->revealed == true;
					TileRevealing(height - 2, c - 1);
				}
				if (!boardTiles[height - 2][c]->flag && !boardTiles[height - 2][c]->revealed && !boardTiles[height - 2][c]->mine) {
					boardTiles[height - 2][c]->revealed == true;
					TileRevealing(height - 2, c);
				}
				if (!boardTiles[height - 2][c + 1]->flag && !boardTiles[height - 2][c + 1]->revealed && !boardTiles[height - 2][c + 1]->mine) {
					boardTiles[height - 2][c + 1]->revealed == true;
					TileRevealing(height - 2, c + 1);
				}
			}
			//Left border
			else if (c == 0 && r > 0 && r < height - 1) {
				if (!boardTiles[r - 1][0]->flag && !boardTiles[r - 1][0]->revealed && !boardTiles[r - 1][0]->mine) {
					boardTiles[r - 1][0]->revealed == true;
					TileRevealing(r - 1, 0);
				}
				if (!boardTiles[r + 1][0]->flag && !boardTiles[r + 1][0]->revealed && !boardTiles[r + 1][0]->mine) {
					boardTiles[r + 1][0]->revealed == true;
					TileRevealing(r + 1, 0);
				}
				if (!boardTiles[r - 1][1]->flag && !boardTiles[r - 1][1]->revealed && !boardTiles[r - 1][1]->mine) {
					boardTiles[r - 1][1]->revealed == true;
					TileRevealing(r- 1, 1);
				}
				if (!boardTiles[r][1]->flag && !boardTiles[r][1]->revealed && !boardTiles[r][1]->mine) {
					boardTiles[r][1]->revealed == true;
					TileRevealing(r, 1);
				}
				if (!boardTiles[r + 1][1]->flag && !boardTiles[r + 1][1]->revealed && !boardTiles[r + 1][1]->mine) {
					boardTiles[r + 1][1]->revealed == true;
					TileRevealing(r + 1, 1);
				}
			}
			//Right border
			else if (c == width - 1 && r > 0 && r < height - 1) {
				if (!boardTiles[r - 1][width - 1]->flag && !boardTiles[r - 1][width - 1]->revealed && !boardTiles[r - 1][width - 1]->mine) {
					boardTiles[r - 1][width - 1]->revealed == true;
					TileRevealing(r - 1, width - 1);
				}
				if (!boardTiles[r + 1][width - 1]->flag && !boardTiles[r + 1][width - 1]->revealed && !boardTiles[r + 1][width - 1]->mine) {
					boardTiles[r + 1][width - 1]->revealed == true;
					TileRevealing(r + 1, width - 1);
				}
				if (!boardTiles[r - 1][width - 2]->flag && !boardTiles[r - 1][width - 2]->revealed && !boardTiles[r - 1][width - 2]->mine) {
					boardTiles[r - 1][width - 2]->revealed == true;
					TileRevealing(r - 1, width - 2);
				}
				if (!boardTiles[r][width - 2]->flag && !boardTiles[r][width - 2]->revealed && !boardTiles[r][width - 2]->mine) {
					boardTiles[r][width - 2]->revealed == true;
					TileRevealing(r, width - 2);
				}
				if (!boardTiles[r + 1][width - 2]->flag && !boardTiles[r + 1][width - 2]->revealed && !boardTiles[r + 1][width - 2]->mine) {
					boardTiles[r + 1][width - 2]->revealed == true;
					TileRevealing(r + 1, width - 2);
				}
			}
			//Corners
			else if ((r == 0 && c == 0) || (r == 0 && c == width -1) || (r == height - 1 && c == 0) || (r == height - 1 && c == width - 1)){
				//Top left corner
				if (r == 0 && c == 0) {
					if (!boardTiles[r][c+1]->flag && !boardTiles[r][c+1]->revealed && !boardTiles[r][c + 1]->mine) {
						boardTiles[r][c + 1]->revealed == true;
						TileRevealing(r, c+1);
					}
					if (!boardTiles[r+1][c]->flag && !boardTiles[r + 1][c]->revealed && !boardTiles[r + 1][c]->mine) {
						boardTiles[r+1][c]->revealed == true;
						TileRevealing(r+1, c);
					}
					if (!boardTiles[r+1][c+1]->flag && !boardTiles[r + 1][c+1]->revealed && !boardTiles[r + 1][c + 1]->mine) {
						boardTiles[r+1][c + 1]->revealed == true;
						TileRevealing(r+1, c+1);
					}
				}
				//Top right corner
				if (r == 0 && c == width - 1) {
					if (!boardTiles[r][c - 1]->flag && !boardTiles[r][c - 1]->revealed && !boardTiles[r][c - 1]->mine) {
						boardTiles[r][c - 1]->revealed == true;
						if (getAdjacentMineCount(r, c - 1) == 0) {
							TileRevealing(r, c - 1);
						}
					}
					if (!boardTiles[r + 1][c]->flag && !boardTiles[r + 1][c]->revealed && !boardTiles[r + 1][c]->mine) {
						boardTiles[r +1][c]->revealed == true;
						if (getAdjacentMineCount(r + 1, c) == 0) {
							TileRevealing(r + 1, c);
						}
					}
					if (!boardTiles[r + 1][c - 1]->flag && !boardTiles[r + 1][c - 1]->revealed && !boardTiles[r + 1][c - 1]->mine) {
						boardTiles[r+1][c - 1]->revealed == true;
						if (getAdjacentMineCount(r + 1, c - 1) == 0) {
							TileRevealing(r + 1, c - 1);
						}
					}
				}
				//Bottom left corner
				if (r == height - 1 && c == 0) {
					if (!boardTiles[r][c + 1]->flag && !boardTiles[r][c + 1]->revealed && !boardTiles[r][c + 1]->mine) {
						boardTiles[r][c + 1]->revealed == true;
						TileRevealing(r, c + 1);
					}
					if (!boardTiles[r - 1][c]->flag && !boardTiles[r - 1][c]->revealed && !boardTiles[r - 1][c]->mine) {
						boardTiles[r-1][c]->revealed == true;
						TileRevealing(r - 1, c);
					}
					if (!boardTiles[r - 1][c + 1]->flag && !boardTiles[r - 1][c + 1]->revealed && !boardTiles[r - 1][c + 1]->mine) {
						boardTiles[r-1][c +1]->revealed == true;
						TileRevealing(r - 1, c + 1);
					}
				}
				//Bottom right corner
				if (r == height - 1 && c == width - 1) {
					if (!boardTiles[r][c - 1]->flag && !boardTiles[r][c - 1]->revealed && !boardTiles[r][c - 1]->mine) {
						boardTiles[r][c - 1]->revealed == true;
						TileRevealing(r, c - 1);
					}
					if (!boardTiles[r - 1][c]->flag && !boardTiles[r - 1][c]->revealed && !boardTiles[r - 1][c]->mine) {
						boardTiles[r-1][c]->revealed == true;
						TileRevealing(r - 1, c);
					}
					if (!boardTiles[r - 1][c - 1]->flag && !boardTiles[r - 1][c - 1]->revealed && !boardTiles[r - 1][c - 1]->mine) {
						boardTiles[r-1][c - 1]->revealed == true;
						TileRevealing(r - 1, c - 1);
					}
				}
			}
			//Normal tiles
			else {
		//		for (int r = 1; r < height - 1; r++) {
		//			for (int c = 1; c < width - 1; c++) {
						if (!boardTiles[r][c + 1]->flag && !boardTiles[r][c + 1]->revealed && !boardTiles[r][c + 1]->mine) {
							boardTiles[r][c+ 1]->revealed == true;
							TileRevealing(r, c + 1);
						}
						if (!boardTiles[r + 1][c]->flag && !boardTiles[r + 1][c]->revealed && !boardTiles[r + 1][c]->mine) {
							boardTiles[r+1][c]->revealed == true;
							TileRevealing(r + 1, c);
						}
						if (!boardTiles[r + 1][c + 1]->flag && !boardTiles[r + 1][c + 1]->revealed && !boardTiles[r + 1][c + 1]->mine) {
							boardTiles[r+1][c + 1]->revealed == true;
							TileRevealing(r + 1, c + 1);
						}
						if (!boardTiles[r][c - 1]->flag && !boardTiles[r][c - 1]->revealed && !boardTiles[r][c - 1]->mine) {
							boardTiles[r][c - 1]->revealed == true;
							TileRevealing(r, c - 1);
						}
						if (!boardTiles[r - 1][c]->flag && !boardTiles[r - 1][c]->revealed && !boardTiles[r - 1][c]->mine) {
							boardTiles[r-1][c]->revealed == true;
							TileRevealing(r - 1, c);
						}
						if (!boardTiles[r - 1][c - 1]->flag && !boardTiles[r - 1][c - 1]->revealed && !boardTiles[r - 1][c - 1]->mine) {
							boardTiles[r-1][c - 1]->revealed == true;
							TileRevealing(r - 1, c - 1);
						}
						if (!boardTiles[r - 1][c + 1]->flag && !boardTiles[r - 1][c + 1]->revealed && !boardTiles[r - 1][c + 1]->mine) {
							boardTiles[r-1][c + 1]->revealed == true;
							TileRevealing(r - 1, c + 1);
						}
						if (!boardTiles[r + 1][c - 1]->flag && !boardTiles[r + 1][c - 1]->revealed && !boardTiles[r + 1][c - 1]->mine) {
							boardTiles[r+1][c - 1]->revealed == true;
							TileRevealing(r + 1, c - 1);
			//			}
			//		}
				}
			}

		}
	}

	//If user right clicks a tile
	void makeTileMove_RightClick(int xPos, int yPos) {
		int rowIndex = yPos / 32;
		int colIndex = xPos / 32;

		//If Tile is NOT REVEALED or a FLAG
		if (!boardTiles[rowIndex][colIndex]->revealed && !boardTiles[rowIndex][colIndex]->flag) {
			//Flag it
			boardTiles[rowIndex][colIndex]->flag = true;
			flagCounter++;
		} 
		//If Tile is NOT REVEALED and IS a FLAG
		else if (!boardTiles[rowIndex][colIndex]->revealed && boardTiles[rowIndex][colIndex]->flag) {
			boardTiles[rowIndex][colIndex]->flag = false;
			flagCounter--;
		}
	}

	//Calculates and resets adjacent mine vector for each tile
	void CalculateAdjacentMines() { //After every turn, calculates adjacent mines for each tile
		//Top border
		ResetAdjacentTiles();
		for (int c = 1; c < width - 1; c++) {
			if (boardTiles[0][c - 1]->mine == true) {
				boardTiles[0][c]->adjacentTiles.push_back(true);
			}
			if (boardTiles[0][c + 1]->mine == true) {
				boardTiles[0][c]->adjacentTiles.push_back(true);
			}
			if (boardTiles[1][c - 1]->mine == true) {
				boardTiles[0][c]->adjacentTiles.push_back(true);
			}
			if (boardTiles[1][c]->mine == true) {
				boardTiles[0][c]->adjacentTiles.push_back(true);
			}
			if (boardTiles[1][c+1]->mine == true) {
				boardTiles[0][c]->adjacentTiles.push_back(true);
			}
		}
		//Bottom border
		for (int c = 1; c < width - 1; c++) {
			if (boardTiles[height - 1][c - 1]->mine == true) {
				boardTiles[height - 1][c]->adjacentTiles.push_back(true);
			}
			if (boardTiles[height - 1][c + 1]->mine == true) {
				boardTiles[height - 1][c]->adjacentTiles.push_back(true);
			}
			if (boardTiles[height - 2][c - 1]->mine == true) {
				boardTiles[height - 1][c]->adjacentTiles.push_back(true);
			}
			if (boardTiles[height - 2][c]->mine == true) {
				boardTiles[height - 1][c]->adjacentTiles.push_back(true);
			}
			if (boardTiles[height - 2][c + 1]->mine == true) {
				boardTiles[height - 1][c]->adjacentTiles.push_back(true);
			}
		}
		//Left border
		for (int r = 1; r < height - 1; r++) {
			if (boardTiles[r - 1][0]->mine == true) {
				boardTiles[r][0]->adjacentTiles.push_back(true);
			}
			if (boardTiles[r + 1][0]->mine == true) {
				boardTiles[r][0]->adjacentTiles.push_back(true);
			}
			if (boardTiles[r - 1][1]->mine == true) {
				boardTiles[r][0]->adjacentTiles.push_back(true);
			}
			if (boardTiles[r][1]->mine == true) {
				boardTiles[r][0]->adjacentTiles.push_back(true);
			}
			if (boardTiles[r + 1][1]->mine == true) {
				boardTiles[r][0]->adjacentTiles.push_back(true);
			}
		}
		//Right border
		for (int r = 1; r < height - 1; r++) {
			if (boardTiles[r - 1][width - 1]->mine == true) {
				boardTiles[r][width - 1]->adjacentTiles.push_back(true);
			}
			if (boardTiles[r + 1][width - 1]->mine == true) {
				boardTiles[r][width - 1]->adjacentTiles.push_back(true);
			}
			if (boardTiles[r - 1][width - 2]->mine == true) {
				boardTiles[r][width - 1]->adjacentTiles.push_back(true);
			}
			if (boardTiles[r][width - 2]->mine == true) {
				boardTiles[r][width - 1]->adjacentTiles.push_back(true);
			}
			if (boardTiles[r + 1][width - 2]->mine == true) {
				boardTiles[r][width - 1]->adjacentTiles.push_back(true);
			}
		}
		//Corners
		{
			//Top left corner
			if (boardTiles[0][1]->mine == true) {
				boardTiles[0][0]->adjacentTiles.push_back(true);
			}
			if (boardTiles[1][0]->mine == true) {
				boardTiles[0][0]->adjacentTiles.push_back(true);
			}
			if (boardTiles[1][1]->mine == true) {
				boardTiles[0][0]->adjacentTiles.push_back(true);
			}
			//Top right corner
			if (boardTiles[0][width - 2]->mine == true) {
				boardTiles[0][width - 1]->adjacentTiles.push_back(true);
			}
			if (boardTiles[1][width - 2]->mine == true) {
				boardTiles[0][width - 1]->adjacentTiles.push_back(true);
			}
			if (boardTiles[1][width - 1]->mine == true) {
				boardTiles[0][width - 1]->adjacentTiles.push_back(true);
			}
			//Bottom left corner
			if (boardTiles[height - 1][1]->mine == true) {
				boardTiles[height - 1][0]->adjacentTiles.push_back(true);
			}
			if (boardTiles[height - 2][0]->mine == true) {
				boardTiles[height - 1][0]->adjacentTiles.push_back(true);
			}
			if (boardTiles[height - 2][1]->mine == true) {
				boardTiles[height - 1][0]->adjacentTiles.push_back(true);
			}
			//Bottom right corner
			if (boardTiles[height - 1][width - 2]->mine == true) {
				boardTiles[height - 1][width - 1]->adjacentTiles.push_back(true);
			}
			if (boardTiles[height - 2][width - 1]->mine == true) {
				boardTiles[height - 1][width - 1]->adjacentTiles.push_back(true);
			}
			if (boardTiles[height - 2][width - 2]->mine == true) {
				boardTiles[height - 1][width - 1]->adjacentTiles.push_back(true);
			}
		}
		//Normal tiles
		for (int r = 1; r < height - 1; r++) {
			for (int c = 1; c < width - 1; c++) {
				if (boardTiles[r - 1][c - 1]->mine == true) {
					boardTiles[r][c]->adjacentTiles.push_back(true);
				}
				if (boardTiles[r - 1][c]->mine == true) {
					boardTiles[r][c]->adjacentTiles.push_back(true);
				}
				if (boardTiles[r - 1][c + 1]->mine == true) {
					boardTiles[r][c]->adjacentTiles.push_back(true);
				}
				if (boardTiles[r][c - 1]->mine == true) {
					boardTiles[r][c]->adjacentTiles.push_back(true);
				}
				if (boardTiles[r][c + 1]->mine == true) {
					boardTiles[r][c]->adjacentTiles.push_back(true);
				}
				if (boardTiles[r + 1][c - 1]->mine == true) {
					boardTiles[r][c]->adjacentTiles.push_back(true);
				}
				if (boardTiles[r + 1][c]->mine == true) {
					boardTiles[r][c]->adjacentTiles.push_back(true);
				}
				if (boardTiles[r + 1][c + 1]->mine == true) {
					boardTiles[r][c]->adjacentTiles.push_back(true);
				}
			}
		}
	}
	void ResetAdjacentTiles() {
		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width; c++) {
				boardTiles[r][c]->adjacentTiles.clear();
			}
		}
	}

	//Checks if game is lost and if so displays mines
	void DefeatBoard() {
		if (gameLost) {
			for (int r = 0; r < height; r++) {
				for (int c = 0; c < width; c++) {
					if (boardTiles[r][c]->mine) {
						boardTiles[r][c]->revealed = true;
					}
				}
			}
		}
	}

	//Checks if game is won and if so displays all mines as flags
	bool VictoryCheck() {
		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width; c++) {
				if (!boardTiles[r][c]->revealed && !boardTiles[r][c]->mine) {
					return false;
				}
			}
		}
		gameWon = true;
		return gameWon;
	}
	void VictoryReveal() {
		if (VictoryCheck()) {
			for (int r = 0; r < height; r++) {
				for (int c = 0; c < width; c++) {
					if (boardTiles[r][c]->mine) {
						boardTiles[r][c]->flag = true;
					}
				}
			}
			debugMode = false;
			flagCounter = numMines;
		}
	}
};

