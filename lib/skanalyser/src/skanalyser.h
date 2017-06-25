

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
    FRST_DebtsRatio,  //[small is better]
    FRST_LTFixedAsset,//[small is better] Long term funds to fixed assets
    FRST_CurrentRatio, //[large is better]
    FRST_QuickRatio, //[large is better]
    FRST_InterestProtectionMultiples, //[large is better] Interest Protection Multiples
    FRST_AvgCollectionTurnove, //[large is better] Average collection turnover(times)
    FRST_AvgCollectionDay, //[small is better] Average collection days
    FRST_AvgInvent, //[large is better]
    FRST_AvgInventoryTurnoverDay, //[small is better] Average inventory turnover days
    FRST_FixedAssetTurnover, //[large is better]Fixed assets turnover (time)
    FRST_TotalAssetTurnover, //[large is better]Total assets turnover (time)
    FRST_ReturnOnTotalAsset,//[large is better]Return on total assets
    FRST_ReturnOnTotalStockholder, //[large is better]Return on total stockholder
    FRST_PerTaxIncomeToCapitalRatio, //[large is better]Per-tax income to capital
    FRST_NetIncomeToSales,//[large is better]Net income to sales
    FRST_EPS, //[large is better]
    FRST_CashFlowRatio, //[large is better]
    FRST_CashFlowAdequacyRatio, //[large is better]Cash flow adequacy ratio
    FRST_CashFlowReinvestmentRatio, //[large is better]Cash flow reinvestment ratio
    FRST_Score,
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

