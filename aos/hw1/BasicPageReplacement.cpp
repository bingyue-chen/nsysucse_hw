#include "PageReplacement.h"
#include <stdio.h>

int BasicPageReplacement::getPagefaultTimes() { return this->pagefault_times;  };
int BasicPageReplacement::getWriteBackTimes() { return this->write_back_times; };
int BasicPageReplacement::getInterruptTimes() { return this->interrupt_times;  };

void BasicPageReplacement::display(){ this->display_flag = 1; }

void BasicPageReplacement::print_times(){
    printf("\n");
    printf("%s page fault times: %d\n", this->name.c_str(), this->getPagefaultTimes());
    printf("%s write back times: %d\n", this->name.c_str(), this->getWriteBackTimes());
    printf("%s interrupt  times: %d\n", this->name.c_str(), this->getInterruptTimes());
}

void BasicPageReplacement::doPageReference(){

    int i, page_number, replace_page_index;

    for(i =  0; i < this->pageReferenceNumber.getAmount(); i++, this->setModify()){

        page_number = this->pageReferenceNumber.getRandomNumber(i);
        if(this->display_flag){
            printf("\npage number: [");
            for(int j = 0; j < DISPALY_PAGE_NUMER; j++){
                printf("%-5d", this->pageReferenceNumber.getRandomNumber(i+j));
            }
            printf("]\n");
        }

        if(this->display_flag) {
            printf("pagetable:   [");
            for(int j = 0; j < this->framesize; j++){
                printf("%5d(%d,%d) ", this->frames[j].getPageNumber(), (this->frames[j].isReferenced()), (this->frames[j].isModified()));
            }
            printf("]\n");
        }

        if(BasicPageReplacement::PageTable::getFrame(page_number) == -1){
            if(this->display_flag) printf("\e[31mpage fault\e[0m\n");
            this->pagefault_times++;
            this->interrupt_times++;

            if(!BasicPageReplacement::PageTable::addPage(page_number)){
                int replace_page_index = -1;

                if(FIFO *f = dynamic_cast<FIFO*>(this)){
                    replace_page_index = f->getReplacePageIndex(i);
                }
                else if(Optimal *o = dynamic_cast<Optimal*>(this)){
                    replace_page_index = o->getReplacePageIndex(i);
                }
                else if(EnhancedSecondChance *e = dynamic_cast<EnhancedSecondChance*>(this)){
                    replace_page_index = e->getReplacePageIndex(i);
                }
                else{
                    replace_page_index = this->getReplacePageIndex(i);
                }

                BasicPageReplacement::PageTable::replacePage(replace_page_index, page_number);

            }

            if(this->display_flag) {
                printf("pagetable:   [");
                for(int j = 0; j < this->framesize; j++){
                    printf("%5d(%d,%d) ", this->frames[j].getPageNumber(), (this->frames[j].isReferenced()), (this->frames[j].isModified()));
                }
                printf("]\n");
            }

        }
        else{
            if(this->display_flag) printf("\e[34mpage hit\e[0m\n");
        }

        if(this->display_flag)
            printf("\n--------------------\n");
    
    }
    
}
