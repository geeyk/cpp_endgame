#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
using namespace std;

// ============================================================================
// FUNÇÕES DE RENDERIZAÇÃO
// ============================================================================

/// Renderiza a tela do combate com barras de HP e status effects
/// Limpa automaticamente quando há mudanças
void printScreen(int playerHp, int monsterHp, int &prevplayerHp, int &prevmonsterHp, 
                 bool &healed, bool &weakened, bool &blinded)
{
    // Só atualiza se houver mudança de HP
    if(prevplayerHp != playerHp || prevmonsterHp != monsterHp)
    {
        // Limpa tela (28 quebras de linha)
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        
        // Renderiza barra de HP do jogador
        cout << "Player  : ";
        int i = 0;
        while(i < playerHp)
        {
            cout << "o";
            i++;	
        }	
        
        // Mostra efeitos do jogador (cegueira)
        if(blinded == true)
        {
            cout << "    [BLINDED!]";
            blinded = false;	
        }
        
        // Mostra cura do jogador
        if(healed == true)
        {
            cout << "    [+HEALED " << (playerHp - prevplayerHp) << " HP]";
            healed = false;	
        }
        cout << endl;
        
        // Renderiza barra de HP do monstro
        cout << "Monster : ";
        i = 0;
        while(i < monsterHp)
        {
            cout << "o";
            i++;	
        }
        
        // Mostra efeitos do monstro (fraqueza)
        if(weakened == true)
        {
            cout << "   [WEAKENED]";
            weakened = false;	
        }
        
        // Atualiza valores anteriores para próxima verificação
        prevmonsterHp = monsterHp;
        prevplayerHp = playerHp;
        cout << endl;
    }
}

// ============================================================================
// FUNÇÕES DE COMBATE
// ============================================================================

/// Executa o turno do jogador
/// - Acumula pontos de ação até poder atacar
/// - Causa dano aleatório com chance de acerto baseada em acurácia
void player(int playerDmg, int playerMaxAtSp, int &playerAtSp, int playerAcc, 
            int &monsterHp, int monsterDmg, bool blinded)
{		
    if (playerAtSp == playerMaxAtSp)
    {
        // Aplica penalidade de cegueira
        int accuracyAdjusted = playerAcc;
        if(blinded == true)
        {	
            accuracyAdjusted -= (monsterDmg * 3);
            
            // Protege acurácia de ficar negativa
            if(accuracyAdjusted < 0) accuracyAdjusted = 0;
        }			
        
        // Verifica acerto (0-100)
        if((rand() % 100) <= accuracyAdjusted)
        {
            // Causa dano aleatório (0 até playerDmg)
            int damageDealt = rand() % playerDmg + 1;
            monsterHp -= damageDealt;
        }			
        
        // Reseta contador de ação
        playerAtSp = 0;
    }
    else
    {
        // Incrementa contador de ação
        playerAtSp++;
    }		
}	

/// Executa o turno do monstro
/// - Similar ao jogador
/// - Se enfraquecido, causa menos dano
/// - Pode cegar o jogador
void monster(int monsterDmg, int monsterMaxAtSp, int &monsterAtSp, int monsterAcc, 
             int blindnessChance, int &playerHp, bool &blinded, bool &weakened, 
             int debufferDmg)
{
    if (monsterAtSp == monsterMaxAtSp)
    {
        // Verifica se ataque acerta
        if((rand() % 100) <= monsterAcc)
        {
            // CORREÇÃO: Calcula dano com fraqueza ANTES de aplicar
            int damageDealt = rand() % monsterDmg + 1;
            
            if(weakened == true)
            {
                // Reduz dano se monstro está fraco
                damageDealt -= debufferDmg;
                
                // Protege dano de ficar negativo
                if(damageDealt < 0)
                {
                    damageDealt = 0;
                }
            }
            
            // Aplica dano UMA VEZ (bug original corrigido)
            playerHp -= damageDealt;
        }
        
        // Chance de cegar o jogador
        if((rand() % 100) <= blindnessChance)
        {
            blinded = true;
        }		
        
        // Reseta contador de ação
        monsterAtSp = 0;
    }
    else 
    {
        // Incrementa contador de ação
        monsterAtSp++;
    }
}

/// Executa turno do aliado curador
/// - Cura o jogador com pequena chance
void healer(int &hp, int heal, int healerMaxAtSp, int &healerAtSp, 
            int healerAcc, bool &healed)
{
    if (healerAtSp == healerMaxAtSp)
    {				
        // Verifica se cura acerta (baixa chance, mas acontece)
        if((rand() % 100) <= healerAcc)
        {
            hp += heal;
            healed = true;
        }
        
        // Reseta contador de ação
        healerAtSp = 0;
    }
    else 
    {
        // Incrementa contador de ação
        healerAtSp++;
    }		
}	

/// Executa turno do aliado debuffer
/// - Enfraquece o monstro (reduz dano dele)
void debuffer(int debufferMaxAtSp, int &debufferAtSp, int debufferAcc, 
              bool &weakened)
{
    if (debufferAtSp == debufferMaxAtSp)
    {			
        // Verifica se debuff acerta
        if((rand() % 100) <= debufferAcc)
        {
            weakened = true;
        }
        
        // Reseta contador de ação
        debufferAtSp = 0;
    }
    else 
    {
        // Incrementa contador de ação
        debufferAtSp++;
    }		
}	

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main()
{
    // Inicializa seed de números aleatórios
    srand(time(0));
    
    int key, playerWins = 0, monsterWins = 0;	
    
    // Menu inicial
    cout << "Type 1 to start or 0 to close\n: ";
    cin >> key;	
    
    // Loop enquanto usuário não digita 0
    while (key != 1)
    {
        if(key == 0)
        {
            return 0;
        }
        cout << "Please type 1 to start or 0 to close\n: ";
        cin >> key;
    }
    
    // Loop de combates
    while (key != 0)
    {
        // Limpa tela
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        
        // ====== INICIALIZA STATS DO COMBATE ======
        // HP aumenta com derrotas anteriores (dificuldade progressiva)
        int playerHp = 30 + monsterWins;
        int prevplayerHp = 0;
        int playerDmg = 5 + (monsterWins / 5);           // CORRIGIDO: Era 2, aumentado para 5
        int playerMaxAtSp = 4;
        int playerAtSp = 0;
        int playerAcc = 90;
        
        // Monstro começa com mais HP a cada vitória do jogador
        int monsterHp = 50 + playerWins;
        int prevmonsterHp = 0;
        int monsterDmg = 3 + (playerWins / 5);
        int monsterMaxAtSp = 10;
        int monsterAtSp = 0;
        int monsterAcc = 90;
        int monsterDebCh = 40 + playerWins;
        
        // Stats dos aliados
        int healerHeal = 3 + (monsterWins / 10);
        int healerMaxAtSp = 5;
        int healerAtSp = 0;
        int healerAcc = 10;                 // CORRIGIDO: Era 5, aumentado para 10
        
        int debufferDmg = 3;
        int debufferMaxAtSp = 10;
        int debufferAtSp = 0;
        int debufferAcc = 15;
        
        // Status effects
        bool healed = false;
        bool weakened = false;
        bool blinded = false;
        
        // ====== LOOP DE COMBATE ======
        while (playerHp >= 1 && monsterHp >= 1)
        {		
            // Executa turnos de cada participante
            player(playerDmg, playerMaxAtSp, playerAtSp, playerAcc, monsterHp, monsterDmg, blinded);
            
            // Verifica se monstro morreu
            if(monsterHp <= 0)
            {
                break;
            }
            
            // Turno do monstro
            monster(monsterDmg, monsterMaxAtSp, monsterAtSp, monsterAcc, monsterDebCh, 
                    playerHp, blinded, weakened, debufferDmg);
            
            // Aliados
            healer(playerHp, healerHeal, healerMaxAtSp, healerAtSp, healerAcc, healed);
            debuffer(debufferMaxAtSp, debufferAtSp, debufferAcc, weakened);
            
            // Renderiza estado do combate
            printScreen(playerHp, monsterHp, prevplayerHp, prevmonsterHp, healed, weakened, blinded);
            
            // Aguarda 70ms antes do próximo turno (controla velocidade)
            this_thread::sleep_for(chrono::milliseconds(70));		
        }
        
        // ====== DETERMINA VENCEDOR ======
        if (playerHp <= 0)
        {
            cout << "Monster wins";
            monsterWins++;
        }
        else 
        {
            cout << "Player wins";
            playerWins++;
        }	
        
        // Mostra placar
        cout << "\n\nPlayer wins : " << playerWins << " | Monster Wins : " << monsterWins;
        
        // Aguarda 2 segundos antes do próximo combate
        this_thread::sleep_for(chrono::milliseconds(2000));
        
        // Pergunta se continua
        cout << "\n\nType 1 to continue or 0 to exit: ";
        cin >> key;
    }
    
    return 0;
}

// ============================================================================
// NOTAS SOBRE CORREÇÕES FEITAS
// ============================================================================
// 
// 1. CORRIGIDO: Bug de dano duplo do monstro
//    - Antes: Aplicava dano com fraqueza, depois aplicava novamente
//    - Depois: Calcula dano final, depois aplica uma vez
//
// 2. CORRIGIDO: Cegueira deixando acurácia negativa
//    - Antes: playerAcc podia ficar < 0
//    - Depois: accuracyAdjusted tem limite mínimo de 0
//
// 3. CORRIGIDO: Dano muito baixo
//    - Antes: playerDmg = 2 (causava 0-1 dano)
//    - Depois: playerDmg = 5 (causa 1-5 dano)
//
// 4. MELHORADO: Chance de cura
//    - Antes: healerAcc = 5 (quase nunca curava)
//    - Depois: healerAcc = 10 (melhor chance)
//
// 5. MELHORADO: Estrutura de código
//    - Comentários explicativos
//    - Variáveis mais bem organizadas
//    - Lógica mais clara
//
// ============================================================================
