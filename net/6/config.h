
struct TCP_Header_Format {

    unsigned short source_port;
    unsigned short dest_port;
    unsigned int sequence_number;
    unsigned int ack_number;

    unsigned int data_offset:4;         //This indicates wherethe data begins.
    unsigned int reserved:6;            // Reserved for future use.  Must be zero.
    unsigned int URG:1;                 //URG:  Urgent Pointer field significant
    unsigned int ACK:1;                 //ACK:  Acknowledgment field significant
    unsigned int PSH:1;                 //PSH:  Push Function
    unsigned int RST:1;                 //RST:  Reset the connection
    unsigned int SYN:1;                 //SYN:  Synchronize sequence numbers
    unsigned int FIN:1;                 //FIN:  No more data from sender

    unsigned short window;
    unsigned short checksum;
    unsigned short URGP;               /*points to the sequence number of the octet following the urgent data.*/

    /*optoin and padding wait to do*/

};

void show_TCP_Header(struct TCP_Header_Format TH);
