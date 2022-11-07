#ifndef _DEBUG          // WinMain is used as the entry point in release mode because
#define main WinMain    // the "WINDOWS" subsystem is used instead of the "CONSOLE"
#endif                  // subsystem (for some reason defining the entry point didn't work)

#include <SFML/Graphics.hpp>
#include <cstdlib>

constexpr auto canvasWidth = 40;
constexpr auto canvasHeight = 40;
constexpr auto squareSize = 16;
constexpr auto framerateLimit = 10;

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

int main(int argc, char** argv)
{
    // Setup
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(canvasWidth * squareSize), static_cast<unsigned int>(canvasHeight * squareSize)), "Snake - Score: 0", sf::Style::Close | sf::Style::Titlebar);
    //window.setFramerateLimit(10u);

    std::vector<sf::RectangleShape> snake;
    Direction direction = RIGHT;
    sf::Vector2f snakePos(2.0F, 4.0F);
    sf::RectangleShape apple;
    int score = 0;
    sf::Clock timer;


start:
    direction = RIGHT;
    snake.clear();
    score = 0;
    snakePos.x = 2.0F;
    snakePos.y = 4.0F;

    apple.setFillColor(sf::Color::Red);
    apple.setSize(sf::Vector2f(squareSize, squareSize));
    apple.setPosition((rand() % 40) * squareSize, (rand() % 40) * squareSize);

    sf::RectangleShape newBlock{};
    newBlock.setFillColor(sf::Color::Green);
    // newBlock.setOutlineColor(sf::Color(0x00AA00FF));
    newBlock.setSize(sf::Vector2f(squareSize, squareSize));
    newBlock.setPosition(snakePos * (float)squareSize);
    snake.emplace_back(newBlock);

    sf::Event event{};
    while (window.isOpen())
    {
        // Poll window events
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
#ifdef _DEBUG
                printf("Window closed\n");
#endif
                window.close();
                break;
            default:
                break;
            }
        }

        // Set the current directional enum
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && direction != DOWN)
            direction = UP;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && direction != UP)
            direction = DOWN;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && direction != RIGHT)
            direction = LEFT;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && direction != LEFT)
            direction = RIGHT;

        // Change the snakes position according to the current direction
        if (timer.getElapsedTime().asMilliseconds() >= 1000 / framerateLimit)
        {
            timer.restart();

            if (direction == UP)
                snakePos.y--;
            if (direction == DOWN)
                snakePos.y++;
            if (direction == LEFT)
                snakePos.x--;
            if (direction == RIGHT)
                snakePos.x++;

            // Add a new block to the end of the snake
            sf::RectangleShape newBlock{};
            newBlock.setFillColor(sf::Color::Green);
            newBlock.setOutlineColor(sf::Color(0x00AA00FF));
            newBlock.setSize(sf::Vector2f(squareSize, squareSize));
            newBlock.setPosition(snakePos * (float)squareSize);
            snake.emplace_back(newBlock);

            // Check if the snake has bumped into itself
            for (size_t i = 0; i < snake.size() - 1U; i++)
            {
                if (snake.at(i).getGlobalBounds().intersects(snake.at(snake.size() - 1U).getGlobalBounds()))
                {
#ifdef _DEBUG
                    printf("Snake collided with itself\n");
#endif
                    goto start;
                }
            }

            // Check if the snake has crossed the window border
            if (!snake.at(snake.size() - 1U).getGlobalBounds().intersects(sf::FloatRect(0, 0, static_cast<unsigned int>(canvasWidth * squareSize), static_cast<unsigned int>(canvasHeight * squareSize))))
            {
#ifdef _DEBUG
                printf("Snake collided with border\n");
#endif
                goto start;
            }

            // Check if the snake has intersected with the apple, if it has then set the apple to a
            // new random position and increment the score, otherwise remove the end block on the snake
            if (snake.at(snake.size() - 1U).getGlobalBounds().intersects(apple.getGlobalBounds()))
            {
            check:
                apple.setPosition((rand() % 40) * squareSize, (rand() % 40) * squareSize);
                for (auto& block : snake)
                {
                    if (block.getGlobalBounds().intersects(apple.getGlobalBounds()))
                    {
                        goto check; // Yeah I'm probably going to get yelled at for doing this...
                    }
                }

                score++;
            }
            else
            {
                snake.erase(snake.begin());
            }

            // Set the window title to correctly display the score
            window.setTitle("Snake - Score: " + std::to_string(score));
        }

        // Draw all the sprites
        window.clear(sf::Color::Black);
        for (const auto& block : snake)
        {
            window.draw(block);
        }
        window.draw(apple);
        window.display();
    }

    return 0;
}
