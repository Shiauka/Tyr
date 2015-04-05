
#include "SKcommon.h"
#include "cvs2sk.h"
#include "string.h"

bool SKApi_CVS2SK_Dividend(const int Code, const char *Inputfile, const char *Outputfile)
{

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
                    continue;
            }
        
            pstr = strtok(NULL," \t\n,\"");
            index++;
        }
        num++;
        length+= sizeof(SK_DIVIDEND);
    }
        
    //dump SK_HEADER and SK_DIVIDEND to output file
    header = malloc(sizeof(SK_HEADER));
    header->code = Code;
    header->datacount = num;
    header->unidatasize = sizeof(SK_DIVIDEND);
    header->datalength = length;
    
    pstr = (char *)header;
    for (index = 0; index < sizeof(SK_HEADER); index++)
    {
        fprintf(pfoutputfile,"%c", pstr[index]);
    }
    
    pstr = (char *)dividend;
    for (index = 0; index < length; index++)
    {
        fprintf(pfoutputfile,"%c", pstr[index]);
    }

    if (pfinputfile !=NULL) fclose(pfinputfile); 
    if (pfoutputfile !=NULL) fclose(pfoutputfile);
    if (dividend !=NULL) free(dividend);
    if (header !=NULL) free(header);
    return true;

FAILED:
    if (pfinputfile !=NULL) fclose(pfinputfile); 
    if (pfoutputfile !=NULL) fclose(pfoutputfile);
    if (dividend !=NULL) free(dividend);
    if (header !=NULL) free(header);
    return false;
    
}

bool SKApi_CVS2SK_Help(void)
{
    printf("cvs2sk [Command] [Code] [CVS file/CVS files List] [SK file]\n\n");

    printf("=============CVS2SK Commnd List==================\n");
    printf("[0] trans price.cvs to sk file\n");
    printf("     cvs2sk 0 price.filelist price.sk\n");
    printf("[1] trans dividend.cvs to sk file\n");
    printf("     cvs2sk 1 dividend.cvs dividend.sk\n");
    printf("[2] trans earning.cvs to sk file\n");
    printf("     cvs2sk 2  earning.cvs earning.sk\n");
    printf("[3] continue ...\n");
    printf("==============================================\n");
    return true;
    
}



