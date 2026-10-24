/* Milestone 1 is an asset-free frame test, not yet the original LBA1 game. */
#include "video_gl.h"
#include <SDL3/SDL_main.h>
#include <stdlib.h>

#define WIDTH 640
#define HEIGHT 480

static void generate_palette(unsigned char palette[768])
{
    int i;
    for (i = 0; i < 256; ++i) {
        palette[i * 3] = (unsigned char)(i & 63);
        palette[i * 3 + 1] = (unsigned char)((i >> 2) & 63);
        palette[i * 3 + 2] = (unsigned char)((255 - i) >> 2);
    }
}

static void generate_frame(unsigned char *pixels, int offset)
{
    int x, y;
    for (y = 0; y < HEIGHT; ++y) {
        for (x = 0; x < WIDTH; ++x) {
            int square = (x >= 80 + (offset % 360) &&
                          x < 160 + (offset % 360) &&
                          y >= 150 && y < 260);
            pixels[y * WIDTH + x] = square ? 240 :
                (unsigned char)(((x >> 2) + (y >> 2) + offset) & 255);
        }
    }
}

int main(int argc, char **argv)
{
    SDL_Window *window = NULL;
    LBA_Video video = {0};
    unsigned char *pixels = NULL;
    unsigned char palette[768];
    int running = 1;
    int result = 1;
    (void)argc; (void)argv;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return 1;
    }
    /* GL 2.1 compatibility for the initial software-renderer bridge. */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("LBA1 Win32/OpenGL M1 - framebuffer test",
                              960, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) goto cleanup;
    if (!lba_video_init(&video, window, WIDTH, HEIGHT)) goto cleanup;
    pixels = (unsigned char *)malloc((size_t)WIDTH * HEIGHT);
    if (!pixels) goto cleanup;
    generate_palette(palette);
    lba_video_set_palette(&video, palette, 1);
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = 0;
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) running = 0;
                if (event.key.key == SDLK_F11 && !event.key.repeat)
                    SDL_SetWindowFullscreen(window,
                        !(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN));
            }
        }
        generate_frame(pixels, (int)((SDL_GetTicks() / 30u) % 256u));
        lba_video_present(&video, pixels);
        SDL_Delay(1);
    }
    result = 0;
cleanup:
    free(pixels);
    lba_video_destroy(&video);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}
