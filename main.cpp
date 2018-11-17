#define _USE_MATH_DEFINES
#define WIDTH 1280
#define HEIGHT 720
#define MAX_BULLETS 16
#define MAX_ASTEROIDS 32
#define MAX_PLAYERS 3
#define MAX_MENU_LEVELS 2
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>

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

namespace others {
	float rand(int min, int max) {
		return (float)(min + std::rand() % (max - min + 1));
	};
}

class asteroid
{
private:
	bool alive;
	float r;
	float deltaAlfa;
	int phase;
	sf::Vector2f deltaPosition;
	sf::CircleShape asteroidShape;
	sf::CircleShape asteroidShadow;
public:
	asteroid();
	void init(float = -1, float = -1, int = 0);
	void draw(sf::RenderWindow &window);
	void move();
	bool isAlive() { return alive; };
	sf::Vector2f getPosition() { return asteroidShape.getPosition(); };
	float getR() { return r; };
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
void asteroid::init(float xIn, float yIn, int phaseIn)
{
	phase = phaseIn;
	r = (float)80 - phase * 30;
	if (r == 80) {
		deltaPosition = sf::Vector2f((others::rand(0, 100) - 50) / 100, (others::rand(0, 100) - 50) / 100);
		deltaAlfa = (others::rand(0, 100) - 50) / 100;
	}
	else if (r == 50) {
		deltaPosition = sf::Vector2f((others::rand(0, 100) - 50) / 60, (others::rand(0, 100) - 50) / 80);
		deltaAlfa = (others::rand(0, 100) - 50) / 60;
	}
	else if (r == 20) {
		deltaPosition = sf::Vector2f((others::rand(0, 100) - 50) / 40, (others::rand(0, 100) - 50) / 60);
		deltaAlfa = (others::rand(0, 100) - 50) / 20;
	}
	asteroidShape.setRadius(r);
	asteroidShape.setOrigin(r, r);
	asteroidShadow.setRadius(r);
	asteroidShadow.setOrigin(r, r);
	if ((xIn == -1) && (yIn = -1)) {
		asteroidShape.setPosition(others::rand(0, WIDTH), others::rand(0, HEIGHT));
	}
	else {
		asteroidShape.setPosition(xIn, yIn);
	}
	asteroidShadow.setPosition(asteroidShape.getPosition());
	alive = true;
}
void asteroid::draw(sf::RenderWindow &window)
{
	window.draw(asteroidShadow);
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
	int id;
	bool alive;
	float r;
	int aliveFor;
	sf::Vector2f deltaPositnion;
	sf::CircleShape bulletShape;
public:
	bullet();
	void move();
};

class player
{
private:
	int id;
	bool alive;
	float r;
	float shapeAlfa;
	float maxPositionDelta;
	float acceleration;
	sf::Vector2f deltaPosition;
	sf::ConvexShape playerShape;
	sf::CircleShape playerShadow;
	bool up;
	bool left;
	bool right;
public:
	player();
	void init(int idIn);
	void draw(sf::RenderWindow &window);
	void move();
	bool isAlive() { return alive; };
	sf::Vector2f getPosition() { return playerShape.getPosition(); };
	float getR() { return r; };
};
player::player()
{
	alive = false;
	up = false;
	left = false;
	right = false;
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
}
void player::init(int idIn)
{
	id = idIn;
	float xIn = others::rand(0, WIDTH);
	float yIn = others::rand(0, HEIGHT);
	playerShape.setPosition(xIn, yIn);
	playerShadow.setPosition(playerShape.getPosition());
	playerShape.setRotation(others::rand(0, 360));
	alive = true;
}
void player::draw(sf::RenderWindow &window)
{
	window.draw(playerShadow);
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
	playerShadow.setPosition(playerShape.getPosition());
}

class game
{
private:
	asteroid *asteroidsArray;
	player *playersArray;
public:
	game();
	void init();
	void draw(sf::RenderWindow &window);
	void move();
	void detectCollisions();
};
game::game()
{
	playersArray = new player[MAX_PLAYERS];
	asteroidsArray = new asteroid[MAX_ASTEROIDS];

}
void game::init()
{
	playersArray[0].init(0);
	playersArray[1].init(1);
	playersArray[2].init(2);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < MAX_ASTEROIDS; j++)
		{
			if (!asteroidsArray[j].isAlive()) {
				asteroidsArray[j].init();
				break;
			}
		}
	}
}
void game::draw(sf::RenderWindow &window)
{
	for (int i = 0; i < MAX_ASTEROIDS; i++)
	{
		if (asteroidsArray[i].isAlive()) {
			asteroidsArray[i].draw(window);
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (playersArray[i].isAlive()) {
			playersArray[i].draw(window);
		}
	}
}
void game::move()
{
	for (int i = 0; i < MAX_ASTEROIDS; i++)
	{
		if (asteroidsArray[i].isAlive()) {
			asteroidsArray[i].move();
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (playersArray[i].isAlive()) {
			playersArray[i].move();
		}
	}
}
void game::detectCollisions()
{
	for (int i = 0; i < MAX_PLAYERS; i++)
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
						// collision
					}
				}
			}
		}
	}
}

int main() 
{
	std::cout << "Crazy Asteroids, author: Karol Konopka\n";
	srand((unsigned int)time(NULL));
	
	sf::Clock clock;
	sf::Time time;

	int activity = 0;
	menu menu;
	game game;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Crazy Asteroids!", sf::Style::Default, settings);
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

				}
			}
			if (event.type == sf::Event::KeyReleased) {
				if (activity == 1) {

				}
			}
			if (event.type == sf::Event::Resized)
			{
				sf::Vector2u size(WIDTH, HEIGHT);
				window.setSize(size);
			}
		}
		time = clock.getElapsedTime();
		if (time.asMilliseconds() > 16) {
			if (activity == 1)
			{
				game.move();
				game.detectCollisions();
			}
			clock.restart();
		}
		window.clear();
		if (activity == 0) {
			menu.draw(window);
		}
		else if (activity == 1) {
			game.draw(window);
		}
		window.display();
	}
	return 0;
}