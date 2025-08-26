#ifndef KILO_H
#define KILO_H

#define KILO_VERSION "0.0.1"

#ifdef __linux__
#define _POSIX_C_SOURCE 200809L
#endif

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>

/* Syntax highlight types */
#define HL_NORMAL 0
#define HL_NONPRINT 1
#define HL_COMMENT 2   /* Single line comment. */
#define HL_MLCOMMENT 3 /* Multi-line comment. */
#define HL_KEYWORD1 4
#define HL_KEYWORD2 5
#define HL_STRING 6
#define HL_NUMBER 7
#define HL_MATCH 8 /* Search match. */

#define HL_HIGHLIGHT_STRINGS (1 << 0)
#define HL_HIGHLIGHT_NUMBERS (1 << 1)

#define KILO_QUIT_TIMES 3
#define KILO_QUERY_LEN 256

/* Key action enumeration */
enum KEY_ACTION
{
    KEY_NULL = 0,    /* NULL */
    CTRL_C = 3,      /* Ctrl-c */
    CTRL_D = 4,      /* Ctrl-d */
    CTRL_F = 6,      /* Ctrl-f */
    CTRL_H = 8,      /* Ctrl-h */
    TAB = 9,         /* Tab */
    CTRL_L = 12,     /* Ctrl+l */
    ENTER = 13,      /* Enter */
    CTRL_Q = 17,     /* Ctrl-q */
    CTRL_S = 19,     /* Ctrl-s */
    CTRL_U = 21,     /* Ctrl-u */
    ESC = 27,        /* Escape */
    BACKSPACE = 127, /* Backspace */
    /* The following are just soft codes, not really reported by the
     * terminal directly. */
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

/* Editor syntax structure */
struct editorSyntax
{
    char **filematch;
    char **keywords;
    char singleline_comment_start[2];
    char multiline_comment_start[3];
    char multiline_comment_end[3];
    int flags;
};

/* This structure represents a single line of the file we are editing. */
typedef struct erow
{
    int idx;           /* Row index in the file, zero-based. */
    int size;          /* Size of the row, excluding the null term. */
    int rsize;         /* Size of the rendered row. */
    char *chars;       /* Row content. */
    char *render;      /* Row content "rendered" for screen (for TABs). */
    unsigned char *hl; /* Syntax highlight type for each character in render.*/
    int hl_oc;         /* Row had open comment at end in last syntax highlight
                          check. */
} erow;

/* Color structure for syntax highlighting */
typedef struct hlcolor
{
    int r, g, b;
} hlcolor;

/* Main editor configuration structure */
struct editorConfig
{
    int cx, cy;     /* Cursor x and y position in characters */
    int rowoff;     /* Offset of row displayed. */
    int coloff;     /* Offset of column displayed. */
    int screenrows; /* Number of rows that we can show */
    int screencols; /* Number of cols that we can show */
    int numrows;    /* Number of rows */
    int rawmode;    /* Is terminal raw mode enabled? */
    erow *row;      /* Rows */
    int dirty;      /* File modified but not saved. */
    char *filename; /* Currently open filename */
    char statusmsg[80];
    time_t statusmsg_time;
    struct editorSyntax *syntax; /* Current syntax highlight, or NULL. */
};

/* Global editor state */
extern struct editorConfig E;

/* Append buffer structure for efficient screen updates */
struct abuf
{
    char *b;
    int len;
};

#define ABUF_INIT {NULL, 0}

#endif /* KILO_H */
