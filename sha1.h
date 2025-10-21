#ifndef SHA1_H
#define SHA1_H

#include <string>

typedef unsigned int uint;

#define one_block_size_bytes 64 // êîëè÷åñòâî áàéò â áëîêå
#define one_block_size_uints 16 // êîëè÷åñòâî 4áàéòîâûõ â áëîêå
#define block_expend_size_uints 80 // êîëè÷åñòâî 4áàéòîâûõ â äîïîëíåííîì áëîêå

#define SHA1HASHLENGTHBYTES 20
#define SHA1HASHLENGTHUINTS 5

typedef uint* Block;
typedef uint ExpendBlock[block_expend_size_uints];

const uint H[5] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
    0xC3D2E1F0
}; // êîíñòàíòû, ñîãëàñíî ñòàíäàðòó

uint cycle_shift_left(uint val, int bit_count);
uint bring_to_human_view(uint val);

std::string sha1(std::string pass); // îòäàííûé ìàññèâ íóæíî óäàëèòü âðó÷íóþ

#endif // SHA1_H
