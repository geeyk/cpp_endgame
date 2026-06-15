#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

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
// Variáveis globais
// ============================================
int playerWins = 0;
int monsterWins = 0;

struct KilledMonster {
    int hp;        // HP máximo original
    int dmg;       // Dano máximo
    int atkSpeed;  // maxAtSp (velocidade de ataque)
};
vector<KilledMonster> killedMonsters;

// ============================================
// CONSTANTES DE BALANCEAMENTO
// ============================================
const int MAP_WIDTH      = 30;
const int MAP_HEIGHT     = 15;
const char BORDER_CHAR   = '+';
const char FLOOR_CHAR    = '.';
const char PLAYER_CHAR   = 'P';
const char MONSTER_CHAR  = 'M';
const int INITIAL_MONSTER_COUNT = 5;

const int PLAYER_BASE_HP      = 30;
const int PLAYER_HP_PER_MONSTER_WIN = 2;
const int PLAYER_BASE_DMG     = 5;
const int PLAYER_DMG_PER_WIN  = 1;
const int PLAYER_ATK_SPEED    = 4;
const int PLAYER_BASE_ACC     = 85;

const int MONSTER_BASE_HP       = 30;
const int MONSTER_HP_PER_WIN    = 3;
const int MONSTER_BASE_DMG      = 6;
const int MONSTER_DMG_PER_WIN   = 1;
const int MONSTER_ATK_SPEED     = 10;
const int MONSTER_BASE_ACC      = 80;
const int MONSTER_BLIND_CHANCE  = 30;
const int MONSTER_BLIND_PER_WIN = 2;
const int BLIND_PENALTY         = 15;

const int HEALER_BASE_AMOUNT = 5;
const int HEALER_AMOUNT_PER_WIN = 1;
const int HEALER_ATK_SPEED   = 5;
const int HEALER_BASE_ACC    = 20;

const int DEBUFF_BASE_REDUCTION   = 4;
const int DEBUFF_REDUCTION_PER_WIN = 1;
const int DEBUFF_ATK_SPEED        = 10;
const int DEBUFF_BASE_ACC         = 25;

const int COMBAT_DELAY_MS = 70;

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
// Protótipos
// ============================================
char getKeyPress();
void initMap(vector<vector<char>>& map);
void drawMap(const vector<vector<char>>& baseMap, const Player& player, const vector<Monster>& monsters);
void spawnMonsters(vector<Monster>& monsters, int count, int playerX, int playerY);
void movePlayer(Player& player, char direction);
bool checkAndStartCombat(Player& player, vector<Monster>& monsters, Healer& healer, Debuffer& debuffer);
void showKillList();
void playerAttack(Player& player, Monster& monster, StatusEffects& status, MessageFlags& msg);
void debufferAction(Debuffer& deb, StatusEffects& status, MessageFlags& msg);
void monsterAttack(Monster& monster, Player& player, StatusEffects& status, MessageFlags& msg, int debuffReduction);
void healerAction(Healer& healer, Player& player, MessageFlags& msg);
void printScreen(int playerHp, int monsterHp, int& prevPlayerHp, int& prevMonsterHp, MessageFlags& msg);

// ============================================
// Captura de tecla cross-platform
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
                map[y][x] = BORDER_CHAR;
            else
                map[y][x] = FLOOR_CHAR;
        }
    }
}

void drawMap(const vector<vector<char>>& baseMap, const Player& player,
             const vector<Monster>& monsters) {
    vector<vector<char>> screen = baseMap;

    for (auto& m : monsters) {
        if (m.y > 0 && m.y < MAP_HEIGHT-1 && m.x > 0 && m.x < MAP_WIDTH-1)
            screen[m.y][m.x] = MONSTER_CHAR;
    }

    if (player.y > 0 && player.y < MAP_HEIGHT-1 && player.x > 0 && player.x < MAP_WIDTH-1)
        screen[player.y][player.x] = PLAYER_CHAR;

    system(CLS);

    for (auto& row : screen) {
        for (char c : row) cout << c;
        cout << endl;
    }

    cout << "\nWASD: mover | L: lista de mortos | Q: sair\n";
    cout << "HP: " << player.hp << " | V: " << playerWins << " | D: " << monsterWins << endl;
}

void spawnMonsters(vector<Monster>& monsters, int count, int playerX, int playerY) {
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
        m.hp   = MONSTER_BASE_HP + monsterWins * MONSTER_HP_PER_WIN;
        m.dmg  = MONSTER_BASE_DMG + monsterWins * MONSTER_DMG_PER_WIN;
        m.maxAtSp = MONSTER_ATK_SPEED;
        m.atSp = 0;
        m.acc  = MONSTER_BASE_ACC;
        m.blindnessChance = MONSTER_BLIND_CHANCE + monsterWins * MONSTER_BLIND_PER_WIN;

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

// ============================================
// Combate + registro de kill list
// ============================================
bool checkAndStartCombat(Player& player, vector<Monster>& monsters,
                         Healer& healer, Debuffer& debuffer) {
    for (auto it = monsters.begin(); it != monsters.end(); ++it) {
        if (it->x == player.x && it->y == player.y) {
            cout << "Um monstro apareceu! Prepare-se para a batalha!" << endl;
            this_thread::sleep_for(chrono::milliseconds(500));

            int originalMonsterHP = it->hp;

            player.atSp = 0;
            it->atSp = 0;
            healer.atSp = 0;
            debuffer.atSp = 0;
            StatusEffects status = { false, false };
            MessageFlags msg = { false, false, false };
            int prevPlayerHp = player.hp, prevMonsterHp = it->hp;

            while (player.hp >= 1 && it->hp >= 1) {
                playerAttack(player, *it, status, msg);
                if (it->hp <= 0) {
                    printScreen(player.hp, it->hp, prevPlayerHp, prevMonsterHp, msg);
                    break;
                }

                debufferAction(debuffer, status, msg);
                monsterAttack(*it, player, status, msg, debuffer.dmgReduction);
                healerAction(healer, player, msg);
                printScreen(player.hp, it->hp, prevPlayerHp, prevMonsterHp, msg);
                this_thread::sleep_for(chrono::milliseconds(COMBAT_DELAY_MS));
            }

            if (player.hp <= 0) {
                cout << "Voce foi derrotado! Game over." << endl;
                monsterWins++;
                monsters.erase(it);
                return false;
            } else {
                cout << "Voce venceu o monstro!" << endl;
                playerWins++;

                KilledMonster km;
                km.hp = originalMonsterHP;
                km.dmg = it->dmg;
                km.atkSpeed = it->maxAtSp;
                killedMonsters.push_back(km);

                monsters.erase(it);
                this_thread::sleep_for(chrono::milliseconds(1000));
                return true;
            }
        }
    }
    return true;
}

// ============================================
// Lista de monstros mortos (pausa)
// ============================================
void showKillList() {
    if (killedMonsters.empty()) {
        cout << "Nenhum monstro foi derrotado ainda.\n";
        cout << "Pressione qualquer tecla para voltar...";
        getKeyPress();
        return;
    }

    vector<KilledMonster> list = killedMonsters;
    sort(list.begin(), list.end(), [](const KilledMonster& a, const KilledMonster& b) {
        return a.hp > b.hp;
    });
    int sortMode = 1;

    bool viewing = true;
    while (viewing) {
        system(CLS);
        cout << "=== MONSTROS DERROTADOS ===\n";
        cout << "Ordenado por: ";
        switch (sortMode) {
            case 1: cout << "HP (maior -> menor)\n"; break;
            case 2: cout << "DANO (maior -> menor)\n"; break;
            case 3: cout << "VELOCIDADE (menor maxAtSp -> maior)\n"; break;
        }
        cout << "Total: " << list.size() << " monstros\n\n";

        cout << " HP | DMG | Vel (maxAtSp)\n";
        cout << "----+-----+--------------\n";
        for (const auto& m : list) {
            printf("%3d | %3d |      %3d\n", m.hp, m.dmg, m.atkSpeed);
        }

        cout << "\n[1] Ordenar por HP\n";
        cout << "[2] Ordenar por Dano\n";
        cout << "[3] Ordenar por Velocidade de ataque\n";
        cout << "[Q] Voltar ao jogo\n";
        cout << "Escolha: ";

        char choice = getKeyPress();
        if (choice == '1') {
            sort(list.begin(), list.end(), [](const KilledMonster& a, const KilledMonster& b) {
                return a.hp > b.hp;
            });
            sortMode = 1;
        } else if (choice == '2') {
            sort(list.begin(), list.end(), [](const KilledMonster& a, const KilledMonster& b) {
                return a.dmg > b.dmg;
            });
            sortMode = 2;
        } else if (choice == '3') {
            sort(list.begin(), list.end(), [](const KilledMonster& a, const KilledMonster& b) {
                return a.atkSpeed < b.atkSpeed;
            });
            sortMode = 3;
        } else if (choice == 'q' || choice == 'Q') {
            viewing = false;
        }
    }
}

// ============================================
// Ações de combate
// ============================================
void playerAttack(Player& player, Monster& monster, StatusEffects& status, MessageFlags& msg) {
    if (player.atSp == player.maxAtSp) {
        int effectiveAcc = player.acc;
        if (status.blindedActive) {
            effectiveAcc -= BLIND_PENALTY;
            msg.showBlinded = true;
            status.blindedActive = false;
        }
        if ((rand() % 100) < effectiveAcc) {
            monster.hp -= rand() % player.dmg;
        }
        player.atSp = 0;
    } else {
        player.atSp++;
    }
}

void debufferAction(Debuffer& deb, StatusEffects& status, MessageFlags& msg) {
    if (deb.atSp == deb.maxAtSp) {
        if ((rand() % 100) < deb.acc) {
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
    (void)msg;
    if (monster.atSp == monster.maxAtSp) {
        if ((rand() % 100) < monster.acc) {
            int damage = rand() % monster.dmg;
            if (status.weakenedActive) {
                damage -= debuffReduction;
                if (damage < 0) damage = 0;
                status.weakenedActive = false;
            }
            player.hp -= damage;
        }
        if ((rand() % 100) < monster.blindnessChance) {
            status.blindedActive = true;
        }
        monster.atSp = 0;
    } else {
        monster.atSp++;
    }
}

void healerAction(Healer& healer, Player& player, MessageFlags& msg) {
    if (healer.atSp == healer.maxAtSp) {
        if ((rand() % 100) < healer.acc) {
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
            cout << "   +" << (playerHp - prevPlayerHp) << " HP";
            msg.showHealed = false;
        }
        if (msg.showBlinded) {
            cout << "   BLINDED!";
            msg.showBlinded = false;
        }
        cout << endl;

        cout << "Monster : ";
        for (int i = 0; i < monsterHp; i++) cout << "o";
        if (msg.showWeakened) {
            cout << "   WEAKENED";
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
    player.hp = PLAYER_BASE_HP + monsterWins * PLAYER_HP_PER_MONSTER_WIN;
    player.dmg = PLAYER_BASE_DMG + playerWins * PLAYER_DMG_PER_WIN;
    player.maxAtSp = PLAYER_ATK_SPEED;
    player.atSp = 0;
    player.acc = PLAYER_BASE_ACC;
    player.x = MAP_WIDTH / 2;
    player.y = MAP_HEIGHT / 2;

    Healer healer;
    healer.healAmount = HEALER_BASE_AMOUNT + playerWins * HEALER_AMOUNT_PER_WIN;
    healer.maxAtSp = HEALER_ATK_SPEED;
    healer.atSp = 0;
    healer.acc = HEALER_BASE_ACC;

    Debuffer debuffer;
    debuffer.dmgReduction = DEBUFF_BASE_REDUCTION + playerWins * DEBUFF_REDUCTION_PER_WIN;
    debuffer.maxAtSp = DEBUFF_ATK_SPEED;
    debuffer.atSp = 0;
    debuffer.acc = DEBUFF_BASE_ACC;

    vector<Monster> monsters;
    spawnMonsters(monsters, INITIAL_MONSTER_COUNT, player.x, player.y);

    bool running = true;
    while (running) {
        drawMap(map, player, monsters);
        char input = getKeyPress();

        if (input == 'q' || input == 'Q') {
            running = false;
            break;
        }

        if (input == 'l' || input == 'L') {
            showKillList();
            continue;
        }

        movePlayer(player, input);

        bool alive = checkAndStartCombat(player, monsters, healer, debuffer);

        if (!alive) {
            cout << "Fim de jogo! Pressione qualquer tecla para continuar...";
            getKeyPress();
            player.hp = PLAYER_BASE_HP + monsterWins * PLAYER_HP_PER_MONSTER_WIN;
            player.dmg = PLAYER_BASE_DMG + playerWins * PLAYER_DMG_PER_WIN;
            player.atSp = 0;
            player.x = MAP_WIDTH / 2;
            player.y = MAP_HEIGHT / 2;

            healer.healAmount = HEALER_BASE_AMOUNT + playerWins * HEALER_AMOUNT_PER_WIN;
            debuffer.dmgReduction = DEBUFF_BASE_REDUCTION + playerWins * DEBUFF_REDUCTION_PER_WIN;

            monsters.clear();
            spawnMonsters(monsters, INITIAL_MONSTER_COUNT, player.x, player.y);
        }

        if (monsters.empty()) {
            spawnMonsters(monsters, INITIAL_MONSTER_COUNT + playerWins, player.x, player.y);
        }
    }

    return 0;
}