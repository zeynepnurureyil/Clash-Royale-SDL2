#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_UNITS 30  // Ordu yağmasını engellemek için birim sınırını düşürdük, taktik öne çıktı
#define MAX_PARTICLES 100
#define MAX_WALLS 5

typedef enum { TEAM_BLUE, TEAM_RED } Team;
typedef enum { CARD_KNIGHT, CARD_GOBLIN, CARD_PEKKA } CardType;
typedef enum { STATE_PLAYING, STATE_GAME_OVER } GameState;

typedef struct {
    float x, y;
    float speed;
    int hp, max_hp;
    int damage;
    int attack_cooldown;
    bool active;
    Team team;
    CardType type;
    float anim_timer;
    int target_unit_idx; // Akıllı hedefleme için düşman asker indeksi
} Unit;

typedef struct {
    float x, y;
    int hp, max_hp;
    bool is_king;
    bool active;
    Team team;
} Tower;

typedef struct {
    float x, y;
    int hp, max_hp;
    bool active;
    SDL_Rect rect;
} Wall;

typedef struct {
    float x, y;
    float vx, vy;
    int life;
    SDL_Color color;
    bool active;
} Particle;

// Global Değişkenler
extern Unit units[MAX_UNITS];
extern Tower towers[6];
extern Wall walls[MAX_WALLS];
extern Particle particles[MAX_PARTICLES];
extern float blue_elixir;
extern float red_elixir;
extern CardType selected_card;
extern GameState game_state;
extern Team winner_team;
extern int current_level;
extern int screen_shake_timer;

extern const char* active_chat_message;
extern int chat_message_timer;

void init_clash_arena();
void spawn_card(Team team, CardType type, float x, float y);
void spawn_wall(float mx, float my);
void update_game_logic();
void render_arena(SDL_Renderer* renderer);
void trigger_chat_message(const char* message);
void create_explosion(float x, float y, SDL_Color color, int force);

#endif