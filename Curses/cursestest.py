from curses import wrapper
import curses
stdscr = curses.initscr()
curses.noecho()
curses.cbreak()

stdscr.keypad(True)


def main(stdscr):
    # Clear screen
    stdscr.clear()
    curses.init_pair(1, curses.COLOR_RED, curses.COLOR_WHITE)

    # This raises ZeroDivisionError when i == 10.
    stdscr.addstr(0, 0, "Current mode: Typing mode",
                  curses.A_REVERSE)

    stdscr.addstr("Pretty text", curses.color_pair(1))

    stdscr.refresh()
    stdscr.getkey()


wrapper(main)

curses.echo()
curses.nocbreak()
stdscr.keypad(True)
