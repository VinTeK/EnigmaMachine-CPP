#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Enigma.hpp"

Machine::Machine() {
    setReflector("B");
    m_rotors.push_back(Rotor("I"));
    m_rotors.push_back(Rotor("II"));
    m_rotors.push_back(Rotor("III"));
}

Machine::Machine(const std::string& param) {
    for (std::string::size_type i = 0; i < param.length(); ++i) {
	Rotor r;
	r.move(param[i]);
	m_rotors.push_back(r);
    }
}

std::string Machine::rotorState() const {
    const_rotor_iterator b = m_rotors.begin(), e = m_rotors.end();
    std::string ret;

    while (b != e)
	ret.push_back((*b++).pos().ascii());

    return ret;
}

void Machine::setRotors(const std::string& param) {
    if (param.length() != m_rotors.size())
	throw std::invalid_argument("Machine(string): size doesn't match");

    for (std::string::size_type i = 0; i < param.length(); ++i)
	m_rotors[i].move(param[i]);
}

void Machine::randSetNthRotor(int n) {
    m_rotors[n].randomize();
}

void Machine::randSetRotors() {
    rotor_iterator b = m_rotors.begin(), e = m_rotors.end();
    while (b != e)
	(*b++).randomize();
}

void Machine::setReflector(const std::string& param) {
    if (param == "B") {
	m_reflector = Map("YRUHQSLDPXNGOKMIEBFZCWVJAT");
    } else if (param == "C") {
	m_reflector = Map("FVPJIAOYEDRZXWGCTKUQSBNMHL");
    } else {
	m_reflector = Map(param);
    }
}

//TODO:
bool doDoubleStep() {
    return false;
}

void Machine::rotateRotors() {
    reverse_rotor_iterator b = m_rotors.rbegin(), e = m_rotors.rend();

    (*b).rotate();
    
    while (b != e) {
	/* Only rotate neighboring rotor if this rotor is in a notch. */
	if (!(*b).isInNotch()) break;
	(*(++b)).rotate();
    }
}

void Machine::unrotateRotors() {
    reverse_rotor_iterator b = m_rotors.rbegin(), e = m_rotors.rend();

    (*b).unrotate();

    while (b != e) {
	/* Only unrotate neighboring rotor if this rotor was in a notch one
	 * rotation ago. */
	if (!((*b).pos()+1 == (*b).notch())) break;
	(*(++b)).unrotate();
    }
}

ltr Machine::signalEnterRotors(const ltr& l) {
    reverse_rotor_iterator b = m_rotors.rbegin(), e = m_rotors.rend();
    ltr ret;

    /* Always rotate the 'fast' rotor upon entering the machine. */
    rotateRotors();

    /* From plugboard to fast rotor. */
    if (b != e)
	ret = (*b++).signalIn(l);
    while (b != e)
	ret = (*b++).signalIn(ret, (*(b-1)).pos());

    return ret;
}

ltr Machine::signalExitRotors(const ltr& l) {
    rotor_iterator b = m_rotors.begin(), e = m_rotors.end();
    ltr ret;

    /* From reflector to 'slow' rotor. */
    if (b != e)
	ret = (*b++).signalOut(l);
    while (b != e)
	ret = (*b++).signalOut(ret, (*(b-1)).pos());

    return ret;
}

ltr Machine::encode(const ltr& l) {
    ltr ret;

    ret = m_plugboard.mapsTo(l);
    ret = signalEnterRotors(ret);
    ret = m_reflector.mapsTo(ret, m_rotors[0].pos());
    ret = signalExitRotors(ret);
    ret = m_plugboard.inverseMapsTo(ret, m_rotors[m_rotors.size()-1].pos());

    return ret;
}
/*
 * Friend functions
 */
std::ostream& operator<<(std::ostream& os, const Machine& mch) {
    typedef std::vector<Rotor>::const_iterator vec_itr;

    vec_itr b = mch.m_rotors.begin(), e = mch.m_rotors.end();

    while (b != e)
	os << (*b++).pos();

    return os;
}
