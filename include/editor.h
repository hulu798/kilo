#ifndef EDITOR_H
#define EDITOR_H

#include "kilo.h"

/* Editor initialization and core functions */
void initEditor(void);
void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen(void);

/* Syntax highlighting functions */
int is_separator(int c);
int editorRowHasOpenComment(erow *row);
void editorUpdateSyntax(erow *row);
int editorSyntaxToColor(int hl);
void editorSelectSyntaxHighlight(char *filename);

/* Editor row operations */
void editorUpdateRow(erow *row);
void editorInsertRow(int at, char *s, size_t len);
void editorFreeRow(erow *row);
void editorDelRow(int at);
char *editorRowsToString(int *buflen);
void editorRowInsertChar(erow *row, int at, int c);
void editorRowAppendString(erow *row, char *s, size_t len);
void editorRowDelChar(erow *row, int at);

/* Editor character and line operations */
void editorInsertChar(int c);
void editorInsertNewline(void);
void editorDelChar(void);

/* File operations */
int editorOpen(char *filename);
int editorSave(void);
int editorFileWasModified(void);

/* Search functionality */
void editorFind(int fd);

/* Input handling */
void editorMoveCursor(int key);
void editorProcessKeypress(int fd);

/* Syntax highlighting database */
extern struct editorSyntax HLDB[];
extern char *C_HL_extensions[];
extern char *C_HL_keywords[];

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

#endif /* EDITOR_H */
