/*
 * Just another Enigma Machine simulator.
 */

#include <algorithm>
#include <iostream>
#include <string>

#include <ncurses.h>

#include "Enigma.hpp"

using namespace std;

char* getArg(char** beg, char** end, const string& arg);
bool argExists(char** beg, char** end, const string& arg);
void usage(const char* progName);
void backspace(WINDOW*);
void hudRefresh(WINDOW*, const Machine&, int nrotor);
void hudRotateRotor(WINDOW*, Machine&, int nrotor, bool rotateUp);

int main(int argc, char** argv) {
    /* Standard usage message. */
    if (argExists(argv, argv+argc, "-h")) {
	usage(argv[0]);
	return 1;
    }
    /* Initialize machine to user-provided rotor configuration. */
    char* machineParams = getArg(argv, argv+argc, "-r");
    Machine mch = (!machineParams) ? Machine() : Machine(machineParams);

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
    hudRefresh(hud, mch, highlightedRotor);

    while ((ch = getch()) != KEY_F(4)) {
	/*
	 * Ascii code for backspace. A portability issue as '127' and 
	 * KEY_BACKSPACE will behave differently depending on platform.
	 */
	if (ch == 127) {
	    backspace(plaintext);
	    backspace(ciphertext);
	    if (encodedChars > 0) {
		mch.unrotateRotors();
		--encodedChars;
	    }
	} else if (ch == KEY_UP) {
	    hudRotateRotor(hud, mch, highlightedRotor, true);
	} else if (ch == KEY_DOWN) {
	    hudRotateRotor(hud, mch, highlightedRotor, false);
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
	} else if (ch == KEY_F(2)) {
	    mch.randSetNthRotor(highlightedRotor);
	} else if (ch == KEY_F(3)) {
	    mch.randSetPlugboard();
	    mch.randSetReflector();
	    mch.randSetRotors();
	} else {
	    wprintw(plaintext, "%c", ch);
	    wprintw(ciphertext, "%c", encodeChar(mch, ch));
	    if (Letter::isLetter(char(ch)))
		++encodedChars;
	}
	refresh();
	wrefresh(plaintextBorder);
	wrefresh(ciphertextBorder);
	wrefresh(plaintext);
	wrefresh(ciphertext);
	hudRefresh(hud, mch, highlightedRotor);
    }
    delwin(plaintextBorder);
    delwin(ciphertextBorder);
    delwin(plaintext);
    delwin(ciphertext);
    delwin(hud);
    endwin();

    return 0;
}

char* getArg(char** beg, char** end, const string& arg) {
    char** itr = find(beg, end, arg);

    if (itr != end && ++itr != end)
	return *itr;
    /* No match found. */
    return NULL;
}

bool argExists(char** beg, char** end, const string& arg) {
    return find(beg, end, arg) != end;
}

void usage(const char* arg) {
    const static string params = " [-h] [-r ROTOR_SETTINGS]";
    cout << "usage: " << arg << params << endl;
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

void hudRefresh(WINDOW* hud, const Machine& mch, int highlightedRotor) {
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
    wattron(hud, A_BOLD);
    mvwprintw(hud, 1, w/2 - rotors.length(), rotors.c_str());
    wattroff(hud, A_BOLD);

    /* Highlight the selected rotor. */
    wmove(hud, 1, w/2 - rotors.length() + highlightedRotor);

    wrefresh(hud);
}

void hudRotateRotor(WINDOW* hud, Machine& mch, int nrotor, bool goUp) {
    string rotors = mch.rotorState();

    /* Rotate a rotor. Ugly conversions for modulo behavior. */
    if (goUp)
	rotors[nrotor] = ltr(ltr(rotors[nrotor]) + 1).ascii();
    else
	rotors[nrotor] = ltr(ltr(rotors[nrotor]) - 1).ascii();

    mch.setRotors(rotors);

    hudRefresh(hud, mch, nrotor);
}
