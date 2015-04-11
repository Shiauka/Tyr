
#ifndef SKDATA_H
#define SKDATA_H

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


#endif

