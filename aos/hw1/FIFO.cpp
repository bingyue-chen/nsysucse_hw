#include "PageReplacement.h"

int FIFO::getReplacePageIndex(int currentRandomIndex){
    
    int temp;

    this->write_back_times++;
    this->interrupt_times++;

    temp = this->firt++;
    this->firt %= this->framesize;

    return temp;

}