#include "PageReplacement.h"

int EnhancedSecondChance::getReplacePageIndex(int currentRandomIndex){
    
    int i, j, round_times;
    bool found;

    round_times = 0;
    found = false;
    i = this->currentFrameIndex;

    while(!found){

        if(i == this->currentFrameIndex){
            round_times++;
        }

        if(round_times == 1 && !this->frames[i].isReferenced() && !this->frames[i].isModified()){
            found = true;
        }
        else if(round_times == 2 && this->frames[i].isReferenced() && !this->frames[i].isModified()){
            found = true;
        }
        else if(round_times == 3 && !this->frames[i].isReferenced() && this->frames[i].isModified()){
            found = true;
        }
        else if(round_times == 4 && this->frames[i].isReferenced() && this->frames[i].isModified()){
            found = true;
        }
        else{
            i++;
            i %= this->framesize;
        }
        
    }

    j = this->currentFrameIndex;
    while(j != i){

        if(this->frames[j].isReferenced()){
            this->frames[j].unreference();
            this->interrupt_times++;
        }

        j++;
        j %= this->framesize;
    }

    currentFrameIndex = (i + 1) % this->framesize;

    if(this->frames[i].isModified()){
        this->write_back_times++;
        this->interrupt_times++;
    }


    return i;

}