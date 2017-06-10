
#include "SKcommon.h"
#include "SKdata.h"
#include "skanalyser.h"


#define FREE_MEM(x)     do{if (x != NULL) {free(x);}}while(0);       
#define STOCK_NUM 1000
static SK_STOCK stock[STOCK_NUM];


static void *SK_Realloc(void *ptr, size_t size)
{
    void *Ret = NULL;
    Ret = realloc(ptr,size);
    if (ptr == NULL && Ret != NULL)
        memset(Ret,'\0',size);
    return Ret;
}

static void *SK_Malloc(size_t size)
{
    void *Ret = NULL;
    Ret = malloc(size);
    if (Ret != NULL)
        memset(Ret,'\0',size);
    return Ret;
}

static void SK_Free(void *ptr)
{
    if (ptr != NULL)
        free(ptr);
}

static int _stock_getindex(unsigned int code)
{
    int i = 0;
    int intRet = -1;
    for (i = 0; i < STOCK_NUM; i++)
    {
        if (stock[i].code == code)
        {
            intRet = i;
            break;
        }
    }
    
    return intRet;
}

static int _stock_getfreeindex(unsigned int code)
{
    int i = 0;
    int intRet = -1;
    for (i = 0; i < STOCK_NUM; i++)
    {
        if (stock[i].code == code)
        {
            intRet = i;
            break;
        }

        if (stock[i].code == 0 && intRet == -1)
        {
            /*NO BREAK, due to check code already exist */
            intRet = i;
        } 
    }
    
    return intRet;
}

static void _stock_freestock_specificindex(unsigned int index)
{
    /*check input index*/
    if (index >= STOCK_NUM)
        return;
    
    /*free memory*/
    FREE_MEM(stock[index].dividend);
    FREE_MEM(stock[index].earning_m);
    FREE_MEM(stock[index].earning_s);
    FREE_MEM(stock[index].price);
    stock[index].code = 0;
}

/*Not use now*/
#if 0
static void _stock_freestock_specificcode(unsigned int code)
{
    unsigned int index = 0;
    /*trans code to index*/
    for (index =0; index <STOCK_NUM; index++)
    {
        if (stock[index].code == code)
            break;
    }

    if (index >= STOCK_NUM)
        return;

    /*free specific stock*/
    _stock_freestock_specificindex(index);
}
#endif

static void _stock_freestock_all(void)
{
    unsigned int index = 0;
     /*free specific stock*/
    for (index =0; index <STOCK_NUM; index++)
    {
        _stock_freestock_specificindex(index);
    }
}

static void _optimize_earning_month(SK_EARNING_MONTH *earning_m)
{
    int i = 0;
    unsigned int total = 0;
    unsigned int year = 0;

    for (i = 0; earning_m[i].year != 0; i++)
    {
        if (year != earning_m[i].year)
        {
            year = earning_m[i].year;
            total = 0;
        }
        else
        {
            total += earning_m[i].income;
            earning_m[i].income = total;
        }
    }
}

static bool _stock_savedata(SK_HEADER header, FILE *pfinputfile)
{
    int index = _stock_getfreeindex(header.code);
    if (index == -1)
    {
        printf("can't get free index\n");
        return false;
    }

    char *p = NULL;
    
     /*malloc and read data to memory*/
    unsigned int size = header.unidatasize*(header.datacount+1); 
    p = malloc(size);
    if (p  == NULL)
        return false;

    memset(p,'\0',size);
    if (fread(p, header.unidatasize, header.datacount, pfinputfile) == 0)
        return false;
    
    /*assinged data point*/
    switch (header.type)
    {
        case SK_DATA_TYPE_PRICE:            
            /*TODO:  add parsing origanl data and combine flow*/ 
            FREE_MEM(stock[index].price);
            stock[index].price = (SK_PRICE *)p;
            break;  
            
        case SK_DATA_TYPE_DIVIDEND:
            /*TODO:  add parsing origanl data and combine flow*/ 
            FREE_MEM(stock[index].dividend);
            stock[index].dividend = (SK_DIVIDEND *)p;
            break;      
            
        case SK_DATA_TYPE_EARNING_MONTH:
            /*TODO:  add parsing origanl data and combine flow*/ 
            FREE_MEM(stock[index].earning_m); 
            stock[index].earning_m = (SK_EARNING_MONTH *)p;
            _optimize_earning_month(stock[index].earning_m);
            break;
            
        case SK_DATA_TYPE_EARNING_SEASON:
             /*TODO:  add parsing origanl data and combine flow*/ 
            FREE_MEM(stock[index].earning_s);
            stock[index].earning_s = (SK_EARNING_SEASON *)p;
            break;

        case SK_DATA_TYPE_FINANCIALREPORT_SEASON:
             /*TODO:  add parsing origanl data and combine flow*/ 
            FREE_MEM(stock[index].financial);
            stock[index].financial = (SK_FINANCIAL *)p;
            break;

        default:
            printf("Code: %d has incorrect type\n",header.code);
            return false;
    }

    /*update code num*/
    stock[index].code = header.code;

    return true;
}

static bool _SK_Datasave(SK_HEADER header, FILE *pfinputfile)
{
    bool bRet = false;
    switch (header.type)
    {
        case SK_DATA_TYPE_PRICE:
            bRet = _stock_savedata(header, pfinputfile);
            break;  
            
        case SK_DATA_TYPE_DIVIDEND:
            bRet =  _stock_savedata(header, pfinputfile);
            break;      
            
        case SK_DATA_TYPE_EARNING_MONTH:
            bRet = _stock_savedata(header, pfinputfile);
            break;
            
        case SK_DATA_TYPE_EARNING_SEASON:
            bRet = _stock_savedata(header, pfinputfile);
            break;
            
        case SK_DATA_TYPE_FINANCIALREPORT_SEASON:
            bRet = _stock_savedata(header, pfinputfile);
            break;

        default:
            printf("Code: %d has incorrect type",header.code);
            break;
    }

    return bRet;
}

static bool _SK_Fileread(const char *filename)
{
    /*check input*/
    if (filename == NULL)
    {
        printf("filename is null");
        return false;
    }

    bool bRet = false;
    FILE *pfinput = NULL;
    SK_HEADER header;
    
    /*open file*/
    pfinput = fopen(filename,"r");
    if (pfinput == NULL)
    {
        printf("open file error: %s\n",filename);
        goto FAILED;
    }

    /*read header*/
    if (fread((char *)&header, sizeof(SK_HEADER), 1, pfinput)!=0)
    {
        //printf("Read file [code : %d],[type : %d]\n",header.code,header.type);
    }
    else
    {
        printf("read header failed\n");
        goto FAILED;
    }

    /*save data*/
    if (!_SK_Datasave(header, pfinput))
    {
        printf("save data failed\n");
        goto FAILED;
    }

    bRet = true;
FAILED:
    if (pfinput != NULL) {fclose(pfinput); pfinput = NULL;}
    return bRet;

}

static bool _Dump_price(unsigned int code)
{
    int i = 0;    
    int index = _stock_getindex(code);
    if (index == -1)
        return false;

    for (i = 0; stock[index].price[i].year != 0; i++)
    {
        printf("%03d-%02d-%02d %d, %ld %0.02f %0.02f %0.02f %0.02f %0.02f %d\n",
            stock[index].price[i].year,stock[index].price[i].month,stock[index].price[i].day, 
            stock[index].price[i].strikestock, stock[index].price[i].turnover, 
            stock[index].price[i].price_start, stock[index].price[i].price_max, 
            stock[index].price[i].price_min, stock[index].price[i].price_end, 
            stock[index].price[i].price_diff, stock[index].price[i].strikenum);
    }
    return true;
}

static bool _Dump_dividend(unsigned int code)
{
    int i = 0;    
    int index = _stock_getindex(code);
    if (index == -1)
        return false;

    for (i  = 0; stock[index].dividend[i].year!=0; i ++)
    {
        printf("%03d %0.02f %0.02f %0.02f %0.02f %0.02f\n",
            stock[index].dividend[i ].year, stock[index].dividend[i ].cash, stock[index].dividend[i ].profit, 
            stock[index].dividend[i ].surplus,stock[index].dividend[i ].stock,stock[index].dividend[i ].total);
    }
    
    return true;
}

static bool _Dump_earning_month(unsigned int code)
{
    int i = 0;    
    int index = _stock_getindex(code);
    if (index == -1)
        return false;

    for (i = 0; stock[index].earning_m[i].year != 0; i++)
    {
        printf("%03d-%02d %d\n",
            stock[index].earning_m[i].year,stock[index].earning_m[i].month,stock[index].earning_m[i].income);
    }
    
    return true;
}

static bool _Dump_earning_season(unsigned int code)
{
    int i = 0;    
    int index = _stock_getindex(code);
    if (index == -1)
        return false;

    for (i = 0; stock[index].earning_s[i].year != 0; i++)
    {
        printf("%03d-%02d  %d, %d\n",
            stock[index].earning_s[i].year,stock[index].earning_s[i].season,
            stock[index].earning_s[i].pretax_income, stock[index].earning_s[i].aftertax_income);
    }

    return true;
}

static bool _Dump_financial_report(unsigned int code)
{
    int i = 0;    
    int index = _stock_getindex(code);
    if (index == -1)
        return false;

    for (i = 0; stock[index].financial[i].Year!= 0; i++)
    {
        printf("%03d-%02d  %d, %d, %d, %d, %d, %d, %0.2f, %0.2f, %0.2f\n",
            stock[index].financial[i].Year,stock[index].financial[i].Season,
            stock[index].financial[i].Operating_Revenues, 
            stock[index].financial[i].Operating_Income,
            stock[index].financial[i].NonOperating_Income, 
            stock[index].financial[i].Income_after_Tax,
            stock[index].financial[i].Capital_Stock, 
            stock[index].financial[i].Income_before_Tax,
            stock[index].financial[i].EPS, 
            stock[index].financial[i].EPS_Class, 
            stock[index].financial[i].Operating_ratio
            );
    }
    return true;
}

bool SKApi_SKANALYSER_Dump(const char *Inputfile)
{
    bool bRet = false;
    FILE *pfinputfile = NULL;
    SK_HEADER header;

    /*read file to memory*/
    if (!_SK_Fileread(Inputfile))
    {
        printf("parsing file failed : %s\n",Inputfile);
    }

    /*read header*/
    pfinputfile = fopen(Inputfile,"r");
    if (pfinputfile == NULL)
    {
        printf("open file error: %s\n",Inputfile);
        goto FAILED;
    }

    /*read header*/
    if (fread((char *)&header, sizeof(SK_HEADER), 1, pfinputfile) ==0)
    {
        printf("read header failed\n");
        goto FAILED;
    }

    /*dump data*/
    switch (header.type)
    {
        case SK_DATA_TYPE_PRICE:
            _Dump_price(header.code);
            break;  
            
        case SK_DATA_TYPE_DIVIDEND:
             _Dump_dividend(header.code);
            break;      
            
        case SK_DATA_TYPE_EARNING_MONTH:
            _Dump_earning_month(header.code);
            break;
            
        case SK_DATA_TYPE_EARNING_SEASON:
            _Dump_earning_season(header.code);
            break;

        case SK_DATA_TYPE_FINANCIALREPORT_SEASON:
            _Dump_financial_report(header.code);
            break;
            
        default:
            printf("Code: %d has incorrect type",header.code);
            break;
    }
   
    bRet = true;
    
FAILED:
    if (pfinputfile != NULL)
        fclose(pfinputfile);
    
    return bRet;
}

bool SKApi_SKANALYSER_EPSEstimation(void)
{
    return false;
}

static SK_FINANCIAL* _GetFinancialReport(unsigned int code, unsigned int year, unsigned int season)
{
    int index = _stock_getindex(code);
    int i = 0;
    if (index == -1)
        goto   FAILED;

    if (stock[index].financial== NULL)
        goto FAILED;    

    for (i = 0; stock[index].financial[i].Year != 0; i++)
    {
        if (stock[index].financial[i].Year == year && stock[index].financial[i].Season == season)
        {
            return &stock[index].financial[i];
        }
    }

FAILED:
    return NULL;
}

static SK_DIVIDEND* _GetDividendReport(unsigned int code, unsigned int year)
{
    int index = _stock_getindex(code);
    int i = 0;
    if (index == -1)
        goto   FAILED;

    if (stock[index].dividend== NULL)
        goto FAILED;    

    for (i = 0; stock[index].dividend[i].year != 0; i++)
    {
        if (stock[index].dividend[i].year == year)
        {
            return &stock[index].dividend[i];
        }
    }

FAILED:
    return NULL;
}

static SK_EARNING_MONTH* _GetEarningmonthReport(unsigned int code, unsigned int year, unsigned int month)
{
    int index = _stock_getindex(code);
    int i = 0;
    if (index == -1)
        goto   FAILED;

    if (stock[index].earning_m== NULL)
        goto FAILED;    

    for (i = 0; stock[index].earning_m[i].year != 0; i++)
    {
        if (stock[index].earning_m[i].year == year && stock[index].earning_m[i].month == month )
        {
            return &stock[index].earning_m[i];
        }
    }

FAILED:
    return NULL;
}


static bool _DividendEstimation_financialdividendmethod(unsigned int code, unsigned int year, Estimation_dividend *Dividend)
{
    #define financialdividendmethod_printf 0
    
    bool bRet = false;
    int index = _stock_getindex(code);
    int i = 0;
    if (index == -1)
        goto   FAILED;

    if (stock[index].dividend == NULL || stock[index].financial== NULL)
        goto FAILED;

    /*financial dividend method*/
    Fnancial_dividend_method FDM[100];
    memset(FDM,'\0',sizeof(Fnancial_dividend_method)*100);
    
    SK_FINANCIAL *last_financial = NULL;
    SK_FINANCIAL *last2_financial = NULL;
    SK_DIVIDEND *curdividend = NULL;
    SK_DIVIDEND *lastdividend = NULL;
    for (i = 0; stock[index].financial[i].Year != 0; i++)
    {
#if (financialdividendmethod_printf == 0)    
        if (stock[index].financial[i].Year != year)
            continue;
#endif    
        FDM[i].year=stock[index].financial[i].Year;
        FDM[i].season=stock[index].financial[i].Season;
        FDM[i].EPS =stock[index].financial[i].EPS;
        curdividend = _GetDividendReport(code,stock[index].financial[i].Year);
        lastdividend = _GetDividendReport(code,stock[index].financial[i].Year-1);            
        last_financial = _GetFinancialReport(code,stock[index].financial[i].Year-1,stock[index].financial[i].Season);
        if (last_financial != NULL && lastdividend != NULL && FDM[i].EPS > 0)
        {
            if (last_financial->EPS <= 0)
            {
                last2_financial = _GetFinancialReport(code,stock[index].financial[i].Year-2,stock[index].financial[i].Season);
                if (last2_financial != NULL && last2_financial->EPS > 0)
                {
                    FDM[i].fdm_stock = lastdividend->stock * FDM[i].EPS / last2_financial->EPS;
                    FDM[i].fdm_cash = lastdividend->cash* FDM[i].EPS / last2_financial->EPS;
                }
            }
            else
            {
                FDM[i].fdm_stock = lastdividend->stock * FDM[i].EPS / last_financial->EPS;
                FDM[i].fdm_cash = lastdividend->cash* FDM[i].EPS / last_financial->EPS;
            }
        } 
        
        if (curdividend != NULL)
        {
            FDM[i].stock = curdividend->stock;
            FDM[i].cash = curdividend->cash;
        }
        curdividend = NULL;
        lastdividend = NULL;
        last_financial = NULL;
        last2_financial = NULL;
        Dividend->stock = FDM[i].fdm_stock;
        Dividend->cash = FDM[i].fdm_cash;
        bRet = true;
    }

#if (financialdividendmethod_printf == 1) 
    int i_loop1 = 0;
    for (i_loop1  = 0; FDM[i_loop1].year!=0; i_loop1++)
    {
        printf("REAL: [year: %d],[season: %d][EPS:%0.02f],[cash : %0.02f],[stock : %0.02f],FDM:[cash : %0.02f],[stock : %0.02f]\n",
        FDM[i_loop1].year,
        FDM[i_loop1].season,
        FDM[i_loop1].EPS,
        FDM[i_loop1].cash,
        FDM[i_loop1].stock,
        FDM[i_loop1].fdm_cash,
        FDM[i_loop1].fdm_stock );        
    }
#endif    

FAILED:    
    return bRet;

}

static bool _DividendEstimation_earningdividendmethod(unsigned int code, unsigned int year, Estimation_dividend *Dividend)
{
    bool bRet = false;
    int index = _stock_getindex(code);
    int i = 0;
    if (index == -1)
        goto   FAILED;

    if (stock[index].dividend == NULL || stock[index].earning_m == NULL)
        goto FAILED;

    /*earning(month) dividend method*/
    SK_EARNING_MONTH *last_earning_m = NULL;
    SK_EARNING_MONTH *last2_earning_m = NULL;
    SK_DIVIDEND *lastdividend = NULL;    
    Earning_dividend_method EDM[36]; // 3 years
    memset(EDM,'\0',sizeof(Earning_dividend_method)*36);       
    
    for (i = 0; stock[index].earning_m[i].year != 0; i++)
    {
        if (stock[index].earning_m[i].year != year)
            continue;

        EDM[i].year = stock[index].earning_m[i].year;
        EDM[i].month = stock[index].earning_m[i].month;
        EDM[i].income = stock[index].earning_m[i].income;
        lastdividend = _GetDividendReport(code,EDM[i].year -1);          
        last_earning_m = _GetEarningmonthReport(code,EDM[i].year -1,EDM[i].month);
        if (last_earning_m != NULL && lastdividend != NULL && EDM[i].income > 0)
        {
            if (last_earning_m->income <= 0)
            {
                last2_earning_m = _GetEarningmonthReport(code,EDM[i].year -2,stock[index].earning_m[i].month);
                if (last2_earning_m  != NULL && last2_earning_m->income  > 0)
                {
                    EDM[i].percent  =  ((float)EDM[i].income / last2_earning_m->income) / (1 + lastdividend->stock /10);
                    EDM[i].stock = lastdividend->stock * EDM[i].percent;
                    EDM[i].cash  = lastdividend->cash* EDM[i].percent;
                }
            }
            else
            {
                EDM[i].percent  =  ((float)EDM[i].income / last_earning_m->income) / (1 + lastdividend->stock /10);
                EDM[i].stock = lastdividend->stock * EDM[i].percent;
                EDM[i].cash  = lastdividend->cash* EDM[i].percent;
            }
        }
        lastdividend = NULL;
        last_earning_m = NULL;
        last2_earning_m = NULL;
        Dividend->cash = EDM[i].cash;
        Dividend->stock = EDM[i].stock;
        bRet = true;
    }
    
FAILED:    
    return bRet;
}

static bool _DividendEstimation_averagedividendmethod(unsigned int code, unsigned int year, Estimation_dividend *Dividend)
{
    bool bRet = false;
    int index = _stock_getindex(code);
    int i = 0;
    int lastyear = 0;
    if (index == -1)
        goto   FAILED;

    if (stock[index].dividend == NULL)
        goto FAILED;
    
    /*average dividend  method*/ 
    float dd_cash = 0, dd_stock = 0;
    int weight = 0, weight_total = 0;
    for (i  = 0; stock[index].dividend[i].year!=0; i ++)
    {
        if (stock[index].dividend[i].year <  year - 5 || stock[index].dividend[i].year >=  year)
            continue;
        
        if (stock[index].dividend[i].year >= lastyear)
            lastyear = stock[index].dividend[i].year;
                
        weight++;
        weight_total += weight;
        dd_cash += stock[index].dividend[i].cash * weight;
        dd_stock += stock[index].dividend[i].stock * weight;
    }
    
    if (weight_total != 0)
    {
        dd_cash = dd_cash/weight_total;
        dd_stock = dd_stock/weight_total;
        Dividend->cash = dd_cash;
        Dividend->stock = dd_stock;
        bRet = true;
    }
    
FAILED:    
        return bRet;
}

static void _Dividendratio_sort(int count, float *ratio)
{
    if (count <=0 || ratio == NULL )
            return;
    
    int i_loop = 0;
    int j_loop = 0;
    float temp = 0;
    for (i_loop = 0; i_loop < count; i_loop++)
    {
        for (j_loop = i_loop+1; j_loop < count; j_loop++)
        {
            if (ratio[i_loop] >ratio[j_loop])
            {
                temp = ratio[i_loop];
                ratio[i_loop] = ratio[j_loop];
                ratio[j_loop] = temp;
            }
        }
    }
}

static float _Dividendratio_sum(int count, float *ratio)
{
    if (count <=0 || ratio == NULL )
            return 0;
    
    int loop = 0;
    float sum = 0;
    for (loop = 0; loop < count; loop++)
    {
        sum += ratio[loop];
    }
    return sum;
}

static float _Dividendratio_average(int count, float *ratio)
{
    if (count <=0 || ratio == NULL )
            return 0;
    
    float average = 0;
    average = _Dividendratio_sum(count, ratio)/count;
    
    return average;
}

static float _Dividendratio_average_range(int count, float *ratio, float low, float up)
{
    if (count <=0 || ratio == NULL )
            return 0;
    
    float average = 0;
    int loop = 0;
    float sum = 0;
    int upper = count * up;
    int lower = count * low;
    int loopcount = 0;
    for (loop = lower; loop < upper; loop++)
    {
        sum += ratio[loop];
        loopcount++;
    }
    average = sum /loopcount;
    return average;
}


static bool _PriceEstimation_Dividendratio(unsigned int code, Estimation_dividend_ratio* EDR)
{
    bool bRet = false;
    int index = _stock_getindex(code);
    SK_DIVIDEND *curdividend = NULL;
    int i = 0;
    float temp_ratio = 0.0;
    float temp_stock = 0.0;
    int ratiosize = 500;
    float *ratio = NULL;
    int count = 0;
    if (index == -1 || EDR == NULL)
        goto   FAILED;

    if (stock[index].dividend == NULL || stock[index].price == NULL)
        goto FAILED;

    ratio = SK_Malloc(sizeof(float)*ratiosize);
    if (ratio ==NULL)
        goto FAILED;

    //caculator dividend ratio per day
    for (i  = 0; stock[index].price[i].year !=0; i ++)
    {
        //if (stock[index].price[i].month != month)
         //   continue;
        
        curdividend = _GetDividendReport(code,stock[index].price[i].year);
        if (curdividend !=NULL && (stock[index].price[i].price_end - curdividend->cash >= 0 ))
        {
            temp_stock = (stock[index].price[i].price_end - curdividend->cash)  / (1+curdividend->stock / 10) * curdividend->stock / 10;
            temp_ratio = (curdividend->cash + temp_stock) / stock[index].price[i].price_end;
            ratio[count] = temp_ratio;
            count++;
            if (count >= ratiosize)
            {
                ratiosize += 100;
                ratio = SK_Realloc(ratio, sizeof(float)*ratiosize);
                if (ratio ==NULL)
                    goto FAILED;
            }
        }
        curdividend = NULL;
    }
    _Dividendratio_sort(count,ratio);

    float temp = 0;
    temp = _Dividendratio_average(count,ratio);
    temp = _Dividendratio_average_range(count,ratio,0.40,0.60);
    EDR->middle = temp;
    temp = _Dividendratio_average_range(count,ratio,0.15,0.35);
    EDR->low = temp;
    temp = _Dividendratio_average_range(count,ratio,0.65,0.85);
    EDR->high = temp;

    bRet = true;
    
FAILED: 
    SK_Free(ratio);
    return bRet;
}

static float _PriceEstimation_ratio2price(float ratio, Estimation_dividend *dividend)
{
    if (dividend == NULL)
        return -1;

    float Price = 0;
    float Z = (dividend->stock / 10)/(1+(dividend->stock / 10));
    
    if (Z >= ratio)
        return 0; 

    Price = ((1-Z)*dividend->cash) /(ratio - Z);
    return Price;
}

static bool SKApi_SKANALYSER_PriceEstimation(unsigned int code,Estimation_dividend *dividend)
{
    bool bRet = false;
    Estimation_dividend_ratio Dividend_ratio = {0};
     
    if (!_PriceEstimation_Dividendratio(code,&Dividend_ratio))
    {
        goto FAILED;
    }

    printf("low: %0.04f,%0.04f \n",Dividend_ratio.low, _PriceEstimation_ratio2price(Dividend_ratio.low,dividend));
    printf("middle: %0.04f,%0.04f \n",Dividend_ratio.middle, _PriceEstimation_ratio2price(Dividend_ratio.middle,dividend));
    printf("high: %0.04f,%0.04f \n",Dividend_ratio.high, _PriceEstimation_ratio2price(Dividend_ratio.high,dividend));
    bRet = true;

FAILED:
    
    return bRet;
}

static bool SKApi_SKANALYSER_DividendEstimation(unsigned int code, unsigned int year, Estimation_dividend *retDividends)
{
    bool bRet = false;
    Estimation_dividend Dividend = {0};
    unsigned int count = 0;
    Estimation_dividend Dividend_final = {0};

    if (_DividendEstimation_averagedividendmethod(code,year,&Dividend))
    {
        //printf("ADM: [year: %d],[cash : %0.02f], [stock : %0.02f]\n",year,Dividend.cash,Dividend.stock);
        count++;
        Dividend_final.cash += Dividend.cash;
        Dividend_final.stock += Dividend.stock;
    }
    
    if ( _DividendEstimation_earningdividendmethod(code,year,&Dividend))
    {
        //printf("EDM: [year: %d],[cash : %0.02f], [stock : %0.02f]\n",year,Dividend.cash,Dividend.stock);
        count++;
        Dividend_final.cash += Dividend.cash;
        Dividend_final.stock += Dividend.stock;
    }
    
    if ( _DividendEstimation_financialdividendmethod(code,year,&Dividend))
    {
        //printf("FDM: [year: %d],[cash : %0.02f], [stock : %0.02f]\n",year,Dividend.cash,Dividend.stock);
        count++;
        Dividend_final.cash += Dividend.cash;
        Dividend_final.stock += Dividend.stock;
    }

    if (count > 0)
    {
        Dividend_final.cash /= count;
        Dividend_final.stock /= count;
        //printf("AVG: [year: %d],[cash : %0.02f],[stock : %0.02f]\n",year,Dividend_final.cash,Dividend_final.stock); 
        bRet = true;
    }
    memcpy(retDividends, &Dividend_final, sizeof(Estimation_dividend));
    
    return bRet;
}

bool SKApi_SKANALYSER_Fileread(const char *codelist, const char * path)
{
    bool bRet = false;
    FILE *flist = NULL;
    unsigned int code = 0;
    char filename[128];
    char line[16];
    
    /*open code list file*/
    flist = fopen(codelist,"r");
    if (flist == NULL)
    {
        printf("open file error: %s\n",codelist);
        goto FAILED;
    }

    while(fgets(line, 16, flist) != NULL)
    {
        code = atoi(line);
        if (code == 0)
            break;
        
        memset(filename,'\0', 128);
        sprintf(filename,"%s/%d.dividend",path,code);
        if (!_SK_Fileread(filename))
        {
            printf("fread failed : %s\n",filename);
        }

        memset(filename,'\0', 128);
        sprintf(filename,"%s/%d.earning.month",path,code);
        if (!_SK_Fileread(filename))
        {
            printf("fread failed : %s\n",filename);
        }

        memset(filename,'\0', 128);
        sprintf(filename,"%s/%d.earning.season",path,code);
        if (!_SK_Fileread(filename))
        {
            printf("fread failed : %s\n",filename);
        }

        memset(filename,'\0', 128);
        sprintf(filename,"%s/%d.price",path,code);
        if (!_SK_Fileread(filename))
        {
            printf("fread failed : %s\n",filename);
        }

        memset(filename,'\0', 128);
        sprintf(filename,"%s/%d.financial",path,code);
        if (!_SK_Fileread(filename))
        {
            printf("fread failed : %s\n",filename);
        }

        Estimation_dividend Dividend = {0};
        SKApi_SKANALYSER_DividendEstimation(code, 104,&Dividend);
        printf("[code : %d],[cash : %0.02f],[stock : %0.02f]\n",code,Dividend.cash,Dividend.stock);  
        SKApi_SKANALYSER_PriceEstimation(code, &Dividend);
        code = 0;
    }

    bRet = true;
    
FAILED:
    if (flist!=NULL)
        fclose(flist);
    
    return bRet;
}

bool SKApi_SKANALYSER_Init(void)
{
    _stock_freestock_all();
    return true;
}

void  SKApi_SKANALYSER_Deinit(void)
{
    _stock_freestock_all();
}

bool SKApi_SKANALYSER_Help(void)
{
    printf("=============Skanalyser Commnd List==================\n");
    printf("[0] dump data\n");
    printf("     ./skanalyser 0 skfile\n");
    printf("[1] analyser data\n");
    printf("     ./skanalyser 1 code.list skfile.path\n");
    printf("=====================================================\n");
    return true;
}



