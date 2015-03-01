
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "html.h"

#define BUFFSIZE 1024

int main(int argc, char **argv) {
	//TODO: dynamic buffer handling to never overflow
	FILE *f, *f_out;
	HtmlDocument *doc;
	HtmlParseState *parse_state;
	char buffer[BUFFSIZE + 1];
	const char *token = buffer;
	
	if(argc != 3) {
		fprintf(stderr, "usage: Html2Xml [input Html file] [output Xml file]\n");
		return 1;
	}


	if(!(f = fopen(argv[1], "r"))) {
		fprintf(stderr, "error: cannot open file %s\n", argv[1]);
		return 1;
	}

    //check file size, avoid space file
    fseek(f,0,SEEK_END);
    if (ftell(f) == 0)
    {
        fclose(f);
        fprintf(stderr, "error: space file %s\n",argv[1]);
        return 1;
    }
    fseek(f,0,SEEK_SET);


	buffer[BUFFSIZE] = 0;
	size_t len = 0;
	size_t buffsize = BUFFSIZE;
	
	parse_state = html_parse_begin();  

	if(!(f_out = fopen(argv[2], "w"))) {
		fprintf(stderr, "error: cannot open file %s\n", argv[2]);
		return 1;
	}

    xmlfile_set(f_out);

	while(!feof(f)) {
		len = fread(buffer + (BUFFSIZE - buffsize), 1, buffsize, f);
		token = html_parse_stream(parse_state, buffer + (BUFFSIZE - buffsize), buffer, len);
		buffsize = (token - buffer);
		memmove(buffer, token, BUFFSIZE - buffsize);
	}
	
	doc = html_parse_end(parse_state);
	fclose(f);
	html_print_dom(doc);
	html_free_document(doc);
    fclose(f_out);
	
	return 0;
}
