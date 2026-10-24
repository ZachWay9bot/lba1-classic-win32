#ifndef LBA1_WIN32_VIDEO_GL_H
#define LBA1_WIN32_VIDEO_GL_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

/* Presentation bridge for LBA1's 8-bit indexed software framebuffer (Log). */
typedef struct LBA_Video {
    SDL_Window *window;             /* owned by caller */
    SDL_GLContext gl_context;
    GLuint texture;
    unsigned char palette[256 * 4]; /* 256 RGBA entries */
    unsigned char *rgba;            /* conversion scratch storage */
    int width;
    int height;
} LBA_Video;

int lba_video_init(LBA_Video *v, SDL_Window *window, int width, int height);
void lba_video_set_palette(LBA_Video *v, const unsigned char *rgb,
                           int palette_is_vga6);
void lba_video_present(LBA_Video *v, const unsigned char *indexed_pixels);
void lba_video_destroy(LBA_Video *v);
#endif
