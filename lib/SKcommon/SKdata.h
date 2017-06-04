
#ifndef SKDATA_H
#define SKDATA_H

//===========================
// enum define
//===========================

typedef enum
{
    SK_DATA_TYPE_DIVIDEND = 0,
    SK_DATA_TYPE_EARNING_MONTH = 1,
    SK_DATA_TYPE_EARNING_SEASON = 2,
    SK_DATA_TYPE_PRICE = 3,
    SK_DATA_TYPE_FINANCIALREPORT_SEASON = 4,
    SK_DATA_TYPE_NULL,
    SK_DATA_TYPE_MAX,
}SK_DATA_TYPE;


typedef enum
{
    /*TO DO*/

   FINANCIAL_CLASS_NONE,
}FINANCIAL_CLASS;

typedef enum
{
    SKData_ErrMSG_Pass = 0,
    SKData_ErrMSG_Pass_Newfile = 1,    
    SKData_ErrMSG_InvalidInput,
    SKData_ErrMSG_Openfilefaild,
    SKData_ErrMSG_InvalidHeader,
    SKData_ErrMSG_InvalidType,
    SKData_ErrMSG_MAX
}SKData_ErrMSG;

//===========================
// struct define
//===========================

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
    unsigned int code;
    char name[32];
    unsigned int strikestock;
    unsigned int strikenum;
    unsigned long turnover;
    float price_start;
    float price_max;
    float price_min;
    float price_end;
    float price_diff;
    float PEratio;
}SK_PRICE;

typedef struct
{
    unsigned int Code;
    unsigned int Class;
    unsigned int Year;
    unsigned int Season;
    int Operating_Revenues;
    int Operating_Income;
    int NonOperating_Income;
    int Income_after_Tax;
    unsigned int Capital_Stock;
    int Income_before_Tax;
    float EPS;
    float EPS_Class;
    float Operating_ratio; // Operating_Income / Total_Income
}SK_FINANCIAL;

typedef struct
{
    unsigned int code;
    SK_PRICE *price;
    SK_EARNING_SEASON *earning_s;
    SK_EARNING_MONTH *earning_m;
    SK_DIVIDEND *dividend;
    SK_FINANCIAL *financial;
}SK_STOCK;

SKData_ErrMSG SKData_FileOpen(const char *filename, SK_DATA_TYPE *type, void *data);
SKData_ErrMSG SKData_FileClose(const char *filename, SK_DATA_TYPE type, void *data);
SKData_ErrMSG SKData_DataInsert(const char *filename, SK_HEADER *insertheader, void *insertData);
SKData_ErrMSG SKData_DataSort(const char *filename);

#endif

