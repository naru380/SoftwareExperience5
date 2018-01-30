/* heaer.h  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1
#define NORMAL 0
#define ERROR 1
#define MAXSTRSIZE 1024

/* Token */
#define	TNAME		1	/* Name : Alphabet { Alphabet | Digit } */
#define	TPROGRAM	2	/* program : Keyword */
#define	TVAR		3	/* var : Keyword */
#define	TARRAY		4	/* array : Keyword */
#define	TOF			5	/* of : Keyword */
#define	TBEGIN		6	/* begin : Keyword */
#define	TEND		7  	/* end : Keyword */
#define	TIF			8  	/* if : Keyword */
#define	TTHEN		9	/* then : Keyword */
#define	TELSE		10	/* else : Keyword */
#define	TPROCEDURE	11	/* procedure : Keyword */
#define	TRETURN		12	/* return : Keyword */
#define	TCALL		13	/* call : Keyword */
#define	TWHILE		14	/* while : Keyword */
#define	TDO			15 	/* do : Keyword */
#define	TNOT		16	/* not : Keyword */
#define	TOR			17	/* or : Keyword */
#define	TDIV		18 	/* div : Keyword */
#define	TAND		19 	/* and : Keyword */
#define	TCHAR		20	/* char : Keyword */
#define	TINTEGER	21	/* integer : Keyword */
#define	TBOOLEAN	22 	/* boolean : Keyword */
#define	TREADLN		23	/* readln : Keyword */
#define	TWRITELN	24	/* writeln : Keyword */
#define	TTRUE		25	/* true : Keyword */
#define	TFALSE		26	/* false : Keyword */
#define	TNUMBER		27	/* unsigned integer */
#define	TSTRING		28	/* String */
#define	TPLUS		29	/* + : symbol */
#define	TMINUS		30 	/* - : symbol */
#define	TSTAR		31 	/* * : symbol */
#define	TEQUAL		32 	/* = : symbol */
#define	TNOTEQ		33 	/* <> : symbol */
#define	TLE			34 	/* < : symbol */
#define	TLEEQ		35 	/* <= : symbol */
#define	TGR			36	/* > : symbol */
#define	TGREQ		37	/* >= : symbol */
#define	TLPAREN		38 	/* ( : symbol */
#define	TRPAREN		39 	/* ) : symbol */
#define	TLSQPAREN	40	/* [ : symbol */
#define	TRSQPAREN	41 	/* ] : symbol */
#define	TASSIGN		42	/* := : symbol */
#define	TDOT		43 	/* . : symbol */
#define	TCOMMA		44	/* , : symbol */
#define	TCOLON		45	/* : : symbol */
#define	TSEMI		46	/* ; : symbol */
#define	TREAD		47	/* read : Keyword */
#define	TWRITE		48	/* write : Keyword */
#define	TBREAK		49	/* break : Keyword */

#define NUMOFTOKEN	49

/* token-list.c */
#define KEYWORDSIZE	28

struct KEY {
	char * keyword;
	int keytoken;
};

extern struct KEY key[KEYWORDSIZE];
extern int error(char *mes);

/* scan.c */
extern int init_scan(char *filename);
extern int scan(void);
extern int num_attr;
extern char string_attr[MAXSTRSIZE];
extern int get_linenum(void);
extern void end_scan(void);

/* parse.c */
#define PRETTY_PRINT 0	/* Decide to do pretty-print */
#define TOKEN 1
#define STRING 2
#define SPACE 3
#define NEWLINE 4
#define INDENT 5

extern int token;
extern int parse_program(void);
int block(void);
int variable_declaration(void);
int variable_names(void);
int type(void);
int array_type(void);
int subprogram_declaration(void);
int formal_parameters(void);
int compound_statement(void);
int statement(void);
int condtion_statement(void);
int iteration_statement(void);
int call_statement(void);
int expressions(void);
int assignment_statement(void);
int variable(void);
int expression(void);
int simple_expression(void);
int term(void);
int factor(void);
int input_statement(void);
int output_statement(void);
int output_format(void);
void pretty_print(int);

/* x-reference.c */
#define GLOBAL 0
#define LOCAL 1
#define ALL 2
#define TPINT 0
#define TPCHAR 1
#define TPBOOL 2
#define TPARRAY 3
#define TPARRAYINT 4
#define TPARRAYCHAR 5
#define TPARRAYBOOL 6
#define TPPROC 7
#define TPNONE 8

struct TYPE {
	int ttype;				/* TPINT TPCHAR TPBOOL TPARRAY
							   TPARRAYINT TPARRAYCHAR TPARRAYBOOL
							   TPRTOC */
	int arraysize;			/* Size of array, if TPARRAY */
	struct TYPE *etp;		/* Pointer to element type if TPARRAY */
	struct TYPE *paratp;	/* Pointer to parameter's type list if ttype is TPROC */
};

struct LINE {
	int reflinenum;
	struct LINE *nextlinep;
};

struct ID {
	char *name;
	char *procname;		/* Procedure name within which this name is defined
						   NULL, if grobal name */
	struct TYPE *itp;
	int ispara;			/* 1: formal parameter, 0: else (variable) */
	int deflinenum;
	struct LINE *irefp;
	struct ID *nextp;
};

struct LOCALIDS {
	struct ID *idroot;
	struct LOCALIDS *nextp;
};

extern char name[MAXSTRSIZE], procname[MAXSTRSIZE];
extern int ttype, arraysize, scope, ispara, isproc, isarr, exptype, sexptype, termtype, facttype, paranum;
extern void init_idtab(void);
extern struct ID *iparap;
extern struct TYPE *parap;
struct ID *search_idtab(char*, int);
extern int id_register(void);
extern int type_register(void);
extern int reflinenum_register(void);
extern int get_vartype(void);
extern int get_arrelmtype(void);
extern int get_parap(void);
extern int storage_localidtabs(void);
void connect_idtabs(void);
struct ID *sort_idtab(struct ID*);
char *get_typestr(struct ID*);
extern int print_xreftab(void);
extern void release_idtab(void);

/* compile.c */
#define CASLII_LIB_EOVF "EOVF\n\tCALL\tWRITELINE\n\tLAD\tgr1,EOVF1\n\tLD\tgr2,gr0\n\tCALL\tWRITESTR\n\tCALL\tWRITELINE\n\tSVC\t1\n"
#define CASLII_LIB_EOVF1 "EOVF1\tDC\t'***** Run-Time Error : Overflow *****'\n"
#define CASLII_LIB_E0DIV "E0DIV\n\tJNZ\tEOVF\n\tCALL\tWRITELINE\n\tLAD\tgr1,E0DIV1\n\tLD\tgr2,gr0\n\tCALL\tWRITESTR\n\tCALL\tWRITELINE\n\tSVC\t2\n"
#define CASLII_LIB_E0DIV1 "E0DIV1\tDC\t'***** Run-Time Error : Zero-Divide *****'\n"
#define CASLII_LIB_EROV "EROV\n\tCALL\tWRITELINE\n\tLAD\tgr1,EROV1\n\tLD\tgr2,gr0\n\tCALL\tWRITESTR\n\tCALL\tWRITELINE\n\tSVC\t3\n"
#define CASLII_LIB_EROV1 "EROV1\tDC\t'***** Run-Time ERROR : Range-Over in Array Index *****'\n"
#define CASLII_LIB_WRITECHAR "WRITECHAR\n\tRPUSH\n\tLD\tgr6,SPACE\n\tLD\tgr7,OBUFSIZE\nWC1\n\tSUBA\tgr2,ONE\n\tJZE\tWC2\n\tJMI\tWC2\n\tST\tgr6,OBUF,gr7\n\tCALL\tBOVFCHECK\n\tJUMP\tWC1\nWC2\n\tST\tgr1,OBUF,gr7\n\tCALL\tBOVFCHECK\n\tST\tgr7,OBUFSIZE\n\tRPOP\n\tRET\n"
#define CASLII_LIB_WRITESTR "WRITESTR\n\tRPUSH\n\tLD\tgr6,gr1\nWS1\n\tLD\tgr4,0,gr6\n\tJZE\tWS2\n\tADDA\tgr6,ONE\n\tSUBA\tgr2,ONE\n\tJUMP\tWS1\nWS2\n\tLD\tgr7,OBUFSIZE\n\tLD\tgr5,SPACE\nWS3\n\tSUBA\tgr2,ONE\n\tJMI\tWS4\n\tST\tgr5,OBUF,gr7\n\tCALL\tBOVFCHECK\n\tJUMP\tWS3\nWS4\n\tLD\tgr4,0,gr1\n\tJZE\tWS5\n\tST\tgr4,OBUF,gr7\n\tADDA\tgr1,ONE\n\tCALL\tBOVFCHECK\n\tJUMP\tWS4\nWS5\n\tST\tgr7,OBUFSIZE\n\tRPOP\n\tRET\n"
#define CASLII_LIB_BOVFCHECK "BOVFCHECK\n\tADDA\tgr7,ONE\n\tCPA\tgr7,BOVFLEVEL\n\tJMI\tBOVF1\n\tCALL\tWRITELINE\n\tLD\tgr7,OBUFSIZE\nBOVF1\n\tRET\n"
#define CASLII_LIB_BOVFLEVEL "BOVFLEVEL\tDC\t256\n"
#define CASLII_LIB_WRITEINT "WRITEINT\n\tRPUSH\n\tLD\tgr7,gr0\n\tCPA\tgr1,gr0\n\tJPL\tWI1\n\tJZE\tWI1\n\tLD\tgr4,gr0\n\tSUBA\tgr4,gr1\n\tCPA\tgr4,gr1\n\tJZE\tWI6\n\tLD\tgr1,gr4\n\tLD\tgr7,ONE\nWI1\n\tLD\tgr6,SIX\n\tST\tgr0,INTBUF,gr6\n\tSUBA\tgr6,ONE\n\tCPA\tgr1,gr0\n\tJNZ\tWI2\n\tLD\tgr4,ZERO\n\tST\tgr4,INTBUF,gr6\n\tJUMP\tWI5\nWI2\n\tCPA\tgr1,gr0\n\tJZE\tWI3\n\tLD\tgr5,gr1\n\tDIVA\tgr1,TEN\n\tLD\tgr4,gr1\n\tMULA\tgr4,TEN\n\tSUBA\tgr5,gr4\n\tADDA\tgr5,ZERO\n\tST\tgr5,INTBUF,gr6\n\tSUBA\tgr6,ONE\n\tJUMP\tWI2\nWI3\n\tCPA\tgr7,gr0\n\tJZE\tWI4\n\tLD\tgr4,MINUS\n\tST\tgr4,INTBUF,gr6\n\tJUMP\tWI5\nWI4\n\tADDA\tgr6,ONE\nWI5\n\tLAD\tgr1,INTBUF,gr6\n\tCALL\tWRITESTR\n\tRPOP\n\tRET\nWI6\n\tLAD\tgr1,MMINT\n\tCALL\tWRITESTR\n\tRPOP\n\tRET\n"
#define CASLII_LIB_MMINT "MMINT\tDC\t'-32768'\n"
#define CASLII_LIB_WRITEBOOL "WRITEBOOL\n\tRPUSH\n\tCPA\tgr1,gr0\n\tJZE\tWB1\n\tLAD\tgr1,WBTRUE\n\tJUMP\tWB2\nWB1\n\tLAD\tgr1,WBFALSE\nWB2\n\tCALL\tWRITESTR\n\tRPOP\n\tRET\n"
#define CASLII_LIB_WBTRUE "WBTRUE\tDC\t'TRUE'\n"
#define CASLII_LIB_WBFALSE "WBFALSE\tDC\t'FALSE'\n"
#define CASLII_LIB_WRITELINE "WRITELINE\n\tRPUSH\n\tLD\tgr7,OBUFSIZE\n\tLD\tgr6,NEWLINE\n\tST\tgr6,OBUF,gr7\n\tADDA\tgr7,ONE\n\tST\tgr7,OBUFSIZE\n\tOUT\tOBUF,OBUFSIZE\n\tST\tgr0,OBUFSIZE\n\tRPOP\n\tRET\n"
#define CASLII_LIB_FLUSH "FLUSH\n\tRPUSH\n\tLD\tgr7,OBUFSIZE\n\tJZE\tFL1\n\tCALL\tWRITELINE\nFL1\n\tRPOP\n\tRET\n"
#define CASLII_LIB_READCHAR "READCHAR\n\tRPUSH\n\tLD\tgr5,RPBBUF\n\tJZE\tRC0\n\tST\tgr5,0,gr1\n\tST\tgr0,RPBBUF\n\tJUMP\tRC3\nRC0\n\tLD\tgr7,INP\n\tLD\tgr6,IBUFSIZE\n\tJNZ\tRC1\n\tIN\tIBUF,IBUFSIZE\n\tLD\tgr7,gr0\nRC1\n\tCPA\tgr7,IBUFSIZE\n\tJNZ\tRC2\n\tLD\tgr5,NEWLINE\n\tST\tgr5,0,gr1\n\tST\tgr0,IBUFSIZE\n\tST\tgr0,INP\n\tJUMP\tRC3\nRC2\n\tLD\tgr5,IBUF,gr7\n\tADDA\tgr7,ONE\n\tST\tgr5,0,gr1\n\tST\tgr7,INP\nRC3\n\tRPOP\n\tRET\n"
#define CASLII_LIB_READINT "READINT\n\tRPUSH\nRI1\n\tCALL\tREADCHAR\n\tLD\tgr7,0,gr1\n\tCPA\tgr7,SPACE\n\tJZE\tRI1\n\tCPA\tgr7,TAB\n\tJZE\tRI1\n\tCPA\tgr7,NEWLINE\n\tJZE\tRI1\n\tLD\tgr5,ONE\n\tCPA\tgr7,MINUS\n\tJNZ\tRI4\n\tLD\tgr5,gr0\n\tCALL\tREADCHAR\n\tLD\tgr7,0,gr1\nRI4\n\tLD\tgr6,gr0\nRI2\n\tCPA\tgr7,ZERO\n\tJMI\tRI3\n\tCPA\tgr7,NINE\n\tJPL\tRI3\n\tMULA\tgr6,TEN\n\tADDA\tgr6,gr7\n\tSUBA\tgr6,ZERO\n\tCALL\tREADCHAR\n\tLD\tgr7,0,gr1\n\tJUMP\tRI2\nRI3\n\tST\tgr7,RPBBUF\n\tST\tgr6,0,gr1\n\tCPA\tgr5,gr0\n\tJNZ\tRI5\n\tSUBA\tgr5,gr6\n\tST\tgr5,0,gr1\nRI5\n\tRPOP\n\tRET\n"
#define CASLII_LIB_READLINE "READLINE\n\tST\tgr0,IBUFSIZE\n\tST\tgr0,INP\n\tST\tgr0,RPBBUF\n\tRET\n"
#define CASLII_LIB_ONE "ONE\tDC\t1\n"
#define CASLII_LIB_SIX "SIX\tDC\t6\n"
#define CASLII_LIB_TEN "TEN\tDC\t10\n"
#define CASLII_LIB_SPACE "SPACE\tDC\t#0020\n"
#define CASLII_LIB_MINUS "MINUS\tDC\t#002D\n"
#define CASLII_LIB_TAB "TAB\tDC\t#0009\n"
#define CASLII_LIB_ZERO "ZERO\tDC\t#0030\n"
#define CASLII_LIB_NINE "NINE\tDC\t#0039\n"
#define CASLII_LIB_NEWLINE "NEWLINE\tDC\t#000A\n"
#define CASLII_LIB_INTBUF "INTBUF\tDS\t8\n"
#define CASLII_LIB_OBUFSIZE "OBUFSIZE\tDC\t0\n"
#define CASLII_LIB_IBUFSIZE "IBUFSIZE\tDC\t0\n"
#define CASLII_LIB_INP "INP\tDC\t0\n"
#define CASLII_LIB_OBUF "OBUF\tDS\t257\n"
#define CASLII_LIB_IBUF "IBUF\tDS\t257\n"
#define CASLII_LIB_RPBBUF "RPBBUF\tDC\t0\n"

struct STRLB {
	int label;
	char string[MAXSTRSIZE];
	struct STRLB *nextp;
};

extern FILE *fp;
extern int label;
extern int break_label;
extern int else_label;
extern int init_generate(char*);
extern int register_strlb(char*);
extern void output_strlb(void);
extern void release_strlb(void);
extern void output_library(void);
extern void end_generate(void);
