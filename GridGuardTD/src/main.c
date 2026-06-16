#define SDL_MAIN_HANDLED
#include "game.h"
#include <time.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow(
        "Clash Royale Tactical Core - [SPACE]: Barikat Kur",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    init_clash_arena();


    bool running = true;
    SDL_Event event;
    int mx = 0, my = 0;

    while (running) {
        SDL_GetMouseState(&mx, &my);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } 
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_q) trigger_chat_message("Iyi oyundu!");
                if (event.key.keysym.sym == SDLK_w) trigger_chat_message("Tesekkurler!");
                if (event.key.keysym.sym == SDLK_e) trigger_chat_message("Aglama :)");

                if (event.key.keysym.sym == SDLK_1) selected_card = CARD_KNIGHT;
                if (event.key.keysym.sym == SDLK_2) selected_card = CARD_GOBLIN;
                if (event.key.keysym.sym == SDLK_3) selected_card = CARD_PEKKA;

                if (event.key.keysym.sym == SDLK_SPACE) {
                    spawn_wall((float)mx, (float)my);
                }

                if (game_state == STATE_GAME_OVER && event.key.keysym.sym == SDLK_r) {
                    if (winner_team == TEAM_BLUE) current_level++;
                    else current_level = 1;
                    init_clash_arena();
                }
            } 
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if ((float)mx < SCREEN_WIDTH / 2 - 20 && (float)my < SCREEN_HEIGHT - 100) {
                    spawn_card(TEAM_BLUE, selected_card, (float)mx, (float)my);
                }
            }
        }

        update_game_logic();
        render_arena(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}