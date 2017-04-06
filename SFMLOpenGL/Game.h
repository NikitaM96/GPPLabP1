#ifndef GAME_H
#define GAME_H

#include <string>
#include <sstream>

#include <iostream>
#include <GL/glew.h>
#include <GL/wglew.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Debug.h>

#include <sstream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace sf;
using namespace glm;
enum class GameState
{
	START, GAME_OVER, PLAY
};
class Game
{
public:
	Game();
	Game(sf::ContextSettings settings);
	~Game();
	void run();
	GLuint createProg(std::string vertexShader, std::string fragmentShader);
private:
	RenderWindow window;
	bool isRunning = false;
	void initialize();
	void update();
	void drawCube(glm::mat4 model, GLuint prog);
	void render();
	void unload(GLuint prog);
	void unloadPlayer(GLuint prog);
	mat4 enemy[6];
	vec3 enemyVec[6], playerVec;
	//std::vector<vec3> enemyVec;
	float enemyspeed = 0.02f;
	sf::FloatRect playerRect;
	float speedMod;
	float score;
	GameState currentState;
	sf::Texture startScreenTexture;
	sf::Sprite startScreenSprite;
	sf::Texture endScreenTexture;
	sf::Sprite endScreenSprite;
};

#endif