#include <xc.h>
#include "mcc_generated_files/lcd.h"
#include "lcd.h"

struct LCDdef Display;

//! Font for numbers.
const int numfont[16]={
        A|B|C|D|E|F, //0
        B|C,     //1
        A|B|G|E|D, //2
        A|B|G|C|D, //3
        F|G|B|C, //4
        A|F|G|C|D, //5
        A|F|G|E|C|D, //6
        A|B|C|F, //7
        A|B|C|D|E|F|G, //8
        A|B|G|F|C|D, //9
        A|F|B|G|E|C, //A
        F|E|G|C|D, //B
        A|F|E|D, //C
        E|G|C|D|B, //D
        A|F|E|G|D, //E
        A|G|F|E  //F
};

const uint8_t ASCII[96] = {
        0,         //(space)
        B|DP,    //!
        F|B,     //"
        F|E|B|C, //#
        A|F|G|D, //$
        A|F|C|D, //%
        G|B|A|F|E|D, //&
        F,       //'
        A|F|E|D, //(
        A|B|C|D, //)
        A|B|G|F, //*
        F|G|E,   //+
        C|D,     //,
        G,       //-
        DP,      //.
        B|G,     ///
        A|B|C|D|E|F, //0
        B|C,       //1
        A|B|G|E|D, //2
        A|B|G|C|D, //3
        F|G|B|C,   //4
        A|F|G|C|D, //5
        A|F|G|E|C|D, //6
        A|B|C|F,   //7
        A|B|C|D|E|F|G, //8
        A|B|G|F|C|D, //9
        G|D,     //:
        G|C|D,   //;
        A|F|G,   //<
        A|G,     //=
        A|B|G,     //>
        A|B|G|E, //?
        A|B|C|D|E|G, //@
        A|F|B|G|E|C, //A
        F|E|G|C|D, //B
        A|F|E|D,   //C
        E|G|C|D|B, //D
        A|F|E|G|D, //E
        A|G|F|E,   //F
        A|F|E|C|D, //G
        F|G|E|C,   //h
        F|E,       //I, distinguished from a 1 by being on the left side.
        E|B|C|D,   //J
        F|E|B|C|G|DP, //K, distinguished from an X by the DP.
        F|E|D,     //L
        A|E|C,     //M  (Less nerdy than mu, but more readable.)
        E|G|C,     //n
        A|B|C|D|E|F, //O
        F|A|B|G|E, //P
        F|A|B|C|D|E|DP,//Q
        E|G,       //R
        A|F|G|C|D, //S  (Looks like a 5.)
        F|E|G|D,   //T
        E|D|C,     //U  (Like a lowercase V)
        F|E|D|C|B, //V  (Like U.  Blame Rome.)
        F|B|D,     //W  (Inverted M)
        F|G|E|B|C, //X
        F|G|B|C|D, //Y
        A|B|G|E|D, //Z
        A|F|E|D, //[
        A|G,     //backslash
        A|B|C|D, //]
        F|A|B,   //^
        D,       //_
        B,       //`
        E|G|C|D|DP, //a
        F|E|D|C|G, //b
        G|E|D,   //c
        B|C|D|E|G, //d
        G|B|A|F|E|D, //e
        F|G|E,   //f
        G|F|A|B|C|D, //g
        F|E|G|C, //h
        C,       //i
        C|D,     //j
        F|E|G|B, //k
        F|E,     //l
        E|C,     //m
        E|G|C,   //n
        E|G|C|D, //o
        E|F|A|B|G, //p
        G|F|A|B|C, //q
        E|G,     //r
        A|F|G|C|D, //s
        F|G|E|D, //t
        E|D|C,   //u
        E|D|C,   //v
        E|C,     //w
        F|B|G|E|C, //x
        F|G|B|C|D, //y
        A|B|G|E|D, //z
        A|F|E|D, //{
        F|E,     //|
        A|B|C|D, //}
        A,       // ~
        0,       // (del)
};


/*
   Clear all display data
 */
void ClearDisplay(void){
        uint8_t *p = (uint8_t *)&Display;
        for (uint8_t i = 0; i < sizeof(Display); i++)
                p[i] = 0;
}
/*
   Segment map: which bit of Display drives which LCD segment.

   SEG(seg, com, field, bit): LCD pixel SEGseg/COMcom (bit SxxCy of LCDDATA)
   is lit when bit 'bit' of Display.field is set.
   LCDDATA layout (PIC16F19197): LCDDATA[com*6 + seg/8], bit seg%8.

   Table-driven instead of 92 separate bit assignments: XC8 compiled each
   assignment into ~13 instructions with a shift loop (~1100 words in total).
 */
/* Byte offsets of the fields in struct LCDdef (lcd.h) - keep in sync.
   offsetof() can't be used here: XC8 can't fold it in a const initializer. */
enum {
        OFS_sign1 = 0, OFS_sign2, OFS_digit8, OFS_digit9, OFS_digit10,
        OFS_digit0, OFS_digit1, OFS_digit2, OFS_digit3,
        OFS_digit4, OFS_digit5, OFS_digit6, OFS_digit7
};

#define SEG(seg, com, field, bit) \
        { (uint8_t)((((com) * 6 + (seg) / 8) << 3) | ((seg) % 8)), \
          (uint8_t)((OFS_##field << 3) | (bit)) }

static const uint8_t segmap[][2] = {
        SEG(31, 1, sign1,   SIGs),
        SEG(31, 2, sign1,   ALMs),
        SEG(31, 3, sign1,   Ks),
        SEG( 0, 1, digit0,  Es),
        SEG( 0, 2, digit0,  Fs),
        SEG( 0, 3, sign1,   AMs),
        SEG( 1, 1, digit0,  Ds),
        SEG( 1, 2, digit0,  Gs),
        SEG( 1, 3, digit0,  As),
        SEG( 2, 1, digit0,  DPs),
        SEG( 2, 2, digit0,  Cs),
        SEG( 2, 3, digit0,  Bs),
        SEG( 3, 1, digit1,  Es),
        SEG( 3, 2, digit1,  Fs),
        SEG( 3, 3, sign1,   PMs),
        SEG( 4, 1, digit1,  Ds),
        SEG( 4, 2, digit1,  Gs),
        SEG( 4, 3, digit1,  As),
        SEG( 5, 1, digit1,  Cs),
        SEG( 5, 2, digit1,  Bs),
        SEG( 6, 1, digit1,  DPs),
        SEG( 6, 2, digit2,  Es),
        SEG( 6, 3, digit2,  Fs),
        SEG( 7, 1, digit2,  Ds),
        SEG( 7, 2, sign2,   COLONs),
        SEG( 7, 3, digit2,  Gs),
        SEG(30, 1, digit2,  DPs),
        SEG(30, 2, digit2,  Cs),
        SEG(30, 3, digit2,  Bs),
        SEG( 8, 1, digit3,  Es),
        SEG( 8, 2, digit3,  Fs),
        SEG( 8, 3, sign1,   MULs),
        SEG( 9, 1, digit3,  Ds),
        SEG( 9, 2, digit3,  Gs),
        SEG( 9, 3, digit3,  As),
        SEG(10, 1, digit3,  DPs),
        SEG(10, 2, digit3,  Cs),
        SEG(10, 3, digit3,  Bs),
        SEG(11, 1, digit4,  Es),
        SEG(11, 2, digit4,  Fs),
        SEG(11, 3, sign1,   MINUSs),
        SEG(29, 1, digit4,  Ds),
        SEG(29, 2, digit4,  Gs),
        SEG(29, 3, digit4,  As),
#ifndef __DEBUG   // Debug pins - remove in HW V.2
        SEG(38, 1, digit4,  DPs),
        SEG(38, 2, digit4,  Cs),
        SEG(38, 3, digit4,  Bs),
#endif
        SEG(36, 1, digit5,  Es),
        SEG(36, 2, digit5,  Fs),
        SEG(36, 3, sign2,   PLUSs),
        SEG(37, 1, digit5,  Ds),
        SEG(37, 2, digit5,  Gs),
        SEG(37, 3, digit5,  As),
#ifndef __DEBUG   // Debug pins - remove in HW V.2
        SEG(39, 1, digit5,  DPs),
        SEG(39, 2, digit5,  Cs),
        SEG(39, 3, digit5,  Bs),
#endif
        SEG(33, 1, digit6,  Es),
        SEG(33, 2, digit6,  Fs),
        SEG(33, 3, digit8,  Ds),
        SEG(18, 1, digit6,  Ds),
        SEG(18, 2, digit6,  Gs),
        SEG(18, 3, digit6,  As),
        SEG(34, 1, digit6,  DPs),
        SEG(34, 2, digit6,  Cs),
        SEG(34, 3, digit6,  Bs),
        SEG(35, 1, digit7,  Es),
        SEG(35, 2, digit7,  Fs),
        SEG(15, 0, digit9,  Ds),
        SEG(15, 1, digit7,  Ds),
        SEG(15, 2, digit7,  Gs),
        SEG(15, 3, digit7,  As),
        SEG(41, 1, digit7,  DPs),
        SEG(41, 2, digit7,  Cs),
        SEG(41, 3, digit7,  Bs),
        SEG(19, 1, digit10, Bs),
        SEG(19, 2, digit10, Cs),
        SEG(19, 3, digit10, Ds),
        SEG(20, 1, digit10, As),
        SEG(20, 2, digit10, Gs),
        SEG(20, 3, digit10, Es),
        SEG(21, 1, digit10, DPs),
        SEG(21, 2, digit10, Fs),
        SEG(21, 3, digit9,  Cs),
        SEG(22, 1, digit9,  As),
        SEG(22, 2, digit9,  Bs),
        SEG(22, 3, digit9,  Gs),
        SEG(23, 1, digit8,  As),
        SEG(23, 2, digit9,  Fs),
        SEG(23, 3, digit9,  Es),
        SEG(24, 1, digit8,  Fs),
        SEG(24, 2, digit2,  As),
        SEG(24, 3, sign1,   DIVs),
};

static const uint8_t bitmask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

/*
   Draw LCD according to display data
 */
void DrawDisplay(void) {
        const uint8_t *src = (const uint8_t *)&Display;
        volatile uint8_t *lcd = &LCDDATA0;      // LCDDATA0..LCDDATA23 are contiguous

        for (uint8_t i = 0; i < sizeof(segmap) / sizeof(segmap[0]); i++) {
                uint8_t dst = segmap[i][0];
                uint8_t s   = segmap[i][1];
                uint8_t m   = bitmask[dst & 7];
                if (src[s >> 3] & bitmask[s & 7])
                        lcd[dst >> 3] |= m;
                else
                        lcd[dst >> 3] &= (uint8_t)~m;
        }
}

void DisplayPrint(char *STR){
        uint8_t *digit = &Display.digit0;       // digit0..digit7 are contiguous
        for (uint8_t i = 0; i < 8; i++)
                digit[i] = ASCII[(uint8_t)STR[i] - 32];
}
