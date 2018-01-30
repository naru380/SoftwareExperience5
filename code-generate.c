#include "header.h"

int label;	/* Next label number */
int break_label;	/* Jumpping destination for break-statement */
int else_label;	/* Jumpping destination for else-statement */
char mesg[1024];	/* String of error message */
FILE *fp;	/* File pointer to output file (object file) */
struct STRLB *strlbroot;	/* Pointer to root of list saving strings and theirs labels */

int init_generate(char *input_filename) {	/* Initilize global variables and open output file */
	int i, input_filename_length;
	char *output_filename;

	label = 1;
	else_label = 0;
	strlbroot = NULL;
	input_filename_length = strlen(input_filename);
	if((output_filename = (char *)malloc(sizeof(char)*(input_filename_length + 1))) == NULL) {
		sprintf(mesg, "Can not malloc in open_file()");
		return(error(mesg));
	}
	for(i = 0; i <= input_filename_length; i++) {
		if(input_filename[i] == '.') {
			break;
		}
	}
	strncpy(output_filename, input_filename, i);
	output_filename[i] = '\0';
	sprintf(output_filename, "%s.csl", output_filename);
	fp = fopen(output_filename, "w");
	return(NORMAL);
}

int register_strlb(char *string) {	/* Register string and its label */
	struct STRLB *p, *q;

	if((p = (struct STRLB *)malloc(sizeof(struct STRLB))) == NULL) {
		sprintf(mesg, "Can not malloc in register_strlb()");
		return(error(mesg));
	}
	p->label = label++;
	strcpy(p->string, string);
	p->nextp = NULL;
	if(strlbroot == NULL) {
		strlbroot = p;
	} else {
		for(q = strlbroot; q->nextp != NULL; q = q->nextp) {
			// Nothing
		}
		q->nextp = p;
	}
}

void output_strlb() {	/* Output registerd string and its label (Output DC-statement) */
	struct STRLB *p;

	for(p = strlbroot; p != NULL; p = p->nextp) {
		if(strlen(p->string) == 0) {
			fprintf(fp, "L%04d\tDC\t0\n", p->label);
		} else {
			fprintf(fp, "L%04d\tDC\t'%s'\n", p->label, p->string);
		}
	}
}

void release_strlb() {	/* Release the data structure */
	struct STRLB *p, *q;

	for(p = strlbroot; p != NULL; p = q) {
		q = p->nextp;
		free(p);
	}
}

void output_library(void) {	/* Output library for CASLII */
	fprintf(fp, CASLII_LIB_EOVF);
	fprintf(fp, CASLII_LIB_EOVF1);
	fprintf(fp, CASLII_LIB_E0DIV);
	fprintf(fp, CASLII_LIB_E0DIV1);
	fprintf(fp, CASLII_LIB_EROV);
	fprintf(fp, CASLII_LIB_EROV1);
	fprintf(fp, CASLII_LIB_WRITECHAR);
	fprintf(fp, CASLII_LIB_WRITESTR);
	fprintf(fp, CASLII_LIB_BOVFCHECK);
	fprintf(fp, CASLII_LIB_BOVFLEVEL);
	fprintf(fp, CASLII_LIB_WRITEINT);
	fprintf(fp, CASLII_LIB_MMINT);
	fprintf(fp, CASLII_LIB_WRITEBOOL);
	fprintf(fp, CASLII_LIB_WBTRUE);
	fprintf(fp, CASLII_LIB_WBFALSE);
	fprintf(fp, CASLII_LIB_WRITELINE);
	fprintf(fp, CASLII_LIB_FLUSH);
	fprintf(fp, CASLII_LIB_READCHAR);
	fprintf(fp, CASLII_LIB_READINT);
	fprintf(fp, CASLII_LIB_READLINE);
	fprintf(fp, CASLII_LIB_ONE);
	fprintf(fp, CASLII_LIB_SIX);
	fprintf(fp, CASLII_LIB_TEN);
	fprintf(fp, CASLII_LIB_SPACE);
	fprintf(fp, CASLII_LIB_MINUS);
	fprintf(fp, CASLII_LIB_TAB);
	fprintf(fp, CASLII_LIB_ZERO);
	fprintf(fp, CASLII_LIB_NINE);
	fprintf(fp, CASLII_LIB_NEWLINE);
	fprintf(fp, CASLII_LIB_INTBUF);
	fprintf(fp, CASLII_LIB_OBUFSIZE);
	fprintf(fp, CASLII_LIB_IBUFSIZE);
	fprintf(fp, CASLII_LIB_INP);
	fprintf(fp, CASLII_LIB_OBUF);
	fprintf(fp, CASLII_LIB_IBUF);
	fprintf(fp, CASLII_LIB_RPBBUF);
}

void end_generate() {	/* End generating object-code */
	fclose(fp);
	release_strlb();
}
