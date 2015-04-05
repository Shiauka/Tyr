

#ifndef CVS2SK_H
#define CVS2SK_H


typedef enum
{
    CVS2SK_PRICE  = 0,
    CVS2SK_DIVIDEND = 1,
    CVS2SK_EARNING = 2,
    CVS2SK_MAX = 0xFFFF,
}CVS2SK_CASE;

typedef struct
{
    unsigned int code;
    unsigned int datacount;
    unsigned int unidatasize;
    unsigned int datalength;
    unsigned int reserved[4];
}SK_HEADER;

typedef struct
{
    unsigned int year;
    float cash;
    float profit;
    float surplus;
    float stock;
    float total;
}SK_DIVIDEND;

bool SKApi_CVS2SK_Dividend(const int Code, const char *Inputfile, const char *Outputfile);
bool SKApi_CVS2SK_Help(void);


#endif

