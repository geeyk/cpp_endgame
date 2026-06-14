#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include "structs.h"
#include "combat.h"

using namespace std;

// ============================================================================
// MAIN - SIMULADOR DE COMBATE I (com STRUCTS)
// ============================================================================

int main()
{
    srand(time(0));
    
    int key, playerWins = 0, monsterWins = 0;
    
    cout << "=== SIMULADOR DE COMBATE I ===" << endl;
    cout << "Type 1 to start or 0 to close\n: ";
    cin >> key;
    
    while (key != 1)
    {
        if(key == 0) return 0;
        cout << "Please type 1 to start or 0 to close\n: ";
        cin >> key;
    }
    
    // ====== LOOP DE COMBATES ======
    while (key != 0)
    {
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        
        // ====== INICIALIZA JOGADOR ======
        Jogador jogador;
        jogador.hp = 30 + monsterWins;
        jogador.hpMax = 30 + monsterWins;
        jogador.dano = 5 + (monsterWins / 5);
        jogador.acuracia = 90;
        jogador.speedAtaque = 4;
        jogador.speedAtual = 0;
        jogador.cego = false;
        
        // ====== INICIALIZA MONSTRO ======
        Monstro monstro;
        monstro.id = 1;
        monstro.hp = 50 + playerWins;
        monstro.hpMax = 50 + playerWins;
        monstro.dano = 3 + (playerWins / 5);
        monstro.acuracia = 90;
        monstro.speedAtaque = 10;
        monstro.speedAtual = 0;
        monstro.fraco = false;
        monstro.chanceCegueira = 40 + playerWins;
        
        // ====== INICIALIZA ALIADOS ======
        int healerHeal = 3 + (monsterWins / 10);
        int healerSpeed = 5;
        int healerSpeedAtual = 0;
        int healerAcc = 10;
        bool healed = false;
        
        int debufferSpeed = 10;
        int debufferSpeedAtual = 0;
        int debufferAcc = 15;
        int debufferDmg = 3;
        
        // ====== LOOP DE COMBATE ======
        while (jogador.hp >= 1 && monstro.hp >= 1)
        {
            // Turnos dos combatentes
            playerAttack(jogador, monstro);
            
            if(monstro.hp <= 0) break;
            
            monsterAttack(monstro, jogador, debufferDmg);
            healer(jogador, healerHeal, healerSpeed, healerSpeedAtual, healerAcc, healed);
            debuffer(monstro, debufferSpeed, debufferSpeedAtual, debufferAcc);
            
            // Renderiza estado
            printScreen(jogador, monstro);
            
            this_thread::sleep_for(chrono::milliseconds(70));
        }
        
        // ====== RESULTADO ======
        if (jogador.hp <= 0)
        {
            cout << "Monster wins";
            monsterWins++;
        }
        else
        {
            cout << "Player wins";
            playerWins++;
        }
        
        cout << "\n\nPlayer wins : " << playerWins << " | Monster Wins : " << monsterWins;
        this_thread::sleep_for(chrono::milliseconds(2000));
        
        cout << "\n\nType 1 to continue or 0 to exit: ";
        cin >> key;
    }
    
    return 0;
}
