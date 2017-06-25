
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
    SK_DATA_TYPE_GOODINFO_FINGRADE = 5,
    SK_DATA_TYPE_MOPSTWSE_FINREPORT = 6,
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
    unsigned int code;
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
    unsigned int code;
    unsigned int income;
}SK_EARNING_MONTH;

typedef struct
{
    unsigned int year;
    unsigned int season;
    unsigned int code;
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
    float cash;
    float receivable;
    float stock;
    float invest;
    float otherasset;
    float debt;
    float GPM; // gross profit margin
    float OPM;// operating profit margin
    float NOPM; // non-operating profit margin
    float NPM; // net profit margin
    float ROE; //return on equity
    float ROA; //return on assets
    float CFR; // cash flow ratio
}SK_FINGRADE;

typedef struct
{
    unsigned int code;
    int year;
    float DebtsRatio;
    float LTFixedAsset;//Long term funds to fixed assets
    float CurrentRatio;
    float QuickRatio;
    float InterestProtectionMultiples; //Interest Protection Multiples
    float AvgCollectionTurnover; //Average collection turnover(times)
    float AvgCollectionDay; //Average collection days
    float AvgInvent;
    float AvgInventoryTurnoverDay; //Average inventory turnover days
    float FixedAssetTurnover; //Fixed assets turnover (time)
    float TotalAssetTurnover; //Total assets turnover (time)
    float ReturnOnTotalAsset;//Return on total assets
    float ReturnOnTotalStockholder; //Return on total stockholder
    float PerTaxIncomeToCapitalRatio; //Per-tax income to capital
    float NetIncomeToSales;//Net income to sales
    float EPS;
    float CashFlowRatio;
    float CashFlowAdequacyRatio; //Cash flow adequacy ratio
    float CashFlowReinvestmentRatio; //Cash flow reinvestment ratio
}SK_MOPSFINREPORT;

typedef struct
{
    unsigned int code;
    float score;
    SK_PRICE *price;
    SK_EARNING_SEASON *earning_s;
    SK_EARNING_MONTH *earning_m;
    SK_DIVIDEND *dividend;
    SK_FINANCIAL *financial;
    SK_FINGRADE *fingrade;
    SK_MOPSFINREPORT *finreport;
}SK_STOCK;

typedef struct
{
    unsigned int *code;
    unsigned int codenum;
}SK_CODELIST;

typedef struct
{
    unsigned int year;
    float Price_max;
    float Price_min;
    float Price_avg;
    float PEratio_max;
    float PEratio_min;
    float PEratio_avg;
    float EPS;
    float rentcash;
    float rentstock;
    float renttotal;
}SK_STOCK_INFO;

SKData_ErrMSG SKData_FileOpen(const char *filename, SK_DATA_TYPE *type, void *data);
SKData_ErrMSG SKData_FileClose(const char *filename, SK_DATA_TYPE type, void *data);
SKData_ErrMSG SKData_DataInsert(const char *filename, SK_HEADER *insertheader, void *insertData);
SKData_ErrMSG SKData_DataSort(const char *filename);

#endif

