#include <iostream>
#include <string>

#include <ncurses.h>

#include "Enigma.hpp"

using namespace std;

void backspace(WINDOW*);
void hudRefresh(WINDOW*, const Machine&);
void hudRotateRotor(WINDOW*, const Machine&, int nrotor, int direction);

int main(int argc, char** argv) {
    Machine mch("AEU");

    int ch, h, w, encodedChars = 0, highlightedRotor = mch.nrotors()-1;

    /* ncurses settings. */
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, h, w);

    /* Create borders and display windows. */
    WINDOW* plaintextBorder = newwin(h/2-1, w, 0, 0);
    wborder(plaintextBorder, 0, 0, 0, 0, 0, 0, 0, 0);
    wattron(plaintextBorder, A_BOLD | COLOR_PAIR(1));
    wprintw(plaintextBorder, "plaintext");

    WINDOW* ciphertextBorder = newwin(h/2-1, w, h/2+1, 0);
    wborder(ciphertextBorder, 0, 0, 0, 0, 0, 0, 0, 0);
    wattron(ciphertextBorder, A_BOLD | COLOR_PAIR(2));
    mvwprintw(ciphertextBorder, 0, w-10, "ciphertext");

    WINDOW* plaintext = newwin(h/2-3, w-2, 1, 1);
    WINDOW* ciphertext = newwin(h/2-3, w-2, h/2+2, 1);
    WINDOW* hud = newwin(2, w, h/2-1, 0);

    /* Initial refresh to display. */
    refresh();
    wrefresh(plaintextBorder);
    wrefresh(ciphertextBorder);
    wrefresh(plaintext);
    wrefresh(ciphertext);
    hudRefresh(hud, mch);

    while ((ch = getch()) != 'Q') {
	if (ch == 127) { // backspace
	    backspace(plaintext);
	    backspace(ciphertext);
	    if (encodedChars > 0) {
		mch.unrotateRotors();
		--encodedChars;
	    }
	} else if (ch == KEY_UP) {
	    hudRotateRotor(hud, mch, highlightedRotor, 1);
	} else if (ch == KEY_DOWN) {
	    hudRotateRotor(hud, mch, highlightedRotor, 0);
	} else if (ch == KEY_LEFT) {
	    if (highlightedRotor == 0)
		highlightedRotor = mch.nrotors()-1;
	    else
		--highlightedRotor;
	} else if (ch == KEY_RIGHT) {
	    if (highlightedRotor == mch.nrotors()-1)
		highlightedRotor = 0;
	    else
		++highlightedRotor;
	} else {
	    wprintw(plaintext, "%c", ch);
	    wprintw(ciphertext, "%c", encodeChar(mch, ch));
	    if (Letter::isLetter(char(ch))) {
		++encodedChars;
	    }
	}
	refresh();
	wrefresh(plaintextBorder);
	wrefresh(ciphertextBorder);
	wrefresh(plaintext);
	wrefresh(ciphertext);
	hudRefresh(hud, mch);
    }
    delwin(plaintextBorder);
    delwin(ciphertextBorder);
    delwin(plaintext);
    delwin(ciphertext);
    delwin(hud);
    endwin();

    return 0;
}

void backspace(WINDOW* win) {
    int h, w, y, x;
    getmaxyx(win, h, w);
    getyx(win, y, x);

    /* Still more text to delete in lines above. */
    if (y != 0 && x == 0) {
	--y;
	x = w;
    }

    wmove(win, y, x-1);
    wdelch(win);
}

void hudRefresh(WINDOW* hud, const Machine& mch) {
    const static string ROTOR_PROMPT = "rotor positions";
    string rotors = mch.rotorState();
    int h, w;
    getmaxyx(hud, h, w);

    werase(hud);

    /* Print prompt. */
    wattron(hud, A_UNDERLINE | COLOR_PAIR(3));
    mvwprintw(hud, 0, w/2+6 - ROTOR_PROMPT.length(), ROTOR_PROMPT.c_str());
    wattroff(hud, A_UNDERLINE | COLOR_PAIR(3));

    /* Print current rotor state. */
    mvwprintw(hud, 1, w/2 - rotors.length(), rotors.c_str());

    wrefresh(hud);
}

void hudRotateRotor(WINDOW* hud, const Machine& mch, int nrotor, int dir) {
    string rotors = mch.rotorState();
    /* When dir == 0, go down a letter; dir == 1, go up a letter. */
    ltr rotated = (dir == 0) ? rotors[nrotor]-ltr(1) : ltr(rotors[nrotor]+ltr(1));
    int h, w;
    getmaxyx(hud, h, w);

    /* Changed just the one rotor that rotates. */
    wprintw(hud, "DO STUFF");
    mvwaddch(hud, 1, (w/2-rotors.length())+nrotor, rotated.ascii()|A_REVERSE);

    wrefresh(hud);
}
