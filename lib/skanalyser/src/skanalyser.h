

#ifndef SKANALYSER_H
#define SKANALYSER_H


typedef enum
{
    SKANALYSER_TYPE_DUMP = 0,
    SKANALYSER_TYPE_ANALYSIS,
    SKANALYSER_TYPE_MAX = 0xFFFF,
}SKANALYSER_TYPE;


/*earning(month) dividend method*/
typedef struct
{
    unsigned int year;
    unsigned int total;
    float percent;
    float stock;
    float cash;
}Earning_dividend_method;

/*financial dividend method*/
typedef struct
{
    unsigned int year;
    unsigned int season;
    float EPS;
    float stock;
    float cash;
    float ratio_stock; //EPS vs stock
    float ratio_cash; //EPS vs cash
}Fnancial_dividend_method;


bool SKApi_SKANALYSER_Dump(const char *Inputfile);
bool SKApi_SKANALYSER_Help(void);
bool SKApi_SKANALYSER_Fileread(const char *codelist, const char * path);
bool SKApi_SKANALYSER_Init(void);
void  SKApi_SKANALYSER_Deinit(void);

#endif

