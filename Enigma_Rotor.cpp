#include <cstdlib>
#include <string>

#include "Enigma.hpp"

Rotor::Rotor(const std::string& param, ltr pos, ltr notch):
    m_pos(pos), m_notch(notch)
{
    if (param == "I") {
	m_notch = 'R';
	m_map = Map("EKMFLGDQVZNTOWYHXUSPAIBRCJ");
    } else if (param == "II") {
	m_notch = 'F';
	m_map = Map("AJDKSIRUXBLHWTMCQGZNPYFVOE");
    } else if (param == "III") {
	m_notch = 'W';
	m_map = Map("BDFHJLCPRTXVZNYEIWGAKMUSQO");
    } else if (param == "IV") {
	m_notch = 'K';
	m_map = Map("ESOVPZJAYQUIRHXLNFTGKDCMWB");
    } else if (param == "V") {
	m_notch = 'A';
	m_map = Map("VZBRGITYUPSDNHLXAWMJQOFECK");
    } else {
	m_map = Map(param);
    }
}

/* Helper function for randomize(). */
int randint() {
    int bucketSize = RAND_MAX / MAX_NALPHA;
    int n;

    do {
	n = rand() / bucketSize;
    } while (n >= MAX_NALPHA);

    return n;
}

void Rotor::randomize() {
    m_notch = randint();
    m_map.randomize();
}

/*
 * The general formula for determining which mapping to use:
 *   INPUT_CHAR + CUR_ROTOR_POS - PREV_ROTOR_POS
 */
ltr Rotor::signalIn(ltr l, ltr offset) {
    return m_map.mapsTo(l + m_pos - offset);
}

ltr Rotor::signalOut(ltr l, ltr offset) {
    return m_map.inverseMapsTo(l + m_pos - offset);
}
