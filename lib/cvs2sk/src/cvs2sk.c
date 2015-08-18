
#include "SKcommon.h"
#include "SKdata.h"
#include "cvs2sk.h"
#include "string.h"


#define MAX_STRBUFFER_SIZE 128
static char strbuf[MAX_STRBUFFER_SIZE];

char *Skip_specific_character(const char* input, unsigned int len, char specific)
{
    unsigned int index = 0;    
    unsigned int count = 0;

    memset(strbuf,'\0',MAX_STRBUFFER_SIZE);

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

bool SKApi_CVS2SK_Dividend(const int Code, const char *Inputfile, const char *Outputfile)
{
    bool bRet = false;
    FILE *pfinputfile = NULL;
    FILE *pfoutputfile = NULL;
    char line[256];
    SK_DIVIDEND *dividend = NULL;
    SK_HEADER *header = NULL;
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
    
    pfoutputfile = fopen(Outputfile,"w");
    if (pfoutputfile == NULL)
    {
        printf("open file error: %s\n",Outputfile);
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
        
    //dump SK_HEADER and SK_DIVIDEND to output file
    header = malloc(sizeof(SK_HEADER));
    if (header == NULL)
    {
        printf("header malloc error\n");
        goto FAILED;
    }

    memset(header,'\0',sizeof(SK_HEADER));
    
    header->code = Code;
    header->type = SK_DATA_TYPE_DIVIDEND;
    header->datacount = num;
    header->unidatasize = sizeof(SK_DIVIDEND);
    header->datalength = length;

    pstr = (char *)header;
    for (index = 0; index < sizeof(SK_HEADER); index++)
    {
        fprintf(pfoutputfile,"%c", pstr[index]);
    }

    Sort_dividend(dividend,num);

    pstr = (char *)dividend;
    for (index = 0; index < length; index++)
    {
        fprintf(pfoutputfile,"%c", pstr[index]);
    }
    
    bRet = true;

FAILED:
    if (pfinputfile !=NULL) fclose(pfinputfile); 
    if (pfoutputfile !=NULL) fclose(pfoutputfile);
    if (dividend !=NULL) free(dividend);
    if (header !=NULL) free(header);
    return bRet;
    
}

bool SKApi_CVS2SK_Earning(const int Code, const char *Inputfile, const char *Outputfile_Month, const char *Outputfile_Season)
{
    bool bRet = false;
    FILE *pfinputfile = NULL;
    FILE *pfoutputfile_month = NULL;
    FILE *pfoutputfile_season = NULL;
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
    for (index = 0; index < sizeof(SK_HEADER); index++)
    {
        fprintf(pfoutputfile_month,"%c", pstr[index]);
    }


    
    pstr = (char *)month;
    for (index = 0; index < length_month; index++)
    {
        fprintf(pfoutputfile_month,"%c", pstr[index]);
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

    bRet = true;
    
FAILED:
    if (pfinputfile !=NULL) fclose(pfinputfile); 
    if (pfoutputfile_month !=NULL) fclose(pfoutputfile_month);
    if (pfoutputfile_season !=NULL) fclose(pfoutputfile_season);
    if (month !=NULL) free(month);
    if (season !=NULL) free(season);
    if (header !=NULL) free(header);
    
    return bRet;
    
}

bool SKApi_CVS2SK_Price(const int Code, const char *Inputfilelist, const char *Outputfile)
{
    bool bRet = false;
    FILE *pfinputfilelist = NULL;
    FILE *pfinputffile = NULL;
    FILE *pfoutputfile = NULL;
    char line[MAX_STRBUFFER_SIZE];
    int linelen = 0;
    char dirpath[MAX_STRBUFFER_SIZE];
    char filepath[MAX_STRBUFFER_SIZE*2];
    char *pstr = NULL, *pstr2 = NULL;
    int listcount  = 0;
    bool bfile = false;
    char tempstr[32];
    int pstrlen = 0;
    bool bcat = false;
    bool bcatdone = false;
    int index = 0;
    SK_PRICE *price = NULL;
    SK_HEADER *header = NULL;
    int pricecount = 0;
    int allocsize = 0;
    char temp_8[8];
    short ymd=0;
    
    //check input value
    if (Inputfilelist == NULL || Outputfile == NULL)
    {
        printf("invalid input value\n");
        goto FAILED;
    }

    //open input list file /output file
    pfinputfilelist = fopen(Inputfilelist,"r");
    if (pfinputfilelist == NULL)
    {
        printf("open file error: %s\n",Inputfilelist);
        goto FAILED;
    }
    
    pfoutputfile = fopen(Outputfile,"w");
    if (pfoutputfile == NULL)
    {
        printf("open file error: %s\n",Outputfile);
        goto FAILED;
    }
    
    //open each file and load data  to structure SK_PRICE
    while (fgets (line, MAX_STRBUFFER_SIZE, pfinputfilelist)!=NULL)
    {
        listcount++;
        bfile = false;
        linelen = strlen(line);
        
        if (line[linelen-2] == ':')
        {
            //get dirpath from listfile
            {
                pstr = strtok(line,":");
                if (strlen(pstr)> MAX_STRBUFFER_SIZE-1)
                {
                    printf("filelist path too long\n");
                    goto FAILED;
                }
                memset(dirpath,'\0',MAX_STRBUFFER_SIZE);
                strncpy(dirpath,pstr,strlen(pstr));
                strcat(dirpath,"/");
            }
        }
        else if (line[linelen-2] != ':' && line[linelen-1] == '\n')
        {
            //get filepath from listfile
            pstr = strtok(line,"\n");
            memset(filepath,'\0',MAX_STRBUFFER_SIZE*2);
            strncpy(filepath,dirpath,strlen(dirpath));   
            strcat(filepath,pstr);
            bfile = true;
        }
        else
        {
            //invaild format
            printf("invaild filelist format %s : %d\n", Inputfilelist,listcount);
            goto FAILED;
        }

        if (bfile)
        {
            //open file
            pfinputffile = fopen(filepath,"r");
            if (pfinputffile == NULL)
            {
                printf("open file error: %s\n",filepath);
                goto FAILED;
            }
            
            while (fgets (line, MAX_STRBUFFER_SIZE, pfinputffile)!=NULL)
            {
                if (strncmp(line," ",1))
                {
                    continue;
                }

                if (pricecount >= allocsize)
                {
                    allocsize = pricecount +100;
                    price = realloc(price, allocsize*sizeof(SK_PRICE));
               
                    if (price == NULL)
                    {
                        printf("price realloc fail \n");
                        goto FAILED;
                    }
                    else
                    {
                        //printf("realloc PRICE buffer to %d x %ld  \n",allocsize,sizeof(SK_PRICE));
                    }
                }
                
                // the following are parse price specific format
                bcat = false;
                index = 0;
                pstr = strtok(line,", \n\t");
                while (pstr !=NULL)
                {
                    pstrlen = strlen(pstr);

                    if (bcat)
                    {
                        strcat(tempstr,pstr);
                    }
                    
                    if (pstr[0] == '"')
                    {
                        bcat = true;
                        memset(tempstr, '\0', 32);
                        strncpy(tempstr, pstr, pstrlen);
                    }
                    else if (pstr[pstrlen-1] == '"')
                    {
                        bcat = false;
                        bcatdone = true;
                    }


                    if (bcat == false)
                    {
                        if (!bcatdone)
                        {
                            memset(tempstr, '\0', 32);
                            strncpy(tempstr, pstr, pstrlen);
                        }
                        else
                        {
                            bcatdone = false;
                        }

                        //printf("%s(%d),",Skip_specific_character(tempstr,  strlen(tempstr),'"'),index);

                        switch (index)
                        {
                            case 0 :
                                pstr = tempstr;
                                ymd = 0;
                                
                                for (pstr2 = tempstr ; *pstr2 != '\0'; pstr2++)
                                {
                                    if (*pstr2 == '/')
                                    {
                                        memset(temp_8,'\0',8);
                                        strncpy(temp_8,pstr,pstr2-pstr);
                                        pstr = pstr2+1;

                                        if (ymd == 0)
                                        {   
                                            price[pricecount ].year = atoi(temp_8);
                                            ymd++;
                                        }
                                        else
                                        {
                                            price[pricecount ].month = atoi(temp_8);
                                            price[pricecount ].day = atoi(pstr);
                                            break;
                                        }
                                    }
                                }
                                
                                break;

                            case 1:
                                price[pricecount ].strikestock = atoi(Skip_specific_character(tempstr,  strlen(tempstr),'"'));
                                break;

                            case 2:
                                price[pricecount ].turnover = atol(Skip_specific_character(tempstr,  strlen(tempstr),'\"'));
                                break;

                            case 3:
                                price[pricecount ].price_start = atof(pstr);
                                break;

                            case 4:
                                price[pricecount ].price_max = atof(tempstr);
                                break;

                            case 5:
                                price[pricecount ].price_min = atof(tempstr);
                                break;

                            case 6:
                                price[pricecount ].price_end = atof(tempstr);
                                break;
                                    
                            case 7:
                                price[pricecount ].price_diff = atof(tempstr);
                                break;

                            case 8:
                                price[pricecount ].strikenum = atoi(Skip_specific_character(tempstr,  strlen(tempstr),'"'));
                                break;
                                
                            default:
                                break;
                        }
                        index++;
                    }
                    
                    pstr = strtok(NULL,", \n\t");
                }
                pricecount++;
                //printf("\n");
            }
            
            fclose(pfinputffile);
            pfinputffile = NULL;
        }
    }


#if 0
    for (index = 0; index < pricecount; index++)
    {
        printf("%d,%d,%d,%d,%ld,%f,%f,%f,%f,%f,%d\n", price[index ].year, 
            price[index].month, price[index].day, price[index].strikestock, 
            price[index].turnover, price[index].price_start, 
            price[index].price_max, price[index].price_min, 
            price[index].price_end, 
            price[index].price_diff,price[index].strikenum);

    }
    printf("count = %d\n",pricecount);
#endif

    //dump SK_HEADER and SK_PRICE to output file
    header = malloc(sizeof(SK_HEADER));

    header->code = Code;
    header->type = SK_DATA_TYPE_PRICE;
    header->datacount = pricecount;
    header->unidatasize = sizeof(SK_PRICE);
    header->datalength = pricecount * sizeof(SK_PRICE);

    pstr = (char *)header;
    for (index = 0; index < sizeof(SK_HEADER); index++)
    {
        fprintf(pfoutputfile,"%c", pstr[index]);
    }

    pstr = (char *)price;
    for (index = 0; index < header->datalength; index++)
    {
        fprintf(pfoutputfile,"%c", pstr[index]);
    }

    bRet = true;
    
FAILED:
    if (pfinputfilelist !=NULL) fclose(pfinputfilelist); 
    if (pfinputffile !=NULL) fclose(pfinputffile);
    if (pfoutputfile !=NULL) fclose(pfoutputfile);
    if (price !=NULL) free(price);
    if (header !=NULL) free(header);
        
    return bRet;

}

#define LINE_LEN 256
#define FINANCIAL_NEW_NUM 1500

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
    SK_FINANCIAL *Financial_new = NULL;
    SK_FINANCIAL *Financial_old = NULL;
    SK_HEADER  *header = NULL;
    int index = 0;
    int loop = 0;
    char outputfilename[128];
    char *pstr = NULL;
    
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
                index++;
        }
        memset(line,'\0',LINE_LEN);
    }

    _Calculate_EPS_class(Financial_new);

    for (index = 0; index < FINANCIAL_NEW_NUM; index++)
    {
        printf("%d,  %d, %d, %f, %f,%f\n",
        Financial_new[index].Code, 
        Financial_new[index].Class, 
        Financial_new[index].NonOperating_Income,
        Financial_new[index].EPS,
        Financial_new[index].EPS_Class,
        Financial_new[index].Operating_ratio);
        
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
            ;
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
            //TODO: add check same financial report flow
            header->datacount++;
            header->datalength = header->datacount * sizeof(SK_FINANCIAL);
            Financial_old = SK_Realloc(Financial_old, header->datalength);
            if (Financial_old==NULL)
            {
                printf("alloc mem  failed : Financial_old\n");
                goto ERROR;
            }
            memcpy(&Financial_old[header->datacount-1],&Financial_new[index], sizeof(SK_FINANCIAL));
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
    printf("     cvs2sk 0 price.filelist price.sk\n");
    printf("[1] trans dividend.cvs to sk file\n");
    printf("     cvs2sk 1 dividend.cvs dividend.sk\n");
    printf("[2] trans earning.cvs to sk file\n");
    printf("     cvs2sk 2  earning.cvs earning.month.sk earning.season.sk\n");
    printf("[3] continue ...\n");
    printf("==============================================\n");
    return true;
    
}



