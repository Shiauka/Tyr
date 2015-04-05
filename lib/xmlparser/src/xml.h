
#ifndef XML_H
#define XML_H

typedef enum
{
    XSC_YAHOO_DIVIDEND,
    XSC_YAHOO_EARNING,
    XSC_YAHOO_COMPANY,
    XSC_MAX
}XML_SPECIFIC_CASE;

typedef struct
{
    int depth;
    FILE *outfile;
    int find;
    int findcount;
}XML_USER_DATA;


bool SKApi_XML_Parsingspecificcase(FILE *infile, FILE *outfile, XML_SPECIFIC_CASE XSC);
bool SKApi_XML_Printinfo(FILE *xmlfile);
bool SKApi_XML_Help(void);

#endif
