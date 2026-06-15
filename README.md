# cpp_endgame
Último jogo feito durante o segundo semestre da faculdade.
## Arquivo de download do [simulador_II.exe](https://github.com/geeyk/cpp_endgame/raw/main/exe_files/simulador_II)
# Mapa
## Caracteres e aparência
Como cada espaço do mapa é representado é ditado por cractéres definidos, sendo eles:
### Bordas:
```txt
+
```
### Espaços vazios:
```txt
.
```
### Monstros:
```txt
M
```
### Jogador:
```txt
P
```
### Imagem de exemplo:
![Imagem de exemplo do mapa funcionando, aberto em uma janela do Konsole no Nobara Linux](material/tela_do_mapa.png)

## Testes do mapa descritos em #2
- [x] Identificação Visual
  - [x] Jogador representado por caractere diferente (ex: 'J')
  - [x] Monstros representados por caractere diferente (ex: 'M')
  - [x] Terreno vazio claramente identificado
     
## Como funciona:
### Definição do mapa
O mapa é definido seguindo constantes declaradas nas linhas 77-78 do arquivo [simulador_II.cpp]()

```cpp
const int MAP_WIDTH  = 30;
const int MAP_HEIGHT = 15;
```


# Funcções extras usadas no trabalho
## `_getch()`:
### Windows
Para evitar que o usuário tenha que clicar em ***Enter*** toda a vez após uma tecla de movimentação para mover-se, foi utilizada a biblioteca `conio.h`, que lê o caractere imediatamente sem eco na tela.
### Linux/Mac
Como eu uso Linux para desenvolver, tive que implementar 2 alternativas para ler o caractere pressionado:
Foi necessário desabilitar o modo canônico e o eco do terminal usando as funções `tcgetattr` e `tcsetattr` do cabeçalho `<termios.h`.
Essa adaptação acontece dentro da função `getKeyPress()`.
## `system("cls")` ou `system("clear")`:
