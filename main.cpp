#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>

// 盤面サイズ
const int WIDTH = 10;
const int HEIGHT = 20;
const int BLOCK_SIZE = 30; // 1マスのピクセル数

// ブロックの種類
enum BlockType { T_BLOCK, O_BLOCK, L_BLOCK };

// ブロックの色を返す
sf::Color getBlockColor(BlockType type) {
    switch (type) {
        case T_BLOCK: return sf::Color::Red;
        case O_BLOCK: return sf::Color::Green;
        case L_BLOCK: return sf::Color(255, 165, 0); // オレンジ
    }
    return sf::Color::White;
}

// ブロックの形状を設定する関数
void getBlockShape(BlockType type, int shape[4][4]) {
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            shape[y][x] = 0;

    switch (type) {
        case T_BLOCK:
            shape[1][0] = shape[1][1] = shape[1][2] = shape[2][1] = 1;
            break;
        case O_BLOCK:
            shape[1][1] = shape[1][2] = shape[2][1] = shape[2][2] = 1;
            break;
        case L_BLOCK:
            shape[0][1] = shape[1][1] = shape[2][1] = shape[2][2] = 1;
            break;
    }
}

// ブロックを回転する関数
void rotate(int shape[4][4]) {
    int temp[4][4] = {};
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            temp[x][3 - y] = shape[y][x];
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            shape[y][x] = temp[y][x];
}

// 衝突判定
bool checkCollision(int board[HEIGHT][WIDTH], int offsetY, int offsetX, int shape[4][4]) {
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            if (shape[y][x]) {
                int py = offsetY + y;
                int px = offsetX + x;
                if (py >= HEIGHT || px < 0 || px >= WIDTH || board[py][px])
                    return true;
            }
    return false;
}

// ブロックをボードに固定する
void placeBlock(int board[HEIGHT][WIDTH], BlockType boardColor[HEIGHT][WIDTH], int offsetY, int offsetX, int shape[4][4], BlockType type) {
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            if (shape[y][x]) {
                int py = offsetY + y;
                int px = offsetX + x;
                if (py >= 0 && py < HEIGHT && px >= 0 && px < WIDTH) {
                    board[py][px] = 1;
                    boardColor[py][px] = type;
                }
            }
}

// 揃った行を削除する
void clearLines(int board[HEIGHT][WIDTH], BlockType boardColor[HEIGHT][WIDTH]) {
    for (int y = HEIGHT - 1; y >= 0; --y) {
        bool full = true;
        for (int x = 0; x < WIDTH; ++x) {
            if (!board[y][x]) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int ty = y; ty > 0; --ty)
                for (int x = 0; x < WIDTH; ++x) {
                    board[ty][x] = board[ty - 1][x];
                    boardColor[ty][x] = boardColor[ty - 1][x];
                }
            for (int x = 0; x < WIDTH; ++x) {
                board[0][x] = 0;
            }
            y++;
        }
    }
}

int main() {
    std::srand(std::time(nullptr));
    sf::RenderWindow window(sf::VideoMode(WIDTH * BLOCK_SIZE, HEIGHT * BLOCK_SIZE), "Tetris SFML");

    int board[HEIGHT][WIDTH] = {};
    BlockType boardColor[HEIGHT][WIDTH] = {};
    int shape[4][4];
    int x = 3, y = 0;
    BlockType type = static_cast<BlockType>(std::rand() % 3);
    getBlockShape(type, shape);

    sf::Clock clock;
    float delay = 0.5f;

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
        }

        // キー入力処理
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (!checkCollision(board, y, x - 1, shape)) x--;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (!checkCollision(board, y, x + 1, shape)) x++;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            int tmp[4][4];
            std::copy(&shape[0][0], &shape[0][0] + 16, &tmp[0][0]);
            rotate(tmp);
            if (!checkCollision(board, y, x, tmp))
                std::copy(&tmp[0][0], &tmp[0][0] + 16, &shape[0][0]);
        }

        static float timer = 0;
        timer += time;
        if (timer > delay) {
            if (!checkCollision(board, y + 1, x, shape)) {
                y++;
            } else {
                placeBlock(board, boardColor, y, x, shape, type);
                clearLines(board, boardColor);
                y = 0;
                x = 3;
                type = static_cast<BlockType>(std::rand() % 3);
                getBlockShape(type, shape);
                if (checkCollision(board, y, x, shape)) {
                    window.close(); // Game Over
                }
            }
            timer = 0;
        }

        // 描画処理
        window.clear(sf::Color::Black);
        sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));

        // 盤面描画
        for (int yb = 0; yb < HEIGHT; ++yb)
            for (int xb = 0; xb < WIDTH; ++xb)
                if (board[yb][xb]) {
                    block.setFillColor(getBlockColor(boardColor[yb][xb]));
                    block.setPosition(xb * BLOCK_SIZE, yb * BLOCK_SIZE);
                    window.draw(block);
                }

        // 現在のブロック描画
        block.setFillColor(getBlockColor(type));
        for (int yb = 0; yb < 4; ++yb)
            for (int xb = 0; xb < 4; ++xb)
                if (shape[yb][xb]) {
                    block.setPosition((x + xb) * BLOCK_SIZE, (y + yb) * BLOCK_SIZE);
                    window.draw(block);
                }

        window.display();
    }

    return 0;
}
