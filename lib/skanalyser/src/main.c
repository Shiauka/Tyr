
#include "SKcommon.h"
#include "skanalyser.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        SKApi_SKANALYSER_Help();
        return 0;
    }

    bool bRet = false;
    int _intcommand = atoi(argv[1]);


    switch (_intcommand)
    {   
        case SKANALYSER_TYPE_DUMP: 
            bRet = SKApi_SKANALYSER_Dump(argv[2]);
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
