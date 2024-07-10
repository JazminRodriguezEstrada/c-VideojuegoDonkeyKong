#include <iostream>
#include <vector>
#include <conio.h> // Para _kbhit() y _getch()
#include <windows.h> // Para Sleep()

using namespace std;

const int WIDTH = 30; // Ancho aumentado
const int HEIGHT = 20; // Altura aumentada

const char EMPTY = ' ';
const char PLAYER = 'P';
const char DONKEY_KONG = 'D';
const char OBSTACLE = 'O';
const char LADDER = 'H';
const char PLATFORM = '=';
const char BARREL = 'B';

const int MAX_LIVES = 3; // Número máximo de vidas

// Estructura para la posición
struct Position {
    int x;
    int y;
};

// Mapa del juego
vector<vector<char>> map(HEIGHT, vector<char>(WIDTH, EMPTY));

// Posiciones de los elementos
Position playerPos = {1, HEIGHT - 2};
Position donkeyKongPos = {1, HEIGHT - 16};
vector<Position> barrels;

int lives = MAX_LIVES; // Contador de vidas

void initializeMap() {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            map[i][j] = EMPTY;
        }
    }

    // Crear bordes
    for (int i = 0; i < HEIGHT; ++i) {
        map[i][0] = map[i][WIDTH - 1] = '|';
    }
    for (int j = 0; j < WIDTH; ++j) {
        map[0][j] = map[HEIGHT - 1][j] = '-';
    }

    // Crear plataformas
    for (int j = 1; j < WIDTH - 1; ++j) {
        map[HEIGHT - 2][j] = PLATFORM;
        map[HEIGHT - 7][j] = PLATFORM;
        map[HEIGHT - 12][j] = PLATFORM;
    }

    // Colocar escaleras
    map[HEIGHT - 3][4] = LADDER;
    map[HEIGHT - 17][15] = LADDER;
    map[HEIGHT - 13][20] = LADDER;

    // Agregar múltiples escaleras
    for (int i = 1; i <= 5; ++i) {
        map[HEIGHT - 8 + i][4] = LADDER;
    }
    // Segunda escalera en el nivel 2, en el extremo derecho
    for (int i = 1; i <= 5; ++i) {
        map[HEIGHT - 13 + i][WIDTH - 2] = LADDER;
    }
    for (int i = 1; i <= 5; ++i) {
        map[HEIGHT - 8 + i][15] = LADDER;
    }

    // Colocar obstáculos
    map[HEIGHT - 2][10] = OBSTACLE;
    map[HEIGHT - 7][18] = OBSTACLE;
    map[HEIGHT - 12][8] = OBSTACLE;

    // Colocar al jugador y a Donkey Kong
    map[playerPos.y][playerPos.x] = PLAYER;
    map[donkeyKongPos.y][donkeyKongPos.x] = DONKEY_KONG;
}

void printMap() {
    system("cls");
    for (const auto& row : map) {
        for (const auto& cell : row) {
            cout << cell;
        }
        cout << endl;
    }
    cout << "Vidas: " << lives << endl;
}

void updateMap() {
    // Limpiar el mapa de los personajes
    for (auto& row : map) {
        for (auto& cell : row) {
            if (cell == PLAYER || cell == BARREL) {
                cell = EMPTY;
            }
        }
    }

    // Actualizar posiciones
    map[playerPos.y][playerPos.x] = PLAYER;
    for (const auto& barrel : barrels) {
        map[barrel.y][barrel.x] = BARREL;
    }
}

bool checkWinCondition() {
    return playerPos.x == donkeyKongPos.x && playerPos.y == donkeyKongPos.y;
}

void movePlayer(char input) {
    Position newPos = playerPos;

    switch (input) {
        case 'w': // Arriba
            if (map[playerPos.y - 1][playerPos.x] == LADDER) {
                newPos.y--;
            }
            break;
        case 's': // Abajo
            if (map[playerPos.y + 1][playerPos.x] == LADDER || map[playerPos.y + 1][playerPos.x] == PLATFORM) {
                newPos.y++;
            }
            break;
        case 'a': // Izquierda
            if (map[playerPos.y][playerPos.x - 1] != '|' && map[playerPos.y][playerPos.x - 1] != OBSTACLE) {
                newPos.x--;
            }
            break;
        case 'd': // Derecha
            if (map[playerPos.y][playerPos.x + 1] != '|' && map[playerPos.y][playerPos.x + 1] != OBSTACLE) {
                newPos.x++;
            }
            break;
    }

    if (map[newPos.y][newPos.x] != OBSTACLE) {
        playerPos = newPos;
    }
}

void moveBarrels() {
    for (auto it = barrels.begin(); it != barrels.end(); ) {
        auto& barrel = *it;
        if (map[barrel.y + 1][barrel.x] != PLATFORM && map[barrel.y + 1][barrel.x] != LADDER) {
            barrel.y++;
        } else if (map[barrel.y][barrel.x + 1] != '|' && map[barrel.y][barrel.x + 1] != OBSTACLE) {
            barrel.x++;
        }
        // Verificar si el barril ha alcanzado el límite inferior
        if (barrel.y >= HEIGHT - 1) {
            it = barrels.erase(it); // Eliminar el barril de la lista
        } else {
            // Verificar si el barril golpea al jugador
            if (barrel.x == playerPos.x && barrel.y == playerPos.y) {
                lives--; // Decrementar el contador de vidas
                if (lives <= 0) {
                    system("cls");
                    cout << "¡Has perdido todas tus vidas!" << endl;
                    exit(0);
                } else {
                    // Restaurar la posición inicial del jugador
                    playerPos = {1, HEIGHT - 2};
                }
            }

            ++it;
        }
    }
}

int main() {
    initializeMap();

    barrels.push_back({donkeyKongPos.x + 1, donkeyKongPos.y});

    int barrelSpawnCounter = 0;

    while (true) {
        printMap();
        moveBarrels();
        updateMap();
        if (_kbhit()) {
            char input = _getch();
            movePlayer(input);
        }

        // Incrementar el contador y añadir un nuevo barril cada cierto tiempo
        barrelSpawnCounter++;
        if (barrelSpawnCounter >= 50) { // Ajusta este valor para cambiar el tiempo entre los barriles
            barrels.push_back({donkeyKongPos.x + 1, donkeyKongPos.y});
            barrelSpawnCounter = 0;
        }

        if (checkWinCondition()) {
            system("cls");
            cout << "¡Has ganado!" << endl;
            break;
        }

        Sleep(200);
    }

return 0;
}
