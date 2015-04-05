
#include "SKcommon.h"
#include "cvs2sk.h"

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        SKApi_CVS2SK_Help();
        return 0;
    }

    bool bRet = false;
    int _intcommand = atoi(argv[1]);

    switch (_intcommand)
    {   
        case CVS2SK_PRICE: 
            break;
            
        case CVS2SK_DIVIDEND:
            bRet = SKApi_CVS2SK_Dividend(atoi(argv[2]), argv[3], argv[4]);
            break;
            
        case CVS2SK_EARNING:
            break;
            
        default:
            break;
    }

    if (!bRet)
    {
        printf("Error CVS2SK\n");
        return 0;
    }
    
    return 1;
}
