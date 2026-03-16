#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string.h>

#define COLS 80
#define ROWS 25
#define WIN_W 800
#define WIN_H 600
#define CELL_W (WIN_W / COLS)   // 10
#define CELL_H (WIN_H / ROWS)   // 24

// The text buffer — fill this with whatever you want to display
char screen[ROWS][COLS + 1];

void screen_write(int row, int col, const char *text) {
    int len = strlen(text);
    for (int i = 0; i < len && col + i < COLS; i++)
        screen[row][col + i] = text[i];
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("KrabbaTek Krona 1 VM", WIN_W, WIN_H, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    TTF_Font *font = TTF_OpenFont("fonts/Px437_EuroPC_CGA.ttf", CELL_H);
    if (!font) { SDL_Log("Font error: %s", SDL_GetError()); return 1; }

    // Fill screen buffer
    memset(screen, ' ', sizeof(screen));
    screen_write(0, 0, "Krabba!");

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_EVENT_QUIT) running = false;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        char ch[2] = {0, 0};
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                ch[0] = screen[row][col];
                if (ch[0] == ' ' || ch[0] == '\0') continue;

                SDL_Surface *surf = TTF_RenderText_Blended(font, ch, 1,
                    (SDL_Color){255, 255, 255, 255});
                if (!surf) continue;
                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_DestroySurface(surf);

                SDL_FRect dst = {col * CELL_W, row * CELL_H, CELL_W, CELL_H};
                SDL_RenderTexture(renderer, tex, NULL, &dst);
                SDL_DestroyTexture(tex);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}