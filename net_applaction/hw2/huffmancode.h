#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <utility>
#include <ctime>

using namespace std;

struct node{
    unsigned char c;
    int frequency;
    struct node *left, *right;
};

class huffmancompression{

private:

    int source_length, compress_length, real_compress_length, compress_percent, isfixed, fixed_bit_size;
    string fileaddress, filename, subfilename;
    vector<node*> huffmannode;
    map<unsigned char, string > huffmantable;
    node* root;

    void    maketestfilename();
    int     readfile();
    void    maketree();
    void    createtable(node* current, string s);
    int     writetable();
    int     writefile();
    void    convert(int, unsigned char []);
    string  int_to_binary_string(int number, int size_of_string);
    void    create_fixed_table();

public:

    huffmancompression();
    int compression(string source, bool fixed = false);
    string get_compression_file_path();
    string get_compression_file_name();
    int get_compress_length();

};

class huffmandecompression{
private:

    int source_length,compress_length,real_compress_length , compress_percent;
    string fileaddress,filename,subfilename;
    map<string,unsigned char> huffmantable;

    void fixed_decompression(int k, int n, int real_compress_length,unsigned int fixed_bit_size, FILE* out, FILE* in);
    void variable_decompression(int k, int n, int real_compress_length,unsigned int fixed_bit_size, FILE* out, FILE* in);

public:

    huffmandecompression();
    int decompression( string );

};

