#ifndef CHARACTER_INCLUDED
#define CHARACTER_INCLUDED

#include "entity.h"
#include "entity_list.h"

#define SMALL_WIDTH 40 // Largura do personagem pequeno
#define SMALL_HEIGHT 48 // Altura do personagem pequeno
#define BIG_WIDTH 40 // Largura do personagem grande
#define BIG_HEIGHT 60 // Altura do personagem grande 

#define CHARACTER_SPEED 4.0
#define JUMP_VELOCITY -15.0
#define FIREBALL_COOLDOWN 30

struct character {
    struct entity* self; // Os atributos comuns de uma entidade
    int power; // Power up do personagem
    int star; // Se está sobre efeito da estrela
    int invincibility; // Por quantos frames a entidade ficará invencível
    int skillCooldown; // Tempo de recarga da fireball
    int currClock; // Quanto tempo resta até poder ser possivel lançar outra
};

// Cria um novo struct character*
struct character* newCharacter(struct entity* en);

// Atualiza os atributos do personagem, com base em inputs e atributos anteriores
struct tile* updateCharacter(struct character* character, struct tile** tiles,
ALLEGRO_SAMPLE** samples, struct entityList* fireballs);

// Realiza a interação entre todas as entidades, se acabou o jogo retorna 1
int entitiesInteract(struct character* character, struct tile** tiles, struct entityList* entities,
struct entityList* fireballs, ALLEGRO_SAMPLE** samples, int* score);

// Realiza a interação do personagem com o terreno
struct entity* tilesInteract(struct character* character, struct tile** tiles, ALLEGRO_SAMPLE** samples,
struct entityList* fireballs, int* score);

#endif