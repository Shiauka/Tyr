
#include "SKcommon.h"
#include "skanalyser.h"


static bool checkparanum(int cmd, int arg)
{
    bool bRet = false;
    switch (cmd)
    {   
        case SKANALYSER_TYPE_DUMP: 
            if (arg == 3)
                bRet = true;
            break;

        case SKANALYSER_TYPE_ANALYSIS:
            if (arg == 4)
                bRet = true;
            break;
            
        default:
            break;
    }

    if (!bRet)
    {
        printf("You use the wrong cmd, please follow below list\n");
        SKApi_SKANALYSER_Help();
    }
    return bRet;
}
    

int main(int argc, char *argv[])
{
    bool bRet = false;
    int _intcommand = SKANALYSER_TYPE_MAX;

    /*assinged cmd*/
    if (argv[1] !=NULL)
        _intcommand = atoi(argv[1]);

    /*check input para with diff cmd*/
    if (!checkparanum(_intcommand,argc))
        return 0;

    /*start function*/
    switch (_intcommand)
    {   
        case SKANALYSER_TYPE_DUMP: 
            bRet = SKApi_SKANALYSER_Dump(argv[2]);
            break;

        case SKANALYSER_TYPE_ANALYSIS:
            printf("to be continue\n");
            break;
            
        default:
            break;
    }

    if (!bRet)
    {
        printf("sk analysis function error\n");
        return 0;
    }
    
    return 1;
}
