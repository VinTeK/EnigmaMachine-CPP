#include <ctype.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "Enigma.hpp"

Letter::Letter(int num) {
    if (num < 0 || num >= MAX_NALPHA)
	throw std::invalid_argument("Letter(int): out of range");

    m_num = num;
    m_ascii = num+'A';
}

Letter::Letter(char ascii) {
    ascii = toupper(ascii);

    if (ascii < 'A' || ascii-'A' >= MAX_NALPHA)
	throw std::invalid_argument("Letter(char): out of range");

    m_ascii = ascii;
    m_num = ascii-'A';
}

bool Letter::isLetter(int ch) {
    try {
	Letter(ch);
    } catch (std::invalid_argument e) {
	return false;
    }
    return true;
}

bool Letter::isLetter(char ch) {
    try {
	Letter(ch);
    } catch (std::invalid_argument e) {
	return false;
    }
    return true;
}

Letter& Letter::operator++() {
    m_num = (m_num + 1) % MAX_NALPHA;
    m_ascii = m_num + 'A';
    return *this;
}

Letter Letter::operator++(int) {
    Letter ret = *this;
    m_num = (m_num + 1) % MAX_NALPHA;
    m_ascii = m_num + 'A';
    return ret;
}

Letter& Letter::operator--() {
    m_num = (ltr(m_num) - 1).num();
    m_ascii = m_num + 'A';
    return *this;
}

Letter Letter::operator--(int) {
    Letter ret = *this;
    m_num = (ltr(m_num) - 1).num();
    m_ascii = m_num + 'A';
    return ret;
}

/*
 * Friend functions
 */
Letter operator+(const Letter& let1, const Letter& let2) {
    return Letter((let1.m_num + let2.m_num) % MAX_NALPHA);
}

Letter operator-(const Letter& let1, const Letter& let2) {
    int result = (let1.m_num - let2.m_num) % MAX_NALPHA;

    return (result < 0) ? result + MAX_NALPHA : result;
}

bool operator==(const Letter& let1, const Letter& let2) {
    return let1.m_num == let2.m_num;
}

std::ostream& operator<<(std::ostream& os, const Letter& l) {
    os << l.m_ascii;
    return os;
}
