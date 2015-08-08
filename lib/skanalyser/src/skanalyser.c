
#include "SKcommon.h"
#include "SKdata.h"
#include "string.h"



#define FREE_MEM(x)     do{if (x != NULL) {free(x);}}while(0);       
#define STOCK_NUM 1000
static SK_STOCK stock[STOCK_NUM];

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
            break;
            
        case SK_DATA_TYPE_EARNING_SEASON:
             /*TODO:  add parsing origanl data and combine flow*/ 
            FREE_MEM(stock[index].earning_s);
            stock[index].earning_s = (SK_EARNING_SEASON *)p;
            break;
            
        default:
            printf("Code: %d has incorrect type",header.code);
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
        printf("Read file [code : %d],[type : %d]\n",header.code,header.type);
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
        sprintf(filename,"%s/%d.earning.season",path,code);
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
        sprintf(filename,"%s/%d.price",path,code);
        if (!_SK_Fileread(filename))
        {
            printf("fread failed : %s\n",filename);
        }

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



