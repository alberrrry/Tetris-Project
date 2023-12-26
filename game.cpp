#include <SFML/Graphics.hpp>
#include <math.h>
using namespace std;
using namespace sf;

const int COLUMNS = 17;
const int ROWS = 20;
const int CELL_SIZE = 40;


enum GameState {
    Home,
    Game,
};

// Define grid
int grid[ROWS][COLUMNS] = { 0 };
int score = 0;

// Define Shape
int shapes[7][4][4] =
{
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0, // I

    1,0,0,0,
    1,0,0,0,
    1,1,0,0,
    0,0,0,0, // L

    1,0,0,0,
    1,1,0,0,
    0,1,0,0,
    0,0,0,0, // Z

    1,1,0,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0, // O

    1,0,0,0,
    1,1,0,0,
    1,0,0,0,
    0,0,0,0, // T

    0,1,0,0,
    1,1,0,0,
    1,0,0,0,
    0,0,0,0, // S

    0,1,0,0,
    0,1,0,0,
    1,1,0,0,
    0,0,0,0  // J
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
    RenderWindow window(VideoMode(1200,800),"Game Space");
    sf::Color gridColors[COLUMNS][ROWS] = {sf::Color::White};

  // Load home screen texture
    Texture home;
    // Create sprites for home screen and game
    home.loadFromFile("resources/space.png");
  Sprite homeSprite(home);
  Vector2u size = home.getSize();
  homeSprite.setOrigin(size.x / 50, size.y / 50);
  homeSprite.scale(Vector2f(0.3,0.3));

   // Create a font
    Font font;
    if (!font.loadFromFile("resources/BlockHead_bold.ttf")) {
        // Handle the case where the font cannot be loaded
        return -1;
    }

    // Create a text object
    Text text;
    text.setFont(font); // Set the font for the text
    text.setString("TETRIS\nEnter space to play"); // Set the string to be displayed
    text.setCharacterSize(80); // Set the character size
    text.setFillColor(Color::White); // Set the fill color
    text.setStyle(Text::Bold); // Set the text style (optional)
    text.setPosition(400.0f, 300.0f); // Set the position of the text

    Text score_display;
    score_display.setFont(font); // Set the font for the text
    score_display.setString("Score: " + std::to_string(score));
    score_display.setCharacterSize(80);
    score_display.setFillColor(sf::Color::White);
    score_display.setPosition(850.0f, 550.0f);

    Text gameover_text;
    gameover_text.setFont(font); // Set the font for the text
    gameover_text.setString("GAME OVER");
    gameover_text.setCharacterSize(80);
    gameover_text.setFillColor(sf::Color::White);
    gameover_text.setPosition(450.0f, 350.0f);

     // Load background image
    sf::Texture blockTexture;
    if (!blockTexture.loadFromFile("resources/block.png")) {
        // Handle the case where the texture cannot be loaded
        return -1;
    }

    //Create sprites for different Tetris blocks
    sf::Sprite blockSprite1(blockTexture);
    blockSprite1.setPosition(100.0f, 100.0f);
    Vector2u size1 = blockTexture.getSize();
    blockSprite1.setOrigin(size1.x / 50, size1.y / 50);
    blockSprite1.scale(Vector2f(0.3,0.3)); 

         // draw a cell in the grid
    RectangleShape cell(Vector2f(CELL_SIZE, CELL_SIZE));
    RectangleShape preview_cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));

      // populate a block
    int block;
    int b_x;
    int b_y;
    int pre;

    block=rand() % 7;
    
    auto preview_block = [&]()
    {
        pre=rand() % 7;
    };

    auto new_block = [&]()
    {
        if(pre==6)
        block=0;
        else
        block=pre+1;
        b_x = COLUMNS / 2;
        b_y = 0;
    };
 preview_block(); 
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
                if (x + b_x < 0 || x + b_x >= COLUMNS || y + b_y >= ROWS)
                    return false;

                // collision with world blocks
                if (grid[y + b_y][x + b_x] )
                    return false;
            }
        }
        return true;
    };


    // clear lines when grid width is full-filled
    auto clear_lines = [&]()
    {
        int deleted_line = ROWS - 1;

        //from bottom line to top line...
        for (int undeleted_line = ROWS - 1; undeleted_line >= 0; undeleted_line--)
        {
            int count_width = 0;
            for (int x = 0; x < COLUMNS; x++)
            {
                if (grid[undeleted_line][x])
                    count_width++;
            }

            // if current line is not full, copy lines
            // else, the line will be deleted
            if (count_width < COLUMNS)
            {
                for (int x = 0; x < COLUMNS; x++)
                    grid[deleted_line][x] = grid[undeleted_line][x];

                deleted_line--;
            }
            else if (count_width == COLUMNS)
            {
                score += 20;{
                score_display.setString("Score: " + std::to_string(score));
                }
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
            // delete lines
            clear_lines();

            // start next block
            new_block();
            preview_block();
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

  
    // Set the initial game state
    GameState gameState = Home;

    // Main loop
    while (window.isOpen()) {
        // Process events
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            } 
            else if (event.type == Event::KeyPressed) {
                // Switch to the game state when the spacebar is pressed
                if (event.key.code == Keyboard::Space && gameState == Home) {
                    gameState = Game;
                }
            }

        }
        window.clear();
        // Draw based on the current game state
        if (gameState == Home) {
            window.draw(homeSprite);
            window.draw(text);
        } 
        else if (gameState == Game) {
              

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
        window.draw(score_display);

        
        RectangleShape preview_border(sf::Vector2f(5 * CELL_SIZE, 5 * CELL_SIZE));
				preview_border.setFillColor(sf::Color(0, 0, 0));
				preview_border.setOutlineThickness(-1);
				preview_border.setPosition(850, 200);
                //Draw the preview border
				window.draw(preview_border);

                auto draw_preview = [&](){
                for (int y = 0; y < 4; y++)
                 {
                for (int x = 0; x < 4; x++)
                {
                    if (shapes[pre][y][x])
                    {
                         preview_cell.setPosition(Vector2f(900 + (x * CELL_SIZE), 220 + (y * CELL_SIZE)));
                         preview_cell.setFillColor(colors[pre]);
                         /*preview_cell.setOutlineThickness(1.0f);
                         preview_cell.setOutlineColor(sf::Color::Black);*/
                         window.draw(preview_cell);
                    }
                }
                 }
                };


                draw_preview();


        // draw grid
        auto draw_grid = [&]()
        {
            for (int y = 0; y < ROWS; y++)
            {
                for (int x = 0; x < COLUMNS; x++)
                {
                    if (grid[y][x])
                    {
                        cell.setFillColor(colors[grid[y][x] - 1]);
                        cell.setPosition(Vector2f(x * CELL_SIZE, y * CELL_SIZE));
                        window.draw(cell);
                    }
                }
            }
        };
        //if(check_block_boundary())
        draw_grid();
        // define C++11 lambda function
        // this function can use all the outside variables, such as block

        //khasne blocks ko function
        auto draw_block = [&]()
        {
            cell.setFillColor(colors[block]);
            for (int y = 0; y < 4; y++)
            {
                for (int x = 0; x < 4; x++)
                {
                    if (shapes[block][y][x])
                    {
                        cell.setPosition(Vector2f((b_x + x) * CELL_SIZE, (b_y + y) * CELL_SIZE));
                        window.draw(cell);
                    }
                }
            }
        };

        // call the above lambda function
        draw_block();

         for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                if (grid[b_y][b_x]){
                window.clear();
                     window.draw(gameover_text);}
            }
        }



        // display rendered object on screen
        window.display();
    }

        }
        // Display the contents of the window
        window.display();
    }
    return 0;
}
