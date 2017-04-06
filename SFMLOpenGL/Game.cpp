
#include <Game.h>
#include <Cube.h>

// Helper to convert Number to String for HUD
template <typename T>
string toString(T number)
{
	ostringstream oss;
	oss << number;
	return oss.str();
}

GLuint	vsid,		// Vertex Shader ID
		fsid,		// Fragment Shader ID
		enemyProgID, playerProgID,		// Program ID
		vao = 0,	// Vertex Array ID
		vbo,		// Vertex Buffer ID
		vib,		// Vertex Index Buffer
		to[1];		// Texture ID
GLint	positionID,	// Position ID
		colorID,	// Color ID
		textureID,	// Texture ID
		uvID,		// UV ID
		mvpID,		// Model View Projection ID
		x_offsetID, // X offset ID
		y_offsetID,	// Y offset ID
		z_offsetID;	// Z offset ID

GLenum	error;		// OpenGL Error Code


//Please see .//Assets//Textures// for more textures
const string filename = ".//Assets//Textures//texture.tga";

int width;						// Width of texture
int height;						// Height of texture
int comp_count;					// Component of texture

unsigned char* img_data;		// image data

mat4 mvp, projection, 
		view, player, 
	    barrier1, barrier2, 
	    rotation1, rotation2;			// Model View Projection

Font font;						// Game font

float x_offset, y_offset, z_offset; // offset on screen (Vertex Shader)

Game::Game() : 
	window(VideoMode(800, 600), 
	"Introduction to OpenGL Texturing")
{
}

Game::Game(sf::ContextSettings settings) : 
	window(VideoMode(800, 600), 
	"Introduction to OpenGL Texturing", 
	sf::Style::Default, 
	settings)
{
}

Game::~Game(){}


void Game::run()
{

	initialize();

	Event event;

	while (isRunning){

#if (DEBUG >= 2)
		DEBUG_MSG("Game running...");
#endif

		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				isRunning = false;
			}

			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				if (player[3].x > -3.9)
				{

					// Set Model Rotation
					//player = rotate(player, 0.01f, glm::vec3(0, 1, 0)); // Rotate
					player = translate(player, vec3(-0.5, 0, 0));
				}
				
			}

			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				if (player[3].x < 3.9)
				{
					// Set Model Rotation
					//player = rotate(player, -0.01f, glm::vec3(0, 1, 0)); // Rotate
					player = translate(player, vec3(0.5, 0, 0));
				}
			}

			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				// Set Model Rotation
				player = rotate(player, -0.01f, glm::vec3(1, 0, 0)); // Rotate
			}

			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				// Set Model Rotation
				player = rotate(player, 0.01f, glm::vec3(1, 0, 0)); // Rotate
			}
		}
		update();
		render();
	}

#if (DEBUG >= 2)
	DEBUG_MSG("Calling Cleanup...");
#endif
	unload(enemyProgID);
	unload(playerProgID);
}

void Game::initialize()
{
	isRunning = true;

	if (!(!glewInit())) { DEBUG_MSG("glewInit() failed"); }

	// Copy UV's to all faces
	for (int i = 1; i < 6; i++)
		memcpy(&uvs[i * 4 * 2], &uvs[0], 2 * 4 * sizeof(GLfloat));

	DEBUG_MSG(glGetString(GL_VENDOR));
	DEBUG_MSG(glGetString(GL_RENDERER));
	DEBUG_MSG(glGetString(GL_VERSION));

	// Vertex Array Buffer
	glGenBuffers(1, &vbo);		// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Vertices (3) x,y,z , Colors (4) RGBA, UV/ST (2)
	glBufferData(GL_ARRAY_BUFFER, ((3 * VERTICES) + (4 * COLORS) + (2 * UVS)) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

	glGenBuffers(1, &vib); //Generate Vertex Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vib);

	// Indices to be drawn
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * INDICES * sizeof(GLuint), indices, GL_STATIC_DRAW);

	/*CREATE SHADER PROGRAMS*/
	playerProgID = createProg("vertexShader.txt", "playerFragShader.txt");
	enemyProgID = createProg("vertexShader.txt", "enemyFragShader.txt");
	/*END OF SHADER PROGRAM CREATIONS*/
	// Set image data
	// https://github.com/nothings/stb/blob/master/stb_image.h
	img_data = stbi_load(filename.c_str(), &width, &height, &comp_count, 4);

	if (img_data == NULL)
	{
		DEBUG_MSG("ERROR: Texture not loaded");
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &to[0]);
	glBindTexture(GL_TEXTURE_2D, to[0]);

	// Wrap around
	// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Filtering
	// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind to OpenGL
	// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexImage2D.xml
	glTexImage2D(
		GL_TEXTURE_2D,			// 2D Texture Image
		0,						// Mipmapping Level 
		GL_RGBA,				// GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RGB, GL_BGR, GL_RGBA 
		width,					// Width
		height,					// Height
		0,						// Border
		GL_RGBA,				// Bitmap
		GL_UNSIGNED_BYTE,		// Specifies Data type of image data
		img_data				// Image Data
		);

	// Projection Matrix 
	projection = perspective(
		45.0f,					// Field of View 45 degrees
		4.0f / 3.0f,			// Aspect ratio
		0.1f,					// Display Range Min : 0.1f unit
		100.0f					// Display Range Max : 100.0f unit
		);

	// Camera Matrix
	view = lookAt(
		vec3(0.0f, 4.0f, 10.0f),	// Camera (x,y,z), in World Space
		vec3(0.0f, 0.0f, 0.0f),		// Camera looking at origin
		vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
		);

	// Model matrix
	player = mat4(
		1.0f					// Identity Matrix
		);

	for (size_t i = 0; i < 6; i++)
	{
		enemy[i] = mat4(
			1.0f					// Identity Matrix
		);
	}
	
	barrier1 = mat4(
		1.0f					//Indentity Matrix
	);
	barrier2 = mat4(
		1.0f
	);
	rotation1 = mat4(
		1.0f					//Indentity Matrix
	);
	rotation2 = mat4(
		1.0f					//Indentity Matrix
	);
	enemy[1] = glm::translate(enemy[1], glm::vec3(-3, 0, -250));//LEFT
	enemy[2] = glm::translate(enemy[2], glm::vec3(3, 0, -250));//RIGHT
	enemy[3] = glm::translate(enemy[3], glm::vec3(-3, 0, -250));//LEFT
	enemy[4] = glm::translate(enemy[4], glm::vec3(3, 0, -250));//RIGHT
	enemy[5] = glm::translate(enemy[5], glm::vec3(-3, 0, -250));//LEFT
	enemy[0] = glm::translate(enemy[0], glm::vec3(0, 0, -250));//MIDDLE

	rotation1 = glm::translate(rotation1, glm::vec3(-5, 3, 0));//LEFT CORNER
	rotation2 = glm::translate(rotation2, glm::vec3(5, 3, 0));//RIGHT CORNER

	barrier1 = glm::translate(barrier1, glm::vec3(-5, 0, 0));
	barrier1 = glm::scale(barrier1, glm::vec3(0, 1, 100)); //LEFT BARRIER
	barrier2 = glm::translate(barrier2, glm::vec3(5, 0, 0));
	barrier2 = glm::scale(barrier2, glm::vec3(0, 1, 100)); //RIGHT BARRIER

	player = glm::scale(player, glm::vec3(0.5, 0.5, 0.5));
	
	// Enable Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	speedMod = 0;
	// Load Font
	font.loadFromFile(".//Assets//Fonts//BBrick.ttf");
	score = 0;
}
/*----------------------------------------------CREATE PROG----------------------------------------*/
GLuint Game::createProg(std::string vertexShader, std::string fragmentShader)
{
	GLuint prog = -1;
	GLint isCompiled = 0;
	GLint isLinked = 0;
	// NOTE: uniforms values must be used within Shader so that they 
	// can be retreived
	ifstream vertexFile;
	std::string vString;
	std::stringstream vContent;
	vertexFile.open(vertexShader);
	vContent << vertexFile.rdbuf();
	vertexFile.close();
	vString = vContent.str();

	const char* vs_src = vString.c_str();


	DEBUG_MSG("Setting Up Vertex Shader");

	vsid = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsid, 1, (const GLchar**)&vs_src, NULL);
	glCompileShader(vsid);

	// Check is Shader Compiled
	glGetShaderiv(vsid, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_TRUE) {
		DEBUG_MSG("Vertex Shader Compiled");
		isCompiled = GL_FALSE;
	}
	else
	{
		DEBUG_MSG("ERROR: Vertex Shader Compilation Error");
	}

	ifstream fragFile;
	std::string fString;
	std::stringstream fContent;
	fragFile.open(fragmentShader);
	fContent << fragFile.rdbuf();
	fragFile.close();
	fString = fContent.str();
	const char* fs_src = fString.c_str();

	DEBUG_MSG("Setting Up Fragment Shader");

	fsid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsid, 1, (const GLchar**)&fs_src, NULL);
	glCompileShader(fsid);

	// Check is Shader Compiled
	glGetShaderiv(fsid, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_TRUE) {
		DEBUG_MSG("Fragment Shader Compiled");
		isCompiled = GL_FALSE;
	}
	else
	{
		DEBUG_MSG("ERROR: Fragment Shader Compilation Error");
	}

	DEBUG_MSG("Setting Up and Linking Shader");
	prog = glCreateProgram();
	glAttachShader(prog, vsid);
	glAttachShader(prog, fsid);
	glLinkProgram(prog);

	// Check is Shader Linked
	glGetProgramiv(prog, GL_LINK_STATUS, &isLinked);

	if (isLinked == 1) {
		DEBUG_MSG("Shader Linked");
	}
	else
	{
		DEBUG_MSG("ERROR: Shader Link Error");
	}

	return prog;
}
void Game::update()
{
#if (DEBUG >= 2)
	DEBUG_MSG("Updating...");
#endif
	// Update Model View Projection
	// For mutiple objects (cubes) create multiple models
	// To alter Camera modify view & projection


	rotation1 = glm::rotate(rotation1, 0.002f, glm::vec3(0, 0, 1));
	rotation2 = glm::rotate(rotation2, 0.002f, glm::vec3(0, 0, 1));

	for (size_t i = 0; i < 6; i++)
	{
		if (enemy[i][3].z > 15)
		{
			enemy[i] = glm::translate(enemy[i], vec3(0, 0, -100));
			speedMod += 0.001f;
			score += 100;
		}
		//enemyspeed = enemyspeed + 0.000000000032f;
	}

	enemy[1] = glm::translate(enemy[1], vec3(0, 0, 0.025 + speedMod));	
	enemy[2] = glm::translate(enemy[2], vec3(0, 0, 0.02 + speedMod));
	/*enemy[3] = glm::translate(enemy[3], vec3(0, 0, 0.03));	
	enemy[4] = glm::translate(enemy[4], vec3(0, 0, 0.035));	
	enemy[5] = glm::translate(enemy[5], vec3(0, 0, 0.02));*/
	enemy[0] = glm::translate(enemy[0], vec3(0, 0, 0.04 + speedMod));

	


	//----------------------------------COLLISION--------------------------------------//
	playerRect = sf::FloatRect(player[3].x, player[3].z, 1, 1);
	for (size_t i = 0; i < 6; i++)
	{
		sf::FloatRect enemyRect = sf::FloatRect(enemy[i][3].x, enemy[i][3].z, 2, 2);
		if (playerRect.intersects(enemyRect))
		{
			std::cout << "hit" << std::endl;
			isRunning = false;
		}
		else
		{
			std::cout << std::endl;
		}
	}
}

void Game::render()
{

#if (DEBUG >= 2)
	DEBUG_MSG("Render Loop...");
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save current OpenGL render states
	// https://www.sfml-dev.org/documentation/2.0/classsf_1_1RenderTarget.php#a8d1998464ccc54e789aaf990242b47f7
	window.pushGLStates();

	// Find mouse position using sf::Mouse
	int x = Mouse::getPosition(window).x;
	int y = Mouse::getPosition(window).y;

	string hud = "Score: "
		+ std::to_string(score);

	Text text(hud, font);

	text.setColor(sf::Color(255, 255, 255, 170));
	text.setPosition(20.f, 20.f);

	window.draw(text);

	// Restore OpenGL render states
	// https://www.sfml-dev.org/documentation/2.0/classsf_1_1RenderTarget.php#a8d1998464ccc54e789aaf990242b47f7

	window.popGLStates();
	
	drawCube(player, playerProgID);

	for (size_t i = 0; i < 6; i++)
	{
		drawCube(enemy[i], enemyProgID);
	}

	drawCube(barrier1, enemyProgID);
	drawCube(barrier2, enemyProgID);
	drawCube(rotation1, enemyProgID);
	drawCube(rotation2, enemyProgID);

	window.display();

	// Disable Arrays
	glDisableVertexAttribArray(positionID);
	glDisableVertexAttribArray(colorID);
	glDisableVertexAttribArray(uvID);

	// Unbind Buffers with 0 (Resets OpenGL States...important step)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Reset the Shader Program to Use
	glUseProgram(0);

	// Check for OpenGL Error code
	error = glGetError();
	if (error != GL_NO_ERROR) {
		DEBUG_MSG(error);
	}
}
void Game::drawCube(glm::mat4 model, GLuint prog)
{
	mvp = projection * view * model;
	// Rebind Buffers and then set SubData
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vib);

	// Use Progam on GPU
	glUseProgram(prog);

	// Find variables within the shader
	// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetAttribLocation.xml
	positionID = glGetAttribLocation(prog, "sv_position");
	if (positionID < 0) { DEBUG_MSG("positionID not found"); }

	colorID = glGetAttribLocation(prog, "sv_color");
	if (colorID < 0) { DEBUG_MSG("colorID not found"); }

	uvID = glGetAttribLocation(prog, "sv_uv");
	if (uvID < 0) { DEBUG_MSG("uvID not found"); }

	textureID = glGetUniformLocation(prog, "f_texture");
	//if (textureID < 0) { DEBUG_MSG("textureID not found"); }

	mvpID = glGetUniformLocation(prog, "sv_mvp");
	if (mvpID < 0) { DEBUG_MSG("mvpID not found"); }

	x_offsetID = glGetUniformLocation(prog, "sv_x_offset");
	if (x_offsetID < 0) { DEBUG_MSG("x_offsetID not found"); }

	y_offsetID = glGetUniformLocation(prog, "sv_y_offset");
	if (y_offsetID < 0) { DEBUG_MSG("y_offsetID not found"); }

	z_offsetID = glGetUniformLocation(prog, "sv_z_offset");
	if (z_offsetID < 0) { DEBUG_MSG("z_offsetID not found"); };

	// VBO Data....vertices, colors and UV's appended
	glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * VERTICES * sizeof(GLfloat), 4 * COLORS * sizeof(GLfloat), colors);
	glBufferSubData(GL_ARRAY_BUFFER, ((3 * VERTICES) + (4 * COLORS)) * sizeof(GLfloat), 2 * UVS * sizeof(GLfloat), uvs);

	// Send transformation to shader mvp uniform [0][0] is start of array
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

	// Set Active Texture .... 32 GL_TEXTURE0 .... GL_TEXTURE31
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(textureID, 0); // 0 .... 31

							   // Set the X, Y and Z offset (this allows for multiple cubes via different shaders)
							   // Experiment with these values to change screen positions
	glUniform1f(x_offsetID, 0.00f);
	glUniform1f(y_offsetID, 0.00f);
	glUniform1f(z_offsetID, 0.00f);

	// Set pointers for each parameter (with appropriate starting positions)
	// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glVertexAttribPointer.xml
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, (VOID*)(3 * VERTICES * sizeof(GLfloat)));
	glVertexAttribPointer(uvID, 2, GL_FLOAT, GL_FALSE, 0, (VOID*)(((3 * VERTICES) + (4 * COLORS)) * sizeof(GLfloat)));

	// Enable Arrays
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);
	glEnableVertexAttribArray(uvID);

	// Draw Element Arrays
	glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);
}
void Game::unload(GLuint prog)
{
#if (DEBUG >= 2)
	DEBUG_MSG("Cleaning up...");
#endif
	glDetachShader(prog, vsid);	// Shader could be used with more than one progID
	glDetachShader(prog, fsid);	// ..
	glDeleteShader(vsid);			// Delete Vertex Shader
	glDeleteShader(fsid);			// Delete Fragment Shader
	glDeleteProgram(prog);		// Delete Shader
	glDeleteBuffers(1, &vbo);		// Delete Vertex Buffer
	glDeleteBuffers(1, &vib);		// Delete Vertex Index Buffer
	//stbi_image_free(img_data);		// Free image stbi_image_free(..)
}

