#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#define _USE_MATH_DEFINES
#define WIDTH 1280
#define HEIGHT 720
#define MAX_BULLETS 16
#define MAX_ASTEROIDS 65
#define MAX_BACKGROUND_ASTEROIDS 64
#define MAX_MENU_LEVELS 5
#define USE_SHADOW 1
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>

typedef struct {
	char name[9];
	int points;
} playerInfo;

namespace other {
	int MAX_PLAYERS = 1;
	bool DIFFICULTY = 0;
	float rand(int min, int max) {
		return (float)(min + std::rand() % (max - min + 1));
	};
	void randName(char *s, int n) {
		int i;
		for (i = 0; i < n - 1; i++) {
			s[i] = other::rand(33, 126);
		}
		s[n] = '\0';
	}
	void swap(playerInfo &a, playerInfo &b) {
		playerInfo temp = a;
		a = b;
		b = temp;
	}
}

class endScreen {
private:
	sf::Font font;
	sf::Text text;
	int frames;
public:
	endScreen();
	void draw(sf::RenderWindow &window);
};

endScreen::endScreen() {
	frames = 0;
	font.loadFromFile("consola.ttf");
	text.setFont(font);
	text.setCharacterSize(40);
	text.setString("game_over");
	text.setPosition(sf::Vector2f(40, HEIGHT - 100));
}

void endScreen::draw(sf::RenderWindow &window) {
	window.draw(text);
	if (frames >= 50) window.close();
	frames++;
}

class menu {
private:
	sf::Font font;
	sf::Text menuText[MAX_MENU_LEVELS];
	sf::Text scoresText[6];
	sf::Text clearScoresText;
	int selectedItem;
	bool next;
	int recordsN;
	bool loadAvailable;
	void addScores(FILE *fp);
public:
	menu();
	void moveUp();
	void moveDown();
	int getSelectedItem() { return selectedItem; }
	void draw(sf::RenderWindow &window);
	void setNext();
	bool getNext() { return next; };
	void clearScores();
};

menu::menu() {
	selectedItem = 1;
	FILE *fp;
	fp = fopen("save.dat", "rb");
	if (fp != NULL) {
		loadAvailable = true;
		fclose(fp);
	}
	else loadAvailable = false;
	font.loadFromFile("consola.ttf");
	menuText[0].setFont(font);
	menuText[0].setCharacterSize(40);
	menuText[0].setString("new_game");
	menuText[0].setPosition(sf::Vector2f(40, HEIGHT - 40 - (MAX_MENU_LEVELS - 0) * 60));
	menuText[1].setFont(font);
	menuText[1].setCharacterSize(40);
	menuText[1].setString("one_player <<");
	menuText[1].setPosition(sf::Vector2f(80, HEIGHT - 40 - (MAX_MENU_LEVELS - 1) * 60));
	menuText[2].setFont(font);
	menuText[2].setCharacterSize(40);
	menuText[2].setString("two_players");
	menuText[2].setPosition(sf::Vector2f(80, HEIGHT - 40 - (MAX_MENU_LEVELS - 2) * 60));
	menuText[3].setFont(font);
	menuText[3].setCharacterSize(40);
	menuText[3].setString("load_game");
	menuText[3].setPosition(sf::Vector2f(40, HEIGHT - 40 - (MAX_MENU_LEVELS - 3) * 60));
	menuText[3].setFillColor(loadAvailable ? sf::Color::White : sf::Color(128, 128, 128));
	menuText[4].setFont(font);
	menuText[4].setCharacterSize(40);
	menuText[4].setString("quit");
	menuText[4].setPosition(sf::Vector2f(40, HEIGHT - 40 - (MAX_MENU_LEVELS - 4) * 60));
	for (int i = 0; i < 6; i++) {
		scoresText[i].setFont(font);
		scoresText[i].setCharacterSize(40);
		scoresText[i].setPosition(sf::Vector2f(WIDTH - 320 - (i == 0 ? 40 : 0), HEIGHT - 40 - (6 - i) * 60));
	}
	scoresText[0].setString("best_scores");
	fp = fopen("best.dat", "rb");
	addScores(fp);
	clearScoresText.setFont(font);
	clearScoresText.setCharacterSize(20);
	clearScoresText.setString("type_C_for_clear_scores");
	clearScoresText.setPosition(sf::Vector2f(WIDTH - 265, HEIGHT - 30));
}

void menu::addScores(FILE *fp) {
	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		recordsN = ftell(fp) / sizeof(playerInfo);
		rewind(fp);
		playerInfo playerRecord;
		playerInfo *playerInfoArray = new playerInfo[recordsN];
		for (int i = 0; i < recordsN; i++) {
			fread(&playerRecord, sizeof(playerInfo), 1, fp);
			strcpy(playerInfoArray[i].name, playerRecord.name);
			playerInfoArray[i].points = playerRecord.points;
		}
		for (int i = 0; i < recordsN - 1; i++) {
			for (int j = 0; j < recordsN - i - 1; j++)
				if (playerInfoArray[j].points < playerInfoArray[j + 1].points)
					other::swap(playerInfoArray[j], playerInfoArray[j + 1]);
		}
		for (int i = 0; i < 5; i++) {
			scoresText[i + 1].setString((i < recordsN) ? (playerInfoArray[i].name + std::string("_") + std::to_string(playerInfoArray[i].points)) : std::string("_"));
		}
		delete[] playerInfoArray;
		fclose(fp);
	}
	else {
		for (int i = 1; i < 6; i++) {
			scoresText[i].setString(std::string("_"));
		}
	}

}
void menu::moveUp() {
	if (selectedItem == 1) {
		if (!next) menuText[selectedItem].setString("one_player");
		else menuText[selectedItem].setString("easy");
	}
	else if (selectedItem == 2) {
		if (!next) menuText[selectedItem].setString("two_players");
		else menuText[selectedItem].setString("hard");
	}
	else if (selectedItem == 3) menuText[selectedItem].setString("load_game");
	else if (selectedItem == 4) menuText[selectedItem].setString("quit");
	if ((selectedItem == 4) && !loadAvailable) {
		selectedItem -= 2;
	}
	else selectedItem--;
	if (selectedItem < 1) selectedItem = MAX_MENU_LEVELS - 1;
	if (selectedItem == 1) {
		if (!next) menuText[selectedItem].setString("one_player <<");
		else menuText[selectedItem].setString("easy <<");
	}
	else if (selectedItem == 2) {
		if (!next) menuText[selectedItem].setString("two_players <<");
		else menuText[selectedItem].setString("hard <<");
	}
	else if (selectedItem == 3) menuText[selectedItem].setString("load_game <<");
	else if (selectedItem == 4) menuText[selectedItem].setString("quit <<");
}

void menu::moveDown() {
	if (selectedItem == 1) {
		if (!next) menuText[selectedItem].setString("one_player");
		else menuText[selectedItem].setString("easy");
	}
	else if (selectedItem == 2) {
		if (!next) menuText[selectedItem].setString("two_players");
		else menuText[selectedItem].setString("hard");
	}
	else if (selectedItem == 3) menuText[selectedItem].setString("load_game");
	else if (selectedItem == 4) menuText[selectedItem].setString("quit");
	if ((selectedItem == 2) && !loadAvailable) {
		selectedItem += 2;
	}
	else selectedItem++;
	if (selectedItem >= MAX_MENU_LEVELS) selectedItem = 1;
	if (selectedItem == 1) {
		if (!next) menuText[selectedItem].setString("one_player <<");
		else menuText[selectedItem].setString("easy <<");
	}
	else if (selectedItem == 2) {
		if (!next) menuText[selectedItem].setString("two_players <<");
		else menuText[selectedItem].setString("hard <<");
	}
	else if (selectedItem == 3) menuText[selectedItem].setString("load_game <<");
	else if (selectedItem == 4) menuText[selectedItem].setString("quit <<");
}

void menu::draw(sf::RenderWindow &window) {
	for (int i = 0; i < MAX_MENU_LEVELS; i++) {
		window.draw(menuText[i]);
	}
	for (int i = 0; i < 6; i++) {
		window.draw(scoresText[i]);
	}
	window.draw(clearScoresText);
}

void menu::setNext() {
	next = 1;
	selectedItem = 1;
	menuText[1].setString("easy <<");
	menuText[2].setString("hard");
}

void menu::clearScores() {
	FILE *fp;
	fp = fopen("best.dat", "wb");
	addScores(fp);
}

class gui {
private:
	sf::Font font;
	sf::Text text;
public:
	gui();
	void drawUserStatus(sf::RenderWindow &window, int id, char *name, int points);
	void drawTime(sf::RenderWindow &window, int time);
};
gui::gui() {
	font.loadFromFile("consola.ttf");
	text.setFont(font);
	text.setCharacterSize(20);
}
void gui::drawUserStatus(sf::RenderWindow &window, int id, char *name, int points) {
	text.setString(name + std::string("_") + std::to_string(points));
	if (id) {
		text.setPosition(sf::Vector2f(5, 30));
	}
	else {
		text.setPosition(sf::Vector2f(5, 5));
	}
	window.draw(text);
}
void gui::drawTime(sf::RenderWindow &window, int time) {
	text.setString(std::string("time_") + std::to_string(time));
	text.setPosition(sf::Vector2f(5, 5));
	window.draw(text);
}

class info {
private:
	sf::Font font;
	sf::Text text;
public:
	info();
	void draw(sf::RenderWindow &window, std::string text1In = "", std::string text2In = "", std::string text3In = "");
};

info::info() {
	font.loadFromFile("consola.ttf");
	text.setFont(font);
	text.setCharacterSize(40);
	text.setFillColor(sf::Color::White);
}

void info::draw(sf::RenderWindow &window, std::string text1In, std::string text2In, std::string text3In) {
	text.setPosition(40, HEIGHT - 180);
	text.setString(text1In);
	window.draw(text);
	text.setPosition(40, HEIGHT - 130);
	text.setString(text2In);
	window.draw(text);
	text.setPosition(40, HEIGHT - 80);
	text.setString(text3In);
	window.draw(text);
}

class backgroundAsteroid {
private:
	sf::CircleShape backgroundAsteroidShape;
public:
	backgroundAsteroid();
	void draw(sf::RenderWindow &window);
};
backgroundAsteroid::backgroundAsteroid() {
	int r = other::rand(5, 40);
	int alfa = other::rand(16, 128);
	backgroundAsteroidShape.setPointCount(5);
	backgroundAsteroidShape.setOutlineThickness(2.f);
	backgroundAsteroidShape.setOutlineColor(sf::Color(255, 255, 255, alfa));
	backgroundAsteroidShape.setFillColor(sf::Color::Transparent);
	backgroundAsteroidShape.setRadius(r);
	backgroundAsteroidShape.setOrigin(r, r);
	backgroundAsteroidShape.setRotation(other::rand(0, 360));
	backgroundAsteroidShape.setPosition(other::rand(0, WIDTH), other::rand(0, HEIGHT));

}
void backgroundAsteroid::draw(sf::RenderWindow &window) {
	window.draw(backgroundAsteroidShape);
}

class asteroid {
private:
	bool alive;
	float r;
	float deltaAlfa;
	int phase;
	bool difficulty;
	sf::Vector2f deltaPosition;
	sf::CircleShape asteroidShape;
	sf::CircleShape asteroidShadow;
public:
	asteroid();
	void init(float = -1, float = -1, int = 0, bool difficultyIn = other::DIFFICULTY);
	void draw(sf::RenderWindow &window);
	void move();
	bool isAlive() { return alive; };
	void setAlive(bool value) { alive = value; };
	sf::Vector2f getPosition() { return asteroidShape.getPosition(); };
	float getR() { return r; };
	int getPhase() { return phase; };
};

asteroid::asteroid() {
	alive = false;
	asteroidShape.setPointCount(5);
	asteroidShape.setOutlineThickness(4.f);
	asteroidShape.setOutlineColor(sf::Color::White);
	asteroidShape.setFillColor(sf::Color::Transparent);
	asteroidShadow.setFillColor(sf::Color(255, 0, 97, 128));
}

void asteroid::init(float xIn, float yIn, int phaseIn, bool difficultyIn) {
	difficulty = difficultyIn;
	phase = phaseIn;
	r = (float)80 - phase * 30;
	if (r == 80) {
		deltaPosition = sf::Vector2f((other::rand(0, 100) - 50) / (!difficulty ? 100 : 40), (other::rand(0, 100) - 50) / (!difficulty ? 100 : 40));
		deltaAlfa = (other::rand(0, 100) - 50) / 100;
	}
	else if (r == 50) {
		deltaPosition = sf::Vector2f((other::rand(0, 100) - 50) / (!difficulty ? 60 : 20), (other::rand(0, 100) - 50) / (!difficulty ? 60 : 20));
		deltaAlfa = (other::rand(0, 100) - 50) / 60;
	}
	else if (r == 20) {
		deltaPosition = sf::Vector2f((other::rand(0, 100) - 50) / (!difficulty ? 20 : 10), (other::rand(0, 100) - 50) / (!difficulty ? 20 : 10));
		deltaAlfa = (other::rand(0, 100) - 50) / 20;
	}
	asteroidShape.setRadius(r);
	asteroidShape.setOrigin(r, r);
	asteroidShadow.setRadius(r);
	asteroidShadow.setOrigin(r, r);
	if ((xIn == -1) && (yIn = -1)) {
		float x, y;
		if (other::rand(0, 1)) {
			x = other::rand(0, 400);
		}
		else {
			x = other::rand(WIDTH - 400, WIDTH);
		}
		if (other::rand(0, 1)) {
			y = other::rand(0, 250);
		}
		else {
			y = other::rand(HEIGHT - 250, HEIGHT);
		}
		asteroidShape.setPosition(x, y);
	}
	else {
		asteroidShape.setPosition(xIn, yIn);
	}
	asteroidShadow.setPosition(asteroidShape.getPosition());
	alive = true;
}

void asteroid::draw(sf::RenderWindow &window) {
	if (USE_SHADOW) {
		window.draw(asteroidShadow);
	}
	window.draw(asteroidShape);
}

void asteroid::move() {
	sf::Vector2f newPos = asteroidShape.getPosition() + deltaPosition;
	if (newPos.x >= WIDTH + r * 1.1) {
		newPos.x = -(r * 1.1);
	}
	else if (newPos.x <= -(r * 1.1)) {
		newPos.x = WIDTH + r * 1.1;
	}
	if (newPos.y >= HEIGHT + r * 1.1) {
		newPos.y = -(r * 1.1);
	}
	else if (newPos.y <= -(r * 1.1)) {
		newPos.y = HEIGHT + r * 1.1;
	}
	asteroidShape.setPosition(newPos);
	asteroidShadow.setPosition(asteroidShape.getPosition());
	asteroidShape.rotate(deltaAlfa);
}

class bullet {
private:
	int playerId;
	bool alive;
	float r;
	int aliveFor;
	sf::Vector2f deltaPosition;
	sf::CircleShape bulletShape;
public:
	bullet();
	void init(sf::Vector2f positionIn, sf::Vector2f deltaPositionIn, int playerIdIn);
	void draw(sf::RenderWindow &window);
	void move();
	bool isAlive() { return alive; };
	void setAlive(bool value) { alive = value; };
	int getPlayerId() { return playerId; };
	sf::Vector2f getPosition() { return bulletShape.getPosition(); };
};

bullet::bullet() {
	alive = false;
	aliveFor = 0;
	bulletShape.setRadius(4.f);
	bulletShape.setOrigin(sf::Vector2f(4, 4));
}

void bullet::init(sf::Vector2f positionIn, sf::Vector2f deltaPositionIn, int playerIdIn) {
	bulletShape.setPosition(positionIn);
	deltaPosition.x = deltaPositionIn.x;
	deltaPosition.y = deltaPositionIn.y;
	playerId = playerIdIn;
	aliveFor = 0;
	alive = true;
}

void bullet::draw(sf::RenderWindow &window) {
	window.draw(bulletShape);
}

void bullet::move() {
	if (alive) {
		if (bulletShape.getPosition().x > WIDTH || bulletShape.getPosition().x < 0) deltaPosition.x = -deltaPosition.x;
		if (bulletShape.getPosition().y > HEIGHT || bulletShape.getPosition().y < 0) deltaPosition.y = -deltaPosition.y;
		bulletShape.setPosition(bulletShape.getPosition() + deltaPosition);
		if (aliveFor > 40) {
			alive = false;
			aliveFor = 0;
		}
		else aliveFor++;
	}
}

class player {
private:
	int id;
	int points;
	bool alive;
	char *name;
	float r;
	float shapeAlfa;
	float maxPositionDelta;
	float acceleration;
	int nFire;
	sf::Vector2f deltaPosition;
	sf::ConvexShape playerShape;
	sf::CircleShape playerShadow;
	sf::Texture fireTexture;
	sf::IntRect fireShape;
	sf::Sprite fire;
	bool up;
	bool left;
	bool right;
public:
	player();
	void init(int idIn, float xIn = -1, float yIn = -1, float alfaIn = -1);
	void draw(sf::RenderWindow &window);
	void move();
	sf::Vector2f getBulletVelocity();
	void addPoints(int pointsIn) { points += pointsIn; };
	void setUp(bool value) { up = value; };
	void setLeft(bool value) { left = value; };
	void setRight(bool value) { right = value; };
	bool isAlive() { return alive; };
	sf::Vector2f getPosition() { return playerShape.getPosition(); };
	float getR() { return r; };
	char *getName() { return name; };
	int getPoints() { return points; };
	void setName(char *nameIn);
	void setPoints(int pointsIn);
	int getId() { return id; };
	float getAlfa() {
		return shapeAlfa;
	};
};

player::player() {
	alive = false;
	up = false;
	left = false;
	right = false;
	nFire = 0;
	name = new char[9];
	other::randName(name, 8);
	r = 25;
	shapeAlfa = 40;
	acceleration = 0.2f;
	maxPositionDelta = 10;
	playerShape.setPointCount(3);
	playerShape.setPoint(0, sf::Vector2f(0.f, -r));
	float a = r * sinf(shapeAlfa * (float)M_PI / 180);
	float b = r * cosf(shapeAlfa * (float)M_PI / 180);
	playerShape.setPoint(1, sf::Vector2f(a, b));
	playerShape.setPoint(2, sf::Vector2f(-a, b));
	playerShape.setOutlineThickness(4.f);
	playerShape.setOutlineColor(sf::Color::White);
	playerShape.setFillColor(sf::Color::Transparent);
	playerShadow.setRadius(r);
	playerShadow.setOrigin(r, r);
	playerShadow.setFillColor(sf::Color(0, 97, 255, 128));
	fireTexture.loadFromFile("fire.png");
	fire.setTexture(fireTexture);
	fireShape.left = 0;
	fireShape.top = 0;
	fireShape.width = 24;
	fireShape.height = 24;
	fire.setTextureRect(fireShape);
	fire.setScale(sf::Vector2f(3, 3));
	fire.setOrigin(12, 30);
}

void player::init(int idIn, float xIn, float yIn, float alfaIn) {
	id = idIn;
	points = 0;
	float x, y;
	if (xIn == -1 && yIn == -1) {
		x = other::rand(400, WIDTH - 400);
		y = other::rand(250, HEIGHT - 250);
	}
	else {
		x = xIn;
		y = yIn;
	}
	playerShape.setPosition(x, y);
	fire.setPosition(x, y);
	fire.setRotation(180 + playerShape.getRotation());
	playerShadow.setPosition(playerShape.getPosition());
	playerShape.setRotation((alfaIn < 0) ? other::rand(0, 360) : alfaIn);
	alive = true;
}

void player::draw(sf::RenderWindow &window) {
	if (USE_SHADOW) {
		window.draw(playerShadow);
	}
	if (up) {
		window.draw(fire);
	}
	window.draw(playerShape);
}

void player::move() {
	if (left && !right) {
		playerShape.rotate(-3.5);
	}
	else if (right && !left) {
		playerShape.rotate(3.5);
	}
	if (up) {
		deltaPosition.x = std::max(-maxPositionDelta, std::min(maxPositionDelta, deltaPosition.x + sinf(playerShape.getRotation() * (float)M_PI / 180) * acceleration));
		deltaPosition.y = std::max(-maxPositionDelta, std::min(maxPositionDelta, deltaPosition.y - cosf(playerShape.getRotation() * (float)M_PI / 180) * acceleration));
	}
	else {
		deltaPosition.x *= 0.98f;
		deltaPosition.x = (std::abs(deltaPosition.x) < 0.1) ? 0 : deltaPosition.x;
		deltaPosition.y *= 0.98f;
		deltaPosition.y = (std::abs(deltaPosition.y) < 0.1) ? 0 : deltaPosition.y;
	}
	sf::Vector2f newPos = playerShape.getPosition() + deltaPosition;
	if (newPos.x > WIDTH + 1.5f * r) {
		newPos.x = -(1.5f * r);
	}
	else if (newPos.x < -(1.5f * r)) {
		newPos.x = WIDTH + 1.5f * r;
	}
	if (newPos.y > HEIGHT + 1.5f * r) {
		newPos.y = -(1.5f * r);
	}
	else if (newPos.y < -(1.5f * r)) {
		newPos.y = HEIGHT + (1.5f * r);
	}
	playerShape.setPosition(newPos);
	fire.setPosition(newPos);
	fire.setRotation(180 + playerShape.getRotation());
	playerShadow.setPosition(playerShape.getPosition());
	nFire++;
	if (nFire >= 10) {
		fireShape.left += 24;
		if (fireShape.left >= 168) fireShape.left = 0;
		fire.setTextureRect(fireShape);
		nFire = 0;
	}
}

sf::Vector2f player::getBulletVelocity() {
	float vX = sinf(playerShape.getRotation() * (float)M_PI / 180);
	float vY = -cosf(playerShape.getRotation() * (float)M_PI / 180);
	return sf::Vector2f(vX, vY);
}

void player::setName(char *nameIn) {
	strcpy(name, nameIn);
}

void player::setPoints(int pointsIn) {
	points = pointsIn;
}

class game {
private:
	asteroid *asteroidsArray;
	bullet *bulletsArray;
	player *playersArray;
	playerInfo *playersInfoArray;
	backgroundAsteroid *backgroundAsteroidsArray;
	gui gameGui;
	info gameInfo;
	int nGui;
	bool stateGui;
	bool pause;
	bool tmpPauseKey;
	int time;
	int countFrames;
	bool whichPause;
	bool gameOver;
public:
	game();
	void init();
	void draw(sf::RenderWindow &window);
	void move();
	void detectCollisions(sf::RenderWindow &window);
	void setKey(int key, int playerNumber, bool value);
	void setPause(bool which);
	bool getPause() { return pause; };
	void savePlayerInfo();
	void saveGameStatus();
	void loadGame();
	bool getGameOver() { return gameOver; };
};

game::game() {
	playersArray = new player[2];
	playersInfoArray = new playerInfo[2];
	bulletsArray = new bullet[MAX_BULLETS];
	asteroidsArray = new asteroid[MAX_ASTEROIDS];
	backgroundAsteroidsArray = new backgroundAsteroid[MAX_BACKGROUND_ASTEROIDS];
}

void game::init() {
	for (int i = 0; i < other::MAX_PLAYERS; i++) {
		playersArray[i].init(i);
	}
	for (int i = 0; i < (other::DIFFICULTY ? 4 : 2); i++) {
		for (int j = 0; j < MAX_ASTEROIDS; j++)
		{
			if (!asteroidsArray[j].isAlive()) {
				asteroidsArray[j].init();
				break;
			}
		}
	}
	nGui = 0;
	pause = 0;
	time = 0;
	countFrames = 0;
	tmpPauseKey = 1;
}

void game::draw(sf::RenderWindow &window) {
	for (int i = 0; i < MAX_BACKGROUND_ASTEROIDS; i++) {
		backgroundAsteroidsArray[i].draw(window);
	}
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (asteroidsArray[i].isAlive()) {
			asteroidsArray[i].draw(window);
		}
	}
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bulletsArray[i].isAlive()) {
			bulletsArray[i].draw(window);
		}
	}
	for (int i = 0; i < other::MAX_PLAYERS; i++) {
		if (playersArray[i].isAlive()) {
			playersArray[i].draw(window);
		}
	}
	if (!stateGui) {
		for (int i = 0; i < other::MAX_PLAYERS; i++) {
			gameGui.drawUserStatus(window, i, playersArray[i].getName(), playersArray[i].getPoints());
		}
	}
	else {
		gameGui.drawTime(window, time);
	}
	if (pause) {
		if (whichPause) {
			gameInfo.draw(window, "pause_", "quit?_(ENTER_for_yes)", "quit_and_save?_(S_for_yes)");
		}
		else {
			gameInfo.draw(window, "pause_", "first_player:_WSAD_keys_and_SPACE", "second_player:_arrow_keys_and_ENTER");
		}
	}
}

void game::move() {
	if (!pause) {
		gameOver = true;
		for (int i = 0; i < MAX_ASTEROIDS; i++) {
			if (asteroidsArray[i].isAlive()) {
				gameOver = false;
				asteroidsArray[i].move();
			}
			else if (gameOver != false && i == MAX_ASTEROIDS - 1) {
				for (int i = 0; i < other::MAX_PLAYERS; i++) {
					strcpy(playersInfoArray[i].name, playersArray[i].getName());
					playersArray[i].addPoints(-1000);
					playersInfoArray[i].points = playersArray[i].getPoints();
				}
				savePlayerInfo();
				gameOver = true;
			}
		}
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bulletsArray[i].isAlive()) {
				bulletsArray[i].move();
			}
		}
		for (int i = 0; i < other::MAX_PLAYERS; i++) {
			if (playersArray[i].isAlive()) {
				playersArray[i].move();
			}
		}
		countFrames++;
		if (countFrames >= 50) {
			countFrames = 0;
			time++;
			for (int i = 0; i < other::MAX_PLAYERS; i++) {
				if (playersArray[i].isAlive()) {
					playersArray[i].addPoints(-50);
				}
			}
		}
		if (nGui >= 50 * 3) {
			nGui = 0;
			stateGui = !stateGui;
		};
		nGui++;
	}
}

void game::detectCollisions(sf::RenderWindow &window) {
	for (int i = 0; i < other::MAX_PLAYERS; i++) {
		if (playersArray[i].isAlive()) {
			for (int j = 0; j < MAX_ASTEROIDS; j++) {
				if (asteroidsArray[j].isAlive()) {
					sf::Vector2f asteroidPosition = asteroidsArray[j].getPosition();
					sf::Vector2f playerPosition = playersArray[i].getPosition();
					float a = asteroidPosition.x - playerPosition.x;
					float b = asteroidPosition.y - playerPosition.y;
					float length = sqrtf(powf(a, 2) + powf(b, 2));
					float asteroidR = asteroidsArray[j].getR();
					float playerR = playersArray[i].getR();
					if (length <= (playerR + asteroidR)) {
						for (int i = 0; i < other::MAX_PLAYERS; i++) {
							strcpy(playersInfoArray[i].name, playersArray[i].getName());
							playersArray[i].addPoints(-1000);
							playersInfoArray[i].points = playersArray[i].getPoints();
						}
						savePlayerInfo();
						gameOver = true;
						break;
					}
				}
			}
		}
	}
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bulletsArray[i].isAlive()) {
			for (int j = 0; j < MAX_ASTEROIDS; j++) {
				if (asteroidsArray[j].isAlive()) {
					sf::Vector2f asteroidPosition = asteroidsArray[j].getPosition();
					sf::Vector2f bulletPosition = bulletsArray[i].getPosition();
					float a = asteroidPosition.x - bulletPosition.x;
					float b = asteroidPosition.y - bulletPosition.y;
					float length = sqrtf(powf(a, 2) + powf(b, 2));
					float asteroidR = asteroidsArray[j].getR();
					if (length <= asteroidR) {
						if (asteroidsArray[j].getPhase() < 2) {
							int k = 0;
							for (int l = 0; l < MAX_ASTEROIDS; l++) {
								if (!asteroidsArray[l].isAlive()) {
									asteroidsArray[l].init(asteroidsArray[j].getPosition().x, asteroidsArray[j].getPosition().y, asteroidsArray[j].getPhase() + 1);
									k++;
									if (k >= 4) break;
								}
							}
						}
						if (asteroidsArray[j].getPhase() == 0) playersArray[bulletsArray[i].getPlayerId()].addPoints(50);
						else if (asteroidsArray[j].getPhase() == 1) playersArray[bulletsArray[i].getPlayerId()].addPoints(70);
						else if (asteroidsArray[j].getPhase() == 2) playersArray[bulletsArray[i].getPlayerId()].addPoints(100);
						asteroidsArray[j].setAlive(false);
						bulletsArray[i].setAlive(false);
					}
				}
			}
		}
	}
}

void game::setKey(int key, int playerNumber, bool value) {
	if (!pause) {
		if (playerNumber == 0) {
			switch (key) {
			case sf::Keyboard::Space:
				if (value) {
					for (int i = 0; i < MAX_BULLETS; i++) {
						if (!bulletsArray[i].isAlive()) {
							sf::Vector2f bulletVelocity = playersArray[0].getBulletVelocity();
							bulletsArray[i].init(playersArray[0].getPosition() + sf::Vector2f(bulletVelocity.x * 30, bulletVelocity.y * 30), sf::Vector2f(bulletVelocity.x * 10, bulletVelocity.y * 10), 0);
							break;
						}
					}
				}
				break;
			case sf::Keyboard::W:
				playersArray[0].setUp(value);
				break;
			case sf::Keyboard::A:
				playersArray[0].setLeft(value);
				break;
			case sf::Keyboard::D:
				playersArray[0].setRight(value);
				break;
			default:
				break;
			}
		}
		else if (playerNumber == 1) {
			switch (key) {
			case sf::Keyboard::Enter:
				if (value) {
					for (int i = 0; i < MAX_BULLETS; i++) {
						if (!bulletsArray[i].isAlive()) {
							sf::Vector2f bulletVelocity = playersArray[1].getBulletVelocity();
							bulletsArray[i].init(playersArray[1].getPosition() + sf::Vector2f(bulletVelocity.x * 30, bulletVelocity.y * 30), sf::Vector2f(bulletVelocity.x * 10, bulletVelocity.y * 10), 1);
							break;
						}
					}
				}
				break;
			case sf::Keyboard::Up:
				playersArray[1].setUp(value);
				break;
			case sf::Keyboard::Left:
				playersArray[1].setLeft(value);
				break;
			case sf::Keyboard::Right:
				playersArray[1].setRight(value);
				break;
			default:
				break;
			}
		}
	}
}

void game::setPause(bool which) {
	whichPause = which;
	pause = !pause;
}

void game::savePlayerInfo() {
	FILE *fp;
	fp = fopen("best.dat", "a+b");
	for (int i = 0; i < other::MAX_PLAYERS; i++) {
		fwrite(&playersInfoArray[i], sizeof(playerInfo), 1, fp);
	}
	fclose(fp);
}

typedef struct {
	struct playersStruct {
		playerInfo info;
		sf::Vector2f position;
		int id;
		float alfa;
	};
	struct asteroidsStruct {
		sf::Vector2f position;
		int phase;
		bool isAlive = false;
	};
	int time;
	int framesAfterTime;
	int maxPlayers;
	int difficulty;
	playersStruct players[2];
	asteroidsStruct asteroids[64];
} gameStatusStruct;

void game::saveGameStatus() {
	gameStatusStruct gameStatus;
	gameStatus.maxPlayers = other::MAX_PLAYERS;
	gameStatus.difficulty = other::DIFFICULTY;
	gameStatus.time = time;
	gameStatus.framesAfterTime = countFrames;
	for (int i = 0; i < other::MAX_PLAYERS; i++) {
		strcpy(gameStatus.players[i].info.name, playersArray[i].getName());
		gameStatus.players[i].info.points = playersArray[i].getPoints();
		gameStatus.players[i].position = playersArray[i].getPosition();
		gameStatus.players[i].id = playersArray[i].getId();
		gameStatus.players[i].alfa = playersArray[i].getAlfa();
	}
	int j = 0;
	for (int i = 0; i < 64; i++) {
		if (asteroidsArray[i].isAlive()) {
			gameStatus.asteroids[j].position = asteroidsArray[i].getPosition();
			gameStatus.asteroids[j].phase = asteroidsArray[i].getPhase();
			gameStatus.asteroids[j].isAlive = true;
			j++;
		}
	}
	FILE *fp;
	fp = fopen("save.dat", "w+b");
	if (fp != NULL) {
		fwrite(&gameStatus, sizeof(gameStatusStruct), 1, fp);
		fclose(fp);
	}
}

void game::loadGame() {
	gameStatusStruct gameStatus;
	FILE *fp;
	fp = fopen("save.dat", "rb");
	if (fp != NULL) {
		fread(&gameStatus, sizeof(gameStatusStruct), 1, fp);
		fclose(fp);
		std::remove("save.dat");
		time = gameStatus.time;
		countFrames = gameStatus.framesAfterTime;
		other::MAX_PLAYERS = gameStatus.maxPlayers;
		other::DIFFICULTY = gameStatus.difficulty;
		for (int i = 0; i < other::MAX_PLAYERS; i++) {
			playersArray[i].init(gameStatus.players[i].id, gameStatus.players[i].position.x, gameStatus.players[i].position.y, gameStatus.players[i].alfa);
			playersArray[i].setName(gameStatus.players[i].info.name);
			playersArray[i].setPoints(gameStatus.players[i].info.points);
		}
		int j = 0;
		for (int i = 0; i < 64; i++) {
			if (!asteroidsArray[i].isAlive()) {
				if (gameStatus.asteroids[j].isAlive) {
					asteroidsArray[i].init(gameStatus.asteroids[j].position.x, gameStatus.asteroids[j].position.y, gameStatus.asteroids[j].phase, other::DIFFICULTY);
					j++;
				}
			}
		}
		nGui = 0;
		pause = 0;
		tmpPauseKey = 1;
	}
}

int main() {
	std::cout << "crazy_asteroids_author:_Karol_Konopka\n";
	srand((unsigned int)time(NULL));
	sf::Clock clock;
	sf::Time time;
	int activity = 0;
	menu menu;
	game game;
	endScreen end;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "crazy_asteroids", sf::Style::Default, settings);
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (activity == 0) {
					if (event.key.code == sf::Keyboard::W) {
						menu.moveUp();
					}
					else if (event.key.code == sf::Keyboard::S) {
						menu.moveDown();
					}
					else if (event.key.code == sf::Keyboard::Space && menu.getSelectedItem() == 1) {
						if (menu.getNext()) {
							other::DIFFICULTY = 0;
							activity = 1;
							game.init();
						}
						else {
							other::MAX_PLAYERS = 1;
							menu.setNext();
						}
					}
					else if (event.key.code == sf::Keyboard::Space && menu.getSelectedItem() == 2) {
						if (menu.getNext()) {
							other::DIFFICULTY = 1;
							activity = 1;
							game.init();
						}
						else {
							other::MAX_PLAYERS = 2;
							menu.setNext();
						}
					}
					else if (event.key.code == sf::Keyboard::Space && menu.getSelectedItem() == 3) {
						activity = 1;
						game.loadGame();
					}
					else if (event.key.code == sf::Keyboard::Space && menu.getSelectedItem() == 4) {
						window.close();
						return 0;
					}
					else if (event.key.code == sf::Keyboard::C) {
						menu.clearScores();
					}
				}
				else if (activity == 1) {
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Space) {
						game.setKey(event.key.code, 0, true);
					}
					else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Enter) {
						game.setKey(event.key.code, 1, true);
						if (event.key.code == sf::Keyboard::Enter && game.getPause()) {
							window.close();
						}
					}
					else if (event.key.code == sf::Keyboard::F1) {
						game.setPause(0);
					}
					else if (event.key.code == sf::Keyboard::Escape) {
						game.setPause(1);
					}
					else if (event.key.code == sf::Keyboard::S && game.getPause()) {
						game.saveGameStatus();
						window.close();
						return 0;
					}
				}
			}
			if (event.type == sf::Event::KeyReleased) {
				if (activity == 1) {
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Space) {
						game.setKey(event.key.code, 0, false);
					}
					else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Enter) {
						game.setKey(event.key.code, 1, false);
					}
				}
			}
			if (event.type == sf::Event::Resized) {
				sf::Vector2u size(WIDTH, HEIGHT);
				window.setSize(size);
			}
		}
		time = clock.getElapsedTime();
		if (time.asMilliseconds() > 20) {
			if (activity == 1) {
				game.move();
				game.detectCollisions(window);
				if (game.getGameOver()) activity = 2;
			}
			window.clear();
			if (activity == 0) {
				menu.draw(window);
			}
			else if (activity == 1) {
				game.draw(window);
			}
			else if (activity == 2) {
				end.draw(window);
			}
			window.display();
			clock.restart();
		}
	}
	return 0;
}
