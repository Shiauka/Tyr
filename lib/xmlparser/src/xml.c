#include "SKcommon.h"
#include "xml.h"
#include "expat.h"
#include "string.h"

#define BUFFSIZE 32768
static char _XMLBuffer[BUFFSIZE];
static int firstyear =0;
static int secondyear = 0;

static bool Skip_specific_string(char* input, unsigned int inputlen, char *specificstr, unsigned int specificlen)
{
    char *orgStr = NULL;
    char *specificStr = NULL;
    char tempStr[512] = {0};
    unsigned int tempLen = 0;
    unsigned int Len = 0;

    if (input ==NULL ||specificstr ==NULL)
    {
        return false;
    }

    orgStr = input;
    specificStr = strstr(input, specificstr);
    while(specificStr != NULL)
    {
        //printf("find nbsp\n");
        tempLen = specificStr - orgStr;
        memcpy(&tempStr[Len], orgStr, tempLen);
        Len += tempLen;
        orgStr = specificStr + specificlen;
        specificStr = NULL;
        specificStr = strstr(orgStr, specificstr);
    }
    
    memcpy(&tempStr[Len], orgStr, strlen(orgStr));
    Len += strlen(orgStr);
    
    if (Len >= 0)
    {
        memset(input, '\0', inputlen);
        memcpy(input, tempStr, Len);
    }
    
    return true;
}

static void _Printinfo_element_handle_start(void *userData, const char *name, const char **attr)
{
	int i;
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;

	for (i = 0; i < userptr->depth; i++)
	{
		printf("  ");
	}
	printf("%s",name);

	for (i = 0; attr[i]; i+=2)
	{
		printf(" %s='%s'", attr[i], attr[i+1]);
	}
	
	printf("\n");

	userptr->depth +=1;
}

static void _Printinfo_element_handle_end(void *userData, const char *name)
{
	int i = 0;
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
	
	userptr->depth -=1;	

	for (i = 0; i < userptr->depth; i++)
	{
		printf("  ");
	}
	printf("%s\n",name);
}

static void _Printinfo_char_handle(void *userData, const XML_Char *s, int len)
{
	int i = 0;
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
	char temp[80];
	
	memset(temp,'\0',80);

	if (len <= 1)
	{
		/*do nothing with length is 1*/
		return;
	}

	for (i = 0; i < userptr->depth-1; i++)
    {
        printf("  ");
    }
	
	if(len < 80)
	{
		memcpy(temp,s,len);
	}
	else
	{
		memcpy(temp,s,80);
	}

	printf("%s\n",temp);
}

bool SKApi_XML_Printinfo(FILE *xmlfile)
{
	int done = 0;
    XML_USER_DATA userdata;
	int len = 0;
	XML_Parser parser = XML_ParserCreate(NULL);

	if (!parser)
	{
		printf("Couldn't allocate memory for parser \n");
		return false;
	}

    userdata.depth = 0;
	XML_SetUserData(parser, &userdata);
	XML_SetElementHandler(parser, _Printinfo_element_handle_start, _Printinfo_element_handle_end);
	XML_SetCharacterDataHandler(parser,_Printinfo_char_handle);

	do
	{
		len = (int)fread(_XMLBuffer, 1, sizeof(_XMLBuffer), xmlfile);
		if (ferror(xmlfile))
		{
			printf("Read xmlfile error\n");
			return false;
		}		
		
		done = feof(xmlfile);
		
		if(XML_Parse(parser, _XMLBuffer, len, done) == XML_STATUS_ERROR)
		{
			printf("Parse error at line %ld:\t%s\n", 
					XML_GetCurrentLineNumber(parser),
					XML_ErrorString(XML_GetErrorCode(parser)));
			return false;
		}	
	}while(!done);
	
	XML_ParserFree(parser);
    return true;
}


static void _XSC_Yahoo_Earning_element_handle_start(void *userData, const char *name, const char **attr)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    int i = 0;

    userptr->depth +=1;
    

    if (strcmp("td",name)==0)
    {
        for (i = 0; attr[i]; i+=2)
        {
            if (strcmp("class",attr[i])==0 && strcmp("ttt",attr[i+1])==0)
            {
                userptr->find = true;
                userptr->findcount +=1;
                break;
            }
        }
    }
    else if (strcmp("b",name)==0)
    {
        /*not change "userptr->find"  statuswhen specific name b */
        ;
    }
    else
    {
        userptr->find = false;
    }
}

static void _XSC_Yahoo_Earning_element_handle_end(void *userData, const char *name)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;

    userptr->depth -=1;
    userptr->find = false;
}

static void _XSC_Yahoo_Earning_char_handle(void *userData, const XML_Char *s, int len)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    char temp[128];

    if ( len <= 1 && (s[0] == 0xA))
    {
        /*do nothing with dummy data from xml parsing bug*/
        return;
    }

    if (userptr->find)
    {
        memset(temp,'\0',128);
        if(len < 128)
        {
            memcpy(temp,s,len);
        }
        else
        {
            memcpy(temp,s,128);
        }

        //printf("count = %d \n",userptr->findcount);

        switch  (userptr->findcount)
        {
            case 1:
            case 121:
            case 159:   
                fprintf(userptr->outfile,"%s\n",temp);
                break;
                
            case 2:
            case 122:
            case 160:   
                firstyear = atoi(temp);
                break;
                
            case 3:
            case 123:
            case 161:    
                secondyear = atoi(temp);
                break;
            default:
                break;
        }

        if (userptr->findcount > 3 && userptr->findcount <= 12) //9+3
        {
            fprintf(userptr->outfile,"\"%s\",",temp);
            if (userptr->findcount % 9 == 3)
            {
                fprintf(userptr->outfile,"\n");
            }
        }
        else if (userptr->findcount > 12 && userptr->findcount <= 120) //13*9+3
        {
            if (userptr->findcount % 9 == 7)
            {
                fprintf(userptr->outfile,"\"%03d%02d\",", secondyear, atoi(temp));
            }
            else if (userptr->findcount % 9 == 4)
            {
                fprintf(userptr->outfile,"\"%03d%02d\",", firstyear, atoi(temp));
            }
            else
            {
                fprintf(userptr->outfile,"\"%s\",",temp);
                if (userptr->findcount % 9 == 3)
                {
                    fprintf(userptr->outfile,"\n");
                }
            }
        }
        else if(userptr->findcount > 123 && userptr->findcount <= 130) //7+123
        {
            fprintf(userptr->outfile,"\"%s\",",temp);
            if (userptr->findcount % 7 == 4)
            {
                fprintf(userptr->outfile,"\n");
            }
        }
        else if(userptr->findcount > 130 && userptr->findcount <= 158) //5*7+123
        {
            if (userptr->findcount % 7 == 1)
            {
                fprintf(userptr->outfile,"\"%03d%02d\",", secondyear, atoi(temp));
            }
            else if (userptr->findcount % 7 == 5)
            {
                fprintf(userptr->outfile,"\"%03d%02d\",", firstyear, atoi(temp));
            }
            else
            {
                fprintf(userptr->outfile,"\"%s\",",temp);
                if (userptr->findcount % 7 == 4)
                {
                    fprintf(userptr->outfile,"\n");
                }
            }
        }
        else if(userptr->findcount > 161 && userptr->findcount <= 168) //7+161
        {
            fprintf(userptr->outfile,"\"%s\",",temp);
            if (userptr->findcount % 7 == 0)
            {
                fprintf(userptr->outfile,"\n");
            }
        }
        else if(userptr->findcount > 168 && userptr->findcount <= 196) //5*7+161
        {
            if (userptr->findcount % 7 == 4)
            {
                fprintf(userptr->outfile,"\"%03d%02d\",", secondyear, atoi(temp));
            }
            else if (userptr->findcount % 7 == 1)
            {
                fprintf(userptr->outfile,"\"%03d%02d\",", firstyear, atoi(temp));
            }
            else
            {
                fprintf(userptr->outfile,"\"%s\",",temp);
                if (userptr->findcount % 7 == 0)
                {
                    fprintf(userptr->outfile,"\n");
                }
            }
        }

    }
}
static void _XSC_Yahoo_Dividend_element_handle_start(void *userData, const char *name, const char **attr)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    int i = 0;

    userptr->depth +=1;
    userptr->find = false;

    if (strcmp("td",name)==0)
    {
        for (i = 0; attr[i]; i+=2)
        {
            if (userptr->findcount > 0 || (strcmp("class",attr[i])==0 && strcmp("ttt",attr[i+1])==0))
            {
                userptr->find = true;
                userptr->findcount +=1;
                break;
            }
        }
    }
}

static void _XSC_Yahoo_Dividend_element_handle_end(void *userData, const char *name)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;

    userptr->depth -=1;
    userptr->find = false;
}

static void _XSC_Yahoo_Dividend_char_handle(void *userData, const XML_Char *s, int len)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    char temp[128];

    if ( len <= 1 && (s[0] == 0xA))
    {
        /*do nothing with dummy data from xml parsing bug*/
        return;
    }

    if (userptr->find)
    {
        memset(temp,'\0',128);
        if(len < 128)
        {
            memcpy(temp,s,len);
        }
        else
        {
            memcpy(temp,s,128);
        }

        //printf("count = %d \n",userptr->findcount);

        if (userptr->findcount == 1) //title 1
        {
            fprintf(userptr->outfile,"%s ",temp);
        }
        else if (userptr->findcount == 2) //title 2
        {
            fprintf(userptr->outfile,"%s\n",temp);
        }
        else if (userptr->findcount > 2) // ?*6 + 2
        {
            fprintf(userptr->outfile,"\"%s\",",temp);
            if (userptr->findcount % 6 == 2)
            {
                fprintf(userptr->outfile,"\n");
            }            
        }

    }
}

static void _XSC_Yahoo_Company_element_handle_start(void *userData, const char *name, const char **attr)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    int i = 0;

    userptr->depth +=1;
    userptr->find = false;

    if (strcmp("td",name)==0)
    {
        for (i = 0; attr[i]; i+=2)
        {
            //if (strcmp("class",attr[i])==0 && strcmp("ttt",attr[i+1])==0)
            {
                userptr->find = true;
                userptr->findcount +=1;
                break;
            }
        }
    }
}

static void _XSC_Yahoo_Company_element_handle_end(void *userData, const char *name)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;

    userptr->depth -=1;
    userptr->find = false;
}

static void _XSC_Yahoo_Company_char_handle(void *userData, const XML_Char *s, int len)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    char temp[128];

    if ( len <= 1 && (s[0] == 0xA))
    {
        /*do nothing with dummy data from xml parsing bug*/
        return;
    }

    if (userptr->find)
    {
        memset(temp,'\0',128);
        if(len < 128)
        {
            memcpy(temp,s,len);
        }
        else
        {
            memcpy(temp,s,128);
        }

        printf("count = %d \n",userptr->findcount);
        fprintf(userptr->outfile,"%s\n",temp);
#if 0
        if (userptr->findcount == 1 || userptr->findcount == 121 || userptr->findcount == 159) //title
        {
            fprintf(userptr->outfile,"%s\n",temp);
        }
        else if (userptr->findcount > 3 && userptr->findcount <= 120) //13*9+3
        {
            fprintf(userptr->outfile,"\"%s\",",temp);
            if (userptr->findcount % 9 == 3)
            {
                fprintf(userptr->outfile,"\n");
            }            
        }
        else if(userptr->findcount > 123 && userptr->findcount <= 158) //5*7+123
        {
            fprintf(userptr->outfile,"\"%s\",",temp);
            if (userptr->findcount % 7 == 4)
            {
                fprintf(userptr->outfile,"\n");
            }
        }
        else if(userptr->findcount > 161 && userptr->findcount <= 196) //5*7+161
        {
            fprintf(userptr->outfile,"\"%s\",",temp);
            if (userptr->findcount % 7 == 0)
            {
                fprintf(userptr->outfile,"\n");
            }
        }
#endif

    }
}

static void _XSC_Goodinfo_Fingrade_element_handle_start(void *userData, const char *name, const char **attr)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    int i =0;
    
    userptr->depth +=1;

    if (strcmp("tr",name)==0)
    {
        for (i = 0; attr[i]; i+=2)
        {
            if (strcmp("onmouseout",attr[i])==0 && strcmp("this.style.background=",attr[i+1])==0)
            {
                userptr->find = true;
                userptr->findcount  = 0;
                break;
             }
        }
    }

}

static void _XSC_Goodinfo_Fingrade_element_handle_end(void *userData, const char *name)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;

    userptr->depth -=1;
    if (strcmp("tr",name)==0)
    {
        userptr->find = false;
    }
}

static void _XSC_Goodinfo_Fingrade_char_handle(void *userData, const XML_Char *s, int len)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    char temp[512]= {0};
    
    if ( len <= 1 && (s[0] == 0xA))
    {
        /*do nothing with dummy data from xml parsing bug*/
        return;
    }

    if (userptr->find)
    {
        memcpy(temp,s,len);
        if (userptr->findcount == 1)
        {
            fprintf(userptr->outfile, " %s ,",temp);
        }
        else if(userptr->findcount == 2)
        {
            fprintf(userptr->outfile, " %s ,\n",temp);
        }
        userptr->findcount++;
    }
}

static void _XSC_Mopstese_FinReport_element_handle_start(void *userData, const char *name, const char **attr)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    //int i =0;
    
    userptr->depth +=1;

    if (strcmp("center",name)==0)
    {
        userptr->find = true;
        /*
        for (i = 0; attr[i]; i+=2)
        {
            if (strcmp("onmouseout",attr[i])==0 && strcmp("this.style.background=",attr[i+1])==0)
            {
                userptr->find = true;
                userptr->findcount  = 0;
                break;
             }
        }
        */
    }

}

static void _XSC_Mopstese_FinReport_element_handle_end(void *userData, const char *name)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;

    userptr->depth -=1;
    if (strcmp("center",name)==0)
    {
        userptr->find = false;
    }
}

static void _XSC_Mopstese_FinReport_char_handle(void *userData, const XML_Char *s, int len)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    char temp[512]= {0};
    //char *pstr = NULL;
    char nbsp[10]= {0x6e,0x62,0x73,0x70,0x3b,'\0'};
    char year[10] = {0xe5,0xb9,0xb4,0xe5,0xba,0xa6, '\0'};
    
    if ( len <= 1 && (s[0] == 0xA))
    {
        /*do nothing with dummy data from xml parsing bug*/
        return;
    }

    if (userptr->find)
    {
        memcpy(temp,s,len);
        Skip_specific_string(temp, strlen(temp), nbsp, strlen(nbsp));
        if (strlen(temp) == 0)
            sprintf(temp, "-");
        
        if (userptr->findcount >= 1  &&  userptr->findcount <= 3) 
        {        
            //year
            Skip_specific_string(temp, strlen(temp), year, strlen(year));
            if (userptr->findcount == 1)
                fprintf(userptr->outfile,"%s,\"%s\",",year, temp);
            else if (userptr->findcount == 3)
                fprintf(userptr->outfile,"\"%s\",\n",temp);
            else
                fprintf(userptr->outfile,"\"%s\",",temp); 
        }
        else if (userptr->findcount >= 6  &&  userptr->findcount <= 13) 
        {
             if (userptr->findcount == 9 || userptr->findcount == 13)
                fprintf(userptr->outfile,"\"%s\",\n",temp);
            else
                fprintf(userptr->outfile,"\"%s\",",temp); 
        }
        else if (userptr->findcount >= 16  &&  userptr->findcount <= 27) 
        {
             if (userptr->findcount == 19 ||userptr->findcount == 23 || userptr->findcount == 27)
                fprintf(userptr->outfile,"\"%s\",\n",temp);
            else
                fprintf(userptr->outfile,"\"%s\",",temp); 
        }
        else if (userptr->findcount >= 32  &&  userptr->findcount <= 55) 
        {
             if (userptr->findcount == 35 ||userptr->findcount == 39 || userptr->findcount == 43 || 
                userptr->findcount == 47 ||userptr->findcount == 51 || userptr->findcount == 55)
                fprintf(userptr->outfile,"\"%s\",\n",temp);
            else
                fprintf(userptr->outfile,"\"%s\",",temp); 
        }
        else if (userptr->findcount >= 60  &&  userptr->findcount <= 79) 
        {
             if (userptr->findcount == 63 ||userptr->findcount == 67 || userptr->findcount == 71 || 
                userptr->findcount == 75 ||userptr->findcount == 79 )
                fprintf(userptr->outfile,"\"%s\",\n",temp);
            else
                fprintf(userptr->outfile,"\"%s\",",temp); 
        }
        else if (userptr->findcount >= 82  &&  userptr->findcount <= 93) 
        {
             if (userptr->findcount == 85 ||userptr->findcount == 89 || userptr->findcount == 93)
                fprintf(userptr->outfile,"\"%s\",\n",temp);
            else
                fprintf(userptr->outfile,"\"%s\",",temp); 
        }
        userptr->findcount++;
    }
}


bool SKApi_XML_Parsingspecificcase(FILE *xmlfile, FILE *outfile, XML_SPECIFIC_CASE XSC)
{    
    int done = 0;
    XML_USER_DATA userdata;
    int len = 0;
    XML_Parser parser = XML_ParserCreate(NULL);

    if (!parser)
    {
        printf("Couldn't allocate memory for parser \n");
        return false;
    }
    
    userdata.depth = 0;
    userdata.outfile = outfile;
    userdata.find = false;
    userdata.findcount = 0;
    XML_SetUserData(parser, &userdata);


    switch(XSC)
    {
        case XSC_YAHOO_EARNING:
            XML_SetElementHandler(parser, _XSC_Yahoo_Earning_element_handle_start, _XSC_Yahoo_Earning_element_handle_end);
            XML_SetCharacterDataHandler(parser,_XSC_Yahoo_Earning_char_handle);
            break;
        case XSC_YAHOO_DIVIDEND:
            XML_SetElementHandler(parser, _XSC_Yahoo_Dividend_element_handle_start, _XSC_Yahoo_Dividend_element_handle_end);
            XML_SetCharacterDataHandler(parser,_XSC_Yahoo_Dividend_char_handle);
            break;
        case XSC_YAHOO_COMPANY:
            XML_SetElementHandler(parser, _XSC_Yahoo_Company_element_handle_start, _XSC_Yahoo_Company_element_handle_end);
            XML_SetCharacterDataHandler(parser,_XSC_Yahoo_Company_char_handle);
            break;
        case XSC_GOODINFO_FINGRADE:
            XML_SetElementHandler(parser, _XSC_Goodinfo_Fingrade_element_handle_start, _XSC_Goodinfo_Fingrade_element_handle_end);
            XML_SetCharacterDataHandler(parser,_XSC_Goodinfo_Fingrade_char_handle);
            break;
        case XSC_MOPSTWSE_FINREPORT:
            XML_SetElementHandler(parser, _XSC_Mopstese_FinReport_element_handle_start, _XSC_Mopstese_FinReport_element_handle_end);
            XML_SetCharacterDataHandler(parser,_XSC_Mopstese_FinReport_char_handle);
            break;
        default:
            goto FAILED_CASE;
    }

    do
    {
        len = (int)fread(_XMLBuffer, 1, sizeof(_XMLBuffer), xmlfile);
        if (ferror(xmlfile))
        {
            printf("Read xmlfile error\n");
            goto FAILED_CASE;
        }		

        done = feof(xmlfile);

        if(XML_Parse(parser, _XMLBuffer, len, done) == XML_STATUS_ERROR)
        {
            printf("Parse error at line %ld:\t%s\n", 
            XML_GetCurrentLineNumber(parser),
            XML_ErrorString(XML_GetErrorCode(parser)));
            goto FAILED_CASE;
        }	
    }while(!done);
	
    XML_ParserFree(parser);

    return true;

FAILED_CASE:
    XML_ParserFree(parser);
    return false;
}


bool SKApi_XML_Help(void)
{
    printf("=============XML Commnd List==================\n");
    printf("[1] print xml information\n");
    printf("     xmlparser 1 example/example1.xml\n");
    printf("[2] parsing yahoo EARNING page and transfer data to out file\n");
    printf("     xmlparser 2 EARNING.xml EARNING.data\n");
    printf("[3] parsing yahoo DIVIDEND page and transfer data to out file\n");
    printf("     xmlparser 3 DIVIDEND.xml DIVIDEND.data\n");
    printf("[4] parsing yahoo COMPANY page and transfer data to out file\n");
    printf("     xmlparser 4 COMPANY.xml COMPANY.data\n");
    printf("[5] continue ...\n");
    printf("==============================================\n");
    return true;
}
