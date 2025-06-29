#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include<algorithm>
using namespace std;

enum class gamestate{Menu, Playing, GameOver, Win};



// Override base class with your custom functionality
class Asteroids : public olc::PixelGameEngine


{
public:
	Asteroids()
	{
		// Name your application
		sAppName = "Asteroids";
	}

private:
	struct sSpaceObject
	{
		int size; 
		float x; 
		float y;
		float dx; 
		float dy; 
		float angle;
	};

	vector<sSpaceObject> vecAsteroids;
	vector<sSpaceObject> vecBullets;
	sSpaceObject player;
	bool bDead = false;
	int nScore = 0;
	int attmptes = 3;
	int WON = 0;
	gamestate gstate = gamestate::Menu;
	float fTotalTime = 0.0f; 



	vector <pair<float, float>>vecModelShip;
	vector <pair<float, float>>vecModelAsteroid;



public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		vecModelShip = {
			{0.0f,-5.0f},
			{-2.5f,+2.5f},
			{ +2.5,+2.5f}
		};  // ship model 

		// make weird shape asteroid
		int verts = 45;

		for (int i = 0; i < verts; i++) {
			float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f; // make the unevaint circule
			vecModelAsteroid.push_back(make_pair(noise * sinf(((float)i / (float)verts) * 6.28318f),
				noise * cosf(((float)i / (float)verts) * 6.28318f)));
		}

		ResetGame(); //--make it  
		return true;
	}





	void Updategame(float fElapsedTime) {
		if (bDead ) { //|| attmptes == 0
			//if (attmptes == 0)
			//	attmptes = 3; //reset

			ResetGame();
		}
		//clear screen 
		Clear(olc::BLACK);

		if (GetKey(olc::Key::LEFT).bHeld)
			player.angle -= 5.0f * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld)
			player.angle += 5.0f * fElapsedTime;

		//apply Acceleration 
		if (GetKey(olc::Key::DOWN).bHeld) {// "break"
			//acceleration changs velocity "with respect to time "
			player.dx -= sin(player.angle) * 30.0f * fElapsedTime;
			player.dy += cos(player.angle) * 30.0f * fElapsedTime;
		}
		if (GetKey(olc::Key::UP).bHeld) {//"acculration"
			//acceleration changs velocity "with respect to time "
			player.dx += sin(player.angle) * 30.0f * fElapsedTime;
			player.dy -= cos(player.angle) * 30.0f * fElapsedTime;
		}



		//velocity change speed with respect to time
		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;

		//keep in game space 
		WarpCoordinates(player.x, player.y, player.x, player.y);




		//check ship collision with astroids 
		for (auto& a : vecAsteroids) {
			if (IsPointerInsideCircle(a.x, a.y, a.size, player.x, player.y)) {
				bDead = true;
				attmptes -= 1;
			}
		}



		//fire bullet in player dirction
		if (GetKey(olc::Key::SPACE).bReleased) {
			vecBullets.push_back({ 0, player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 100.0f });

		}

		//update & draw astroids 
		for (auto& a : vecAsteroids) {
			//velocity change pos 
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			a.angle += 0.5f * fElapsedTime; // Add swanky rotation
			//keep every thing in game space 
			WarpCoordinates(a.x, a.y, a.x, a.y);


			//draw astroids
			DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, (float)a.size, olc::CYAN);

		}

		// Any new asteroids created after collision detection are stored
		// in a temporary vector, so we don't interfere with the asteroids
		// vector iterator in the for(auto)
		vector<sSpaceObject> newAstroids;


		//update bullets (I NEED MORE BULLETTSS)

		for (auto& b : vecBullets) {
			b.x += b.dx * fElapsedTime;
			b.y += b.dy * fElapsedTime;
			WarpCoordinates(b.x, b.y, b.x, b.y);
			b.angle -= 1.0f * fElapsedTime;

			// Check collisiton with asteroids
			for (auto& a : vecAsteroids)
			{
				//if (IsPointInsideRectangle(a.x, a.y, a.x + a.nSize, a.y + a.nSize, b.x, b.y))
				if (IsPointerInsideCircle(a.x, a.y, a.size, b.x, b.y))
				{
					// Asteroid Hit - Remove bullet
					// We've already updated the bullets, so force bullet to be offscreen
					// so it is cleaned up by the removal algorithm. 
					b.x = -100;

					// Create child asteroids
					if (a.size > 4)
					{
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						newAstroids.push_back({ (int)a.size >> 1 ,a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), 0.0f });
						newAstroids.push_back({ (int)a.size >> 1 ,a.x, a.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), 0.0f });
					}

					// Remove asteroid - Same approach as bullets
					a.x = -100;
					nScore += 100; // Small score increase for hitting asteroid
				}
			}

		}


		// append new shithead to exisisting vector 
		for (auto& a : newAstroids)
			vecAsteroids.push_back(a);

		//clear up dead object {out of game space}

		//remove asteroids that have been blown up typeshit

		if (vecAsteroids.size() > 0) {

			auto i = remove_if(vecAsteroids.begin(), vecAsteroids.end(), [&](sSpaceObject o) { return (o.x < 0); });
			if (i != vecAsteroids.end())
				vecAsteroids.erase(i);
		}



		if (vecAsteroids.empty()) {

			//leve clear 
			nScore += 1000;
			WON++;
			vecAsteroids.clear();
			vecBullets.clear();
			// Add two new asteroids, but in a place where the player is not, we'll simply
			// add them 90 degrees left and right to the player, their coordinates will
			// be wrapped by th enext asteroid update


			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle - 3.14159f / 2.0f) + player.x,
									  30.0f * cosf(player.angle - 3.14159f / 2.0f) + player.y,
									  10.0f * sinf(player.angle), 10.0f * cosf(player.angle), 0.0f });

			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle + 3.14159f / 2.0f) + player.x,
											  30.0f * cosf(player.angle + 3.14159f / 2.0f) + player.y,
											  10.0f * sinf(-player.angle), 10.0f * cosf(-player.angle), 0.0f });

		}

		//remove bullets that gone off the screen 
		if (vecBullets.size() > 0) {

			auto i = remove_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject o) { return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() - 1 || o.y >= ScreenHeight() - 1); });
			if (i != vecBullets.end())
				vecBullets.erase(i);

		}

		//draw bullets 
		for (auto b : vecBullets) {
			DrawWrapped(b.x, b.y); // i don't think this is correct 

		}

		// Draw Ship
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);

		// Draw Score
		//DrawString(2, 2, "SCORE: " + std::to_string(nScore), olc::WHITE);
		DrawStringDecal({ 2.0f, 2.0f }, "SCORE: " + std::to_string(nScore), olc::GREEN, { 0.5f, 0.5f });
		//Draw attmpts
		DrawStringDecal({ 2.0f, 7.0f }, "ATTMPTS: " + std::to_string(attmptes), olc::RED, { 0.5f, 0.5f });
		//draw level
		DrawStringDecal({ 2.0f, 12.0f }, "LEVEL: " + std::to_string(WON), olc::BLUE, { 0.5f, 0.5f });

	}




	bool OnUserUpdate(float fElapsedTime) override
	{
		fTotalTime += fElapsedTime;

		
		switch (gstate) {

		case gamestate::Menu:
		{
			Clear(olc::BLUE);

			std::string msg = "PRESS SPACE TO START";
			olc::vi2d textSize = GetTextSize(msg);
			float centerX = (ScreenWidth() - textSize.x) / 2.0f;
			float centerY = ScreenHeight() / 2.0f;

			// Fade in/out using sine wave
			float alpha = (sinf(fTotalTime * 2.0f) + 1.0f) / 2.0f; // between 0 and 1
			uint8_t a = (uint8_t)(alpha * 255.0f); // convert to 0-255
			olc::Pixel fadeColor = olc::Pixel(255, 255, 255, a);   // White with dynamic alpha

			// Draw fading text
			DrawStringDecal({ centerX, centerY }, msg, fadeColor);

			if (GetKey(olc::Key::SPACE).bPressed) {
				ResetGame();
				gstate = gamestate::Playing;
			}
		}
			break;


		case gamestate::Playing:
			Updategame(fElapsedTime);
			if (attmptes == 0)
				gstate = gamestate::GameOver;
			if (WON == 5)
				gstate = gamestate::Win;
			break;
		case gamestate::GameOver:
			Clear(olc::DARK_RED);
			{
				std::string msg1 = "GAME OVER";
				std::string msg2 = "PRESS R TO RESTART";
				std::string msg3 = "Score: " + std::to_string(nScore);

				DrawString(ScreenWidth() / 2 - GetTextSize(msg1).x / 2, ScreenHeight() / 2, msg1, olc::WHITE);
				DrawString(ScreenWidth() / 2 - GetTextSize(msg2).x / 2, ScreenHeight() / 2 + 10, msg2, olc::WHITE);
				DrawString(ScreenWidth() / 2 - GetTextSize(msg3).x / 2, ScreenHeight() / 2 + 20, msg3, olc::WHITE);
			}
			
			if (GetKey(olc::Key::R).bPressed) {
				gstate = gamestate::Menu;
				attmptes = 3;
				WON = 0;
				nScore = false;
			}
			break;

		case gamestate::Win:
			Clear(olc::DARK_GREEN);
			{
				std::string msg1 = "YOU WIN!";
				std::string msg2 = "PRESS R TO RESTART";
				std::string msg3 = "Score: " + std::to_string(nScore);

				DrawString(ScreenWidth() / 2 - GetTextSize(msg1).x / 2, ScreenHeight() / 2 - 10, msg1, olc::YELLOW);
				DrawString(ScreenWidth() / 2 - GetTextSize(msg2).x / 2, ScreenHeight() / 2 + 10, msg2, olc::WHITE);
				DrawString(ScreenWidth() / 2 - GetTextSize(msg3).x / 2, ScreenHeight() / 2 + 20, msg3, olc::WHITE);
			}




			if (GetKey(olc::Key::R).bPressed) {
				gstate = gamestate::Menu;
				attmptes = 3;
				WON = 0;
				nScore = false;
			}
			break;










		}



				






		
		return true;
	}




	void DrawWireFrameModel(const vector<pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, olc::Pixel col = olc::WHITE)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		vector<pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine(vecTransformedCoordinates[i % verts].first, vecTransformedCoordinates[i % verts].second,
				vecTransformedCoordinates[j % verts].first, vecTransformedCoordinates[j % verts].second,  col);
		}
	}




	//when you die: -- implemnt lives :)
	void ResetGame() {

		// Initialise Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		vecBullets.clear();
		vecAsteroids.clear();

		// Put in two asteroids
		vecAsteroids.push_back({ (int)16, 20.0f, 20.0f, 8.0f, -6.0f, 0.0f });
		vecAsteroids.push_back({ (int)16, 100.0f, 20.0f, -5.0f, 3.0f, 0.0f });

		// Reset game
		bDead = false;
		//nScore = false;


	}


	// Implements "wrap around" for various in-game sytems
	void WarpCoordinates(float ix, float iy, float& ox, float& oy) {

		ox = ix; 
		oy = iy;
		if (ix < 0.0f) ox = ix + (float)ScreenWidth(); // out of screen border
		if (ix >= (float)ScreenWidth()) ox = ix - (float)ScreenWidth();
		if (iy < 0.0f)	oy = iy + (float)ScreenHeight();
		if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
	}

	void DrawWrapped(int x, int y, olc::Pixel color = olc::WHITE)
	{
		float fx, fy;
		WarpCoordinates((float)x, (float)y, fx, fy);
		Draw((int)fx, (int)fy, color);
	}

	bool IsPointerInsideCircle(float cx, float cy, float radius, float x, float y) {
		return sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) < radius; // eww math 
	}






















};

int main()
{
	Asteroids demo;
	if (demo.Construct(165, 140, 4, 4))
		demo.Start();
	return 0;
}