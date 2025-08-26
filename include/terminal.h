#ifndef TERMINAL_H
#define TERMINAL_H

#include "kilo.h"

/* Terminal handling functions */
void disableRawMode(int fd);
void editorAtExit(void);
int enableRawMode(int fd);
int editorReadKey(int fd);
int getCursorPosition(int ifd, int ofd, int *rows, int *cols);
int getWindowSize(int ifd, int ofd, int *rows, int *cols);
void updateWindowSize(void);
void handleSigWinCh(int unused);

/* Append buffer functions */
void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);

#endif /* TERMINAL_H */
