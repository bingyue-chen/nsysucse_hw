#include <cstdlib>
#include <ctime>

class Random{

protected:
    int amount, *number;

    void set(int *number, int amount){
        int i;
        for(i = 0; i < this->amount; i++)
            this->number[i] = number[i];
    }

public:
    Random(int *number, int amount, int type){

        int random_number, i, j, end;

        this->amount = amount;
        this->number = new int[amount];

        if(type == 3){
            this->set(number, amount);
        }
        else{
            
            end = *number;

            for(i = 0; i < this->amount; i++){
                this->number[i] = (std::rand() % end) + 1;
                if(type == 2){
                    random_number = (std::rand() % (int)(end/6)) + (int)(end/12);
                    for(j = 1; j <= random_number && this->number[i] + j <= end && i + j < this->amount; j++){
                        this->number[i+j] = this->number[i] + j;
                    }
                    i += j;
                }
            }
        }
    }

    ~Random(){
        delete[] this->number;
    };

    int  getRandomNumber(int index);
    int  getAmount();
    
};
