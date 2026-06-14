#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>

#ifdef _WIN32
    #include <conio.h>
    #define CLS "cls"
#else
    #include <termios.h>
    #include <unistd.h>
    #define CLS "clear"
#endif

using namespace std;

// ============================================
// Variáveis globais (vitórias/derrotas)
// ============================================
int playerWins = 0;
int monsterWins = 0;

// ============================================
// Estruturas
// ============================================

struct Player {
    int hp;
    int dmg;
    int maxAtSp;
    int atSp;
    int acc;
    int x, y;
};

struct Monster {
    int hp;
    int dmg;
    int maxAtSp;
    int atSp;
    int acc;
    int blindnessChance;
    int x, y;
    char symbol;
};

struct Healer {
    int healAmount;
    int maxAtSp;
    int atSp;
    int acc;
};

struct Debuffer {
    int dmgReduction;
    int maxAtSp;
    int atSp;
    int acc;
};

struct StatusEffects {
    bool blindedActive;
    bool weakenedActive;
};

struct MessageFlags {
    bool showBlinded;
    bool showHealed;
    bool showWeakened;
};

// ============================================
// Constantes do mapa
// ============================================
const int MAP_WIDTH  = 30;
const int MAP_HEIGHT = 15;

// ============================================
// Protótipos das funções de combate
// ============================================
void playerAttack(Player& player, Monster& monster, StatusEffects& status, MessageFlags& msg);
void debufferAction(Debuffer& deb, StatusEffects& status, MessageFlags& msg);
void monsterAttack(Monster& monster, Player& player, StatusEffects& status, MessageFlags& msg, int debuffReduction);
void healerAction(Healer& healer, Player& player, MessageFlags& msg);
void printScreen(int playerHp, int monsterHp, int& prevPlayerHp, int& prevMonsterHp, MessageFlags& msg);

// ============================================
// Função para ler uma tecla sem Enter (cross-platform)
// ============================================
char getKeyPress() {
    #ifdef _WIN32
        return _getch();
    #else
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    #endif
}

// ============================================
// Funções do mapa
// ============================================

void initMap(vector<vector<char>>& map) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (y == 0 || y == MAP_HEIGHT-1 || x == 0 || x == MAP_WIDTH-1)
                map[y][x] = '+';
            else
                map[y][x] = '.';
        }
    }
}

void drawMap(const vector<vector<char>>& baseMap, const Player& player,
             const vector<Monster>& monsters) {
    vector<vector<char>> screen = baseMap;

    for (auto& m : monsters) {
        if (m.y > 0 && m.y < MAP_HEIGHT-1 && m.x > 0 && m.x < MAP_WIDTH-1)
            screen[m.y][m.x] = 'M';
    }

    if (player.y > 0 && player.y < MAP_HEIGHT-1 && player.x > 0 && player.x < MAP_WIDTH-1)
        screen[player.y][player.x] = 'P';

    system(CLS);

    for (auto& row : screen) {
        for (char c : row) {
            cout << c;
        }
        cout << endl;
    }

    cout << "\nWASD: mover | Q: sair\n";
    cout << "Vida: " << player.hp << " | Vitorias: " << playerWins << " | Derrotas: " << monsterWins << endl;
}

void spawnMonsters(vector<Monster>& monsters, int count, int playerX, int playerY,
                   int baseDmg, int baseHp, int /*monsterWinsArg*/) {
    for (int i = 0; i < count; i++) {
        int x, y;
        bool valid;
        do {
            valid = true;
            x = rand() % (MAP_WIDTH - 2) + 1;
            y = rand() % (MAP_HEIGHT - 2) + 1;

            if (x == playerX && y == playerY) valid = false;

            for (auto& m : monsters) {
                if (m.x == x && m.y == y) {
                    valid = false;
                    break;
                }
            }
        } while (!valid);

        Monster m;
        m.x = x;
        m.y = y;
        m.symbol = 'M';
        m.hp   = baseHp + monsterWins * 2;
        m.dmg  = baseDmg + (monsterWins / 5);
        m.maxAtSp = 10;
        m.atSp = 0;
        m.acc  = 90;
        m.blindnessChance = 40 + monsterWins;

        monsters.push_back(m);
    }
}

void movePlayer(Player& player, char direction) {
    int newX = player.x, newY = player.y;
    switch (direction) {
        case 'w': case 'W': newY--; break;
        case 's': case 'S': newY++; break;
        case 'a': case 'A': newX--; break;
        case 'd': case 'D': newX++; break;
    }
    if (newX > 0 && newX < MAP_WIDTH-1 && newY > 0 && newY < MAP_HEIGHT-1) {
        player.x = newX;
        player.y = newY;
    }
}

bool checkAndStartCombat(Player& player, vector<Monster>& monsters,
                         Healer& healer, Debuffer& debuffer,
                         int& /*playerWinsRef*/, int& /*monsterWinsRef*/) {
    for (auto it = monsters.begin(); it != monsters.end(); ++it) {
        if (it->x == player.x && it->y == player.y) {
            cout << "Um monstro apareceu! Prepare-se para a batalha!" << endl;
            this_thread::sleep_for(chrono::milliseconds(500));

            player.atSp = 0;
            it->atSp = 0;
            healer.atSp = 0;
            debuffer.atSp = 0;
            StatusEffects status = { false, false };
            MessageFlags msg = { false, false, false };
            int prevPlayerHp = player.hp, prevMonsterHp = it->hp;

            while (player.hp >= 1 && it->hp >= 1) {
                playerAttack(player, *it, status, msg);
                if (it->hp <= 0) break;

                debufferAction(debuffer, status, msg);
                monsterAttack(*it, player, status, msg, debuffer.dmgReduction);
                healerAction(healer, player, msg);

                printScreen(player.hp, it->hp, prevPlayerHp, prevMonsterHp, msg);
                this_thread::sleep_for(chrono::milliseconds(70));
            }

            if (player.hp <= 0) {
                cout << "Voce foi derrotado! Game over." << endl;
                monsterWins++;
                monsters.erase(it);
                return false;
            } else {
                cout << "Voce venceu o monstro!" << endl;
                playerWins++;
                monsters.erase(it);
                this_thread::sleep_for(chrono::milliseconds(1000));
                return true;
            }
        }
    }
    return true;
}

// ============================================
// Implementação das funções de combate
// ============================================

void playerAttack(Player& player, Monster& monster, StatusEffects& status, MessageFlags& msg) {
    if (player.atSp == player.maxAtSp) {
        int effectiveAcc = player.acc;
        if (status.blindedActive) {
            effectiveAcc -= monster.dmg * 3;
            msg.showBlinded = true;
            status.blindedActive = false;
        }
        if ((rand() % 100) <= effectiveAcc) {
            monster.hp -= rand() % player.dmg;
        }
        player.atSp = 0;
    } else {
        player.atSp++;
    }
}

void debufferAction(Debuffer& deb, StatusEffects& status, MessageFlags& msg) {
    if (deb.atSp == deb.maxAtSp) {
        if ((rand() % 100) <= deb.acc) {
            status.weakenedActive = true;
            msg.showWeakened = true;
        }
        deb.atSp = 0;
    } else {
        deb.atSp++;
    }
}

void monsterAttack(Monster& monster, Player& player, StatusEffects& status,
                   MessageFlags& msg, int debuffReduction) {
    if (monster.atSp == monster.maxAtSp) {
        if ((rand() % 100) <= monster.acc) {
            int damage = rand() % monster.dmg;
            if (status.weakenedActive) {
                damage -= debuffReduction;
                if (damage < 0) damage = 0;
                status.weakenedActive = false;
            }
            player.hp -= damage;
        }
        if ((rand() % 100) <= monster.blindnessChance) {
            status.blindedActive = true;
        }
        monster.atSp = 0;
    } else {
        monster.atSp++;
    }
}

void healerAction(Healer& healer, Player& player, MessageFlags& msg) {
    if (healer.atSp == healer.maxAtSp) {
        if ((rand() % 100) <= healer.acc) {
            player.hp += healer.healAmount;
            msg.showHealed = true;
        }
        healer.atSp = 0;
    } else {
        healer.atSp++;
    }
}

void printScreen(int playerHp, int monsterHp, int& prevPlayerHp, int& prevMonsterHp,
                 MessageFlags& msg) {
    if (prevPlayerHp != playerHp || prevMonsterHp != monsterHp ||
        msg.showBlinded || msg.showHealed || msg.showWeakened) {

        cout << "Player  : ";
        for (int i = 0; i < playerHp; i++) cout << "o";
        if (msg.showHealed) {
            cout << "    Player was healed for " << (playerHp - prevPlayerHp) << "!";
            msg.showHealed = false;
        }
        if (msg.showBlinded) {
            cout << "    Player is blinded!";
            msg.showBlinded = false;
        }
        cout << endl;

        cout << "Monster : ";
        for (int i = 0; i < monsterHp; i++) cout << "o";
        if (msg.showWeakened) {
            cout << "   Monster was weakened";
            msg.showWeakened = false;
        }
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        prevPlayerHp = playerHp;
        prevMonsterHp = monsterHp;
    }
}

// ============================================
// Função principal
// ============================================
int main() {
    srand(time(0));

    vector<vector<char>> map(MAP_HEIGHT, vector<char>(MAP_WIDTH, ' '));
    initMap(map);

    Player player;
    player.hp = 30 + monsterWins;
    player.dmg = 2 + (monsterWins / 5);
    player.maxAtSp = 4;
    player.atSp = 0;
    player.acc = 90;
    player.x = MAP_WIDTH / 2;
    player.y = MAP_HEIGHT / 2;

    Healer healer;
    healer.healAmount = 3 + (monsterWins / 10);
    healer.maxAtSp = 5;
    healer.atSp = 0;
    healer.acc = 5;

    Debuffer debuffer;
    debuffer.dmgReduction = 3;
    debuffer.maxAtSp = 10;
    debuffer.atSp = 0;
    debuffer.acc = 15;

    vector<Monster> monsters;
    int monsterCount = 5;

    spawnMonsters(monsters, monsterCount, player.x, player.y, 3, 50, monsterWins);

    char input;
    bool running = true;

    while (running) {
        drawMap(map, player, monsters);

        input = getKeyPress();  // Agora é cross-platform

        if (input == 'q' || input == 'Q') {
            running = false;
            break;
        }

        movePlayer(player, input);

        bool alive = checkAndStartCombat(player, monsters, healer, debuffer,
                                         playerWins, monsterWins);

        if (!alive) {
            cout << "Fim de jogo! Pressione qualquer tecla para continuar...";
            getKeyPress();
            player.hp = 30 + monsterWins;
            player.dmg = 2 + (monsterWins / 5);
            player.atSp = 0;
            player.x = MAP_WIDTH / 2;
            player.y = MAP_HEIGHT / 2;

            monsters.clear();
            spawnMonsters(monsters, monsterCount, player.x, player.y, 3, 50, monsterWins);
        }

        if (monsters.empty()) {
            monsterCount++;
            spawnMonsters(monsters, monsterCount, player.x, player.y, 3, 50, monsterWins);
        }
    }

    return 0;
}