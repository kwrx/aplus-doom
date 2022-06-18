/*
 * Author:
 *      Antonino Natale <antonio.natale97@hotmail.com>
 * 
 * Copyright (c) 2013-2022 Antonino Natale
 * 
 * 
 * This file is part of aplus.
 * 
 * aplus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * aplus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with aplus.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <assert.h>
#include <poll.h>
#include <sched.h>

#include <aplus/fb.h>
#include <aplus/events.h>

#define _POSIX_SOURCE
#include <time.h>




static struct {

    struct fb_var_screeninfo var;
    struct fb_fix_screeninfo fix;

    int kbd;

} context;



void DG_Init() {

    int fd;
    
    if((fd = open("/dev/fb0", O_RDWR)) < 0) {
        fprintf(stderr, "doom: open() failed: cannot open /dev/fb0: %s\n", strerror(errno));
        exit(1);
    }

    if(ioctl(fd, FBIOGET_VSCREENINFO, &context.var) < 0) {
        fprintf(stderr, "doom: ioctl(FBIOGET_VSCREENINFO) failed: %s\n", strerror(errno));
        exit(1);
    }

    if(ioctl(fd, FBIOGET_FSCREENINFO, &context.fix) < 0) {
        fprintf(stderr, "doom: ioctl(FBIOGET_FSCREENINFO) failed: %s\n", strerror(errno));
        exit(1);
    }

    if(!context.fix.smem_start || !context.var.xres_virtual || !context.var.yres_virtual) {
        fprintf(stderr, "doom: wrong framebuffer configuration\n");
        exit(1);
    }

    if(close(fd) < 0) {
        fprintf(stderr, "doom: close() failed: %s\n", strerror(errno));
        exit(1);
    }

    if((context.kbd = open("/dev/kbd", O_RDONLY)) < 0) {
        fprintf(stderr, "doom: open() failed: cannot open /dev/kbd: %s\n", strerror(errno));
        exit(1);
    }

    if(fcntl(context.kbd, F_SETFL, O_NONBLOCK) < 0) {
        fprintf(stderr, "doom: fcntl(F_SETFL) failed: %s\n", strerror(errno));
        exit(1);
    }

}


void DG_DrawFrame() {

    uintptr_t d = (uintptr_t) context.fix.smem_start;
    uintptr_t s = (uintptr_t) DG_ScreenBuffer;

    for(size_t y = 0; y < DOOMGENERIC_RESY; y++) {

        memcpy((void*) d, (const void*) s, DOOMGENERIC_RESX * sizeof(uint32_t));

        s += DOOMGENERIC_RESX * sizeof(uint32_t);
        d += context.fix.line_length;

    }

}


void DG_SleepMs(unsigned int ms) {
    usleep(ms * 1000);
}


uint32_t DG_GetTicksMs() {

    struct timespec ts;

    if(clock_gettime(CLOCK_MONOTONIC, &ts) < 0) {
        fprintf(stderr, "doom: clock_gettime() failed: %s\n", strerror(errno));
        exit(1);
    }

    return (uint32_t) (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);

}


int DG_GetKey(int* pressed, unsigned char* key) {

    event_t ev;

    if(read(context.kbd, &ev, sizeof(ev)) == sizeof(event_t)) {
        
        switch(ev.ev_key.vkey) {

            case 28:
                *key = KEY_ENTER;
                break;

            case 1:
                *key = KEY_ESCAPE;
                break;

            case 105:
                *key = KEY_LEFTARROW;
                break;

            case 106:
                *key = KEY_RIGHTARROW;
                break;

            case 103:
                *key = KEY_UPARROW;
                break;

            case 108:
                *key = KEY_DOWNARROW;
                break;

            case 29:
                *key = KEY_FIRE;
                break;

            case 57:
                *key = KEY_USE;
                break;

            case 54:
                *key = KEY_RSHIFT;
                break;

            case 2 ... 11:
                *key = "1234567890"[ev.ev_key.vkey - 2];
                break;

            case 16 ... 25:
                *key = "qwertyuiop"[ev.ev_key.vkey - 16];
                break;

            case 30 ... 39:
                *key = "asdfghjkl"[ev.ev_key.vkey - 30];
                break;

            case 44 ... 53:
                *key = "zxcvbnm"[ev.ev_key.vkey - 44];
                break;

            default:
                return 0;

        }

        *pressed = ev.ev_key.down;

        return 1;

    } else {

        return 0;

    }

}


void DG_SetWindowTitle(const char* title) {
    (void) title;
}

