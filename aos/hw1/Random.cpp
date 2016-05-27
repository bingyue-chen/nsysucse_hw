#include "Random.h"

int Random::getRandomNumber(int index){

    if(index < 0 || index >= amount)
        return -1;

    return this->number[index];

}

int Random::getAmount(){
    return this->amount;
}
