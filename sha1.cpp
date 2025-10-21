#include "sha1.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>

uint cycle_shift_left(uint val, int bit_count) {
    return (val << bit_count | val >> (32 - bit_count));
}

uint bring_to_human_view(uint val) {
    return  ((val & 0x000000FF) << 24) |
        ((val & 0x0000FF00) << 8) |
        ((val & 0x00FF0000) >> 8) |
        ((val & 0xFF000000) >> 24);
}

//uint* sha1(std::string _pass)
std::string sha1(std::string _pass)
{    
    const char* pass = _pass.c_str();
    uint msize_bytes = _pass.size();    

    //èíèöèàëèçàöèÿ
    uint A = H[0];
    uint B = H[1];
    uint C = H[2];
    uint D = H[3];
    uint E = H[4];

    // ïîäñ÷åò öåëîãî ÷èñëà áëîêîâ
    uint totalBlockCount = msize_bytes / one_block_size_bytes;

    // ïîäñ÷åò, ñêîëüêî áàéò íóæíî, ÷òîáû äîïîëíèòü ïîñëåäíèé áëîê
    uint needAdditionalBytes =
        one_block_size_bytes - (msize_bytes - totalBlockCount * one_block_size_bytes);

    if (needAdditionalBytes < 8) {
        totalBlockCount += 2;
        needAdditionalBytes += one_block_size_bytes;
    }
    else {
        totalBlockCount += 1;
    }

    // ðàçìåð äîïîëíåííîãî ïî âñåì ïðàâèëàì ñîîáùåíèÿ
    uint extendedMessageSize = msize_bytes + needAdditionalBytes;

    // âûäåëÿåì íîâûé áóôåð è êîïèðóåì â íåãî èñõîäíûé
    unsigned char* newMessage = new unsigned char[extendedMessageSize];
    memcpy(newMessage, pass, msize_bytes);

    // ïåðâûé áèò ñòàâèì '1', îñòàëüíûå îáíóëÿåì
    newMessage[msize_bytes] = 0x80;
    memset(newMessage + msize_bytes + 1, 0, needAdditionalBytes - 1);

    // çàäàåì äëèíó èñõîäíîãî ñîîáùåíèÿ â áèòàõ
    uint* ptr_to_size = (uint*)(newMessage + extendedMessageSize - 4);
    *ptr_to_size = bring_to_human_view(msize_bytes * 8);

    ExpendBlock exp_block;
    //ðàóíäû ïîåõàëè
    for (int i = 0; i < totalBlockCount; i++) {

        // áåðåì òåêóùèé áëîê è äîïîëíÿåì åãî
        unsigned char* cur_p = newMessage + one_block_size_bytes * i;
        Block block = (Block)cur_p;

        // ïåðâûå 16 4áàéòîâûõ ÷èñåë
        for (int j = 0; j < one_block_size_uints; j++) {
            exp_block[j] = bring_to_human_view(block[j]);
        }
        // ñëåäóþùèå 64...
        for (int j = one_block_size_uints; j < block_expend_size_uints; j++) {
            exp_block[j] =
                exp_block[j - 3] ^
                exp_block[j - 8] ^
                exp_block[j - 14] ^
                exp_block[j - 16];
            exp_block[j] = cycle_shift_left(exp_block[j], 1);
        }

        // èíèöèàëèçàöèÿ 
        uint a = H[0];
        uint b = H[1];
        uint c = H[2];
        uint d = H[3];
        uint e = H[4];

        // ïåðåñ÷èòûâàåì
        for (int j = 0; j < block_expend_size_uints; j++) {
            uint f;
            uint k;
            // â çàâèñèìîñòè îò ðàóíäà ñ÷èòàåì ïî-ðàçíîìó
            if (j < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }
            else if (j < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            else if (j < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            // ïåðåìåøèâàíèå
            uint temp = cycle_shift_left(a, 5) + f + e + k + exp_block[j];
            e = d;
            d = c;
            c = cycle_shift_left(b, 30);
            b = a;
            a = temp;
        }
        // ïåðåñ÷èòûâàåì
        A = A + a;
        B = B + b;
        C = C + c;
        D = D + d;
        E = E + e;
    }

    // A,B,C,D,E ÿâëÿþòñÿ âûõîäíûìè 32á ñîñòàâëÿþùèìè ïîñ÷èòàííîãî õýøà
    uint* digest = new uint[5];
    digest[0] = A;
    digest[1] = B;
    digest[2] = C;
    digest[3] = D;
    digest[4] = E;

    // ÷èñòèì çà ñîáîé
    delete[] newMessage;
    
    std::ostringstream hash;
    for (int i = 0; i < SHA1HASHLENGTHUINTS; i++) {
        hash << std::hex << std::setw(8) << std::setfill('0') << digest[i];
    }    
    return hash.str();
}
