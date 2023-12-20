#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

sf::Color gridColor(128, 128, 128); // Gray color for grid lines

// Define grid parameters
const int cellSize = 40;
const int width = 10;
const int height = 20;

// Define grid
int grid[height][width] = { 0 };


// Define Shape
int shapes[7][4][4] =
{
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0, // I

    1,0,0,0,
    1,1,0,0,
    0,1,0,0,
    0,0,0,0, // Z

    0,1,0,0,
    1,1,0,0,
    1,0,0,0,
    0,0,0,0, // S

    1,0,0,0,
    1,1,0,0,
    1,0,0,0,
    0,0,0,0, // T

    1,0,0,0,
    1,0,0,0,
    1,1,0,0,
    0,0,0,0, // L

    0,1,0,0,
    0,1,0,0,
    1,1,0,0,
    0,0,0,0, // J

    1,1,0,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0 // O
};

// Define colors
const Color colors[] =
{
    Color::Green,
    Color::Blue,
    Color::Red,
    Color::Yellow,
    Color::White,
    Color::Magenta,
    Color::Cyan
};


int main()
{

    // a window that can render 2D drawings
    RenderWindow window(VideoMode(width * cellSize, height * cellSize), "Tetris");
    // Create a grid by drawing vertical and horizontal lines
    sf::VertexArray gridLines(sf::Lines);

    for (int x = 0; x <= width; x += cellSize) {
        gridLines.append(sf::Vertex(sf::Vector2f(x, 0), gridColor));
        gridLines.append(sf::Vertex(sf::Vector2f(x, height), gridColor));
    }

    for (int y = 0; y <= height; y += cellSize) {
        gridLines.append(sf::Vertex(sf::Vector2f(0, y), gridColor));
        gridLines.append(sf::Vertex(sf::Vector2f(width, y), gridColor));
    }

     // draw a cell in the grid
    RectangleShape cell(Vector2f(cellSize, cellSize));


    // populate a block
    int block;
    int b_x;
    int b_y;

    auto new_block = [&]()
    {
        block = rand() % 7;
        b_x = width / 2;
        b_y = 0;
    };

    new_block();

    // boundary check for a block
    auto check_block_boundary = [&]()
    {
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                if (shapes[block][y][x] == 0)
                    continue;

                // hit world boundary
                if (x + b_x < 0 || x + b_x >= width || y + b_y >= height)
                    return false;

                // collsion with world blocks
                if (grid[y + b_y][x + b_x] )
                    return false;
            }

        }
        return true;
    };

    // clear lines when grid width is full-filled
    auto clear_lines = [&]()
    {
        int deleted_line = height - 1;

        //from bottom line to top line...
        for (int undeleted_line = height - 1; undeleted_line >= 0; undeleted_line--)
        {
            int count_width = 0;
            for (int x = 0; x < width; x++)
            {
                if (grid[undeleted_line][x])
                    count_width++;
            }

            // if current line is not full, copy lines
            // else, the line will be deleted
            if (count_width < width)
            {
                for (int x = 0; x < width; x++)
                    grid[deleted_line][x] = grid[undeleted_line][x];

                deleted_line--;
            }

        }
    };

    // fall down
    auto fall_down = [&]()
    {
        b_y++;

        // hit bottom
        if (check_block_boundary() == false)
        {
            b_y--;
            for (int y = 0; y < 4; y++)
            {
                for (int x = 0; x < 4; x++)
                {
                    if (shapes[block][y][x])
                    {
                        // +1 for avoiding 0
                        grid[b_y + y][b_x + x] = block + 1;
                    }

                }
            }
            // delete lines if necessary (bhitrai condition check huncha)
            clear_lines();

            // start next block
            new_block();

            return false;
        }
        return true;
    };

    // rotate shape
    auto rotate = [&]()
    {
        // check rotation block size
        int len = 0;
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                if (shapes[block][y][x])
                {
                    len = max(max(x, y) + 1, len);
                }
            }
        }

        int rotated_block[4][4] = { 0 };

        // rotate conter-clock-wise (90 degree)
        for (int y = 0; y < len; y++)
        {
            for (int x = 0; x < len; x++)
            {
                if (shapes[block][y][x])
                {
                    rotated_block[len - 1 - x][y] = 1;
                }
            }
        }

        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                shapes[block][y][x] = rotated_block[y][x];
            }
        }
    };

    Clock clock;

    while (window.isOpen())
    {
        // start clock
        static float prev = clock.getElapsedTime().asSeconds();
        if (clock.getElapsedTime().asSeconds() - prev >= 0.5)
        {
            prev = clock.getElapsedTime().asSeconds();
            fall_down();
        }

        // Define system event
        Event e;

        // polling event (eg. key pressed)
        while (window.pollEvent(e))
        {
            // close window
            if (e.type == Event::Closed)
                window.close();

            // keyboard interrupt
            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Left) {
                    b_x--;
                    if (check_block_boundary() == false) b_x++;
                }
                else if (e.key.code == Keyboard::Right)
                {
                    b_x++;
                    if (check_block_boundary() == false) b_x--;
                }
                else if (e.key.code == Keyboard::Down)
                {
                    fall_down();
                }
                else if (e.key.code == Keyboard::Space)
                {
                    // fall down until reaches the bottom
                    while (fall_down() == true);
                }
                else if (e.key.code == Keyboard::Up)
                {
                    rotate();

                    // if rotation hits boundary, do not allow to rotate
                    if (check_block_boundary() == false)
                    {
                        rotate(), rotate(), rotate();
                    }
                }
            }
        }

        // clear window every frame
        window.clear();

        // draw grid
        auto draw_grid = [&]()
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    if (grid[y][x])
                    {
                        cell.setFillColor(colors[grid[y][x] - 1]);
                        cell.setPosition(Vector2f(x * cellSize, y * cellSize));
                        window.draw(cell);
                    }
                }
            }
        };
        draw_grid();

        // define C++11 lambda function
        // this function can use all the outside variables, such as block
        auto draw_block = [&]()
        {
            cell.setFillColor(colors[block]);
            for (int y = 0; y < 4; y++)
            {
                for (int x = 0; x < 4; x++)
                {
                    if (shapes[block][y][x])
                    {
                        cell.setPosition(Vector2f((b_x + x) * cellSize, (b_y + y) * cellSize));
                        window.draw(cell);
                    }
                }
            }
        };

        // call the above lambda function
        draw_block();
        // display rendered object on screen
        window.display();
    }

    return 0;
}
