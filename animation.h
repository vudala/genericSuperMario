#ifndef ANIMATION_INCLUDED
#define ANIMATION_INCLUDED

#include "game.h"
#include "entity.h"
#include "level.h"
#include "entity_list.h"

#define FRAMES_N 4 // Quantos frames tem a animação
#define FRAME_DURATION 10 // Duração padrão de cada frame da animação
#define IDLE_FRAME 1 // Indice padrão do frame de IDLE
#define JUMP_FRAME 3 // Indice padrão do frame de JUMPING

#define ENTITY_SPRITES_N 7
typedef enum {
    MAIN_CHARACTER_SPRITE = 0,
    GOOMBA_SPRITE,
    TURTLE_SPRITE,
    FLOWER_SPRITE,
    STAR_SPRITE,
    SHELL_SPRITE,
    MUSHROOM_SPRITE,
    COIN_SPRITE
} EntitySpritesID;

#define TILES_SPRITES_N 9
typedef enum {
    EMPTY_BLOCK_SPRITE = 0,
    BRICK_BLOCK_SPRITE,
    HARD_BRICK_BLOCK_SPRITE,
    PIPE_BLOCK_SPRITE,
    PIPE_TOP_BLOCK_SPRITE,
    COIN_BLOCK_SPRITE,
    STAR_BLOCK_SPRITE,
    MUSHROOM_BLOCK_SPRITE,
    FLOWER_BLOCK_SPRITE
} TileSpritesID;

struct animation {
    int whichSprite; // ID da sprite que deve utilizar
    int lastFrame; // Indice do último frame da animação
    int frameDuration; // Quantos clocks duram um frame
    int currentClock; // Clock atual
    int currentFrame; // Frame atual
    int reset; // Se deve começar a resetar os frames
};

ALLEGRO_BITMAP** loadTileSprites();

ALLEGRO_BITMAP** loadMainFrames();

ALLEGRO_BITMAP** loadGoombaFrames();

ALLEGRO_BITMAP*** loadSprites();

// Retorna o ID da sprite correspondente ao tipo da entidade
int entitySpriteID(char type);

// Retorna o ID da sprite correspondente ao tipo da tile
int tileSpriteID(char type);

// Cria um conjunto de atributos utilizados para fazer uma animação
struct animation* newAnimation(int whichSprite, int lFrame, int fDuration);

// Desenha uma entidade qualquer utilizando um deslocamento de câmera
void drawEntity(struct entity* en, int* offset, ALLEGRO_BITMAP*** sprites);

// Desenha as tiles
void drawTiles(struct tile** tiles, ALLEGRO_BITMAP** sprites, int* offset);

#endif