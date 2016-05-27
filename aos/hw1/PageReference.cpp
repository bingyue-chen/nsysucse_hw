#include "PageReplacement.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <ctime>

void QTest();

int main(int argc, char const *argv[]) {
    
    int framesize, page_reference_times, display_flag, type, pagefaultTimes;
    int *number = NULL;
    float pvalue;

    if(argc < 2){
        printf("%s {FIFO/OPT/ESC/MPR}\n",argv[0]);
        return 0;
    }

    printf("set framesize: ");
    scanf("%d", &framesize);

    printf("set reference times: ");
    scanf("%d", &page_reference_times);

    printf("set modify p value: ");
    scanf("%f", &pvalue);

    printf("display course(1/0): ");
    scanf("%d", &display_flag);

    printf("set reference string{1(random), 2(local), 3(user define)}: ");
    scanf("%d", &type);
    
    if(type == 3){
        printf("set page number string(1 3 5 7 9 2 4 6 8 ...):\n");
        number = new int[page_reference_times];
        for(int i = 0; i < page_reference_times; i++){
            scanf("%d", &number[i]);
        }
    }
    else{
        number = new int[1];
        number[0] = 600;
    }

    std::srand(std::time(0));

    if(strcmp(argv[1], "FIFO") == 0){
        FIFO fifo(framesize, number, page_reference_times, type, pvalue, display_flag);
        fifo.doPageReference();
        fifo.print_times();
    }
    else if(strcmp(argv[1], "OPT") == 0){
        Optimal optimal(framesize, number, page_reference_times, type, pvalue, display_flag);
        optimal.doPageReference();
        optimal.print_times();
    }
    else if(strcmp(argv[1], "ESC") == 0){
        EnhancedSecondChance enhancedSecondChance(framesize, number, page_reference_times, type, pvalue, display_flag);
        enhancedSecondChance.doPageReference();
        enhancedSecondChance.print_times();
    }
    else if(strcmp(argv[1], "MPR") == 0){
        MyPageReplace myPageReplace(framesize, number, page_reference_times, type, pvalue, display_flag);
        myPageReplace.doPageReference();
        myPageReplace.print_times();
    }

    
    delete[] number; 

    return 0;
}
