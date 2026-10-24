#include "video_gl.h"
#include <stdlib.h>
#include <string.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

int lba_video_init(LBA_Video *v, SDL_Window *window, int width, int height)
{
    size_t n;
    if (!v || !window || width <= 0 || height <= 0 ||
        width > 4096 || height > 4096) return 0;
    memset(v, 0, sizeof(*v));
    v->window = window;
    v->width = width;
    v->height = height;
    n = (size_t)width * (size_t)height;
    v->rgba = (unsigned char *)malloc(n * 4u);
    if (!v->rgba) return 0;
    v->gl_context = SDL_GL_CreateContext(window);
    if (!v->gl_context) {
        SDL_Log("OpenGL context: %s", SDL_GetError());
        lba_video_destroy(v);
        return 0;
    }
    SDL_GL_SetSwapInterval(1);
    glGenTextures(1, &v->texture);
    if (!v->texture) {
        lba_video_destroy(v);
        return 0;
    }
    glBindTexture(GL_TEXTURE_2D, v->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    if (glGetError() != GL_NO_ERROR) {
        SDL_Log("OpenGL texture allocation failed");
        lba_video_destroy(v);
        return 0;
    }
    SDL_Log("LBA1 indexed video: %dx%d, GL %s", width, height,
            (const char *)glGetString(GL_VERSION));
    return 1;
}

void lba_video_set_palette(LBA_Video *v, const unsigned char *rgb,
                           int palette_is_vga6)
{
    int i, c;
    if (!v || !rgb) return;
    for (i = 0; i < 256; ++i) {
        for (c = 0; c < 3; ++c) {
            unsigned int component = rgb[i * 3 + c];
            if (palette_is_vga6) {
                if (component > 63u) component = 63u;
                component = (component * 255u + 31u) / 63u;
            }
            v->palette[i * 4 + c] = (unsigned char)component;
        }
        v->palette[i * 4 + 3] = 255u;
    }
}

void lba_video_present(LBA_Video *v, const unsigned char *pixels)
{
    size_t i, n;
    int w, h, vw, vh, vx, vy;
    if (!v || !pixels || !v->rgba || !v->texture) return;
    n = (size_t)v->width * (size_t)v->height;
    for (i = 0; i < n; ++i)
        memcpy(v->rgba + 4u * i, v->palette + 4u * pixels[i], 4u);

    SDL_GetWindowSizeInPixels(v->window, &w, &h);
    if (w <= 0 || h <= 0) return;
    vw = w;
    vh = (int)((double)w * v->height / v->width);
    if (vh > h) {
        vh = h;
        vw = (int)((double)h * v->width / v->height);
    }
    vx = (w - vw) / 2;
    vy = (h - vh) / 2;

    glViewport(0, 0, w, h);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(vx, vy, vw, vh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0., 1., 1., 0., -1., 1.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v->texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, v->width, v->height,
                    GL_RGBA, GL_UNSIGNED_BYTE, v->rgba);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
    glTexCoord2f(1.f, 0.f); glVertex2f(1.f, 0.f);
    glTexCoord2f(1.f, 1.f); glVertex2f(1.f, 1.f);
    glTexCoord2f(0.f, 1.f); glVertex2f(0.f, 1.f);
    glEnd();
    SDL_GL_SwapWindow(v->window);
}

void lba_video_destroy(LBA_Video *v)
{
    if (!v) return;
    if (v->texture) glDeleteTextures(1, &v->texture);
    if (v->gl_context) SDL_GL_DestroyContext(v->gl_context);
    free(v->rgba);
    memset(v, 0, sizeof(*v));
}
