#include <stdexcept>
#include <string>

#include "Enigma.hpp"

char encodeChar(Machine& mch, char ch) {
    char ret = ch;

    /*
     * Only encode given char if it is a valid letter. Otherwise, perform
     * no encryption.
     */
    try {
	ret = mch.encode(ch).ascii();
    } catch (std::invalid_argument e) {}

    return ret;
}

std::string encodeString(Machine& mch, const std::string& str) {
    std::string ret(str.length(), ' ');

    for (std::string::size_type i = 0; i < str.length(); ++i)
	ret[i] = encodeChar(mch, str[i]);

    return ret;
}

std::string encodeFile(Machine& mch, std::ifstream& file) {
    if (!file.is_open())
	throw std::invalid_argument("encodeFile: file does not exist");

    std::string ret, s;

    while (!file.eof()) {
	getline(file, s);

	ret += encodeString(mch, s);
	ret.push_back('\n');
    }
    return ret;
}
