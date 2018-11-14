#define WIDTH 1280
#define HEIGHT 720
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
	std::cout << "Crazy Asteroids, author: Karol Konopka\n";
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT),"Crazy Asteroids!");
	while(window.isOpen()){
		sf::Event event;while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed) window.close();
			if(event.type == sf::Event::KeyPressed) printf("Button pressed\n");
			if(event.type == sf::Event::Resized){
				sf::Vector2u size(WIDTH, HEIGHT);
				window.setSize(size);
			}
		}
		window.clear();

		window.display();
	}
	return 0;
}