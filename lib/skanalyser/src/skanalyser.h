

#ifndef SKANALYSER_H
#define SKANALYSER_H


typedef enum
{
    SKANALYSER_TYPE_DUMP = 0,
    SKANALYSER_TYPE_ANALYSIS,
    SKANALYSER_TYPE_MAX = 0xFFFF,
}SKANALYSER_TYPE;


bool SKApi_SKANALYSER_Dump(const char *Inputfile);
bool SKApi_SKANALYSER_Help(void);


#endif

