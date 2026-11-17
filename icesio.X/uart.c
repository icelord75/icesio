#include "mcc_generated_files/eusart2.h"

void print(char *string){
    uint8_t pos=0;
    while (string[pos]!=0)
        EUSART2_Write(string[pos++]);
    return;
}
