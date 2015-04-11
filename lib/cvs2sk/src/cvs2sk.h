

#ifndef CVS2SK_H
#define CVS2SK_H


typedef enum
{
    CVS2SK_PRICE  = 0,
    CVS2SK_DIVIDEND = 1,
    CVS2SK_EARNING = 2,
    CVS2SK_MAX = 0xFFFF,
}CVS2SK_CASE;


bool SKApi_CVS2SK_Dividend(const int Code, const char *Inputfile, const char *Outputfile);
bool SKApi_CVS2SK_Earning(const int Code, const char *Inputfile, const char *Outputfile_Month, const char *Outputfile_Season);
bool SKApi_CVS2SK_Price(const int Code, const char *Inputfilelist, const char *Outputfile);
bool SKApi_CVS2SK_Help(void);


#endif

