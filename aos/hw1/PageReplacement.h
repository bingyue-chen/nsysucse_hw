#ifndef PAGE_REPLACE_MENT_H
#define PAGE_REPLACE_MENT_H

#include "PageTable.h"
#include "Random.h"
#include <stdio.h>
#include <string>

#define DISPALY_PAGE_NUMER 20

class BasicPageReplacement : public PageTable {

protected:
    int pagefault_times, write_back_times, interrupt_times, display_flag;
    Random pageReferenceNumber;
    std::string name;
public:

    BasicPageReplacement(int framesize, int *number, int page_reference_times, int type, float pvalue, int display_flag)
    : PageTable(framesize, pvalue), pageReferenceNumber(number, page_reference_times, type){
        this->pagefault_times = this->write_back_times = this->interrupt_times = 0;
        this->name = "";
        this->display_flag = display_flag;
    };
    
    void print_times();
    void display();
    void doPageReference();
    int getPagefaultTimes();
    int getWriteBackTimes();
    int getInterruptTimes();


    virtual int getReplacePageIndex(int currentRandomIndex){return -1;};

};

class FIFO : public BasicPageReplacement {
private:
    int firt;

public:

    FIFO(int framesize, int *number, int page_reference_times, int type, float pvalue, int display_flag)
    : BasicPageReplacement(framesize, number, page_reference_times, type, pvalue, display_flag){
        this->firt = 0;
        this->name = "FIFO";
    };

    int getReplacePageIndex(int currentRandomIndex);
    
};

class Optimal : public BasicPageReplacement {

public:

    Optimal(int framesize, int *number, int page_reference_times, int type, float pvalue, int display_flag)
    : BasicPageReplacement(framesize, number, page_reference_times, type, pvalue, display_flag){
        this->name = "Optimal";
    };
    
    int getReplacePageIndex(int currentRandomIndex);

};

class EnhancedSecondChance : public BasicPageReplacement {
protected:
    int currentFrameIndex;
public:

    EnhancedSecondChance(int framesize, int *number, int page_reference_times, int type, float pvalue, int display_flag)
    : BasicPageReplacement(framesize, number, page_reference_times, type, pvalue, display_flag){
        this->name = "EnhancedSecondChance";
        this->currentFrameIndex = 0;
    };
    
    int getReplacePageIndex(int currentRandomIndex);

};

class MyPageReplace : public BasicPageReplacement {
protected:
    int currentFrameIndex;
public:

    MyPageReplace(int framesize, int *number, int page_reference_times, int type, float pvalue, int display_flag)
    : BasicPageReplacement(framesize, number, page_reference_times, type, pvalue, display_flag){
        this->name = "MyPageReplace";
        this->currentFrameIndex = 0;
    };
    
    int getReplacePageIndex(int currentRandomIndex);

};

#endif
