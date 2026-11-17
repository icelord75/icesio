#include <xc.h>
#include "mcc_generated_files/eusart2.h"

void print(char *string){
    uint8_t pos=0;
    // EUSART2_Write() waits for TX2IF, which never sets while the
    // transmitter is disabled (TXEN = 0 in the MCC setup): don't hang
    if (!TX2STAbits.TXEN) return;
    while (string[pos]!=0)
        EUSART2_Write(string[pos++]);
    return;
}
