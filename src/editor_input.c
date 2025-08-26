#include "kilo.h"
#include "editor.h"
#include "terminal.h"

/* Insert the specified char at the current prompt position. */
void editorInsertChar(int c)
{
    int filerow = E.rowoff + E.cy;
    int filecol = E.coloff + E.cx;
    erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

    /* If the row where the cursor is currently located does not exist in our
     * logical representaion of the file, add enough empty rows as needed. */
    if (!row)
    {
        while (E.numrows <= filerow)
            editorInsertRow(E.numrows, "", 0);
    }
    row = &E.row[filerow];
    editorRowInsertChar(row, filecol, c);
    if (E.cx == E.screencols - 1)
        E.coloff++;
    else
        E.cx++;
    E.dirty++;
}

/* Inserting a newline is slightly complex as we have to handle inserting a
 * newline in the middle of a line, splitting the line as needed. */
void editorInsertNewline(void)
{
    int filerow = E.rowoff + E.cy;
    int filecol = E.coloff + E.cx;
    erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

    if (!row)
    {
        if (filerow == E.numrows)
        {
            editorInsertRow(filerow, "", 0);
            goto fixcursor;
        }
        return;
    }
    /* If the cursor is over the current line size, we want to conceptually
     * think it's just over the last character. */
    if (filecol >= row->size)
        filecol = row->size;
    if (filecol == 0)
    {
        editorInsertRow(filerow, "", 0);
    }
    else
    {
        /* We are in the middle of a line. Split it between two rows. */
        editorInsertRow(filerow + 1, row->chars + filecol, row->size - filecol);
        row = &E.row[filerow];
        row->chars[filecol] = '\0';
        row->size = filecol;
        editorUpdateRow(row);
    }
fixcursor:
    if (E.cy == E.screenrows - 1)
    {
        E.rowoff++;
    }
    else
    {
        E.cy++;
    }
    E.cx = 0;
    E.coloff = 0;
}

/* Delete the char at the current prompt position. */
void editorDelChar(void)
{
    int filerow = E.rowoff + E.cy;
    int filecol = E.coloff + E.cx;
    erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

    if (!row || (filecol == 0 && filerow == 0))
        return;
    if (filecol == 0)
    {
        /* Handle the case of column 0, we need to move the current line
         * on the right of the previous one. */
        filecol = E.row[filerow - 1].size;
        editorRowAppendString(&E.row[filerow - 1], row->chars, row->size);
        editorDelRow(filerow);
        row = NULL;
        if (E.cy == 0)
            E.rowoff--;
        else
            E.cy--;
        E.cx = filecol;
        if (E.cx >= E.screencols)
        {
            int shift = (E.screencols - E.cx) + 1;
            E.cx -= shift;
            E.coloff += shift;
        }
    }
    else
    {
        editorRowDelChar(row, filecol - 1);
        if (E.cx == 0 && E.coloff)
            E.coloff--;
        else
            E.cx--;
    }
    if (row)
        editorUpdateRow(row);
    E.dirty++;
}

/* Handle cursor position change because arrow keys were pressed. */
void editorMoveCursor(int key)
{
    int filerow = E.rowoff + E.cy;
    int filecol = E.coloff + E.cx;
    int rowlen;
    erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

    switch (key)
    {
    case ARROW_LEFT:
        if (E.cx == 0)
        {
            if (E.coloff)
            {
                E.coloff--;
            }
            else
            {
                if (filerow > 0)
                {
                    E.cy--;
                    E.cx = E.row[filerow - 1].size;
                    if (E.cx > E.screencols - 1)
                    {
                        E.coloff = E.cx - E.screencols + 1;
                        E.cx = E.screencols - 1;
                    }
                }
            }
        }
        else
        {
            E.cx -= 1;
        }
        break;
    case ARROW_RIGHT:
        if (row && filecol < row->size)
        {
            if (E.cx == E.screencols - 1)
            {
                E.coloff++;
            }
            else
            {
                E.cx += 1;
            }
        }
        else if (row && filecol == row->size)
        {
            E.cx = 0;
            E.coloff = 0;
            if (E.cy == E.screenrows - 1)
            {
                E.rowoff++;
            }
            else
            {
                E.cy += 1;
            }
        }
        break;
    case ARROW_UP:
        if (E.cy == 0)
        {
            if (E.rowoff)
                E.rowoff--;
        }
        else
        {
            E.cy -= 1;
        }
        break;
    case ARROW_DOWN:
        if (filerow < E.numrows)
        {
            if (E.cy == E.screenrows - 1)
            {
                E.rowoff++;
            }
            else
            {
                E.cy += 1;
            }
        }
        break;
    }
    /* Fix cx if the current line has not enough chars. */
    filerow = E.rowoff + E.cy;
    filecol = E.coloff + E.cx;
    row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
    rowlen = row ? row->size : 0;
    if (filecol > rowlen)
    {
        E.cx -= filecol - rowlen;
        if (E.cx < 0)
        {
            E.coloff += E.cx;
            E.cx = 0;
        }
    }
}

/* Process events arriving from the standard input, which is, the user
 * is typing stuff on the terminal. */
void editorProcessKeypress(int fd)
{
    /* When the file is modified, requires Ctrl-q to be pressed N times
     * before actually quitting. */
    static int quit_times = KILO_QUIT_TIMES;

    int c = editorReadKey(fd);
    switch (c)
    {
    case ENTER: /* Enter */
        editorInsertNewline();
        break;
    case CTRL_C: /* Ctrl-c */
        /* We ignore ctrl-c, it can't be so simple to lose the changes
         * to the edited file. */
        break;
    case CTRL_Q: /* Ctrl-q */
        /* Quit if the file was already saved. */
        if (E.dirty && quit_times)
        {
            editorSetStatusMessage("WARNING!!! File has unsaved changes. "
                                   "Press Ctrl-Q %d more times to quit.",
                                   quit_times);
            quit_times--;
            return;
        }
        exit(0);
        break;
    case CTRL_S: /* Ctrl-s */
        editorSave();
        break;
    case CTRL_F:
        editorFind(fd);
        break;
    case BACKSPACE: /* Backspace */
    case CTRL_H:    /* Ctrl-h */
    case DEL_KEY:
        editorDelChar();
        break;
    case PAGE_UP:
    case PAGE_DOWN:
        if (c == PAGE_UP && E.cy != 0)
            E.cy = 0;
        else if (c == PAGE_DOWN && E.cy != E.screenrows - 1)
            E.cy = E.screenrows - 1;
        {
            int times = E.screenrows;
            while (times--)
                editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        }
        break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        editorMoveCursor(c);
        break;
    case CTRL_L: /* ctrl+l, clear screen */
        /* Just refresht the line as side effect. */
        break;
    case ESC:
        /* Nothing to do for ESC in this mode. */
        break;
    default:
        editorInsertChar(c);
        break;
    }

    quit_times = KILO_QUIT_TIMES; /* Reset it to the original value. */
}
