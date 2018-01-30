/* scan.c */
#include "header.h"

FILE *input;	/* File pointer */
int linenum;	/* Current number of the line */
int num_attr;	/* Number of the token */
char string_attr[MAXSTRSIZE];	/* String of the token */

int init_scan(char *filename) {	/* Initialize the scanner: open the file  */
	linenum = 0;
	if((input = fopen(filename, "r")) == NULL)
		return -1;
	else
		return 0;
}

int scan() {	/* Get the next token and discriminate that kind */
	int i, num, line;
	char c, *p;
	char mesg[MAXSTRSIZE * 2];

	if(linenum == 0) linenum = 1;

	p = string_attr;

	do {
		c = fgetc(input);
		while(c == '\n' || c == '\r') {
			if(c == '\n') {
				if((c = fgetc(input)) == '\r') {
					linenum++;
					c = fgetc(input);
				}
				else {
					linenum++;
					continue;
				}
			}
			if(c == '\r') {
				if((c = fgetc(input)) == '\n') {
					linenum++;
					c = fgetc(input);
				}
				else {
					linenum++;
					continue;
				}
			}
		}
		while(c == '{') {
			line = get_linenum();
			while(1){
				if((c = fgetc(input)) == '}') {
					c = fgetc(input);
					break;
				} else if(c == '\n') {
					if((c = fgetc(input)) == '\r') {
						linenum++;
					} else {
						linenum++;
						ungetc(c, input);
					}
				} else if(c == '\r') {
					if((c = fgetc(input)) == '\n') {
						linenum++;
					} else {
						linenum++;
						ungetc(c, input);
					}
				}
				if(c == EOF) {
					sprintf(mesg, "unterminated comment at line %d\n", line);
					error(mesg);
					return -1;
				}
			}
		}
		while(c == '/') {
			*p++ = c;
			if((c = fgetc(input)) == '*') {
				*p--;
				line = get_linenum();
				while(1){
					if((c = fgetc(input)) == '*') {
						if((c = fgetc(input)) == '/') {
							c = fgetc(input);
							break;
						}
					}
					if(c == '\n') {
						if((c = fgetc(input)) == '\r') {
							linenum++;
						} else {
							linenum++;
							ungetc(c, input);
						}
					} else if(c == '\r') {
						if((c = fgetc(input)) == '\n') {
							linenum++;
						} else {
							linenum++;
							ungetc(c, input);
						}
					}
					if(c == EOF) {
						sprintf(mesg, "unterminated comment at line %d\n", line);
						error(mesg);
						return -1;
					}
				}
			} else {
				ungetc(c, input);
				c = *--p;
				break;
			}
		}
	} while(c == ' ' || c == '\t' || c == '\n' || c == '\r');

	if(c == EOF) {
		return -1;
	} else if(isalpha(c)) {
		*p++ = c;
		while(isalnum(c = fgetc(input))) {
			if(strlen(string_attr) >= MAXSTRSIZE-1) {
				sprintf(mesg, "too long name \"%s...\" at line %d\n", string_attr, get_linenum());
				error(mesg);
				return -1;
			}
			*p++ = c;
		}
		ungetc(c, input);
		*p++ = '\0';
		for(i = 0; i < KEYWORDSIZE; i++) {
			if(!strcmp(string_attr, key[i].keyword)) {
				return key[i].keytoken;
			};
		}
		return TNAME;
	} else if(isdigit(c)) {
		*p++ = c;
		while(isdigit(c = fgetc(input))) {
				if(strlen(string_attr) >= MAXSTRSIZE-1) {
					sprintf(mesg, "too many digits \'%s...\' at line %d\n", string_attr, get_linenum());
					error(mesg);
					return -1;
				}
			*p++ = c;
		}
		ungetc(c, input);
		*p++ = '\0';
		if((num = atoi(string_attr)) > 32767) {
			sprintf(mesg, "invalid value '%d' at line %d\n", num, get_linenum());
			error(mesg);
			return -1;
		} else {
			num_attr = num;
			return TNUMBER;
		}
	} else if(c == '\'') {
		while(1) {
			if((c = fgetc(input)) == '\'') {
				*p++ = c;
				if((c = fgetc(input)) == '\'') {
					*p++ = c;
				} else {
					p--;
					ungetc(c, input);
					break;
				}
			} else {
				if(c == '\n' || c == '\r' || c == EOF) {
					sprintf(mesg, "unterminated string at line %d\n", get_linenum());
					error(mesg);
					return -1;
				}
				if(strlen(string_attr) >= MAXSTRSIZE-1) {
					sprintf(mesg, "too long string \'%s...\' at line %d\n", string_attr, get_linenum());
					error(mesg);
					return -1;
				}
				*p++ = c;
			}
		}
		*p++ = '\0';
		return TSTRING;
	} else if(c == '<') {
		*p++ = c;
		if((c = fgetc(input)) == '>') {
			*p++ = c;
			*p++ = '\0';
			return TNOTEQ;
		} else if(c == '=') {
			*p++ = c;
			*p++ = '\0';
			return TLEEQ;
		} else {
			ungetc(c, input);
			*p++ = '\0';
			return TLE;
		}
	} else if(c == '>') {
		*p++ = c;
		if((c = fgetc(input)) == '=') {
			*p++ = c;
			*p++ = '\0';
			return TGREQ;
		} else {
			ungetc(c, input);
			*p++ = '\0';
			return TGR;
		}
	} else if(c == ':') {
		*p++ = c;
		if((c = fgetc(input)) == '=') {
			*p++ = '=';
			*p++ = '\0';
			return TASSIGN;
		} else {
			ungetc(c, input);
			*p++ = '\0';
			return TCOLON;
		}
	} else if(c == '-') {
		*p++ = c;
		*p++ = '\0';
		return TMINUS;
	} else if(c == '+') {
		*p++ = c;
		*p++ = '\0';
		return TPLUS;
	} else if(c == '*') {
		*p++ = c;
		*p++ = '\0';
		return TSTAR;
	} else if(c == '=') {
		*p++ = c;
		*p++ = '\0';
		return TEQUAL;
	} else if(c == '(') {
		*p++ = c;
		*p++ = '\0';
		return TLPAREN;
	} else if(c == ')') {
		*p++ = c;
		*p++ = '\0';
		return TRPAREN;
	} else if(c == '[') {
		*p++ = c;
		*p++ = '\0';
		return TLSQPAREN;
	} else if(c == ']') {
		*p++ = c;
		*p++ = '\0';
		return TRSQPAREN;
	} else if(c == '.') {
		*p++ = c;
		*p++ = '\0';
		return TDOT;
	} else if(c == ',') {
		*p++ = c;
		*p++ = '\0';
		return TCOMMA;
	} else if(c == ';') {
		*p++ = c;
		*p++ = '\0';
		return TSEMI;
	} else {
		*p++ = c;
		*p++ = '\0';
		sprintf(mesg, "invalid character '%c' at line %d\n", c, get_linenum());
		error(mesg);
		return -1;
	}
}

int get_linenum(){	/* Get current number of the line */
	return linenum;
}

void end_scan() { /* Close the file to end the scannig */
	fclose(input);
}
