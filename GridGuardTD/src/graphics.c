#include "game.h"
#include <stdio.h>
#include <math.h>

void draw_character(SDL_Renderer* renderer, Unit* u, int ox, int oy) {
    int x = (int)u->x + ox;
    int y = (int)u->y + oy;
    int b = (int)(sin(u->anim_timer) * 2);

    SDL_Color team_color = (u->team == TEAM_BLUE) ? (SDL_Color){41, 128, 185, 255} : (SDL_Color){192, 41, 43, 255};

    if (u->type == CARD_KNIGHT) {
        SDL_SetRenderDrawColor(renderer, team_color.r - 20, team_color.g, team_color.b, 255);
        SDL_Rect cape = { x - 10, y - 4 + b, 20, 14 };
        SDL_RenderFillRect(renderer, &cape);

        SDL_SetRenderDrawColor(renderer, team_color.r, team_color.g, team_color.b, 255);
        SDL_Rect body = { x - 8, y - 8 + b, 16, 16 };
        SDL_RenderFillRect(renderer, &body);

        SDL_SetRenderDrawColor(renderer, 149, 165, 166, 255);
        SDL_Rect helm = { x - 5, y - 14 + b, 10, 7 };
        SDL_RenderFillRect(renderer, &helm);
    } 
    else if (u->type == CARD_GOBLIN) {
        SDL_SetRenderDrawColor(renderer, 46, 204, 113, 255);
        SDL_Rect body = { x - 6, y - 6 + b, 12, 12 };
        SDL_RenderFillRect(renderer, &body);
        SDL_Rect head = { x - 4, y - 11 + b, 8, 6 };
        SDL_RenderFillRect(renderer, &head);
    } 
    else if (u->type == CARD_PEKKA) {
        SDL_SetRenderDrawColor(renderer, 44, 62, 80, 255);
        SDL_Rect body = { x - 13, y - 13 + b, 26, 26 };
        SDL_RenderFillRect(renderer, &body);

        SDL_SetRenderDrawColor(renderer, 155, 89, 182, 255);
        SDL_RenderDrawLine(renderer, x - 8, y - 13 + b, x - 12, y - 19 + b);
        SDL_RenderDrawLine(renderer, x + 8, y - 13 + b, x + 12, y - 19 + b);
    }

    float hp_pct = (float)u->hp / u->max_hp;
    SDL_Rect hp_bg = { x - 10, y - 20, 20, 3 };
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &hp_bg);

    SDL_Rect hp_fg = { x - 10, y - 20, (int)(20 * hp_pct), 3 };
    SDL_SetRenderDrawColor(renderer, 46, 204, 113, 255);
    SDL_RenderFillRect(renderer, &hp_fg);
}

void render_arena(SDL_Renderer* renderer) {
    // Screen Shake (Ekran Sarsıntısı) Hesabı
    int ox = 0, oy = 0;
    if (screen_shake_timer > 0) {
        ox = (rand() % 7) - 3;
        oy = (rand() % 7) - 3;
    }

    SDL_SetRenderDrawColor(renderer, 46, 204, 113, 255);
    SDL_RenderClear(renderer);

    // Nehir
    SDL_Rect river = { SCREEN_WIDTH / 2 - 15 + ox, oy, 30, SCREEN_HEIGHT - 100 };
    SDL_SetRenderDrawColor(renderer, 52, 152, 219, 255);
    SDL_RenderFillRect(renderer, &river);

    // Köprüler
    SDL_Rect b1 = { SCREEN_WIDTH / 2 - 20 + ox, 140 + oy, 40, 40 };
    SDL_Rect b2 = { SCREEN_WIDTH / 2 - 20 + ox, 440 + oy, 40, 40 };
    SDL_SetRenderDrawColor(renderer, 211, 84, 0, 255);
    SDL_RenderFillRect(renderer, &b1);
    SDL_RenderFillRect(renderer, &b2);

    // Savunma Duvarları
    for (int i = 0; i < MAX_WALLS; i++) {
        if (!walls[i].active) continue;
        SDL_SetRenderDrawColor(renderer, 127, 140, 141, 255);
        SDL_Rect w_render = { walls[i].rect.x + ox, walls[i].rect.y + oy, walls[i].rect.w, walls[i].rect.h };
        SDL_RenderFillRect(renderer, &w_render);
    }

    // Kuleler
    for (int i = 0; i < 6; i++) {
        if (!towers[i].active) continue;
        if (towers[i].team == TEAM_BLUE) SDL_SetRenderDrawColor(renderer, 41, 128, 185, 255);
        else SDL_SetRenderDrawColor(renderer, 192, 41, 43, 255);

        int size = towers[i].is_king ? 46 : 32;
        SDL_Rect t_rect = { (int)towers[i].x - size/2 + ox, (int)towers[i].y - size/2 + oy, size, size };
        SDL_RenderFillRect(renderer, &t_rect);
    }

    // Karakterler
    for (int i = 0; i < MAX_UNITS; i++) {
        if (units[i].active) draw_character(renderer, &units[i], ox, oy);
    }

    // Efektler
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            SDL_SetRenderDrawColor(renderer, particles[i].color.r, particles[i].color.g, particles[i].color.b, 255);
            SDL_Rect p_rect = { (int)particles[i].x + ox, (int)particles[i].y + oy, 3, 3 };
            SDL_RenderFillRect(renderer, &p_rect);
        }
    }

    // Alt UI Panel
    SDL_Rect ui_panel = { 0, SCREEN_HEIGHT - 100, SCREEN_WIDTH, 100 };
    SDL_SetRenderDrawColor(renderer, 44, 62, 80, 255);
    SDL_RenderFillRect(renderer, &ui_panel);

    SDL_Rect elixir_bg = { 40, SCREEN_HEIGHT - 45, 200, 14 };
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &elixir_bg);

    SDL_Rect elixir_fg = { 40, SCREEN_HEIGHT - 45, (int)(200 * (blue_elixir / 10.0f)), 14 };
    SDL_SetRenderDrawColor(renderer, 155, 89, 182, 255);
    SDL_RenderFillRect(renderer, &elixir_fg);

    if (game_state == STATE_GAME_OVER) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 210);
        SDL_Rect overlay = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderFillRect(renderer, &overlay);

        SDL_Rect banner = { SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 40, 280, 80 };
        if (winner_team == TEAM_BLUE) SDL_SetRenderDrawColor(renderer, 41, 128, 185, 255);
        else SDL_SetRenderDrawColor(renderer, 192, 41, 43, 255);
        SDL_RenderFillRect(renderer, &banner);
    }
}