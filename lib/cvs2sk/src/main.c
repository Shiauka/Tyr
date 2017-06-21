
#include "SKcommon.h"
#include "cvs2sk.h"

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        SKApi_CVS2SK_Help();
        return 0;
    }

    bool bRet = false;
    int _intcommand = atoi(argv[1]);

    //check input value
    if (_intcommand == CVS2SK_EARNING && argc < 6)
    {
        SKApi_CVS2SK_Help();
        return 0;
    }

    switch (_intcommand)
    {   
        case CVS2SK_PRICE: 
            bRet = SKApi_CVS2SK_Price(argv[2], argv[3]);
            break;
            
        case CVS2SK_DIVIDEND:
            bRet = SKApi_CVS2SK_Dividend(atoi(argv[2]), argv[3], argv[4]);
            break;
            
        case CVS2SK_EARNING:
            bRet = SKApi_CVS2SK_Earning(atoi(argv[2]), argv[3], argv[4], argv[5]);
            break;

        case CVS2SK_GOODINFO_FINGRADE:
            bRet = SKApi_CVS2SK_GInfoFingrade(atoi(argv[2]), argv[3], argv[4]);
            break;
            
        case CVS2SK_FINANCIALREPORT_S:
            bRet = SKApi_CVS2SK_FinancialReport(argv[2],argv[3]);
            break;

        case CVS2SK_MOPSTWSE_FINREPORT:
            bRet = SKApi_CVS2SK_MopstwseFinReport(atoi(argv[2]), argv[3], argv[4]);
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
