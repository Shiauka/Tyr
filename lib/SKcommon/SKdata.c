
#include "SKcommon.h"
#include "SKdata.h"


#if 0
static void *SK_Realloc(void *ptr, size_t size)
{
    void *Ret = NULL;
    Ret = realloc(ptr,size);
    if (ptr == NULL && Ret != NULL)
        memset(Ret,'\0',size);
    return Ret;
}
#endif

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

SKData_ErrMSG SKData_FileOpen(const char *filename, SK_DATA_TYPE *type, void *data)
{
    FILE *finput = NULL;
    SKData_ErrMSG Retmsg= SKData_ErrMSG_Pass;
    SK_HEADER header;

    /*check input value*/
    if (filename == NULL || type == NULL  || data == NULL )
        return SKData_ErrMSG_InvalidInput;

    /*open file*/
    printf("%s\n",filename);
    finput = fopen(filename, "a");
    if (finput == NULL)
        return SKData_ErrMSG_Openfilefaild;
    
    /*read header*/
    header.type = SK_DATA_TYPE_NULL;
    if (fread((char *)&header, sizeof(SK_HEADER), 1, finput) !=0)
    {
        //printf("Read file [code : %d],[type : %d]\n",header.code,header.type);
    }
    else
    {
        Retmsg = SKData_ErrMSG_Pass_Newfile;
        goto FAILED;
    }
   
FAILED:
    
    if (finput != NULL)
        fclose(finput);

    return Retmsg;
}

SKData_ErrMSG SKData_FileClose(const char *filename, SK_DATA_TYPE type, void *data)
{
    SKData_ErrMSG Retmsg= SKData_ErrMSG_Pass;
    return Retmsg;
}

static bool _SKData_DataSort_Price(SK_PRICE *orgData,  SK_HEADER *orgHeader)
{
    //sort data
    int index_new = 0, index_org = 0;
    int day_new = 0, day_org = 0;
    SK_PRICE temp;
    for (index_new = 0; index_new < orgHeader->datacount; index_new++)
    {
        day_new = orgData[index_new].day + orgData[index_new].month * 31 + orgData[index_new].year * 380;
        for (index_org = 0; index_org < orgHeader->datacount; index_org++)
        {
            day_org = orgData[index_org].day + orgData[index_org].month * 31 + orgData[index_org].year * 380;
            if (day_new >= day_org)
            {
                memcpy(&temp, &orgData[index_new], orgHeader->unidatasize);
                memcpy(&orgData[index_new], &orgData[index_org], orgHeader->unidatasize);
                memcpy(&orgData[index_org], &temp, orgHeader->unidatasize);
                day_new = orgData[index_new].day + orgData[index_new].month * 31 + orgData[index_new].year * 380;
            }
        }
    }

    return true;
}

static bool _SKData_DataInsert_EarningSeason(SK_EARNING_SEASON *orgData,  SK_HEADER *orgHeader, SK_EARNING_SEASON *newData,  SK_HEADER *newHeader)
{
    int index_new = 0, index_org = 0;
    
    //check data
    for (index_new = 0; index_new < newHeader->datacount; index_new++)
    {
        for (index_org = 0; index_org < orgHeader->datacount; index_org++)
        {
            if ((orgData[index_org].year== newData[index_new].year) && 
                (orgData[index_org].season== newData[index_new].season))
            {
                break;
            }
        }

        //add data
        if (index_org >= orgHeader->datacount)
        {
            orgHeader->datalength += orgHeader->unidatasize;
            orgHeader->datacount++;
            //SK_Realloc((void *)orgData, orgHeader->datalength + orgHeader->unidatasize);
            memcpy(&orgData[index_org], &newData[index_new], orgHeader->unidatasize);
        }
    }
    
    return true;
}

static bool _SKData_DataInsert_EarningMonth(SK_EARNING_MONTH *orgData,  SK_HEADER *orgHeader, SK_EARNING_MONTH *newData,  SK_HEADER *newHeader)
{
    int index_new = 0, index_org = 0;
    
    //check data
    for (index_new = 0; index_new < newHeader->datacount; index_new++)
    {
        for (index_org = 0; index_org < orgHeader->datacount; index_org++)
        {
            if ((orgData[index_org].year== newData[index_new].year) && 
                (orgData[index_org].month== newData[index_new].month))
            {
                break;
            }
        }

        //add data
        if (index_org >= orgHeader->datacount)
        {
            orgHeader->datalength += orgHeader->unidatasize;
            orgHeader->datacount++;
            //SK_Realloc((void *)orgData, orgHeader->datalength + orgHeader->unidatasize);
            memcpy(&orgData[index_org], &newData[index_new], orgHeader->unidatasize);
        }
    }
    
    return true;
}

static bool _SKData_DataInsert_Dividend(SK_DIVIDEND *orgData,  SK_HEADER *orgHeader, SK_DIVIDEND *newData,  SK_HEADER *newHeader)
{
    int index_new = 0, index_org = 0;

    //check data
    for (index_new = 0; index_new < newHeader->datacount; index_new++)
    {
        for (index_org = 0; index_org < orgHeader->datacount; index_org++)
        {
            if (orgData[index_org].year== newData[index_new].year)
            {
                break;
            }
        }

        //add data
        if (index_org >= orgHeader->datacount)
        {
            orgHeader->datalength += orgHeader->unidatasize;
            orgHeader->datacount++;
            //SK_Realloc((void *)orgData, orgHeader->datalength + orgHeader->unidatasize);
            memcpy(&orgData[index_org], &newData[index_new], orgHeader->unidatasize);
        }
    }
    
    return true;
}

static bool _SKData_DataInsert_GIFingrade(SK_FINGRADE *orgData,  SK_HEADER *orgHeader, SK_FINGRADE *newData,  SK_HEADER *newHeader)
{
    int index_new = 0, index_org = 0;
    memcpy(&orgData[index_org], &newData[index_new], orgHeader->unidatasize);    
    return true;
}

static bool _SKData_DataInsert_Price(SK_PRICE *orgData,  SK_HEADER *orgHeader, SK_PRICE *newData,  SK_HEADER *newHeader)
{
/*
    printf("[org]datalength: %d \n",orgHeader->datalength);
    printf("[org]Data[0]->month: %d \n",orgData[0].month);
    printf("[org]Data[0]->day: %d \n",orgData[0].day);

    printf("[new]datalength: %d \n",newHeader->datalength);
    printf("[new]Data[0]->month: %d \n",newData[0].month);
    printf("[new]Data[0]->day: %d \n",newData[0].day);
*/
    int index_new = 0, index_org = 0;

    //check data
    for (index_new = 0; index_new < newHeader->datacount; index_new++)
    {
        for (index_org = 0; index_org < orgHeader->datacount; index_org++)
        {
            if ((orgData[index_org].day == newData[index_new].day) && 
            (orgData[index_org].month== newData[index_new].month) &&
            (orgData[index_org].year== newData[index_new].year))
            {
                break;
            }
        }

        //add data
        if (index_org >= orgHeader->datacount)
        {
            orgHeader->datalength += orgHeader->unidatasize;
            orgHeader->datacount++;
            //SK_Realloc((void *)orgData, orgHeader->datalength + orgHeader->unidatasize);
            memcpy(&orgData[index_org], &newData[index_new], orgHeader->unidatasize);
        }
    }
    
    return true;
}

SKData_ErrMSG SKData_DataSort(const char *filename)
{
    SKData_ErrMSG Retmsg= SKData_ErrMSG_Pass;
    FILE *ffile = NULL;
    SK_HEADER fheader = {0};
     void *skdata = NULL;
     char * pstr = NULL;
     int index = 0;
     
    /*check input value*/
    if (filename == NULL)
        return SKData_ErrMSG_InvalidInput;

    /*open input file*/
    ffile = fopen(filename, "r");
    if (ffile == NULL)
    {
        Retmsg = SKData_ErrMSG_Openfilefaild;
        goto FAILED;
    }
    else
    {
        /*read file header and data*/
        if (fread((char *)&fheader, sizeof(SK_HEADER), 1, ffile) !=0)
        {
            skdata = SK_Malloc(fheader.datalength+1);
            if ((skdata !=NULL) && (fread((char *)skdata, fheader.datalength, 1, ffile) !=0) )
            {
                _SKData_DataSort_Price((SK_PRICE *)skdata, &fheader);
            }
        }
        else
        {
            Retmsg = SKData_ErrMSG_InvalidHeader;
            goto FAILED;
        }
        fclose(ffile);
        ffile = NULL;
    }

    /*open output file*/
    ffile = fopen(filename, "w");
    if (ffile == NULL)
    {
        Retmsg = SKData_ErrMSG_Openfilefaild;
        goto FAILED;
    }
    
    pstr = (char *)&fheader;
    for (index = 0; index < sizeof(SK_HEADER); index++)
    {
        fprintf(ffile,"%c", pstr[index]);
    }

    //printf("[save]datalength = %d\n",  fheader.datalength);
    
    pstr = (char *)skdata;
    for (index = 0; index < fheader.datalength; index++)
    {
        fprintf(ffile,"%c", pstr[index]);
    }
    fclose(ffile);
    ffile = NULL;
    
FAILED:
    if (ffile != NULL)
        fclose(ffile);

    if (skdata != NULL)
        SK_Free(skdata);

    return Retmsg;
}

SKData_ErrMSG SKData_DataInsert(const char *filename, SK_HEADER *insertheader, void *insertData)
{
    SKData_ErrMSG Retmsg= SKData_ErrMSG_Pass;
    FILE *ffile = NULL;
    SK_HEADER fheader = {0};
    char *pstr = NULL;
    unsigned int index = 0;
    void *skdata = NULL;

    /*check input value*/
    if (filename == NULL || insertheader == NULL  || insertData == NULL )
    {
        Retmsg = SKData_ErrMSG_InvalidInput;
        goto FAILED;
    }

    /*open input file*/
    ffile = fopen(filename, "r");
    if (ffile == NULL)
    {
        Retmsg = SKData_ErrMSG_Pass_Newfile;
    }
    else
    {
        /*read file header and data*/
        if (fread((char *)&fheader, sizeof(SK_HEADER), 1, ffile) !=0)
        {
             if (insertheader->type != fheader.type)
             {
                Retmsg = SKData_ErrMSG_InvalidType;
                goto FAILED;
             }
        
            //printf("Read file [code : %d],[type : %d]\n",fheader.code,fheader.type);
            //printf("datacount: %d\n",fheader.datacount);
            //printf("datalength: %d\n",fheader.datalength);
            //printf("unidatasize: %d\n",fheader.unidatasize); 
            skdata = SK_Malloc(fheader.datalength + insertheader->datalength + 1);
            if ((skdata !=NULL) && (fread((char *)skdata, fheader.datalength, 1, ffile) !=0) )
            {
                ;
            }
        }
        else
        {
            Retmsg = SKData_ErrMSG_Pass_Newfile;
        }
        fclose(ffile);
        ffile = NULL;
    }

    /*open output file*/
    ffile = fopen(filename, "w");
    if (ffile == NULL)
    {
        Retmsg = SKData_ErrMSG_Openfilefaild;
        goto FAILED;
    }
    
    /*write file*/
    if (Retmsg == SKData_ErrMSG_Pass_Newfile)
    {
        pstr = (char *)insertheader;
        for (index = 0; index < sizeof(SK_HEADER); index++)
        {
            fprintf(ffile,"%c", pstr[index]);
        }
        
        pstr = (char *)insertData;
        for (index = 0; index < insertheader->datalength; index++)
        {
            fprintf(ffile,"%c", pstr[index]);
        }
    }
    else
    {
        /*old file*/
        switch (fheader.type)
        {
            case SK_DATA_TYPE_PRICE:
                _SKData_DataInsert_Price((SK_PRICE *)skdata, &fheader, (SK_PRICE *)insertData, insertheader);
                break;
                
            case SK_DATA_TYPE_DIVIDEND:
                _SKData_DataInsert_Dividend((SK_DIVIDEND *)skdata, &fheader, (SK_DIVIDEND *)insertData, insertheader);
                break;

            case SK_DATA_TYPE_EARNING_MONTH:
                _SKData_DataInsert_EarningMonth((SK_EARNING_MONTH *)skdata, &fheader, (SK_EARNING_MONTH *)insertData, insertheader);
                break;
                
            case SK_DATA_TYPE_EARNING_SEASON:
                _SKData_DataInsert_EarningSeason((SK_EARNING_SEASON *)skdata, &fheader, (SK_EARNING_SEASON *)insertData, insertheader);
                break;

            case SK_DATA_TYPE_GOODINFO_FINGRADE:
                _SKData_DataInsert_GIFingrade((SK_FINGRADE *)skdata, &fheader, (SK_FINGRADE *)insertData, insertheader);
                break;
                
            default:
                Retmsg = SKData_ErrMSG_InvalidType;
                goto FAILED;
                break;
        }
        pstr = (char *)&fheader;
        for (index = 0; index < sizeof(SK_HEADER); index++)
        {
            fprintf(ffile,"%c", pstr[index]);
        }

        //printf("[save]datalength = %d\n",  fheader.datalength);
        
        pstr = (char *)skdata;
        for (index = 0; index < fheader.datalength; index++)
        {
            fprintf(ffile,"%c", pstr[index]);
        }
    }
    fclose(ffile);
    ffile = NULL;
    
FAILED:
    if (ffile != NULL)
        fclose(ffile);

    if (skdata != NULL)
        SK_Free(skdata);

    return Retmsg;
}


