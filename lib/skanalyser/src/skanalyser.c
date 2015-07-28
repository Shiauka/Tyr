
#include "SKcommon.h"
#include "SKdata.h"

//#define MAX_STRBUFFER_SIZE 128
//static char strbuf[MAX_STRBUFFER_SIZE]; 


static bool _Dump_price(FILE *pfinputfile, unsigned int count)
{
    if (pfinputfile == NULL || count == 0)
        return false;

    SK_PRICE *price = NULL;

    price = malloc(sizeof(SK_PRICE)*count+1);
    if (price == NULL)
        return false;

    if (fread((char *)price, sizeof(SK_PRICE), count, pfinputfile) == 0)
    {
        free(price);
        return false;
    }
        
    int index = 0;

    for (index = 0; index< count; index++)
    {
        printf("%03d-%02d-%02d %d, %ld %0.02f %0.02f %0.02f %0.02f %0.02f %d\n",
            price[index].year,price[index].month,price[index].day, 
            price[index].strikestock, price[index].turnover, 
            price[index].price_start, price[index].price_max, 
            price[index].price_min, price[index].price_end, 
            price[index].price_diff, price[index].strikenum);
    }
    free(price);
    return true;
}

static bool _Dump_dividend(FILE *pfinputfile, unsigned int count)
{
    if (pfinputfile == NULL || count == 0)
        return false;

    SK_DIVIDEND *dividend = NULL;

    dividend = malloc(sizeof(SK_DIVIDEND)*count+1);
    if (dividend == NULL)
        return false;

    if (fread((char *)dividend, sizeof(SK_DIVIDEND), count, pfinputfile) == 0)
    {
        free(dividend);
        return false;
    }
        
    int index = 0;

    for (index = 0; index< count; index++)
    {
        printf("%03d %0.02f %0.02f %0.02f %0.02f %0.02f\n",
            dividend[index].year, dividend[index].cash, dividend[index].profit, 
            dividend[index].surplus,dividend[index].stock,dividend[index].total);
    }
    free(dividend);
    return true;
}

static bool _Dump_earning_month(FILE *pfinputfile, unsigned int count)
{
    if (pfinputfile == NULL || count == 0)
        return false;

    SK_EARNING_MONTH *month = NULL;

    month = malloc(sizeof(SK_EARNING_MONTH)*count+1);
    if (month == NULL)
        return false;

    if (fread((char *)month, sizeof(SK_EARNING_MONTH), count, pfinputfile) == 0)
    {
        free(month);
        return false;
    }
        
    int index = 0;

    for (index = 0; index< count; index++)
    {
        printf("%03d-%02d %d\n",
            month[index].year,month[index].month,month[index].income);
    }
    free(month);
    return true;
}

static bool _Dump_earning_season(FILE *pfinputfile, unsigned int count)
{
    if (pfinputfile == NULL || count == 0)
        return false;

    SK_EARNING_SEASON *season= NULL;

    season = malloc(sizeof(SK_EARNING_SEASON)*count+1);
    if (season == NULL)
        return false;

    if (fread((char *)season, sizeof(SK_EARNING_SEASON), count, pfinputfile) == 0)
    {
        free(season);
        return false;
    }
        
    int index = 0;
    for (index = 0; index< count; index++)
    {
        printf("%03d-%02d  %d, %d\n",
            season[index].year,season[index].season,
            season[index].pretax_income, season[index].aftertax_income);
    }
    free(season);
    return true;
}


bool SKApi_SKANALYSER_Dump(const char *Inputfile)
{
    bool bRet = false;
    FILE *pfinputfile = NULL;
    SK_HEADER header;

    //open input file
    pfinputfile = fopen(Inputfile,"r");
    if (pfinputfile == NULL)
    {
        printf("open file error: %s\n",Inputfile);
        goto FAILED;
    }

    if (fread((char *)&header, sizeof(SK_HEADER), 1, pfinputfile)!=0)
    {
        printf("code : %d \n",header.code);
        printf("type : %d \n",header.type);
        printf("datacount : %d \n",header.datacount);
        printf("unidatasize : %d \n",header.unidatasize);
        printf("datalength : %d \n",header.datalength);

        int i =0;
        for (i =0; i < 7; i++)
        {
            printf("reserved[%d] : %d \n",i,header.reserved[i]);
        }    
    }

    switch (header.type)
    {
        case SK_DATA_TYPE_PRICE:
            _Dump_price(pfinputfile, header.datacount);
            break;  
            
        case SK_DATA_TYPE_DIVIDEND:
             _Dump_dividend(pfinputfile, header.datacount);
            break;      
            
        case SK_DATA_TYPE_EARNING_MONTH:
            _Dump_earning_month(pfinputfile, header.datacount);
            break;
            
        case SK_DATA_TYPE_EARNING_SEASON:
            _Dump_earning_season(pfinputfile, header.datacount);
            break;
            
        default:
            printf("Code: %d has incorrect type",header.code);
            break;
    }
   
    fclose(pfinputfile);
    bRet = true;
    
FAILED:

    return bRet;
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



