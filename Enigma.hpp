#ifndef GUARD_ENIGMA_H
#define GUARD_ENIGMA_H

#define MAX_NALPHA 26

#include <ctype.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

/*
 * Letter class:
 */
typedef class Letter {
    public:
	Letter(): m_num(0), m_ascii('A') {}
	Letter(int num);
	Letter(char ascii);

	static bool isLetter(int);
	static bool isLetter(char);

	int num() const   { return m_num; }
	char ascii() const { return m_ascii; }

	bool operator<(const Letter& rhs) const { return m_num < rhs.m_num; }
	Letter& operator++();
	Letter operator++(int);
	Letter& operator--();
	Letter operator--(int);

    private:
	int m_num;
	char m_ascii;

	friend Letter operator+(const Letter&, const Letter&);
	friend Letter operator-(const Letter&, const Letter&);
	friend bool operator==(const Letter&, const Letter&);
	friend std::ostream& operator<<(std::ostream&, const Letter&);
} ltr;
/*
 * Map class:
 */
class Map {
    public:
	Map();
	Map(const std::string&);

	std::map<ltr,ltr> mapping() const { return m_map; }
	std::map<ltr,ltr> inverseMapping() const {return m_inverse_map;}

	ltr mapsTo(const ltr&, const ltr& offset = 0);
	ltr inverseMapsTo(const ltr&, const ltr& offset = 0);

	void randomize();

    private:
	std::map<ltr, ltr> m_map;
	std::map<ltr, ltr> m_inverse_map;

	friend std::ostream& operator<<(std::ostream&, const Map&);
};
/*
 * Rotor class:
 */
class Rotor {
    public:
	Rotor() {}
	/* When using a preset, the notch cannot be set.  */
	Rotor(const std::string&, ltr pos = 'A', ltr notch = 'A');

	ltr pos() const { return m_pos; }
	ltr notch() const { return m_notch; }
	Map map() const { return m_map; }

	void randomize();
	void rotate() { ++m_pos; }
	void unrotate() { --m_pos; }
	void move(ltr n) { m_pos = n; };
	bool isInNotch() const { return m_pos == m_notch; }

	/* Returns the mapping as it enters the rotor from the right. */
	ltr signalIn(ltr, ltr offset = 0);
	/* Returns the mapping as it enters the rotor from the left. */
	ltr signalOut(ltr, ltr offset = 0);

    private:
	ltr m_pos;
	ltr m_notch;
	Map m_map;
};
/*
 * Machine class:
 */
class Machine {
    public:
	typedef std::vector<Rotor>::const_iterator const_rotor_iterator;
	typedef std::vector<Rotor>::iterator rotor_iterator;
	typedef std::vector<Rotor>::reverse_iterator reverse_rotor_iterator;
	/*
	 * Default machine settings:
	 * - unchanged plugboard (straight mapping)
	 * - reflector of type "B"
	 * - rotors of type "I", "II", and "III", from left to right
	 * - all rotors at 'A' position.
	 */
	Machine(const std::string& = "AAA");

	int nrotors() const { return m_rotors.size(); }
	std::string rotorState() const;

	void setNthRotor(int, const std::string&);
	/* Sets each rotor to a given position. */
	void setRotors(const std::string&);
	void randSetNthRotor(int);
	void randSetRotors();

	void setPlugboard(const std::string&);
	void randSetPlugboard() { m_plugboard.randomize(); }

	void setReflector(const std::string&);
	void randSetReflector() { m_reflector.randomize(); }

	void rotateRotors();
	void unrotateRotors();
	/* Goes through rotors from RIGHT to LEFT. */
	ltr signalEnterRotors(const ltr&);
	/* Goes through rotors from LEFT to RIGHT. */
	ltr signalExitRotors(const ltr&);
	ltr encode(const ltr&);

    private:
	Map m_plugboard;
	Map m_reflector;
	std::vector<Rotor> m_rotors;
	friend std::ostream& operator<<(std::ostream&, const Machine&);
};
/*
 * Interface for a Enigma machine
 */
char encodeChar(Machine&, char);
std::string encodeString(Machine&, const std::string&);
std::string encodeFile(Machine&, std::ifstream&);

#endif
