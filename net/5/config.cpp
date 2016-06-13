#include <stdio.h>
#include "config.h"

void show_TCP_Header(struct TCP_Header_Format TH){
    printf("         Get a packet (seq = %d , ack = %d)\n",TH.sequence_number, TH.ack_number);
}
