#include "graphics.h"


#ifdef _WIN32
void DrawInTerminal(CPU_ARG_INT_T* memory, size_t start_ind_video_mem) {
    printf("\n");

    for (int h_count = 0; h_count < HEIGHT_WINDOW; ++h_count) {
        for (int w_count = 0; w_count < WIDTH_WINDOW; ++w_count) {
            if (memory[start_ind_video_mem + h_count * WIDTH_WINDOW + w_count] != 0) {
                printf("*");
            }
            else {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

#elif __linux__

void DrawInTerminal(CPU_ARG_INT_T* memory, size_t start_ind_video_mem) {
    sf::RenderWindow window(sf::VideoMode(HEIGHT_WINDOW, WIDTH_WINDOW), "CPU GRAPHICS");
    
    sf::Texture bufferTexture;
    sf::Sprite  buffferSprite;
    sf::Image   img;
    img.create(WIDTH_WINDOW, HEIGHT_WINDOW, sf::Color::White);

    for (int w_count = 0; w_count < WIDTH_WINDOW; ++w_count) {
        for (int h_count = 0; h_count < HEIGHT_WINDOW; ++h_count) {
            if(memory[start_ind_video_mem + w_count * WIDTH_WINDOW + h_count] != 0) {
                img.setPixel(w_count, h_count, sf::Color::Green);
            } else {
                img.setPixel(w_count, h_count, sf::Color::Red);
            }
        }
    }    

    bufferTexture.loadFromImage(img);
    buffferSprite.setTexture(bufferTexture);
    
    sf::Event event;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
			if (event.type == sf::Event::KeyPressed) {
				window.close();
                return;
			}
		}


        window.clear();    
        window.draw(buffferSprite);
        window.display();
    }

    return;
}

#endif