
#ifndef SKDATA_H
#define SKDATA_H

typedef enum
{
    SK_DATA_TYPE_DIVIDEND = 0,
    SK_DATA_TYPE_EARNING_MONTH = 1,
    SK_DATA_TYPE_EARNING_SEASON = 2,
    SK_DATA_TYPE_PRICE = 3,
    SK_DATA_TYPE_FINANCIALREPORT_SEASON = 4,
    SK_DATA_TYPE_MAX,
}SK_DATA_TYPE;

typedef struct
{
    unsigned int code;
    SK_DATA_TYPE type;
    unsigned int datacount;
    unsigned int unidatasize;
    unsigned int datalength;
    unsigned int reserved[7];
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

typedef struct
{
    unsigned int year;
    unsigned int month;
    unsigned int income;
}SK_EARNING_MONTH;

typedef struct
{
    unsigned int year;
    unsigned int season;
    unsigned int pretax_income;
    unsigned int aftertax_income;
}SK_EARNING_SEASON;

typedef struct
{
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int strikestock;
    unsigned long turnover;
    float price_start;
    float price_max;
    float price_min;
    float price_end;
    float price_diff;
    unsigned int strikenum;
}SK_PRICE;

typedef struct
{
    unsigned int code;
    SK_PRICE *price;
    SK_EARNING_SEASON *earning_s;
    SK_EARNING_MONTH *earning_m;
    SK_DIVIDEND *dividend;
}SK_STOCK;


#endif

