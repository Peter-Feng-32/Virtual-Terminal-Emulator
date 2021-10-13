#include <stdio.h>
#include <stdlib.h>
#include "libtmt/tmt.h"
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include <unistd.h> // for sleep

/* Forward declaration of a callback.
 * libtmt will call this function when the terminal's state changes.
 */
static void finish(int sig);

void callback(tmt_msg_t m, TMT *vt, const void *a, void *p);

void printTerminal(TMT *vt);

void initCurses();

int main(void)
{
    /* Open a virtual terminal with 2 lines and 10 columns.
     * The first NULL is just a pointer that will be provided to the
     * callback; it can be anything. The second NULL specifies that
     * we want to use the default Alternate Character Set; this
     * could be a pointer to a wide string that has the desired
     * characters to be displayed when in ACS mode.
     */

    const size_t SCREEN_HEIGHT = 13;
    const size_t SCREEN_WIDTH = 67;

    TMT *vt = tmt_open(SCREEN_HEIGHT + 1, SCREEN_WIDTH + 1, callback, NULL, NULL); //Allocate one extra space for newline characters.
    if (!vt)
    {
        return perror("could not allocate terminal"), EXIT_FAILURE;
    }

    FILE *f = fopen("ASCIIStarWars/starwars.txt", "r"); //Load ASCII star wars.
    char buff[255];

    while (fgets(buff, 255, (FILE *)f))
    {
        long long frameLength = 0;
        frameLength = atoi(buff);

        int fps = 20;
        tmt_write(vt, "\033[0;0H", 0); //Bring cursor to (0,0).
        tmt_write(vt, "\033[2J", 0);   //Clear entire terminal virtual screen
        for (int i = 0; i < 13; i++)
        {
            fgets(buff, 255, (FILE *)f);
            tmt_write(vt, buff, 0);
            tmt_write(vt, "\033[1;E\033[1;F", 0); //Moves to first row on next line.
        }
        //printf(frame);
        printTerminal(vt);
        usleep(frameLength * (1000000 / fps));
        printf("\033[2J");
    }

    return EXIT_SUCCESS;
}

void printTerminal(TMT *vt)
{
    /* grab a pointer to the virtual screen */
    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *c = tmt_cursor(vt);

    for (size_t r = 0; r < s->nline; r++)
    {
        for (size_t c = 0; c < s->ncol; c++)
        {
            /*
                printf("contents of %zd,%zd: %lc (%s bold)\n", r, c,
                       s->lines[r]->chars[c].c,
                       s->lines[r]->chars[c].a.bold ? "is" : "is not");
            */

            printf("%c", s->lines[r]->chars[c].c);
        }
        printf("\n");
    }
    tmt_clean(vt);
}

void callback(tmt_msg_t m, TMT *vt, const void *a, void *p)
{
    /* grab a pointer to the virtual screen */
    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *c = tmt_cursor(vt);

    switch (m)
    {
    case TMT_MSG_BELL:
        /* the terminal is requesting that we ring the bell/flash the
             * screen/do whatever ^G is supposed to do; a is NULL
             */
        printf("bing!\n");
        break;

    case TMT_MSG_UPDATE:
        /* the screen image changed; a is a pointer to the TMTSCREEN */
        /*
        for (size_t r = 0; r < s->nline; r++)
        {
            if (s->lines[r]->dirty)
            {
                for (size_t c = 0; c < s->ncol; c++)
                {
                    printf("contents of %zd,%zd: %lc (%s bold)\n", r, c,
                           s->lines[r]->chars[c].c,
                           s->lines[r]->chars[c].a.bold ? "is" : "is not");
                }
            }
        }
        */
        /* let tmt know we've redrawn the screen */
        //tmt_clean(vt);
        break;

    case TMT_MSG_ANSWER:
        /* the terminal has a response to give to the program; a is a
             * pointer to a string */
        printf("terminal answered %s\n", (const char *)a);
        break;

    case TMT_MSG_MOVED:
        /* the cursor moved; a is a pointer to the cursor's TMTPOINT */
        //printf("cursor is now at %zd,%zd\n", c->r, c->c);
        break;
    }
}

void initCurses()
{
    /* Test Curses */
    int num = 0;
    //(void)initscr();      /* initialize the curses library */

    initscr();
    keypad(stdscr, TRUE); /* enable keyboard mapping */
    (void)nonl();         /* tell curses not to do NL->CR/NL on output */
    (void)cbreak();       /* take input chars one at a time, no wait for \n */
    (void)echo();         /* echo input - in color  */

    if (has_colors())
    {
        start_color();

        /*
         * Simple color assignment, often all we need.  Color pair 0 cannot
         * be redefined.  This example uses the same value for the color
         * pair as for the foreground color, though of course that is not
         * necessary:
         */
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_CYAN, COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
    }
}