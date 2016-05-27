#ifndef null_page_number
#define null_page_number -1
#endif

#include <cstdlib>
#include <ctime>

class PageTable {

protected:
    class Frame {

    private:
        int page_number;
        bool r_bit, m_bit;
    public:
        Frame(){ this->reset(); };
        //~Frames();
        void reset();
        void setPageNumber(int page_number);
        int  getPageNumber();
        void reference();
        void modify();
        void unreference();
        bool isReferenced();
        bool isModified();
    };

    int framesize;
    float pvalue;
    Frame *frames;

    int   getFrame(int number);
    bool  addPage(int number);
    bool  replacePage(int index, int number);
    void  setModify();

public:
    PageTable(int framesize, float pvalue){
        std::srand(std::time(0));
        this->framesize = framesize;
        this->pvalue = pvalue;
        this->frames = new Frame[this->framesize];
    };

    virtual ~PageTable(){ delete[] this->frames; };
    
};
