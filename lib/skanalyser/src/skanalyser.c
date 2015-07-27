
#include "SKcommon.h"
#include "SKdata.h"

//#define MAX_STRBUFFER_SIZE 128
//static char strbuf[MAX_STRBUFFER_SIZE]; 


static bool _Dump_price(FILE *pfinputfile, unsigned int count)
{
    if (pfinputfile == NULL && count != 0)
        return false;

    SK_PRICE *price = NULL;

    printf("count = %d\n",count);

    price = malloc(sizeof(SK_PRICE)*count+1);
    if (price == NULL)
        return false;
    printf("count = %d\n",count);

    if (fread((char *)price, sizeof(SK_PRICE), count, pfinputfile) == 0)
    {
        free(price);
        return false;
    }
        
        printf("count = %d\n",count);

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

        default:
            printf("Code: %d has incorrect type",header.code);
            break;
    }
   
    fclose(pfinputfile);
    bRet = true;
    
FAILED:

    return bRet;
}


