#include "PageTable.h"

/**
 * define Frames
 */

void PageTable::Frame::reset(){

    this->page_number = null_page_number;
    this->r_bit = false;
    this->m_bit = false;

}

void PageTable::Frame::setPageNumber(int page_number){

    this->page_number = page_number;

}

int PageTable::Frame::getPageNumber(){

    return this->page_number;

}

void PageTable::Frame::reference(){

    this->r_bit = true;

}

void PageTable::Frame::modify(){

    this->m_bit = true;

}

void PageTable::Frame::unreference(){

    this->r_bit = false;

}


bool PageTable::Frame::isReferenced(){

    return this->r_bit;

}

bool PageTable::Frame::isModified(){

    return this->m_bit;

}

int PageTable::getFrame(int number){
    int i, page_number;

    for(i = 0; i < this->framesize; ++i){
        if(this->frames[i].getPageNumber() == number){
            this->frames[i].reference();
            return i;
        }
    }

    return -1;
}

bool PageTable::addPage(int number){
    int i;

    for(i = 0; i < this->framesize; ++i){
        if(this->frames[i].getPageNumber() == null_page_number){
            return this->replacePage(i, number);
        }
    }

    return false;

}

bool PageTable::replacePage(int index, int number){

    if(index < 0 || index > this->framesize) return false;
    
    this->frames[index].reset();
    this->frames[index].setPageNumber(number);
    this->frames[index].reference();

    return true;

}

void PageTable::setModify(){
    int i;
    float r;

    for(i = 0; i < this->framesize; ++i){
        if(this->frames[i].getPageNumber() != null_page_number && !this->frames[i].isModified()){
            r = ((std::rand() % 100) + 1)/100.0;
            if(r >= (1-this->pvalue))
                this->frames[i].modify();
        }
    }
}
