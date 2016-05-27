#include "PageReplacement.h"

int Optimal::getReplacePageIndex(int currentRandomIndex){

    int i, j, found, found_random_index, number;

    this->write_back_times++;
    this->interrupt_times++;

    for(j = found_random_index = 0, found = -1; j < this->framesize; j++){
        number = this->frames[j].getPageNumber();
        for(i = currentRandomIndex + 1; i < this->pageReferenceNumber.getAmount(); i++){
            if(this->pageReferenceNumber.getRandomNumber(i) == number)
                break;
        }
        if(i > found_random_index){
            found_random_index = i;
            found = j;
        }
    }

    return found;

}
