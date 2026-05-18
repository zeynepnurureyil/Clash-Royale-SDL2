#include "game.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

Unit units[MAX_UNITS];
Tower towers[6];
Wall walls[MAX_WALLS];
Particle particles[MAX_PARTICLES];
float blue_elixir = 5.0f;
float red_elixir = 5.0f;
CardType selected_card = CARD_KNIGHT;
int ai_timer = 0;
GameState game_state = STATE_PLAYING;
Team winner_team = TEAM_BLUE;
int current_level = 1;
int screen_shake_timer = 0;

const char* active_chat_message = NULL;
int chat_message_timer = 0;

void create_explosion(float x, float y, SDL_Color color, int force) {
    int count = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].active = true;
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = ((rand() % 100) - 50) / 6.0f;
            particles[i].vy = ((rand() % 100) - 50) / 6.0f;
            particles[i].life = 15 + rand() % 15;
            particles[i].color = color;
            count++;
            if (count > force) break;
        }
    }
}

void init_clash_arena() {
    game_state = STATE_PLAYING;
    blue_elixir = 5.0f;
    red_elixir = 5.0f;
    ai_timer = 0;
    screen_shake_timer = 0;
    
    for (int i = 0; i < MAX_UNITS; i++) units[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;
    for (int i = 0; i < MAX_WALLS; i++) walls[i].active = false;

    // Oyuncu Kuleleri (Sol)
    towers[0] = (Tower){ 70, SCREEN_HEIGHT/2, 3000, 3000, true, true, TEAM_BLUE };
    towers[1] = (Tower){ 180, 150, 1600, 1600, false, true, TEAM_BLUE };
    towers[2] = (Tower){ 180, 450, 1600, 1600, false, true, TEAM_BLUE };

    // Düşman Kuleleri (Sağ)
    int hp_bonus = (current_level - 1) * 500;
    towers[3] = (Tower){ 730, SCREEN_HEIGHT/2, 3000 + hp_bonus, 3000 + hp_bonus, true, true, TEAM_RED };
    towers[4] = (Tower){ 620, 150, 1600 + hp_bonus, 1600 + hp_bonus, false, true, TEAM_RED };
    towers[5] = (Tower){ 620, 450, 1600 + hp_bonus, 1600 + hp_bonus, false, true, TEAM_RED };
}

void trigger_chat_message(const char* message) {
    active_chat_message = message;
    chat_message_timer = 90;
}

void spawn_wall(float mx, float my) {
    if (blue_elixir < 4.0f || game_state == STATE_GAME_OVER) return;
    if (mx >= SCREEN_WIDTH / 2 - 20) return;

    for (int i = 0; i < MAX_WALLS; i++) {
        if (!walls[i].active) {
            walls[i].active = true;
            walls[i].x = mx; walls[i].y = my;
            walls[i].max_hp = 600; walls[i].hp = 600;
            walls[i].rect = (SDL_Rect){ (int)mx - 12, (int)my - 25, 24, 50 };
            blue_elixir -= 4.0f;
            create_explosion(mx, my, (SDL_Color){149, 165, 166, 255}, 8);
            return;
        }
    }
}

void spawn_card(Team team, CardType type, float x, float y) {
    if (game_state == STATE_GAME_OVER) return;

    int cost = (type == CARD_PEKKA) ? 6 : ((type == CARD_GOBLIN) ? 2 : 3);
    if (team == TEAM_BLUE && blue_elixir < cost) return;
    if (team == TEAM_RED && red_elixir < cost) return;

    if (team == TEAM_BLUE) blue_elixir -= cost;
    else red_elixir -= cost;

    for (int i = 0; i < MAX_UNITS; i++) {
        if (!units[i].active) {
            units[i].active = true;
            units[i].x = x; units[i].y = y;
            units[i].team = team; units[i].type = type;
            units[i].attack_cooldown = 0;
            units[i].anim_timer = 0;
            units[i].target_unit_idx = -1;

            if (type == CARD_PEKKA) { units[i].speed = 0.4f; units[i].hp = 900; units[i].damage = 75; }
            else if (type == CARD_GOBLIN) { units[i].speed = 1.5f; units[i].hp = 120; units[i].damage = 18; }
            else { units[i].speed = 0.8f; units[i].hp = 400; units[i].damage = 32; }
            
            units[i].max_hp = units[i].hp;
            return;
        }
    }
}

void update_game_logic() {
    if (screen_shake_timer > 0) screen_shake_timer--;

    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].life--;
            if (particles[i].life <= 0) particles[i].active = false;
        }
    }

    if (game_state == STATE_GAME_OVER) return;

    if (blue_elixir < 10.0f) blue_elixir += 0.012f;
    if (red_elixir < 10.0f) red_elixir += 0.012f;

    if (chat_message_timer > 0) chat_message_timer--;

    // Yapay Zeka Karar Mekanizması
    ai_timer++;
    if (ai_timer % 180 == 0) {
        float r_y = (rand() % 2 == 0) ? 150 : 450;
        bool player_has_pekka = false;
        for(int u=0; u<MAX_UNITS; u++) {
            if(units[u].active && units[u].team == TEAM_BLUE && units[u].type == CARD_PEKKA) player_has_pekka = true;
        }
        if (player_has_pekka) spawn_card(TEAM_RED, CARD_GOBLIN, 620, r_y);
        else spawn_card(TEAM_RED, rand() % 3, 620, r_y);
    }

    // ⚔️ AKILLI DÜELLO VE SAVAŞ DÖNGÜSÜ
    for (int i = 0; i < MAX_UNITS; i++) {
        if (!units[i].active) continue;

        units[i].anim_timer += 0.1f;

        // Her adımda en yakındaki karşı takım askerini tara (Görüş Menzili: 140 piksel)
        float min_enemy_unit_dist = 140.0f; 
        units[i].target_unit_idx = -1;

        for (int u = 0; u < MAX_UNITS; u++) {
            if (units[u].active && units[u].team != units[i].team) {
                float d = sqrt(pow(units[u].x - units[i].x, 2) + pow(units[u].y - units[i].y, 2));
                if (d < min_enemy_unit_dist) {
                    min_enemy_unit_dist = d;
                    units[i].target_unit_idx = u; // Hedef kilitlendi!
                }
            }
        }

        // 🔥 DURUM 1: EĞER YAKINDA DÜŞMAN ASKERİ VARSA (BİRBİRLERİNİ ÖLDÜRME MANTIĞI)
        if (units[i].target_unit_idx != -1 && units[units[i].target_unit_idx].active) {
            Unit* enemy = &units[units[i].target_unit_idx];
            float edx = enemy->x - units[i].x;
            float edy = enemy->y - units[i].y;
            float edist = sqrt(edx*edx + edy*edy);

            if (edist > 22.0f) { // Yakınlaş
                units[i].x += (edx / edist) * units[i].speed;
                units[i].y += (edy / edist) * units[i].speed;
            } else { // Saldır!
                if (units[i].attack_cooldown <= 0) {
                    enemy->hp -= units[i].damage;
                    units[i].attack_cooldown = 45;
                    
                    // Kan/Kıvılcım parçacık efekti
                    SDL_Color blood = {231, 76, 60, 255};
                    create_explosion(enemy->x, enemy->y, blood, 4);

                    // Eğer düşman askeri öldüyse
                    if (enemy->hp <= 0) {
                        enemy->active = false;
                        create_explosion(enemy->x, enemy->y, (SDL_Color){236, 240, 241, 255}, 8); // Yok olma efekti
                        units[i].target_unit_idx = -1; // Hedefi boşa çıkar
                    }
                }
            }
            if (units[i].attack_cooldown > 0) units[i].attack_cooldown--;
            continue; // Diğer kule hedeflerini es geç, çünkü şu an askerle savaşıyor!
        }

        // DURUM 2: DÜŞMAN DUVARINA TAKILMA KONTROLÜ (Düşmanlar için geçerli)
        int intercepted_wall_idx = -1;
        if (units[i].team == TEAM_RED) {
            for (int w = 0; w < MAX_WALLS; w++) {
                if (walls[w].active && fabs(units[i].y - walls[w].y) < 35 && fabs(units[i].x - walls[w].x) < 25) {
                    intercepted_wall_idx = w;
                    break;
                }
            }
        }

        if (intercepted_wall_idx != -1) {
            if (units[i].attack_cooldown <= 0) {
                walls[intercepted_wall_idx].hp -= units[i].damage;
                units[i].attack_cooldown = 50;
                if (walls[intercepted_wall_idx].hp <= 0) walls[intercepted_wall_idx].active = false;
            }
            if (units[i].attack_cooldown > 0) units[i].attack_cooldown--;
            continue;
        }

        // DURUM 3: ETRAFTA ASKER YOKSA KULELERE YÜRÜME MANTIĞI
        float target_x = (units[i].team == TEAM_BLUE) ? SCREEN_WIDTH : 0;
        float target_y = units[i].y;
        float close_d = 99999.0f;
        int target_tower_idx = -1;

        for (int t = 0; t < 6; t++) {
            if (towers[t].active && towers[t].team != units[i].team) {
                if (towers[t].is_king && (towers[units[i].team == TEAM_BLUE ? 4 : 1].active || towers[units[i].team == TEAM_BLUE ? 5 : 2].active)) {
                    continue; 
                }
                float d = sqrt(pow(towers[t].x - units[i].x, 2) + pow(towers[t].y - units[i].y, 2));
                if (d < close_d) { close_d = d; target_x = towers[t].x; target_y = towers[t].y; target_tower_idx = t; }
            }
        }

        if (target_tower_idx == -1) continue;

        float dx = target_x - units[i].x;
        float dy = target_y - units[i].y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist > 30.0f) {
            units[i].x += (dx / dist) * units[i].speed;
            units[i].y += (dy / dist) * units[i].speed;
        } else {
            if (units[i].attack_cooldown <= 0) {
                towers[target_tower_idx].hp -= units[i].damage;
                units[i].attack_cooldown = 60;
                screen_shake_timer = 8; 

                // PEKKA Splash Damage Alan Hasarı
                if (units[i].type == CARD_PEKKA) {
                    for(int u=0; u<MAX_UNITS; u++) {
                        if(units[u].active && units[u].team != units[i].team) {
                            float u_dist = sqrt(pow(units[u].x - units[i].x, 2) + pow(units[u].y - units[i].y, 2));
                            if(u_dist < 60.0f) {
                                units[u].hp -= 30;
                                if(units[u].hp <= 0) units[u].active = false;
                            }
                        }
                    }
                }

                if (towers[target_tower_idx].hp <= 0) {
                    towers[target_tower_idx].active = false;
                    create_explosion(towers[target_tower_idx].x, towers[target_tower_idx].y, (SDL_Color){241, 196, 15, 255}, 25);
                    if (towers[target_tower_idx].is_king) {
                        game_state = STATE_GAME_OVER;
                        winner_team = units[i].team;
                    }
                }
            }
        }
        if (units[i].attack_cooldown > 0) units[i].attack_cooldown--;
    }
}