#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
using namespace std;

void printScreen(int playerHp, int monsterHp, int &prevplayerHp, int &prevmonsterHp, bool &healed, bool &weakened, bool &blinded)
{
	if(prevplayerHp != playerHp || prevmonsterHp != monsterHp)
	{
		int i =0;
		cout << "Player  : ";
		while(i < playerHp)
		{
			cout << "o";
			i++;	
		}	

		if(blinded == true)
		{
			cout << "    Player is blinded!";
			blinded = false;	
		}
		if(healed == true)
		{
			cout << "    Player was healed for " << (playerHp - prevplayerHp) << "!";
			healed = false;	
		}
		cout << endl;
		cout << "Monster : ";
		i = 0;
		while(i < monsterHp)
		{
			cout << "o";
			i++;	
		}
		if(weakened == true)
		{
			cout << "   Monster was weakened";
			weakened = false;	
		}
		prevmonsterHp = monsterHp;
		prevplayerHp = playerHp;
		cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		}
}

void player(int playerDmg, int playerMaxAtSp, int &playerAtSp,int playerAcc, int &monsterHp,int monsterDmg, bool blinded)
{		
	if (playerAtSp == playerMaxAtSp)
	{
		if(blinded == true)
		{	
			playerAcc -= monsterDmg*3;	
		}			
		if((rand() % 100) <= playerAcc)
		{
			monsterHp -= rand() % playerDmg;
		}			
		playerAtSp = 0;
	}
	else
	{
		playerAtSp++;
	}		
}	

void monster(int monsterDmg, int monsterMaxAtSp, int &monsterAtSp,int monsterAcc, int blindnessChance, int &playerHp,bool &blinded, bool &weakened,int debufferDmg)
{
	if (monsterAtSp == monsterMaxAtSp)
	{
		if((rand() % 100) <= monsterAcc)
		{
			if(weakened == true)
			{
				int notnegative;
				notnegative = rand() % monsterDmg-debufferDmg;
				if(notnegative < 0)
				{
					notnegative = 0;
				}
				playerHp -= notnegative;				
			}
			playerHp -= rand() % monsterDmg;
		}
		if((rand() % 100) <= blindnessChance)
		{
			blinded = true;
		}		
		monsterAtSp = 0;
	}
	else 
	{
		monsterAtSp++;
	}
}

void healer(int &hp,int heal,int healerMaxAtSp, int &healerAtSp, int healerAcc, bool &healed)
{
	if (healerAtSp == healerMaxAtSp)
	{				
		if((rand() % 100) <= healerAcc)
		{
			hp += heal;
			healed = true;
		}
		healerAtSp = 0;
	}
	else 
	{
		healerAtSp++;
	}		
}	

void debuffer(int debufferMaxAtSp, int &debufferAtSp, int debufferAcc, bool &weakened)
{
	if (debufferAtSp == debufferMaxAtSp)
	{			
		if((rand() % 100) <= debufferAcc)
		{
			weakened = true;
		}
		debufferAtSp = 0;
	}
	else 
	{
		debufferAtSp++;
	}		
}	
	
int main(){
	srand(time(0));
	int key, playerWins = 0, monsterWins = 0;	
	cout << "Type 1 to start or 0 to close\n: ";
	cin >> key;	
	while (key != 1)
	{
		if(key == 0)
		{
			return 0 ;
		}
		cout << "Please type 1 to start or 0 to close\n: ";
		cin >> key;
	}
	while (key != 0)
	{
		cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		int playerHp = 30+monsterWins, prevplayerHp = 0, playerDmg = 2+(monsterWins/5), playerMaxAtSp = 4, playerAtSp = 0,playerAcc = 90;
		int monsterHp = 50+playerWins, prevmonsterHp = 0, monsterDmg = 3+(playerWins/5), monsterMaxAtSp = 10, monsterAtSp = 0, monsterAcc = 90, monsterDebCh = 40+playerWins;
		int healerHeal = 3+(monsterWins/10), healerMaxAtSp = 5, healerAtSp = 0, healerAcc = 5;
		int debufferDmg = 3, debufferMaxAtSp = 10, debufferAtSp = 0, debufferAcc = 15;
		int i = 0;
		bool healed = false, weakened = false, blinded = false; 
		while (playerHp >= 1 && monsterHp >= 1)
		{		
			player(playerDmg, playerMaxAtSp, playerAtSp, playerAcc, monsterHp, monsterDmg, blinded);
			if(monsterHp <= 0)
			{
				break;
			}
			monster(monsterDmg, monsterMaxAtSp, monsterAtSp, monsterAcc, monsterDebCh, playerHp, blinded, weakened, debufferDmg);
			healer(playerHp, healerHeal, healerMaxAtSp, healerAtSp, healerAcc, healed);
			debuffer(debufferMaxAtSp, debufferAtSp, debufferAcc, weakened);
			printScreen(playerHp,monsterHp,prevplayerHp,prevmonsterHp, healed,weakened,blinded);
			this_thread::sleep_for(chrono::milliseconds(70));		
		}
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
		cout << "\n\nPlayer wins :" << playerWins << " Monster Wins :" << monsterWins;
		this_thread::sleep_for(chrono::milliseconds(2000));		
			
	}
	return 0; 	
 }
