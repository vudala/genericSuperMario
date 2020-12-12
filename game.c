#include "game.h"
#include "level.h"
#include "entity.h"
#include "animation.h"
#include "entity_list.h"
#include "utils.h"
#include "character.h"
#include "sound.h"

#include <stdio.h>

void gameInit(){
    mustInit(al_init(), "allegro");
    mustInit(al_install_keyboard(), "keyboard");
    mustInit(al_init_image_addon(), "image addon");
    mustInit(al_install_audio(), "audio");
    mustInit(al_init_acodec_addon(), "audio codecs");
    mustInit(al_reserve_samples(16), "reserve samples");

    srand((unsigned) time(NULL));

    timer = al_create_timer(1.0 / FPS);
    mustInit(timer, "timer");

    queue = al_create_event_queue();
    mustInit(queue, "queue");

    disp = al_create_display(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    mustInit(disp, "display");

    font = al_create_builtin_font();
    mustInit(font, "font");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    al_start_timer(timer);

    memset(key, 0, sizeof(key));
}

void updateCameraOffset(int* offset, struct character* character){
    *offset = -(character->self->x - (VIRTUAL_WIDTH / 2));
    if(*offset > -TILE_WIDTH)
        *offset = -TILE_WIDTH;
    else if (character->self->x + (VIRTUAL_WIDTH / 2) > (TILE_WIDTH)* (MAP_WIDTH - 1))
        *offset = -(TILE_WIDTH * (MAP_WIDTH - 1) - VIRTUAL_WIDTH);
}

int gamePlay(int* score, ALLEGRO_BITMAP** screens, ALLEGRO_SAMPLE** samples){
    
    ALLEGRO_BITMAP*** entitySprites = loadEntitySprites();
    ALLEGRO_BITMAP** tileSprite = loadTileSprites();
    ALLEGRO_BITMAP* background = al_load_bitmap("resources/sprites/background.png");

    struct entityList* entities = malloc(sizeof(struct entityList));
    mustInit(entities, "entities");
    createList(entities);

    struct entityList* fireballs = malloc(sizeof(struct entityList));
    mustInit(fireballs, "fireballs");
    createList(fireballs);

    struct tile** tiles = loadLevel("resources/database/level.txt", entities, entitySprites);
    
    struct character* character = newCharacter(newEntity(MAIN_SMALL, 120, 510,
        SMALL_WIDTH, SMALL_HEIGHT, RIGHT, newAnimation(SMALL_CHAR_SPRITE), -1));
        
    bool done = false;
    bool redraw = true;
    int newState = DESTROY;

    int skillCooldown = 30; // Tempo de recarga da fireball
    int currClock = 0; // Quanto tempo resta até poder ser possivel lançar outra

    int offset = 0;
    struct entity* newEn = NULL;
    
    for(;;)
    {
        al_wait_for_event(queue, &event);
        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                // Se estiver sob poder da flor e o tempo de recarga esgotado
                // Ao apertar espaço solta uma bola de fogo
                if (key[ALLEGRO_KEY_SPACE] && currClock < 1 && character->power == FLOWER_POWER){
                    currClock = skillCooldown;
                    addFireball(fireballs, character->self, entitySprites);
                    al_play_sample(samples[FIREBALL_SAMPLE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                currClock -= 1;
                
                newEn = tilesInteract(character, tiles, samples, score);
                if(newEn) insertEntity(entities, newEn);

                // Se colidiu com outra entidade inimiga sem matá-la, termina o jogo
                if(entitiesInteract(character, tiles, entities, fireballs, samples, score)){
                    newState = drawScreen(screens, SCORES_SCREEN, samples, score);
                    done = true;
                    break;
                }
                
                updateCameraOffset(&offset, character);
               
                if(key[ALLEGRO_KEY_ESCAPE]) {
                    newState = DESTROY;
                    done = true;
                    break;
                }

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++) key[i] &= KEY_SEEN;

                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                if(key[ALLEGRO_KEY_H]) // Se apertou H, desenha a tela de ajuda
                    drawScreen(screens, HELP_SCREEN, samples, NULL);
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                newState = DESTROY;
                done = true;
                break;
        }

        if(done) break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(127, 127, 127));

            // Desenha o mapa
            al_draw_bitmap(background, 0, 0, 0);
            drawTiles(tiles, tileSprite, &offset);

            // Pintar de branco não interfere na cor
            ALLEGRO_COLOR color = al_map_rgb(255,255,255);

            // Desenha as diversas entidades
            drawEntities(fireballs, &offset, entitySprites, color);
            drawEntities(entities, &offset, entitySprites, color);

            // Desenha o personagem principal
            if(character->star) // Se estiver no modo estrela, desenha em cores aleatórias
                color = al_map_rgb(rand() % RGB_MAX, rand() % RGB_MAX, rand() % RGB_MAX);
                
            drawEntity(character->self, &offset, entitySprites, color);
            
            al_flip_display();
            redraw = false;
        }
    }

    for(int i = 0; i < TILES_N; i++) al_destroy_bitmap(tileSprite[i]);
    free(tileSprite);
    free(tiles[0]);
    free(tiles);
    destroyEntity(character->self);
    destroyList(entities);
    
    for(int i = 0; i < ENTITY_SPRITES_N; i++)
        for(int j = 0; j < FRAMES_N; j++)
            al_destroy_bitmap(entitySprites[i][j]);

    return newState;
}

void gameDestroy(ALLEGRO_BITMAP** screens, ALLEGRO_SAMPLE** samples){
    for(int i = 0; i < SAMPLES_N; i++)
    al_destroy_sample(samples[i]);

    for(int i = 0; i < SCREENS_N; i++)
        al_destroy_bitmap(screens[i]);
    
    al_destroy_display(disp);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}