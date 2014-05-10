#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "Enigma.hpp"

Map::Map() {
    for (int i = 0; i < MAX_NALPHA; ++i)
	m_map[i] = i;

    m_inverse_map = m_map;
}

Map::Map(const std::string& param) {
    if (param.length() > MAX_NALPHA)
	throw std::invalid_argument("Map(string): param too long");

    for (int i = 0; i < param.length(); ++i)
	m_map[i] = param[i];
    /* Set inverse mapping. */
    for (int i = 0; i < m_map.size(); ++i)
	m_inverse_map[m_map[i]] = i;
}

ltr Map::mapsTo(const ltr& l, const ltr& offset) {
    if (m_map.find(l - offset) == m_map.end())
	throw std::invalid_argument("Map::mapsto: no mapping");

    return m_map[l - offset];
}

ltr Map::inverseMapsTo(const ltr& l, const ltr& offset) {
    if (m_inverse_map.find(l - offset) == m_inverse_map.end())
	throw std::invalid_argument("Map::inverse_mapsto: no inverse mapping");

    return m_inverse_map[l - offset];
}

void Map::randomize() {
    typedef std::map<ltr,ltr>::const_iterator map_itr;

    /* Shuffle around some letters to remap. */
    std::vector<ltr> tmp;
    for (map_itr i = m_map.begin(); i != m_map.end(); ++i)
	tmp.push_back(i->second);
    std::random_shuffle(tmp.begin(), tmp.end());

    /* Set shuffled result to map. */
    for (int i = 0; i < m_map.size(); ++i)
	m_map[i] = tmp[i];
}
/*
 * Friend functions
 */
std::ostream& operator<<(std::ostream& os, const Map& m) {
    std::map<ltr, ltr>::const_iterator b = m.m_map.begin();
    std::map<ltr, ltr>::const_iterator e = m.m_map.end();

    if (b != e) {
	os << b->first << "->" << b->second;
	++b;
    }
    while (b != e) {
	os << ", " << b->first << "->" << b->second;
	++b;
    }
    return os;
}
