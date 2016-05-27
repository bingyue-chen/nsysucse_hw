#include "PageReplacement.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

int MyPageReplace::getReplacePageIndex(int currentRandomIndex){
    
    int i, j, found, number, seed[this->framesize], seed_temp[this->framesize];


    for(j = 0, found = -1; j < this->framesize; j++){
        number = this->frames[j].getPageNumber();
        for(i = currentRandomIndex - 1; i>= 0 && i >= (currentRandomIndex - this->framesize*5); i--){
            if(this->pageReferenceNumber.getRandomNumber(i) == number)
                break;
        }
        seed[j] = seed_temp[j] = i;
    }

    std::sort(seed_temp, seed_temp+this->framesize);
    number = seed_temp[this->framesize/2];

    /*printf("number = %d\n", number);
    for(i = 0; i < this->framesize; i++){
        printf("seed[%d] = %d, seed_temp[%d] = %d\n", i, seed[i], i, seed_temp[i]);
    }
    printf("\n");*/

    for(i = 0; i < this->framesize; i++){
        if(number == seed[i]){
            if(this->frames[i].isModified()){
                this->write_back_times++;
                this->interrupt_times++;
            }
            return i;
        }
    }
    
    return -1;

}