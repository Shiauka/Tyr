
#include "SKcommon.h"
#include "SKdata.h"
#include "cvs2sk.h"
#include "string.h"


#define MAX_STRBUFFER_SIZE 128
#define LINE_LEN 256
#define FINANCIAL_NEW_NUM 1500

static char strbuf[LINE_LEN];

char *Skip_specific_character(const char* input, unsigned int len, char specific)
{
    unsigned int index = 0;    
    unsigned int count = 0;

    memset(strbuf,'\0',LINE_LEN);

    for (index = 0; index < len; index++)
    {
        if (input[index] == specific)
        {
            continue;
        }
        else
        {
            strbuf[count] = input[index];
            count++;
        }
    }

    return strbuf;
}

char *Skip_specific_character_negative(const char* input, unsigned int len, char specific)
{
    unsigned int index = 0;    
    unsigned int count = 0;

    memset(strbuf,'\0',MAX_STRBUFFER_SIZE);

    for (index = 0; index < len; index++)
    {
        if (input[index] == specific ||input[index] == ')' )
        {
            continue;
        }
        else if (input[index] == '(')
        {
            strbuf[count] = '-';
            count++;
        }
        else
        {
            strbuf[count] = input[index];
            count++;
        }
    }
    
    return strbuf;
}

void Sort_dividend( SK_DIVIDEND *dividend, int num)
{
    SK_DIVIDEND temp;
    int i,j;

    for (i = 0; i < num; i++)
    {
        for (j = 0; j < num -i - 1; j++)
        {
            if (dividend[j].year >= dividend[j+1].year)
            {
                memcpy(&temp, &dividend[j],sizeof(SK_DIVIDEND));
                memcpy(&dividend[j], &dividend[j+1],sizeof(SK_DIVIDEND));
                memcpy(&dividend[j+1], &temp,sizeof(SK_DIVIDEND));
            }
        }
    }
}

static bool _Dividend_Output(SK_DIVIDEND *dividend, int datacount, const char* Outputfile)
{
    bool bRet = false;
    SK_HEADER header = {0};
    SKData_ErrMSG msg = SKData_ErrMSG_Pass;
    
    //check input value
    if (dividend == NULL || Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    //dump SK_HEADER and SK_DIVIDEND to output file
    
    //memset(&header,'\0',sizeof(SK_HEADER));
    
    header.code = dividend[0].code;
    header.type = SK_DATA_TYPE_DIVIDEND;
    header.datacount = datacount;
    header.unidatasize = sizeof(SK_DIVIDEND);
    header.datalength = header.datacount * sizeof(SK_DIVIDEND);

    msg = SKData_DataInsert(Outputfile, &header, dividend);
    if (!(msg == SKData_ErrMSG_Pass || msg == SKData_ErrMSG_Pass_Newfile))
    {
        goto FAILED;
    }

/*
    printf("HEADER:\n");
    printf("Code = %d\n",header.code);
    printf("type = %d\n",header.type);
    printf("datacount = %d\n",header.datacount);
    printf("unidatasize = %d\n",header.unidatasize);
    printf("datalength = %d\n",header.datalength);
    int index = 0;
    for (index = 0; index < datacount; index++)
    {
        printf("%d\t%f\n",dividend[index].year, dividend[index].total);
    }
*/
    bRet = true;
    
FAILED:
    

    return bRet;
}

static bool _Dividend_Parsing(const int Code, const char *Inputfile, const char *Outputfile)
{
    bool bRet = false;
    FILE *pfinputfile = NULL;
    char line[256];
    SK_DIVIDEND *dividend = NULL;
    char *pstr;
    int index = 0;
    int num = 0;
    int length = 0; 

    
    //check input value
    if (Inputfile == NULL || Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    //open input/output file
    pfinputfile = fopen(Inputfile,"r");
    if (pfinputfile == NULL)
    {
        printf("open file error: %s\n",Inputfile);
        goto FAILED;
    }
    
    dividend = malloc(sizeof(SK_DIVIDEND)*20);

    //load data to structure SK_DIVIDEND
    while (fgets (line, 256, pfinputfile)!=NULL)
    {
        if (strncmp(line,"\"",1))
        {
            continue;
        }

        pstr = strtok(line," \t\n,\"");
        if (atoi(pstr)==0)
        {
            continue;
        }

        index = 0;
        
        while (pstr != NULL)
        {
            switch (index)
            {
                case 0:
                    dividend[num].year = atoi(pstr);
                    dividend[num].code = Code;
                    break;
                    
                case 1:
                    dividend[num].cash= atof(pstr);
                    break; 

                case 2:
                    dividend[num].profit= atof(pstr);
                    break; 
                    
                case 3:
                    dividend[num].surplus= atof(pstr);
                    break; 

                case 4:
                    dividend[num].stock= atof(pstr);
                    break; 

                case 5:
                    dividend[num].total= atof(pstr);
                    break; 
                    
                default:
                    break;
            }
        
            pstr = strtok(NULL," \t\n,\"");
            index++;
        }
        num++;
        length+= sizeof(SK_DIVIDEND);
    }

    if (!_Dividend_Output(dividend, num, Outputfile))
    {
        printf("Dividend output error \n");
        goto FAILED;
    }

    bRet = true;

FAILED:
    if (pfinputfile !=NULL) fclose(pfinputfile); 
    if (dividend !=NULL) free(dividend);

    return bRet;
}

bool SKApi_CVS2SK_Dividend(const int Code, const char *Inputfile, const char *Outputfile)
{
    bool bRet = false;
    //check input value
    if (Inputfile == NULL || Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    if (!_Dividend_Parsing(Code, Inputfile, Outputfile))
    {
        printf("Dividend Pasrsing error\n");
        goto FAILED;
    }

    //Sort_dividend(dividend,num);
    
    bRet = true;
FAILED:
    
    return bRet;
    
}

static bool _GInfoFingrade_Output(SK_FINGRADE fingrade, const char* Outputfile)
{
    bool bRet = false;
    SK_HEADER header = {0};
    SKData_ErrMSG msg = SKData_ErrMSG_Pass;
    
    //check input value
    if (Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    //dump SK_HEADER and SK_DIVIDEND to output file
    
    header.code = fingrade.code;
    header.type = SK_DATA_TYPE_GOODINFO_FINGRADE;
    header.datacount = 1;
    header.unidatasize = sizeof(SK_FINGRADE);
    header.datalength = header.datacount * sizeof(SK_FINGRADE);

    msg = SKData_DataInsert(Outputfile, &header, &fingrade);
    if (!(msg == SKData_ErrMSG_Pass || msg == SKData_ErrMSG_Pass_Newfile))
    {
        goto FAILED;
    }

/*
    printf("HEADER:\n");
    printf("Code = %d\n",header.code);
    printf("type = %d\n",header.type);
    printf("datacount = %d\n",header.datacount);
    printf("unidatasize = %d\n",header.unidatasize);
    printf("datalength = %d\n",header.datalength);
    printf("fingrade.cash = %.02f\n",fingrade.cash);
    printf("fingrade.receivable = %.02f\n",fingrade.receivable);
    printf("fingrade.stock = %.02f\n",fingrade.stock);
    printf("fingrade.invest = %.02f\n",fingrade.invest);
    printf("fingrade.otherasset = %.02f\n",fingrade.otherasset);
    printf("fingrade.debt = %.02f\n",fingrade.debt);
    printf("fingrade.GPM = %.02f\n",fingrade.GPM);
    printf("fingrade.OPM = %.02f\n",fingrade.OPM);
    printf("fingrade.NOPM = %.02f\n",fingrade.NOPM);
    printf("fingrade.NPM = %.02f\n",fingrade.NPM);
    printf("fingrade.ROE = %.02f\n",fingrade.ROE);
    printf("fingrade.ROA = %.02f\n",fingrade.ROA);
    printf("fingrade.CFR = %.02f\n",fingrade.CFR);
*/    
    bRet = true;
    
FAILED:
    

    return bRet;
}

static bool _GInfoFingrade_Parsing(const int Code, const char *Inputfile, const char *Outputfile)
{
    bool bRet = false;
    FILE *pfinputfile = NULL;
    char line[256];
    SK_FINGRADE fingrade;
    char *pstr;
    int index = 0;

    //check input value
    if (Inputfile == NULL || Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    //open input/output file
    pfinputfile = fopen(Inputfile,"r");
    if (pfinputfile == NULL)
    {
        printf("open file error: %s\n",Inputfile);
        goto FAILED;
    }
    
    //load data to structure SK_DIVIDEND
    while (fgets (line, 256, pfinputfile)!=NULL)
    {
        pstr = strtok(line," \t\n,\"(%)");
        pstr = strtok(NULL," \t\n,\"(%)");    
        
        switch (index)
        {
            case 0:
                fingrade.code = Code;
                fingrade.cash = atof(pstr);
                break;
                
            case 1:
                fingrade.receivable = atof(pstr);
                break;
                
            case 2:
                fingrade.stock = atof(pstr);
                break;
                
            case 3:
                fingrade.invest = atof(pstr);
                break;
                
            case 4:
                fingrade.otherasset = atof(pstr);
                break;
                
            case 5:
                fingrade.debt = atof(pstr);
                break;
                
            case 6:
                fingrade.GPM = atof(pstr);
                break;
                
            case 7:
                fingrade.OPM = atof(pstr);
                break;
                
            case 8:
                fingrade.NOPM = atof(pstr);
                break;
                
            case 9:
                fingrade.NPM = atof(pstr);
                break;
                
            case 10:
                fingrade.ROE = atof(pstr);
                break;
                
            case 11:
                fingrade.ROA = atof(pstr);
                break;
                
            case 12:
                fingrade.CFR = atof(pstr);
                break;
                
            default:
                break;
        }
        index++;
    }
    
    if (!_GInfoFingrade_Output(fingrade, Outputfile))
    {
        printf("GInfoFingrade output error \n");
        goto FAILED;
    }

    bRet = true;

FAILED:
    if (pfinputfile !=NULL) fclose(pfinputfile); 
    
    return bRet;
}

bool SKApi_CVS2SK_GInfoFingrade(const int Code, const char *Inputfile, const char *Outputfile)
{
    bool bRet = false;
    //check input value
    if (Inputfile == NULL || Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    if (!_GInfoFingrade_Parsing(Code, Inputfile, Outputfile))
    {
        printf("GInfoFingrade Pasrsing error\n");
        goto FAILED;
    }

    bRet = true;
FAILED:
    
    return bRet;
    
}

static bool _MopstwseFinReport_Output(SK_MOPSFINREPORT *finreport, int datacount, const char* Outputfile)
{
    bool bRet = false;
    SK_HEADER header = {0};
    SKData_ErrMSG msg = SKData_ErrMSG_Pass;
    
    //check input value
    if (Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    //dump SK_HEADER and SK_DIVIDEND to output file
    header.code = finreport[0].code;
    header.type = SK_DATA_TYPE_MOPSTWSE_FINREPORT;
    header.datacount = datacount;
    header.unidatasize = sizeof(SK_MOPSFINREPORT);
    header.datalength = header.datacount * sizeof(SK_MOPSFINREPORT);

    msg = SKData_DataInsert(Outputfile, &header, finreport);
    if (!(msg == SKData_ErrMSG_Pass || msg == SKData_ErrMSG_Pass_Newfile))
    {
        goto FAILED;
    }
    
    bRet = true;
    
FAILED:
    

    return bRet;
}


static bool _MopstwseFinReport_Parsing(const int Code, const char *Inputfile, const char *Outputfile)
{
    bool bRet = false;
    FILE *pfinputfile = NULL;
    char line[256];
    SK_MOPSFINREPORT finreport[3];
    char *pstr1, *pstr2, *pstr3;
    int index = 0;

    //check input value
    if (Inputfile == NULL || Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    //open input/output file
    pfinputfile = fopen(Inputfile,"r");
    if (pfinputfile == NULL)
    {
        printf("open file error: %s\n",Inputfile);
        goto FAILED;
    }

    //load data to structure SK_DIVIDEND
    while (fgets (line, 256, pfinputfile)!=NULL)
    {
        pstr1 = strtok(line," \t\n,\"");
        pstr1 = strtok(NULL," \t\n,\"");
        pstr2 = strtok(NULL," \t\n,\"");
        pstr3 = strtok(NULL," \t\n,\"");
        
        switch (index)
        {
            case 0:
                finreport[0].code = Code;
                finreport[0].year = atoi(pstr1);
                finreport[1].code = Code;
                finreport[1].year = atoi(pstr2);
                finreport[2].code = Code;
                finreport[2].year = atoi(pstr3);
                break;

            case 1:
                finreport[0].DebtsRatio = atof(pstr1);
                finreport[1].DebtsRatio = atof(pstr2);
                finreport[2].DebtsRatio = atof(pstr3);
                break;

            case 2:
                finreport[0].LTFixedAsset = atof(pstr1);
                finreport[1].LTFixedAsset = atof(pstr2);
                finreport[2].LTFixedAsset = atof(pstr3);
                break;

            case 3:
                finreport[0].CurrentRatio = atof(pstr1);
                finreport[1].CurrentRatio = atof(pstr2);
                finreport[2].CurrentRatio = atof(pstr3);
                break;

            case 4:
                finreport[0].QuickRatio = atof(pstr1);
                finreport[1].QuickRatio = atof(pstr2);
                finreport[2].QuickRatio = atof(pstr3);
                break;
                
            case 5:
                finreport[0].InterestProtectionMultiples = atof(pstr1);
                finreport[1].InterestProtectionMultiples = atof(pstr2);
                finreport[2].InterestProtectionMultiples = atof(pstr3);
                break;
                
            case 6:
                finreport[0].AvgCollectionTurnover = atof(pstr1);
                finreport[1].AvgCollectionTurnover = atof(pstr2);
                finreport[2].AvgCollectionTurnover = atof(pstr3);
                break;
                
            case 7:
                finreport[0].AvgCollectionDay = atof(pstr1);
                finreport[1].AvgCollectionDay = atof(pstr2);
                finreport[2].AvgCollectionDay = atof(pstr3);
                break;
                
            case 8:
                finreport[0].AvgInvent = atof(pstr1);
                finreport[1].AvgInvent = atof(pstr2);
                finreport[2].AvgInvent = atof(pstr3);
                break;
                
            case 9:
                finreport[0].AvgInventoryTurnoverDay = atof(pstr1);
                finreport[1].AvgInventoryTurnoverDay = atof(pstr2);
                finreport[2].AvgInventoryTurnoverDay = atof(pstr3);
                break;

            case 10:
                finreport[0].FixedAssetTurnover = atof(pstr1);
                finreport[1].FixedAssetTurnover = atof(pstr2);
                finreport[2].FixedAssetTurnover = atof(pstr3);
                break;     
                
             case 11:
                finreport[0].TotalAssetTurnover = atof(pstr1);
                finreport[1].TotalAssetTurnover = atof(pstr2);
                finreport[2].TotalAssetTurnover = atof(pstr3);
                break;
        
             case 12:
                finreport[0].ReturnOnTotalAsset = atof(pstr1);
                finreport[1].ReturnOnTotalAsset = atof(pstr2);
                finreport[2].ReturnOnTotalAsset = atof(pstr3);
                break;    

            case 13:
                finreport[0].ReturnOnTotalStockholder = atof(pstr1);
                finreport[1].ReturnOnTotalStockholder = atof(pstr2);
                finreport[2].ReturnOnTotalStockholder = atof(pstr3);
                break; 
               
            case 14:
                finreport[0].PerTaxIncomeToCapitalRatio = atof(pstr1);
                finreport[1].PerTaxIncomeToCapitalRatio = atof(pstr2);
                finreport[2].PerTaxIncomeToCapitalRatio = atof(pstr3);
                break;
              
            case 15:
                finreport[0].NetIncomeToSales = atof(pstr1);
                finreport[1].NetIncomeToSales = atof(pstr2);
                finreport[2].NetIncomeToSales = atof(pstr3);
                break;

            case 16:
                finreport[0].EPS = atof(pstr1);
                finreport[1].EPS = atof(pstr2);
                finreport[2].EPS = atof(pstr3);
                break;
                
            case 17:
                finreport[0].CashFlowRatio = atof(pstr1);
                finreport[1].CashFlowRatio = atof(pstr2);
                finreport[2].CashFlowRatio = atof(pstr3);
                break;
                    
            case 18:
                finreport[0].CashFlowAdequacyRatio = atof(pstr1);
                finreport[1].CashFlowAdequacyRatio = atof(pstr2);
                finreport[2].CashFlowAdequacyRatio = atof(pstr3);
                break;
                
            case 19:
                finreport[0].CashFlowReinvestmentRatio = atof(pstr1);
                finreport[1].CashFlowReinvestmentRatio = atof(pstr2);
                finreport[2].CashFlowReinvestmentRatio = atof(pstr3);
                break;
                
            default:
                break;

        }
        index++;
    }
    
    if (!_MopstwseFinReport_Output(finreport, 3, Outputfile))
    {
        printf("GInfoFingrade output error \n");
        goto FAILED;
    }

    bRet = true;

FAILED:
    if (pfinputfile !=NULL) fclose(pfinputfile); 
    
    return bRet;
}


bool SKApi_CVS2SK_MopstwseFinReport(const int Code, const char *Inputfile, const char *Outputfile)
{
    bool bRet = false;
    //check input value
    if (Inputfile == NULL || Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    if (!_MopstwseFinReport_Parsing(Code, Inputfile, Outputfile))
    {
        printf("GInfoFingrade Pasrsing error\n");
        goto FAILED;
    }

    bRet = true;
FAILED:
    
    return bRet;
    
}


bool SKApi_CVS2SK_Earning(const int Code, const char *Inputfile, const char *Outputfile_Month, const char *Outputfile_Season)
{
    bool bRet = false;
    FILE *pfinputfile = NULL;
    //FILE *pfoutputfile_month = NULL;
    //FILE *pfoutputfile_season = NULL;
    char line[256];
    SK_EARNING_MONTH *month = NULL;
    SK_EARNING_SEASON *season = NULL;
    SK_HEADER *header = NULL;
    char *pstr;
    char temp[16];
    int index = 0;
    int num = 0;
    int num_month = 0;
    int num_season = 0;
    int length_month = 0; 
    int length_season = 0; 
    SKData_ErrMSG msg = SKData_ErrMSG_Pass;

    //check input value
    if (Inputfile == NULL || Outputfile_Month == NULL || Outputfile_Season == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    //open input/output file
    pfinputfile = fopen(Inputfile,"r");
    if (pfinputfile == NULL)
    {
        printf("open file error: %s\n",Inputfile);
        goto FAILED;
    }

    /*
    pfoutputfile_month = fopen(Outputfile_Month,"w");
    if (pfoutputfile_month == NULL)
    {
        printf("open file error: %s\n",Outputfile_Month);
        goto FAILED;
    }

    pfoutputfile_season = fopen(Outputfile_Season,"w");
    if (pfoutputfile_season == NULL)
    {
        printf("open file error: %s\n",Outputfile_Season);
        goto FAILED;
    }
*/

    //load data to structure SK_EARNING_MONTH and SK_EARNING_SEASON
    month = malloc(sizeof(SK_EARNING_MONTH)*36);
    season = malloc(sizeof(SK_EARNING_SEASON)*12);
    
    while (fgets (line, 256, pfinputfile)!=NULL)
    {
        if (strncmp(line,"\"",1))
        {
            continue;
        }

        pstr = strtok(line," \t\n,\"");
        if (atoi(pstr)==0)
        {
            continue;
        }

        index = 0;

        if (num < 12)
        {
            while (pstr != NULL)
            {
                
                if (!strncmp(pstr,",",1))
                {
                    pstr = strtok(NULL," \t\n\"");
                    continue;
                }
                
                switch (index)
                {

                    case 0:
                        memset(temp,'\0',16);
                        memcpy(temp,pstr,3);
                        month[num+12].year = atoi(temp);
                        month[num+12].month = atoi(pstr+3);
                        month[num].year = month[num+12].year -1;
                        month[num].month = month[num+12].month;
                        break;
                        
                    case 1:
                        month[num+12].income =  atoi(Skip_specific_character(pstr, strlen(pstr),','));
                        break; 

                    case 2:
                        month[num].income =month[num+12].income  /(1+atof(pstr) / 100) ;
                        break;

                    case 3:
                        memset(temp,'\0',16);
                        memcpy(temp,pstr,3);
                        month[num+24].year = atoi(temp);
                        month[num+24].month = atoi(pstr+3);
                        break; 

                    case 4:
                        month[num+24].income =  atoi(Skip_specific_character(pstr, strlen(pstr), ','));
                        break;
                        
                    default:
                        break;
                }
            
                pstr = strtok(NULL," \t\n\"");
                index++;
            }
        }
        else if (num >= 12 && num <16)
        {
            while (pstr != NULL)
            {
                if (!strncmp(pstr,",",1))
                {
                    pstr = strtok(NULL," \t\n\"");
                    continue;
                }

                switch (index)
                {

                    case 0:
                        memset(temp,'\0',16);
                        memcpy(temp,pstr,3);
                        season[num-12+4].year = atoi(temp);
                        season[num-12+4].season= atoi(pstr+3);
                        season[num-12].year = season[num-12+4].year -1;
                        season[num-12].season = season[num-12+4].season;
                        break;
                        
                    case 1:
                        season[num-12+4].aftertax_income=  atoi(Skip_specific_character(pstr, strlen(pstr),','));
                        break; 

                    case 2:
                        season[num-12].aftertax_income = season[num-12+4].aftertax_income /(1+ atof(pstr) / 100) ;
                        break;

                    case 3:
                        memset(temp,'\0',16);
                        memcpy(temp,pstr,3);
                        season[num-12+8].year = atoi(temp);
                        season[num-12+8].season= atoi(pstr+3);
                        break; 

                    case 4:
                        season[num-12+8].aftertax_income =  atoi(Skip_specific_character(pstr, strlen(pstr),','));
                        break;
                        
                    default:
                        break;
                }            
                pstr = strtok(NULL," \t\n\"");
                index++;
            }
        }
        else if (num >= 16 && num <20)
        {
           while (pstr != NULL)
            {
                if (!strncmp(pstr,",",1))
                {
                    pstr = strtok(NULL," \t\n\"");
                    continue;
                }
               
                switch (index)
                {
                        
                    case 1:
                        season[num-16+4].pretax_income=  atoi(Skip_specific_character(pstr, strlen(pstr),','));
                        break; 

                    case 2:
                        season[num-16].pretax_income = season[num-16+4].pretax_income / (1+ atof(pstr) / 100) ;
                        break;

                    case 4:
                        season[num-16+8].pretax_income =  atoi(Skip_specific_character(pstr, strlen(pstr),','));
                        break;
                        
                    default:
                        break;
                }            
                pstr = strtok(NULL," \t\n\"");
                index++;
            }
        }        
        num++;
    }

    //dump SK_HEADER and SK_EARNING_MONTH to output file
    header = malloc(sizeof(SK_HEADER));
    
    for (index = 0 ; index<36; index++)
    {
        if (month[index].income == 0)
        {
            break;
        }
        num_month++;
        length_month += sizeof(SK_EARNING_MONTH);
    }

    header->code = Code;
    header->type = SK_DATA_TYPE_EARNING_MONTH;
    header->datacount = num_month;
    header->unidatasize = sizeof(SK_EARNING_MONTH);
    header->datalength = length_month;
    pstr = (char *)header;
    /*
    for (index = 0; index < sizeof(SK_HEADER); index++)
    {
        fprintf(pfoutputfile_month,"%c", pstr[index]);
    }

    pstr = (char *)month;
    for (index = 0; index < length_month; index++)
    {
        fprintf(pfoutputfile_month,"%c", pstr[index]);
    }
    */
    msg = SKData_DataInsert(Outputfile_Month, header, month);
    if (!(msg == SKData_ErrMSG_Pass || msg == SKData_ErrMSG_Pass_Newfile))
    {
        goto FAILED;
    }

    
    //dump SK_HEADER and SK_EARNING_SEASON to output file
    for (index = 0 ; index<12; index++)
    {
        num_season++;
        length_season += sizeof(SK_EARNING_SEASON);
    }
    
    header->code = Code;
    header->type = SK_DATA_TYPE_EARNING_SEASON;
    header->datacount = num_season;
    header->unidatasize = sizeof(SK_EARNING_SEASON);
    header->datalength = length_season;

    msg = SKData_DataInsert(Outputfile_Season, header, season);
    if (!(msg == SKData_ErrMSG_Pass || msg == SKData_ErrMSG_Pass_Newfile))
    {
        goto FAILED;
    }

    /*
    pstr = (char *)header;
    for (index = 0; index < sizeof(SK_HEADER); index++)
    {
        fprintf(pfoutputfile_season,"%c", pstr[index]);
    }
    
    pstr = (char *)season;
    for (index = 0; index < length_season; index++)
    {
        fprintf(pfoutputfile_season,"%c", pstr[index]);
    }
    */
    bRet = true;
    
FAILED:
    if (pfinputfile !=NULL) fclose(pfinputfile); 
    //if (pfoutputfile_month !=NULL) fclose(pfoutputfile_month);
    //if (pfoutputfile_season !=NULL) fclose(pfoutputfile_season);
    if (month !=NULL) free(month);
    if (season !=NULL) free(season);
    if (header !=NULL) free(header);
    
    return bRet;
    
}

static bool _Price_Output(SK_PRICE skprice, const char* OutputPath)
{
    bool bRet = false;
    SK_HEADER header;
    char filename[LINE_LEN+1];
    SKData_ErrMSG msg = SKData_ErrMSG_Pass;

    //check input value
    if (OutputPath == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    sprintf(filename,"%s/%d.price",OutputPath,skprice.code);

    header.code =  skprice.code;
    header.type = SK_DATA_TYPE_PRICE;
    header.datacount = 1;
    header.unidatasize = sizeof(SK_PRICE);
    header.datalength = header.unidatasize * header.datacount;

    msg = SKData_DataInsert(filename, &header, &skprice);
    if (!(msg == SKData_ErrMSG_Pass || msg == SKData_ErrMSG_Pass_Newfile))
    {
        goto FAILED;
    }
    //printf("%d, %s, %f, %f,%f\n",skprice.code, skprice.name, skprice.price_end, skprice.price_start, skprice.PEratio);

    bRet = true;

FAILED:
    
    return bRet;
}

static bool _Price_Parsing(const char *Inputfile, const char* OutputPath)
{

    //Local define 
    char keyword1[20] = {0xE6,0xAF,0x8F,0xE6,0x97,0xA5,0xE6,0x94,0xB6,0xE7,0x9B,0xA4,'\0'}; // // ¨C   ¤é    ¦¬   ½L
    char keyword2[20] = {0x22, 0xe8,0xad,0x89,0xe5,0x88,0xb8,0xe4,0xbb,0xa3,0xe8,0x99,
        0x9f, 0x22, '\0'}; // "ÃÒ   ¨÷   ¥N  ¸¹"  " 
    char keyword3[20] = {0x22,0xE5,0x82,0x99,0xE8,0xA8,0xBB,0x3A,0x22,'\0'}; // " ³Æµ    µù  

    char s_year[4] = {0xe5,0xb9,0xb4,'\0'}; // ¦~
    char s_month[4] = {0xe6,0x9c,0x88,'\0'};// ¤ë
    char s_day[4] = {0xe6,0x97,0xa5,'\0'};// ¤é


    bool bRet = false;
    FILE *finput = NULL;
    char line[LINE_LEN+1];
    char *pstr = NULL;
    bool bParsing = false;
    bool bGetdata = false;
    SK_PRICE skprice = {0};
    int year,month,day;
    int index = 0;
    
    //check input value
    if (Inputfile == NULL || OutputPath == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    finput = fopen(Inputfile, "r");
    if (finput == NULL)
    {
        printf("Can't open file :%s \n", Inputfile);
        goto FAILED;
    }

    while (fgets (line, LINE_LEN, finput)!=NULL)
    {
#if 0
        printf("%s",line);
        for (index = 0; index < 30; index++)
        {
            printf("0x%02X,",(unsigned char)line[index]);
        }
        printf("\n");
#endif
    
        if (bParsing)
        {
            if (!(line[0] == 0x22 || line[0]  == 0x3D)  || strstr (line,keyword3)  != NULL) // " and =
            {
                break;
            }
            else
            {
                memset(&skprice, '\0', sizeof(SK_PRICE));
                index = 0;

                //printf("%s",line);
                pstr = Skip_specific_character(line, LINE_LEN, ',');
                pstr = strtok(pstr,"\"= \t\n+-");
                
                while (pstr != NULL)
                {
                    switch (index)
                    {
                        case 0:
                            skprice.year = year;
                            skprice.month = month;
                            skprice.day = day;
                            skprice.code = atoi(pstr);
                            break;
                            
                        case 1:
                            sprintf(skprice.name,"%s",pstr);
                            break;
                            
                        case 2:
                            skprice.strikestock = atoi(pstr);
                            break;
                            
                        case 3:
                            skprice.strikenum = atoi(pstr);
                            break;
                            
                        case 4:
                            skprice.turnover = atoi(pstr);
                            break;
                            
                        case 5:
                            skprice.price_start= atof(pstr);
                            break;
                            
                        case 6:
                            skprice.price_max= atof(pstr);
                            break;
                            
                        case 7:
                            skprice.price_min= atof(pstr);
                            break;
                            
                        case 8:
                            skprice.price_end = atof(pstr);
                            break;
                            
                        case 9:
                            skprice.price_diff = atof(pstr);
                            break;
                            
                        case 14:
                            skprice.PEratio = atof(pstr);
                            break;
                            
                        default:
                            break;
                    }
                
                    pstr = strtok(NULL,"\"= \t\n,+-");
                    index++;
                }
                //printf("%d, %s, %f, %f\n",skprice.code, skprice.name, skprice.price_end, skprice.PEratio);
                _Price_Output(skprice, OutputPath);
            }
        }
        else if (bGetdata)
        {
            pstr = strstr (line,keyword2);
            if (pstr !=NULL)
            {
                bParsing = true;
            }
        }
        else
        {
            pstr = strstr (line,keyword1);
            if (pstr !=NULL)
            {
                pstr = Skip_specific_character(line, LINE_LEN, '"');
                memcpy(line, pstr, LINE_LEN);
                pstr = strstr (line,s_year);
                strncpy (pstr,"   ",3);
                pstr = strstr (line,s_month);
                strncpy (pstr,"   ",3);
                pstr = strstr (line,s_day);
                strncpy (pstr,"   ",3);
                
                sscanf(line,"%d  %d  %d  ",&year, &month,&day);

                //printf("%d-%d-%d\n",year, month, day);            
                bGetdata = true;
            }
        }     
        pstr = NULL;
    }
    
    bRet = true;
    
    FAILED:
    if (finput !=NULL) 
        fclose(finput); 

    return bRet;
}

bool SKApi_CVS2SK_Price(const char *Inputfilelist, const char *OutputPath)
{
    bool bRet = false;
    FILE *finputFileList = NULL;
    char line[LINE_LEN+1];
    char inputlistpath[LINE_LEN+1];
    char inputfile[LINE_LEN+1];
    char *pstr = NULL;

    //check input value
    if (Inputfilelist == NULL || OutputPath == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    //open filelist
    finputFileList = fopen(Inputfilelist,"r");
    if (finputFileList == NULL)
    {
        printf("Can't open file :%s \n", Inputfilelist);
        goto FAILED;
    }

    //read filelist
    if (fgets (line, LINE_LEN, finputFileList)!=NULL)
    {
        pstr = strtok(line," \t\n,\":");
        sprintf(inputlistpath,"%s/",pstr);
    }

    while (fgets (line, LINE_LEN, finputFileList)!=NULL)
    {
        pstr = strtok(line," \t\n,\":");
        if (pstr != NULL)
        {
            sprintf(inputfile,"%s%s",inputlistpath,line);
            printf("%s\n",inputfile);
            _Price_Parsing(inputfile, OutputPath);
        }
        pstr = NULL;
    }
    bRet = true;
FAILED:
    if (finputFileList !=NULL) fclose(finputFileList); 

    
    return bRet;

}

static void _Item2Financial(int item, char *str, SK_FINANCIAL* financial)
{
    int Operating_total = 0;
    static unsigned int Class = 0;

    switch (item)
    {
        case 0:
            financial->Code = atoi(str);
            if (financial->Code < 1000) // class, not stock
            {
                Class = financial->Code;
            }
            financial->Class = Class;
            break;
            
        case 2:
            financial->Operating_Revenues = atoi(str);
            break;
            
        case 5:
            financial->Operating_Income = atoi(str);
            break;
            
        case 7:
            financial->NonOperating_Income = atoi(str);
            Operating_total =  financial->Operating_Income+financial->NonOperating_Income;
            if (Operating_total  >= 0 )
                financial->Operating_ratio = ((float)financial->Operating_Income) / Operating_total;
            break;
            
        case 9:
            financial->Income_after_Tax = atoi(str);
            break;
            
        case 12:
            financial->Capital_Stock = atoi(str);
            if (financial->Capital_Stock != 0 )
                financial->EPS = ((float)financial->Income_after_Tax) / financial->Capital_Stock * 10;
            break;
            
        case 19:
            financial->Income_before_Tax = atoi(str);
            break;
            
        default:
            break;
    }
}

void Financial_sort( int num, SK_FINANCIAL* Financial )
{
    SK_FINANCIAL temp;
    int i,j;

    for (i = 0; i < num; i++)
    {
        for (j = 0; j < num -i - 1; j++)
        {
            if (Financial[j].Year > Financial[j+1].Year || 
                (Financial[j].Year == Financial[j+1].Year && Financial[j].Season>= Financial[j+1].Season))
            {
                memcpy(&temp, &Financial[j],sizeof(SK_FINANCIAL));
                memcpy(&Financial[j], &Financial[j+1],sizeof(SK_FINANCIAL));
                memcpy(&Financial[j+1], &temp,sizeof(SK_FINANCIAL));
            }
        }
    }
}

static bool Financial_check(int count, SK_FINANCIAL* Financial_old,  SK_FINANCIAL *Financial_new)
{
    //check same financial report flow
    bool bRet = false;
    int index = 0;
    for (index = 0; index < count; index++)
    {
        if(Financial_old[index].Year == Financial_new->Year && 
        Financial_old[index].Season== Financial_new->Season)
        {
            bRet = true;
            break;
        }
    }
    
    return bRet;
}

static bool _Financial_dateparsing(char *line, int *season, int *year)
{
    bool bRet = false;
    char *pstr = NULL;
    int data_count = 0;

    pstr = strtok(line," \t\n");
    while(pstr != NULL)
    {
        if (data_count >= 2)
            break;

        if (data_count == 0)
        {
            if(strncmp(pstr,"Jan-Mar",7)==0)
            {
                *season = 1;
            }
            else if (strncmp(pstr,"Jan-Jun",7)==0)
            {
                *season = 2;
            }
            else if (strncmp(pstr,"Jan-Sep",7)==0)
            {
                *season = 3;
            }
            else if (strncmp(pstr,"Jan-Dec",7)==0)
            {
                *season = 4;
            }
        }
        else if (data_count == 1)
        {
            *year = (atoi(pstr)-1911);
        }
        pstr = strtok(NULL," \t\n");
        data_count++;
    }    
    bRet = true;
    return bRet;
}

static bool _Financial_parsing(char *line, SK_FINANCIAL* financial)
{
    char *p_start=NULL,*p_end=NULL;
    int index = 0;
    int item = 0;
    char temp[32];
    int len = 0;
    bool bRet = false;

    for (index = 0; index < strlen(line);index++)
     {   
         if (line[index] == '\t' )
         {
             if (p_start != NULL)
                 p_end = line + index;
             
             if (line[index+1]=='\t')
                 p_start = line + index+1;
         }
         else if(line[index] == '\"' || line[index]==' ')
         {
             if (p_start != NULL)
                 p_end = line+index;
         }
         else if (p_start == NULL)
         {
             p_start=line+index;
         }
         else if (line[index]=='\n')
         {
             if (p_start != NULL)
                 p_end = line+index-1;
             else
             {
                 p_start = line+index;
                 p_end = line+index;
             }
         }
         
         if (p_start != NULL  && p_end != NULL)
         {
             memset(temp,'\0',32);
             len = p_end - p_start;
             if (len < 0)
             {
                _Item2Financial(item, "0", financial);
                 item++;
             }               
             else
                 memcpy(temp,p_start,p_end-p_start);
             
             if (item == 0 || item == 2 || item == 5 || item == 7 || item == 9 || item == 12 || item == 19)
             {
                 if (len <= 0)
                    _Item2Financial(item, "0", financial);
                 else
                    _Item2Financial(item, Skip_specific_character_negative(temp,strlen(temp),','), financial);
             }
             p_start=NULL;
             p_end=NULL;
             item++;
         }       
     }
    
    bRet = true;
    return bRet;
}

static void _Calculate_EPS_class(SK_FINANCIAL * financial)
{
    int index = 0;
    float classeps[100] = {0}; // 100 is class number
    int classnum[100] = {0}; // 100 is class number
    
    for (index = 0; index < FINANCIAL_NEW_NUM; index++)
    {
        if (financial[index].Code < 1000)
        {
            classeps[0] += financial[index].EPS;
            classnum[0]++;
        }
        else
        {
            classeps[financial[index].Class] += financial[index].EPS;
            classnum[financial[index].Class]++;
        }

        if (financial[index].Code == 0)
            break;
    }


    for (index = 0; index < FINANCIAL_NEW_NUM; index++)
    {
        financial[index].EPS_Class = classeps[financial[index].Class] / classnum[financial[index].Class];

        if (financial[index].Code == 0)
            break;
    }
    
}

void *SK_Realloc(void *ptr, size_t size)
{
    void *Ret = NULL;
    Ret = realloc(ptr,size);
    if (ptr == NULL && Ret != NULL)
        memset(Ret,'\0',size);
    return Ret;
}

void *SK_Malloc(size_t size)
{
    void *Ret = NULL;
    Ret = malloc(size);
    if (Ret != NULL)
        memset(Ret,'\0',size);
    return Ret;
}

void SK_Free(void *ptr)
{
    free(ptr);
}

bool SKApi_CVS2SK_FinancialReport(const char *inputfile, const char *outpath)
{
    bool bRet = false;
    FILE *pfinput = NULL;
    FILE *pfoutput = NULL;
    char line[LINE_LEN]="";
    int line_count = 0;
    SK_FINANCIAL *Financial_new = NULL;
    SK_FINANCIAL *Financial_old = NULL;
    SK_HEADER  *header = NULL;
    int index = 0;
    int loop = 0;
    char outputfilename[128];
    char *pstr = NULL;
    int season =0;
    int year = 0;
    
    pfinput = fopen(inputfile,"r");
    if (pfinput == NULL)
    {
        printf("open file : %s failed \n",inputfile);
        goto FAILED;
    }

    Financial_new =  malloc(sizeof(SK_FINANCIAL)*FINANCIAL_NEW_NUM);
    if (Financial_new == NULL)
    {
        printf("alloc mem  failed : Financial_new\n");
        goto FAILED;
    }
    memset(Financial_new, '\0',sizeof(SK_FINANCIAL)*FINANCIAL_NEW_NUM);

    //parsing new financial report
    while(fgets(line,LINE_LEN,pfinput)!=NULL)
    {
        if (line[0]>='0' && line[0]<='9')
        {
            if (_Financial_parsing(line, &Financial_new[index]))
            {
                Financial_new[index].Year = year;
                Financial_new[index].Season = season;
                index++;
            }
        }
        else if (line_count == 1)
        {
            _Financial_dateparsing(line, &season, &year);
        }
        memset(line,'\0',LINE_LEN);
        line_count++;
    }

    _Calculate_EPS_class(Financial_new);

    for (index = 0; index < FINANCIAL_NEW_NUM; index++)
    {

        /*
        printf("%d-%d, %d,  %d, %d, %f, %f,%f\n",
        Financial_new[index].Year, 
        Financial_new[index].Season,
        Financial_new[index].Code, 
        Financial_new[index].Class, 
        Financial_new[index].NonOperating_Income,
        Financial_new[index].EPS,
        Financial_new[index].EPS_Class,
        Financial_new[index].Operating_ratio);
        */
        
        sprintf(outputfilename,"%s%d.financial",outpath,Financial_new[index].Code);
        pfoutput = fopen(outputfilename,"a+");
        if (pfoutput == NULL)
        {
            printf("open file : %s failed \n",outputfilename);
            goto ERROR;
        }

        header =  SK_Malloc(sizeof(SK_HEADER));
        if (header == NULL)
        {
            printf("alloc mem  failed : header\n");
            goto ERROR;
        }

        /*read header and old financial report*/
        if (fread(header, sizeof(SK_HEADER), 1, pfoutput) ==0)
        {
            //new file
        }
        else
        {
            //already exist file
            // parsing - older financial report
            Financial_old =  SK_Malloc(sizeof(SK_FINANCIAL)*header->datacount);
            if (Financial_old == NULL)
            {
                printf("alloc mem  failed : Financial_old\n");
                goto ERROR;
            }
            if (fread(Financial_old, sizeof(SK_FINANCIAL), header->datacount, pfoutput) ==0)
            {
                printf("read Financial report failed\n");
                goto ERROR;
            }
            
            fclose(pfoutput);
            pfoutput = fopen(outputfilename,"w");
        }

        if (header->datacount == 0)
        {
            //new report
            header->code = Financial_new[index].Code;
            header->type = SK_DATA_TYPE_FINANCIALREPORT_SEASON;
            header->datacount = 1;
            header->unidatasize = sizeof(SK_FINANCIAL);
            header->datalength = header->datacount * sizeof(SK_FINANCIAL);

            Financial_old = SK_Realloc(Financial_old, sizeof(SK_FINANCIAL));
            if (Financial_old==NULL)
            {
                printf("alloc mem  failed : Financial_old\n");
                goto ERROR;
            }
            memcpy(&Financial_old[0],&Financial_new[index], sizeof(SK_FINANCIAL));
        }
        else
        {
            if (!Financial_check(header->datacount, Financial_old, &Financial_new[index]))
            {
                header->datacount++;
                header->datalength = header->datacount * sizeof(SK_FINANCIAL);
                Financial_old = SK_Realloc(Financial_old, header->datalength);
                if (Financial_old==NULL)
                {
                    printf("alloc mem  failed : Financial_old\n");
                    goto ERROR;
                }
                memcpy(&Financial_old[header->datacount-1],&Financial_new[index], sizeof(SK_FINANCIAL));
                Financial_sort(header->datacount,Financial_old);
            }
        }

        //output financial report to outpu file
        pstr = (char *)header;
        for (loop = 0; loop < sizeof(SK_HEADER); loop++)
        {
            fprintf(pfoutput,"%c", pstr[loop]);
        }

        pstr = (char *)Financial_old;
        for (loop = 0; loop < header->datalength; loop++)
        {
            fprintf(pfoutput,"%c", pstr[loop]);
        }
       
ERROR:
        if (pfoutput != NULL) {fclose(pfoutput);pfoutput=NULL;}
        if (header !=NULL){SK_Free(header);header = NULL;}
        if (Financial_old !=NULL){SK_Free(Financial_old);Financial_old = NULL;}

        
        if (Financial_new[index].Code == 0) // zero is the laster financial report
            break;
    }

    bRet = true;
FAILED:
    if (pfinput != NULL) {fclose(pfinput); pfinput = NULL;}
    if (Financial_new != NULL)  {free(Financial_new); Financial_new = NULL;}
    
    return bRet;
}

bool SKApi_CVS2SK_Help(void)
{
    printf("\ncvs2sk [Command] [Code] [CVS file/CVS files List] [SK file] [SK file 2]\n\n");
    printf("=============CVS2SK Commnd List==================\n");
    printf("[0] trans price.cvs to sk file\n");
    printf("     cvs2sk 0 price.filelist price.outputpath\n");
    printf("[1] trans dividend.cvs to sk file\n");
    printf("     cvs2sk 1 code dividend.cvs dividend.sk\n");
    printf("[2] trans earning.cvs to sk file\n");
    printf("     cvs2sk 2  code earning.cvs earning.month.sk earning.season.sk\n");
    printf("[3] continue ...\n");
    printf("==============================================\n");
    return true;
}



