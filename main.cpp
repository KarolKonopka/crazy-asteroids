#define _USE_MATH_DEFINES
#define WIDTH 1280
#define HEIGHT 720
#define MAX_BULLETS 16
#define MAX_ASTEROIDS 257
#define MAX_BACKGROUND_ASTEROIDS 64
#define MAX_MENU_LEVELS 2
#define USE_SHADOW 1
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>

namespace other {
	int MAX_PLAYERS = 2;
	bool DIFFICULTY = 1;
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
}

class menu
{
private:
	sf::Font font;
	sf::Text menuText[MAX_MENU_LEVELS];
	int selectedItem = 0;
public:
	menu();
	void moveUp();
	void moveDown();
	int getSelectedItem() { return selectedItem; }
	void draw(sf::RenderWindow &window);
};
menu::menu()
{
	font.loadFromFile("consola.ttf");
	menuText[0].setFont(font);
	menuText[0].setCharacterSize(40);
	menuText[0].setString("new_game <<");
	menuText[0].setPosition(sf::Vector2f(40, HEIGHT - 40 - (MAX_MENU_LEVELS - 0) * 60));
	menuText[1].setFont(font);
	menuText[1].setCharacterSize(40);
	menuText[1].setString("quit");
	menuText[1].setPosition(sf::Vector2f(40, HEIGHT - 40 - (MAX_MENU_LEVELS - 1) * 60));
}
void menu::moveUp()
{
	if (selectedItem == 0) menuText[selectedItem].setString("new_game");
	else if (selectedItem == 1) menuText[selectedItem].setString("quit");
	selectedItem--;
	if (selectedItem < 0) selectedItem = MAX_MENU_LEVELS - 1;
	if (selectedItem == 0) menuText[selectedItem].setString("new_game <<");
	else if (selectedItem == 1) menuText[selectedItem].setString("quit <<");
}
void menu::moveDown()
{
	if (selectedItem == 0) menuText[selectedItem].setString("new_game");
	else if (selectedItem == 1) menuText[selectedItem].setString("quit");
	selectedItem++;
	if (selectedItem >= MAX_MENU_LEVELS) selectedItem = 0;
	if (selectedItem == 0) menuText[selectedItem].setString("new_game <<");
	else if (selectedItem == 1) menuText[selectedItem].setString("quit <<");
}
void menu::draw(sf::RenderWindow &window)
{
	for (int i = 0; i < MAX_MENU_LEVELS; i++)
	{
		window.draw(menuText[i]);
	}
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

class asteroid
{
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
asteroid::asteroid()
{
	alive = false;
	asteroidShape.setPointCount(5);
	asteroidShape.setOutlineThickness(4.f);
	asteroidShape.setOutlineColor(sf::Color::White);
	asteroidShape.setFillColor(sf::Color::Transparent);
	asteroidShadow.setFillColor(sf::Color(255, 0, 97, 128));
}
void asteroid::init(float xIn, float yIn, int phaseIn, bool difficultyIn)
{
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
		asteroidShape.setPosition(other::rand(0, WIDTH), other::rand(0, HEIGHT));
	}
	else {
		asteroidShape.setPosition(xIn, yIn);
	}
	asteroidShadow.setPosition(asteroidShape.getPosition());
	alive = true;
}
void asteroid::draw(sf::RenderWindow &window)
{
	if (USE_SHADOW) {
		window.draw(asteroidShadow);
	}
	window.draw(asteroidShape);
}
void asteroid::move()
{
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

class bullet
{
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
void bullet::draw(sf::RenderWindow &window)
{
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

class player
{
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
	void init(int idIn);
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
};
player::player()
{
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
void player::init(int idIn)
{
	id = idIn;
	points = 0;
	float xIn = other::rand(0, WIDTH);
	float yIn = other::rand(0, HEIGHT);
	playerShape.setPosition(xIn, yIn);
	fire.setPosition(xIn, yIn);
	fire.setRotation(180 + playerShape.getRotation());
	playerShadow.setPosition(playerShape.getPosition());
	playerShape.setRotation(other::rand(0, 360));
	alive = true;
}
void player::draw(sf::RenderWindow &window)
{
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
		playerShape.rotate(-2.5);
	}
	else if (right && !left) {
		playerShape.rotate(2.5);
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

class game
{
private:
	asteroid *asteroidsArray;
	bullet *bulletsArray;
	player *playersArray;
	backgroundAsteroid *backgroundAsteroidsArray;
	gui gameGui;
	info gameInfo;
	int nGui;
	bool stateGui;
	bool pause;
	bool tmpPauseKey;
	int time;
	int countTime;
	bool whichPause;
public:
	game();
	void init();
	void draw(sf::RenderWindow &window);
	void move();
	void detectCollisions();
	void setKey(int key, int playerNumber, bool value);
	void setPause(bool which, bool val);
	bool getPause() { return pause; };
};
game::game()
{
	playersArray = new player[other::MAX_PLAYERS];
	bulletsArray = new bullet[MAX_BULLETS];
	asteroidsArray = new asteroid[MAX_ASTEROIDS];
	backgroundAsteroidsArray = new backgroundAsteroid[MAX_BACKGROUND_ASTEROIDS];
}
void game::init()
{
	for (int i = 0; i < other::MAX_PLAYERS; i++) {
		playersArray[i].init(i);
	}
	for (int i = 0; i < (other::DIFFICULTY ? 8 : 4); i++) {
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
	countTime = 0;
	tmpPauseKey = 1;
}
void game::draw(sf::RenderWindow &window)
{
	for (int i = 0; i < MAX_BACKGROUND_ASTEROIDS; i++)
	{
		backgroundAsteroidsArray[i].draw(window);
	}
	for (int i = 0; i < MAX_ASTEROIDS; i++)
	{
		if (asteroidsArray[i].isAlive()) {
			asteroidsArray[i].draw(window);
		}
	}

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (bulletsArray[i].isAlive()) {
			bulletsArray[i].draw(window);
		}
	}

	for (int i = 0; i < other::MAX_PLAYERS; i++)
	{
		if (playersArray[i].isAlive()) {
			playersArray[i].draw(window);
		}
	}
	if (stateGui) {
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
void game::move()
{
	if (!pause) {
		for (int i = 0; i < MAX_ASTEROIDS; i++)
		{
			if (asteroidsArray[i].isAlive()) {
				asteroidsArray[i].move();
			}
		}
		for (int i = 0; i < MAX_BULLETS; i++)
		{
			if (bulletsArray[i].isAlive()) {
				bulletsArray[i].move();
			}
		}
		for (int i = 0; i < other::MAX_PLAYERS; i++)
		{
			if (playersArray[i].isAlive()) {
				playersArray[i].move();
			}
		}
		countTime++;
		if (countTime >= 50) {
			countTime = 0;
			time++;
		}
		if (nGui >= 50 * 3) {
			nGui = 0;
			stateGui = !stateGui;
		};
		nGui++;
	}
}
void game::detectCollisions()
{
	for (int i = 0; i < other::MAX_PLAYERS; i++)
	{
		if (playersArray[i].isAlive()) {
			for (int j = 0; j < MAX_ASTEROIDS; j++)
			{
				if (asteroidsArray[j].isAlive()) {
					sf::Vector2f asteroidPosition = asteroidsArray[j].getPosition();
					sf::Vector2f playerPosition = playersArray[i].getPosition();
					float a = asteroidPosition.x - playerPosition.x;
					float b = asteroidPosition.y - playerPosition.y;
					float length = sqrtf(powf(a, 2) + powf(b, 2));
					float asteroidR = asteroidsArray[j].getR();
					float playerR = playersArray[i].getR();
					if (length <= (playerR + asteroidR)) {
						//kolizja
					}
				}
			}
		}
	}
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (bulletsArray[i].isAlive()) {
			for (int j = 0; j < MAX_ASTEROIDS; j++)
			{
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
void game::setPause(bool which, bool val) {
	whichPause = which;
	if (tmpPauseKey == 1 && val == 1) {
		pause = val;
		tmpPauseKey = 0;
	}
	else if (tmpPauseKey == 1 && val == 0) {
		pause = val;
		tmpPauseKey = 1;
	}
	else if (tmpPauseKey == 0 && val == 1) {
		pause = !val;
		tmpPauseKey = 1;
	}
	else if (tmpPauseKey == 0 && val == 0) {
		pause = !val;
		tmpPauseKey = 0;
	}
}
int main()
{
	std::cout << "crazy_asteroids_author:_Karol_Konopka\n";
	srand((unsigned int)time(NULL));

	sf::Clock clock;
	sf::Time time;

	int activity = 0;
	menu menu;
	game game;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "crazy_asteroids", sf::Style::Default, settings);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (activity == 0)
				{
					if (event.key.code == sf::Keyboard::W) {
						menu.moveUp();
					}
					else if (event.key.code == sf::Keyboard::S) {
						menu.moveDown();
					}
					else if (event.key.code == sf::Keyboard::Space && menu.getSelectedItem() == 0) {
						activity = 1;
						game.init();
					}
					else if (event.key.code == sf::Keyboard::Space && menu.getSelectedItem() == 1) {
						window.close();
						return 0;
					}
				}
				else if (activity == 1) {
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Space) {
						game.setKey(event.key.code, 0, true);
					}
					else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Enter) {
						game.setKey(event.key.code, 1, true);
						if (game.getPause()) {
							window.close();
						}
					}
					else if (event.key.code == sf::Keyboard::F1) {
						game.setPause(0, 1);
					}
					else if (event.key.code == sf::Keyboard::Escape) {
						game.setPause(1, 1);
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
					else if (event.key.code == sf::Keyboard::F1) {
						game.setPause(0, 0);
					}
					else if (event.key.code == sf::Keyboard::Escape) {
						game.setPause(1, 0);
					}
				}
			}
			if (event.type == sf::Event::Resized)
			{
				sf::Vector2u size(WIDTH, HEIGHT);
				window.setSize(size);
			}
		}
		time = clock.getElapsedTime();
		if (time.asMilliseconds() > 20) {
			if (activity == 1)
			{
				game.move();
				game.detectCollisions();
			}
			window.clear();
			if (activity == 0) {
				menu.draw(window);
			}
			else if (activity == 1) {
				game.draw(window);
			}
			window.display();
			clock.restart();
		}
	}
	return 0;
}