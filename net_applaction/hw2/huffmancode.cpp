/*
Author:陳秉躍 B013040040
Date:2013/11/27
Purpose:using huffman to compress and decompress
*/
#include "huffmancode.h"

#define huffman_read_buffer_size 1048576
#define huffman_write_buffer_size 1048576

static bool compare(const node *a, const node *b ){

    if( a->frequency == b->frequency )
        return a->c < b->c;
    else
        return a->frequency > b->frequency;

}

static unsigned char huffman_read_buffer[huffman_read_buffer_size];
static unsigned char huffman_write_buffer[huffman_write_buffer_size];

static int read_block_file(FILE* fileptr){
    return fread(huffman_read_buffer, sizeof(unsigned char), huffman_read_buffer_size, fileptr);
}

static int write_block_file(FILE* fileptr, size_t write_size){
    int n;
    write_size = (write_size < huffman_write_buffer_size) ? write_size : huffman_write_buffer_size;
    n = fwrite(huffman_write_buffer, sizeof(unsigned char), write_size, fileptr);
    fflush(fileptr);
    return n;
}

huffmancompression::huffmancompression(){

    source_length = compress_length = real_compress_length = compress_percent = isfixed = fixed_bit_size = 0;
    fileaddress = filename = subfilename = "";
    root = NULL;
    memset(huffman_read_buffer, 0, sizeof(huffman_read_buffer));
    memset(huffman_write_buffer, 0, sizeof(huffman_write_buffer));
}

int huffmancompression::compression(string source, bool fixed){

    this->fileaddress = source;
    maketestfilename();
    if(readfile()) return 1;

    if(!fixed){
        maketree();
        if(root != NULL)
            createtable(root, "" );
    }
    else{
        isfixed = 1;
        create_fixed_table();
    }

    if(writetable()) return 2;
    if(writefile()) return 3;

    cout << "file path = " << fileaddress << endl;
    cout << "file name = " << filename << endl;
    cout << "subfilename = " << subfilename << endl;
    cout << "source length = " << source_length << endl;
    cout << "after compress length = " << compress_length << endl;
    if(isfixed) cout << "using fixed huffman coding." << endl;
    else cout << "using variable huffman coding." << endl;
    cout << "compresspercent = " << compress_percent << "%" << endl;
    cout << endl;
    return 0;
}

void huffmancompression::maketestfilename(){

    size_t found1,found2;

    found1 = fileaddress.find_last_of(".");
    if( found1 != string::npos )
        subfilename = fileaddress.substr( found1 + 1 );

    found2 = fileaddress.find_last_of("\\");
    if( found2 != string::npos ){
        filename = fileaddress.substr( found2 + 1 , found1 - found2 - 1 );
        fileaddress = fileaddress.substr( 0 , found2 + 1 );
    }
    else{
        filename = fileaddress.substr( 0 ,  found1);
        fileaddress = "";
    }

}

int huffmancompression::readfile(){

    int n, i, count_word[256]={0};
    FILE* in;

    if ( ( in = fopen( ( fileaddress + filename + "." + subfilename ).c_str() , "rb" ) ) == NULL ) return 1;

    while((n = read_block_file(in)) > 0){
        while(n--){
            count_word[huffman_read_buffer[n]]++;
            source_length++;
        }
    }

    for(i = 0; i < 256; i++){
        if(count_word[i] > 0){
            node* newnode = new node;
            newnode->c = i;
            newnode->frequency = count_word[i];
            newnode->left = newnode->right = NULL;
            huffmannode.push_back(newnode);
        }
    }

    if(huffmannode.size() > 1)
        sort(huffmannode.begin(), huffmannode.end(), compare);

    fclose( in );
    return 0;
}

void huffmancompression::maketree(){

    while( huffmannode.size() > 1 ){

        node *newnode = new node;

        newnode->left = huffmannode[huffmannode.size()-1];
        newnode->right = huffmannode[huffmannode.size()-2];

        newnode->c = min(newnode->left->c, newnode->right->c);
        newnode->frequency = newnode->left->frequency + newnode->right->frequency;

        huffmannode.pop_back();
        huffmannode.pop_back();
        huffmannode.push_back( newnode );

        sort( huffmannode.begin(), huffmannode.end(), compare );
    }
    if( huffmannode.size() == 1 )
        root = huffmannode[0];
}

void huffmancompression::createtable( node* current , string s ){

    if( current->left != NULL )
        createtable( current->left , s + "0" );

    if( current->right != NULL )
        createtable( current->right , s + "1" );

    if( current->left == NULL && current->right == NULL ){
        if(huffmantable.empty()){
            fixed_bit_size = (s.length() > 0) ? s.length() : 1;
        }
        else{
            fixed_bit_size = ((unsigned int)fixed_bit_size < s.length()) ? fixed_bit_size : s.length();
        }
        if( current == root )huffmantable.insert( pair< unsigned char , string > ( current->c , "0" ) );
        else huffmantable.insert( pair< unsigned char , string > ( current->c , s ) );
    }

    delete current;
}

void huffmancompression::create_fixed_table(){

    int word_amount;

    for(word_amount = huffmannode.size()-1, fixed_bit_size = 1; word_amount > 1; word_amount = word_amount >> 1, fixed_bit_size++);

    word_amount = huffmannode.size();
    while(word_amount--){
        huffmantable.insert(pair<unsigned char, string>(huffmannode[word_amount]->c,  this->int_to_binary_string(word_amount, fixed_bit_size)));
    }
}

int huffmancompression::writetable(){

    FILE *out;

    if ( ( out = fopen( ( fileaddress + filename + "_Huffman_table.txt" ).c_str() , "w" ) ) == NULL ) return 1;

    fprintf(out, "ASCII -> Huffman code\n");

    for( map< unsigned char , string >::iterator it = huffmantable.begin() ; it != huffmantable.end() ; ++it )
        fprintf( out , "%5d -> %s\n" , it->first , it->second.c_str() );
    fclose( out );
    return 0;
}

int huffmancompression::writefile(){

    FILE *in,*out;
    unsigned char c,temp[8];
    int i, n, tempsize, k;

    if ( ( in = fopen( ( fileaddress + filename + "." + subfilename ).c_str() , "rb" ) ) == NULL ) return 1;
    if( subfilename == "snow" ) filename += ".snow";
    if ( ( out = fopen( ( fileaddress + filename + ".snow" ).c_str() , "wb" ) ) == NULL ) return 1;

    string s ="";

    fprintf( out , "1234123412341234" );
    compress_length = 16;

    fprintf( out , "%s," , subfilename.c_str() );
    compress_length += ( 1 + subfilename.length() );

    fprintf(out, "%c", (isfixed) ? 1 : 0);
    fprintf(out, "%c", (unsigned char)fixed_bit_size);
    compress_length += 2;

    for( map< unsigned char , string >::iterator it = huffmantable.begin() ; it != huffmantable.end() ; it++ ){
        fprintf( out , "%c%s," , it->first , ( it->second ).c_str() );
        compress_length += ( 2 + ( it->second ).length() );
    }

    tempsize = 0;
    while((n = read_block_file(in)) > 0){
        for(k = 0; k < n; k++){
            c = huffman_read_buffer[k];
            s += huffmantable.find( c )->second;
            while( s.length() >= 8 ){

                compress_length++;
                real_compress_length += 8;

                for( i = 0 ; i < 8 ; i++ )
                    temp[i] = (s[i] == '1') ? 1 : 0;

                s.erase( s.begin() , s.begin() + 8 );

                huffman_write_buffer[tempsize++] = (temp[0]<<7) | (temp[1]<<6) | (temp[2]<<5) | (temp[3]<<4) | (temp[4]<<3) | (temp[5]<<2) | (temp[6]<<1) | (temp[7]);

                if(tempsize >= huffman_write_buffer_size ){
                    write_block_file(out, tempsize);
                    tempsize = 0;
                }
            }
        }

    }

    if( s.length() > 0 ){

        compress_length ++;
        real_compress_length += s.length();

        for( i = 0 ; i < (int)s.length() ; i++ )
            temp[i] = (s[i] == '1') ? 1 : 0;

        for( ; i < 8 ; i++ )
            temp[i] = 0;

        huffman_write_buffer[tempsize++] = (temp[0]<<7) | (temp[1]<<6) | (temp[2]<<5) | (temp[3]<<4) | (temp[4]<<3) | (temp[5]<<2) | (temp[6]<<1) | (temp[7]);

    }

    if(tempsize > 0) write_block_file(out, tempsize);



    if( source_length > 0 )
        compress_percent = (float)compress_length / source_length * 100;

    rewind( out );
    unsigned char uc[4]={0};

    convert( source_length , uc );
    fprintf( out , "%c%c%c%c" , uc[0] , uc[1] , uc[2] , uc[3] );

    convert( compress_length , uc );
    fprintf( out , "%c%c%c%c" , uc[0] , uc[1] , uc[2] , uc[3] );

    convert( compress_percent , uc );
    fprintf( out , "%c%c%c%c" , uc[0] , uc[1] , uc[2] , uc[3] );

    convert( real_compress_length , uc );
    fprintf( out , "%c%c%c%c" , uc[0] , uc[1] , uc[2] , uc[3] );

    fclose( in );
    fclose( out );
    return 0;
}

void huffmancompression::convert(int x , unsigned char uc[4] ){

    static int base_256[4]={16777216,65536,256,1};

    for( int i = 0 ; i < 4 ; i++ ){
        uc[i] = (unsigned char)( x / base_256[i] );
        x = x%base_256[i];
    }

}

string huffmancompression::int_to_binary_string(int number, int size_of_string){
    string binary_string = "";
    number = number << (32-size_of_string);

    while(size_of_string--){
        binary_string += (number >> 31) ? "1" : "0";
        number <<= 1;
    }

    return binary_string;
}

string huffmancompression::get_compression_file_path(){
    return fileaddress + filename + ".snow";
}

string huffmancompression::get_compression_file_name(){
    return filename + ".snow";
}

int huffmancompression::get_compress_length(){
    return compress_length;
}

huffmandecompression::huffmandecompression(){
    source_length = compress_length = real_compress_length = compress_percent = 0;
    fileaddress = filename= subfilename = "";
    memset(huffman_read_buffer, 0, sizeof(huffman_read_buffer));
    memset(huffman_write_buffer, 0, sizeof(huffman_write_buffer));
}

int huffmandecompression::decompression( string source){

    int i, n, temp_size;
    unsigned char uc;
    FILE *in;

    if( source.find(".snow") == string::npos ) return 1;

    if( ( in = fopen( source.c_str() , "rb" ) ) == NULL ) return 1;

    /****** start read head ******/


    int base_256[4]={16777216,65536,256,1};

    n = read_block_file(in);
    temp_size = 0;

    for( i = 0 ; i < 4 ; i++ ){
        uc = huffman_read_buffer[temp_size++];
        source_length += ( base_256[i] * uc );
    }

    for( i = 0 ; i < 4 ; i++ ){
        uc = huffman_read_buffer[temp_size++];
        compress_length += ( base_256[i] * uc );
    }

    for( i = 0 ; i < 4 ; i++ ){
        uc = huffman_read_buffer[temp_size++];
        compress_percent += ( base_256[i] * uc );
    }

    for( i = 0 ; i < 4 ; i++ ){
        uc = huffman_read_buffer[temp_size++];
        real_compress_length += ( base_256[i] * uc );
    }

    cout << "source length = " << source_length << endl;
    cout << "after compress length = " << compress_length << endl;
    cout << "compresspercent = " << compress_percent << "%" << endl;

    do{
        uc = huffman_read_buffer[temp_size++];
        if( uc != ',' ) subfilename += uc;
    }while( uc != ',' );
    cout << "subfilename = " << subfilename << endl;

    string outfilename = "";
    size_t found;

    found = source.find_last_of(".");
    if( subfilename == "snow" )
        outfilename = source.substr( 0 , found );
    else
        outfilename = source.substr( 0 , found ) + "." + subfilename;

    cout << "output file path = " << outfilename << endl;

    unsigned char isfixed, fixed_bit_size;

    isfixed = huffman_read_buffer[temp_size++];
    fixed_bit_size = huffman_read_buffer[temp_size++];

    if(isfixed) cout << "using fixed huffman coding." << endl;
    else cout << "using variable huffman coding." << endl;


    int end_of_readtable = compress_length - 16 - subfilename.length() - 1 - 2 - real_compress_length / 8 - ( real_compress_length%8 != 0  );
    while( end_of_readtable ){

        unsigned char c;
        string s = "";

        uc = huffman_read_buffer[temp_size++];

        c = uc;
        do{
            uc = huffman_read_buffer[temp_size++];
            if( uc != ',' ) s += uc;
        }while( uc != ',' );
        end_of_readtable -= ( 2 + s.length() );
        huffmantable.insert( pair<string,unsigned char>( s , c ) );
    }

    /****** end read head ******/

    /****** start read file ******/

    FILE *out;

    if( ( out = fopen( outfilename.c_str() , "wb" ) ) == NULL ) return 1;

    if(isfixed) this->fixed_decompression(temp_size, n, real_compress_length, fixed_bit_size, out, in);
    else this->variable_decompression(temp_size, n, real_compress_length, fixed_bit_size, out, in);

    /****** end read file ******/

    cout << endl;
    fclose( in );
    fclose( out );

    return 0;
}

void huffmandecompression::variable_decompression(int k, int n, int real_compress_length,unsigned int fixed_bit_size, FILE* out, FILE* in){

    int temp_size, i;
    unsigned int j;
    unsigned char uc, base_2;
    string tempstring = "";
    map<string,unsigned char>::iterator it;

    temp_size = 0;
    do{
        for(; k < n; k++){
            uc = huffman_read_buffer[k];
            for( i = 0 , base_2 = 128; i < 8 && real_compress_length > 0 ; i ++ , real_compress_length-- ){
                tempstring += ( ( uc & base_2 ) == 0 ) ? "0" : "1";
                base_2 >>= 1;
            }
            for( j = fixed_bit_size ; j <= tempstring.length() ; j++ ){
                it = huffmantable.find( tempstring.substr( 0 , j ) );
                if( it != huffmantable.end() ){
                    huffman_write_buffer[temp_size++] = it->second;
                    tempstring.erase( 0 , j );
                    j = fixed_bit_size-1;
                    if(temp_size >= huffman_write_buffer_size){
                        write_block_file(out, temp_size);
                        temp_size = 0;
                    }
                }
            }
        }
        k = 0;
    }while((n = read_block_file(in)) > 0);

    if(temp_size > 0) write_block_file(out, temp_size);
}

void huffmandecompression::fixed_decompression(int k, int n, int real_compress_length,unsigned int fixed_bit_size, FILE* out, FILE* in){

    int temp_size, i;
    unsigned char uc, base_2;
    string tempstring = "";
    map<string,unsigned char>::iterator it;

    temp_size = 0;
    do{
        for(; k < n; k++){
            uc = huffman_read_buffer[k];
            for( i = 0 , base_2 = 128; i < 8 && real_compress_length > 0 ; i ++ , real_compress_length-- ){
                tempstring += ( ( uc & base_2 ) == 0 ) ? "0" : "1";
                base_2 >>= 1;
            }
            while(tempstring.length() >= fixed_bit_size){
                it = huffmantable.find( tempstring.substr( 0 , fixed_bit_size ) );
                huffman_write_buffer[temp_size++] = it->second;
                tempstring.erase( 0 , fixed_bit_size );
                if(temp_size >= huffman_write_buffer_size){
                    write_block_file(out, temp_size);
                    temp_size = 0;
                }
            }
        }
        k = 0;
    }while((n = read_block_file(in)) > 0);

    if(temp_size > 0) write_block_file(out, temp_size);

}

