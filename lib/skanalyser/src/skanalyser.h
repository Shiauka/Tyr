

#ifndef SKANALYSER_H
#define SKANALYSER_H


typedef enum
{
    SKANALYSER_TYPE_DUMP = 0,
    SKANALYSER_TYPE_ANALYSIS = 1,
    SKANALYSER_TYPE_RENTSTOCK = 2,
    SKANALYSER_TYPE_FUNDAMENTALRANK = 3,
    SKANALYSER_TYPE_MAX = 0xFFFF,
}SKANALYSER_TYPE;

typedef enum
{
    FRST_CASH,
    FRST_RECEIVABLE,
    FRST_STOCK,
    FRST_INVEST,
    FRST_OTHERASSET,
    FRST_DEBT,
    FRST_GPM,
    FRST_OPM,
    FRST_NOPM,
    FRST_NPM,
    FRST_ROE,
    FRST_ROA,
    FRST_CFR,
}FundamentalRank_SortType;

/*earning(month) dividend method*/
typedef struct
{
    unsigned int year;
    unsigned int month;
    unsigned int income;
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
    float fdm_stock;
    float fdm_cash;
}Fnancial_dividend_method;

typedef struct
{
    float stock;
    float cash;
}Estimation_dividend;

typedef struct
{
    float middle;
    float high;
    float low;
}Estimation_dividend_ratio;

bool SKApi_SKANALYSER_Dump(const char *Inputfile);
bool SKApi_SKANALYSER_Help(void);
bool SKApi_SKANALYSER_Fileread(const char *codelist, const char * path);
bool SKApi_SKANALYSER_RentStock(void);
bool SKApi_SKANALYSER_FundamentalRank(void);
bool SKApi_SKANALYSER_Init(void);
void  SKApi_SKANALYSER_Deinit(void);

#endif

