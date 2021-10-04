#include <stdio.h>
#include <stdlib.h>
#include "tmt.h"

/* Forward declaration of a callback.
 * libtmt will call this function when the terminal's state changes.
 */
void callback(tmt_msg_t m, TMT *vt, const void *a, void *p);

int main(void)
{
    /* Open a virtual terminal with 2 lines and 10 columns.
     * The first NULL is just a pointer that will be provided to the
     * callback; it can be anything. The second NULL specifies that
     * we want to use the default Alternate Character Set; this
     * could be a pointer to a wide string that has the desired
     * characters to be displayed when in ACS mode.
     */
    TMT *vt = tmt_open(2, 10, callback, NULL, NULL);
    if (!vt)
        return perror("could not allocate terminal"), EXIT_FAILURE;

    /* Write some text to the terminal, using escape sequences to
     * use a bold rendition.
     *
     * The final argument is the length of the input; 0 means that
     * libtmt will determine the length dynamically using strlen.
     */
    tmt_write(vt, "\033[1mhello, world (in bold!)\033[0m", 0);

    tmt_write(vt, "\033[2J", 0);

    tmt_write(vt, "\033[0;0;H", 0);

    /* Writing input to the virtual terminal can (and in this case, did)
     * call the callback letting us know the screen was updated. See the
     * callback below to see how that works.
     */
    tmt_close(vt);
    return EXIT_SUCCESS;
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

        /* let tmt know we've redrawn the screen */
        tmt_clean(vt);
        break;

    case TMT_MSG_ANSWER:
        /* the terminal has a response to give to the program; a is a
             * pointer to a string */
        printf("terminal answered %s\n", (const char *)a);
        break;

    case TMT_MSG_MOVED:
        /* the cursor moved; a is a pointer to the cursor's TMTPOINT */
        printf("cursor is now at %zd,%zd\n", c->r, c->c);
        break;
    }
}
