#include <iostream>
#include <string>

#include <ncurses.h>

#include "Enigma.hpp"

using namespace std;

void updateHud(WINDOW*, const Machine&);

int main(int argc, char** argv) {
    Machine mch("AEU");

    int ch, h, w, encodedChars = 0;

    /* ncurses settings. */
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
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

    while ((ch = getch()) != 'Q') {
	if (ch == 127) {
	    int y, x;
	    getyx(plaintext, y, x);

	    wmove(plaintext, y, x-1);
	    wdelch(plaintext);

	    wmove(ciphertext, y, x-1);
	    wdelch(ciphertext);

	    if (encodedChars > 0) {
		mch.unrotateRotors();
		--encodedChars;
	    }
	} else {
	    wprintw(plaintext, "%c", ch);
	    wprintw(ciphertext, "%c", encodeChar(mch, ch));
	    if (Letter::isLetter(ch)) {
		//wprintw(plaintext, "INCRE");
		++encodedChars;
	    }
	}

	wrefresh(plaintextBorder);
	wrefresh(ciphertextBorder);
	updateHud(hud, mch);
	wrefresh(plaintext);
	wrefresh(ciphertext);
    }
    delwin(plaintextBorder);
    delwin(ciphertextBorder);
    delwin(plaintext);
    delwin(ciphertext);
    delwin(hud);
    endwin();

    return 0;
}

void updateHud(WINDOW* hud, const Machine& mch) {
    const static string ROTOR_PROMPT = "rotor positions";
    string rotors = mch.rotorState();
    int h, w;
    getmaxyx(hud, h, w);

    werase(hud);

    wattron(hud, A_UNDERLINE);
    mvwprintw(hud, 0, (w/2+6) - ROTOR_PROMPT.length(), ROTOR_PROMPT.c_str());
    wattroff(hud, A_UNDERLINE);
    wattron(hud, A_BOLD);
    mvwprintw(hud, 1, w/2 - rotors.length(), rotors.c_str());
    wattroff(hud, A_BOLD);

    wrefresh(hud);
}
