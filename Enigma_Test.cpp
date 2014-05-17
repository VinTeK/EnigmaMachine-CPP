#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Enigma.hpp"

using namespace std;

int main() {
    srand(time(NULL));

    /*
     * Letter testing.
     */
    cout << "\'A\' is a letter? : " << Letter::isLetter('A') << endl;
    cout << "1 is a letter?   : " << Letter::isLetter(1) << endl;
    cout << "\'&\' is a letter? : " << Letter::isLetter('&') << endl;
    cout << "27 is a letter?  : " << Letter::isLetter(27) << endl << endl;

    Machine m;
    /*
     * String encryption.
     */ 
    string savedSettings = m.rotorState();
    string plainText = "The quick brown fox jumps over the lazy dog.";
    string cipherText = encodeString(m, plainText);
    m.setRotors(savedSettings);
    string decipherText = encodeString(m, cipherText);

    cout << "plainText: " << plainText << endl;
    cout << "cipherText: " << cipherText << endl;
    cout << "decipherText: " << decipherText << endl << endl;
    /*
     * File encryption.
     */ 
    ifstream file("words.txt");
    savedSettings = m.rotorState();
    string cipherFile = encodeFile(m, file);
    m.setRotors(savedSettings);
    string decipherFile = encodeString(m, cipherFile);

    cout << "cipherFile: " << cipherFile;
    cout << "decipherFile: " << decipherFile;

    return 0;
}
