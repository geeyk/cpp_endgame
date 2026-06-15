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

´´´cpp
const int MAP_WIDTH  = 30;
const int MAP_HEIGHT = 15;
```
