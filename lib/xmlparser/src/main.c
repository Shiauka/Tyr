
#include "SKcommon.h"
#include "xml.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("xmlparser [Command] [XML file] [OUT file]\n");
        SKApi_XML_Help();
        return 0;
    }

    int _intXMLcommand = atoi(argv[1]);

    if (argc != 4 && _intXMLcommand != 1)
    {
        printf("xmlparser [Command] [XML file] [OUT file]\n");
        SKApi_XML_Help();
        return 0;
    }

    FILE *_pfXMLinputfile = NULL;
    FILE *_pfXMLoutputfile = NULL;
    bool _bXMLret = false;
    _pfXMLinputfile = fopen(argv[2],"r");
    if (_pfXMLinputfile == NULL)
    {
        printf("open file error: %s\n",argv[2]);
        return 0;
    }

    if (_intXMLcommand != 1)
    {
        _pfXMLoutputfile = fopen(argv[3],"w");
        if (_pfXMLoutputfile == NULL)
        {
            printf("open file error: %s\n",argv[3]);
            return 0;
        }
    }

    //printf("parsing file : %s \n",argv[2]);

    switch (_intXMLcommand)
    {
        case 4:
            _bXMLret = SKApi_XML_Parsingspecificcase(_pfXMLinputfile, _pfXMLoutputfile, XSC_YAHOO_COMPANY);
            break;
        case 3:
            _bXMLret = SKApi_XML_Parsingspecificcase(_pfXMLinputfile, _pfXMLoutputfile, XSC_YAHOO_DIVIDEND);
            break;
        case 2:
            _bXMLret = SKApi_XML_Parsingspecificcase(_pfXMLinputfile, _pfXMLoutputfile, XSC_YAHOO_EARNING);
            break;     
        case 1: /*print XML file info*/
        default:
            _bXMLret = SKApi_XML_Printinfo(_pfXMLinputfile);
            break;
    }	

    fclose(_pfXMLinputfile);
    if (_intXMLcommand != 1)
    {
        fclose(_pfXMLoutputfile);
    }


    if (!_bXMLret)
    {
        printf("failed\n");
    }

    return 1;
}
