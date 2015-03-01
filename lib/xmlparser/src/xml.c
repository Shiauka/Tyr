#include "SKcommon.h"
#include "xml.h"
#include "expat.h"
#include "string.h"

#define BUFFSIZE 8192
static char _XMLBuffer[BUFFSIZE];


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

static void _XSC_YB_element_handle_start(void *userData, const char *name, const char **attr)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;
    int i = 0;

    userptr->depth +=1;
    userptr->find = false;

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
}

static void _XSC_YB_element_handle_end(void *userData, const char *name)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;

    userptr->depth -=1;
    userptr->find = false;
}

static void _XSC_YB_char_handle(void *userData, const XML_Char *s, int len)
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

    }
}

static void _XSC_YI_element_handle_start(void *userData, const char *name, const char **attr)
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

static void _XSC_YI_element_handle_end(void *userData, const char *name)
{
    XML_USER_DATA *userptr = (XML_USER_DATA *)userData;

    userptr->depth -=1;
    userptr->find = false;
}

static void _XSC_YI_char_handle(void *userData, const XML_Char *s, int len)
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
        case XSC_YAHOO_BONUS:
            XML_SetElementHandler(parser, _XSC_YB_element_handle_start, _XSC_YB_element_handle_end);
            XML_SetCharacterDataHandler(parser,_XSC_YB_char_handle);
            break;
        case XSC_YAHOO_INCOME:
            XML_SetElementHandler(parser, _XSC_YI_element_handle_start, _XSC_YI_element_handle_end);
            XML_SetCharacterDataHandler(parser,_XSC_YI_char_handle);
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
    printf("[2] parsing yahoo bonus page and transfer data to out file\n");
    printf("     xmlparser 2 bonus.xml bonus.cvs\n");
    printf("[3] parsing yahoo income page and transfer data to out file\n");
    printf("     xmlparser 3 income.xml income.cvs\n");
    printf("[4] continue ...\n");
	printf("==============================================\n");

	return true;
}
