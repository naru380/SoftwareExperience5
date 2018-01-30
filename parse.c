#include "header.h"

int token;	/* Token code which returned from scan()*/
int indent;	/* The amount of indent */
int i;	/* Valiable for 'for' statement */
int loop;	/* Number of non-finished 'while' statement */
int isopr;	/* Flag that indicates whether operater was scanned*/
char mesg[MAXSTRSIZE*2];	/* Message for error handling */

int parse_program() {	/* Parse program */
	indent = 0;
	loop = 0;
	if(token != TPROGRAM) {
		sprintf(mesg, "Keyword 'program' is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	token = scan();
	if(token != TNAME) {
		sprintf(mesg, "Program name is not found at line %d", get_linenum());
		return(error(mesg));
	}
	fprintf(fp, "$$%s\tSTART\n\tLAD\tgr0,0\n\tCALL\tL%04d\n\tCALL\tFLUSH\n\tSVC\t0\n", string_attr, label++);
	pretty_print(SPACE);
	pretty_print(TOKEN);
	token = scan();
	if(token != TSEMI) {
		sprintf(mesg, "Semicolon is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	pretty_print(NEWLINE);
	token = scan();
	if(block() == ERROR)
		return(ERROR);
	if(token != TDOT) {
		sprintf(mesg, "Period is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	pretty_print(NEWLINE);
	token = scan();
	output_strlb();
	output_library();
	fprintf(fp, "\tEND\n");
	return(NORMAL);
}

int block() {	/* Parse block */
	while(token == TVAR || token == TPROCEDURE) {
		indent++;
		if(token == TVAR) {
			if(variable_declaration() == ERROR)
				return(ERROR);
		} else if(token == TPROCEDURE) {
			if(subprogram_declaration() == ERROR)
				return(ERROR);
		}
		indent--;
	}
	if(token != TBEGIN) {
		sprintf(mesg, "Keyword 'begin' is not found at line %d", get_linenum());
		return(error(mesg));
	}
	fprintf(fp, "L0001\n");
	if(compound_statement() == ERROR)
		return(ERROR);
	fprintf(fp, "\tRET\n");
	return(NORMAL);
}

int variable_declaration() {	/* Parse variable declaration */
	pretty_print(INDENT);
	pretty_print(TOKEN);
	pretty_print(SPACE);
	token = scan();
	if(variable_names() == ERROR)
		return(ERROR);
	if(token != TCOLON) {
		sprintf(mesg, "Colon is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	pretty_print(SPACE);
	token = scan();
	if(type() == ERROR)
		return(ERROR);
	if(type_register() == ERROR) {
		return(ERROR);
	}
	if(token != TSEMI) {
		sprintf(mesg, "Semicolon is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	pretty_print(NEWLINE);
	token = scan();
	while(token == TNAME) {
		indent++;
		pretty_print(INDENT);
		if(variable_names() == ERROR)
			return(ERROR);
		if(token != TCOLON) {
			sprintf(mesg, "Colon is not found at line %d", get_linenum());
			return(error(mesg));
		}
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(type() == ERROR)
			return(ERROR);
		if(type_register() == ERROR) {
			return(ERROR);
		}
		if(token != TSEMI) {
			sprintf(mesg, "Semicolon is not found at line %d", get_linenum());
			return(error(mesg));
		}
		pretty_print(TOKEN);
		pretty_print(NEWLINE);
		token = scan();
		indent--;
	}
	return(NORMAL);
}

int variable_names() {	/* Parse variable names */
	if(token != TNAME) {
		sprintf(mesg, "Variable name is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	strcpy(name, string_attr);
	if(id_register() == ERROR) {
		return(ERROR);
	}
	token = scan();
	while(token == TCOMMA) {
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(token != TNAME) {
			sprintf(mesg, "Variable name is not found at line %d", get_linenum());
			return(error(mesg));
		}
		pretty_print(TOKEN);
		strcpy(name, string_attr);
		if(id_register() == ERROR) {
			return(ERROR);
		}
		token = scan();
	}
	return(NORMAL);
}

int type() {	/* Parse type */
	if(token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
		pretty_print(TOKEN);
		switch(token) {
			case TINTEGER:
				ttype = TPINT;
				break;
			case TBOOLEAN:
				ttype = TPBOOL;
				break;
			case TCHAR:
				ttype = TPCHAR;
				break;
			default:
				// not-reach
				exit(EXIT_FAILURE);
				break;
		}
		token = scan();
	} else if(token == TARRAY) {
		if(array_type() == ERROR)
			return(ERROR);
	} else {
		sprintf(mesg, "Type is not found at line %d", get_linenum());
		return(error(mesg));
	}
	return(NORMAL);
}

int array_type() {	/* Parse array type */
	pretty_print(TOKEN);
	isarr = TRUE;
	token = scan();
	if(token != TLSQPAREN) {
		sprintf(mesg, "Left square bracket is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	token = scan();
	if(token != TNUMBER) {
		sprintf(mesg, "Unsigned integer is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	arraysize = num_attr;
	if(arraysize < 1) {
		sprintf(mesg, "Size of size is most be 1 or more  at line %d", get_linenum());
		return(error(mesg));
	}
	token = scan();
	if(token != TRSQPAREN) {
		sprintf(mesg, "Right square bracket is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	pretty_print(SPACE);
	token = scan();
	if(token != TOF) {
		sprintf(mesg, "Keyword 'of' is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	pretty_print(SPACE);
	token = scan();
	if(token != TINTEGER && token != TBOOLEAN && token != TCHAR) {
		sprintf(mesg, "Standard type is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	switch(token) {
		case TINTEGER:
			ttype = TPINT;
			break;
		case TBOOLEAN:
			ttype = TPBOOL;
			break;
		case TCHAR:
			ttype = TPCHAR;
			break;
		default:
			// not-reach
			exit(EXIT_FAILURE);
			break;
	}
	token = scan();
	return(NORMAL);
}

int subprogram_declaration() {	/* Parse subprogram declaration */
	int i;
	struct ID *p;

	pretty_print(INDENT);
	pretty_print(TOKEN);
	pretty_print(SPACE);
	isproc = TRUE;
	token = scan();
	if(token != TNAME) {
		sprintf(mesg, "Procedure name is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	strcpy(name, string_attr);
	if(id_register() == ERROR) {
		return(ERROR);
	}
	ttype = TPPROC;
	if(type_register() == ERROR) {
		return(ERROR);
	}
	strcpy(procname, string_attr);
	scope = LOCAL;
	token = scan();
	if(token == TLPAREN) {
		if(formal_parameters() == ERROR)
			return(ERROR);
	}
	isproc = FALSE;
	if(token != TSEMI) {
		sprintf(mesg, "Semicolon is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	pretty_print(NEWLINE);
	token = scan();
	indent++;
	if(token == TVAR)
		if(variable_declaration() == ERROR)
			return(ERROR);
	indent--;
	scope = GLOBAL;
	if(token != TBEGIN) {
		sprintf(mesg, "Keyword 'begin' is not found at line %d", get_linenum());
		return(error(mesg));
	}
	fprintf(fp, "$%s\n", procname);
	strcpy(name, procname);
	get_parap();
	if(paranum != 0) {
		fprintf(fp, "\tPOP\tgr2\n");
		for(i = 0, p = iparap; i < paranum; i++, p = p->nextp) {
			fprintf(fp, "\tPOP\tgr1\n\tST\tgr1,$%s%%%s\n", p->name, p->procname);
		}
		fprintf(fp, "\tPUSH\t0,gr2\n");
	}
	scope = ALL;
	if(compound_statement() == ERROR)
		return(ERROR);
	fprintf(fp, "\tRET\n");
	if(token != TSEMI) {
		sprintf(mesg, "Semicolon is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	pretty_print(NEWLINE);
	storage_localidtabs();
	procname[0] = '\0';
	scope = GLOBAL;
	token = scan();
	return(NORMAL);
}

int formal_parameters() {	/* Parse formal parameters */
	pretty_print(TOKEN);
	token = scan();
	ispara = TRUE;
	if(variable_names() == ERROR)
		return(ERROR);
	if(token != TCOLON) {
		sprintf(mesg, "Colon is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	pretty_print(SPACE);
	token = scan();
	if(type() == ERROR)
		return(ERROR);
	if(isarr) {
		sprintf(mesg, "Type must be standard-type at line %d", get_linenum());
		return(error(mesg));
	}
	if(type_register() == ERROR) {
		return(ERROR);
	}
	while(token == TSEMI) {
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(variable_names() == ERROR)
			return(ERROR);
		if(token != TCOLON) {
			sprintf(mesg, "Colon is not found at line %d", get_linenum());
			return(error(mesg));
		}
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(type() == ERROR)
			return(ERROR);
		if(ttype != TPINT && ttype != TPCHAR && ttype != TPBOOL) {
			sprintf(mesg, "Type must be standard-type at line %d", get_linenum());
			return(error(mesg));
		}
		if(type_register() == ERROR) {
			return(ERROR);
		}
	}
	if(token != TRPAREN) {
		sprintf(mesg, "Right parenthesis is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(TOKEN);
	token = scan();
	ispara = FALSE;
	return(NORMAL);
}

int compound_statement() {	/* Parse compound statement */
	pretty_print(INDENT);
	pretty_print(TOKEN);
	pretty_print(NEWLINE);
	indent++;
	token = scan();
	if(token == TEND) {
		indent--;
		pretty_print(INDENT);
		pretty_print(TOKEN);
		token = scan();
		return(NORMAL);
	}
	while(1) {
		if(statement() == ERROR)
			return(ERROR);
		if(token == TEND) {
			indent--;
			pretty_print(NEWLINE);
			pretty_print(INDENT);
			pretty_print(TOKEN);
			token = scan();
			return(NORMAL);
		} else if(token == TSEMI) {
			pretty_print(TOKEN);
			pretty_print(NEWLINE);
			token = scan();
		} else {
			sprintf(mesg, "Semicolon is not found at line %d", get_linenum());
			return(error(mesg));
		}
	}
}

int statement() { /* Parse statement */
	int b_label;

	switch(token) {
		case TNAME:
			if(assignment_statement() == ERROR)
				return(ERROR);
			break;
		case TIF:
			pretty_print(INDENT);
			pretty_print(TOKEN);
			pretty_print(SPACE);
			indent++;
			if(condtion_statement() == ERROR)
				return(ERROR);
			break;
		case TWHILE:
			b_label = break_label;
			if(iteration_statement() == ERROR)
				return(ERROR);
			break_label = b_label;
			break;
		case TBREAK:
			pretty_print(INDENT);
			pretty_print(TOKEN);
			token = scan();
			if(loop == 0) {
				sprintf(mesg, "Keyword 'break' most be in more less than one-loop at line %d", get_linenum());
				return(error(mesg));
			}
			fprintf(fp, "\tJUMP\tL%04d\n", break_label);
			break;
		case TCALL:
			if(call_statement() == ERROR)
				return(ERROR);
			break;
		case TRETURN:
			pretty_print(INDENT);
			pretty_print(TOKEN);
			token = scan();
			fprintf(fp, "\tRET\n");
			break;
		case TREAD:
		case TREADLN:
			if(input_statement() == ERROR)
				return(ERROR);
			break;
		case TWRITE:
		case TWRITELN:
			if(output_statement() == ERROR)
				return(ERROR);
			break;
		case TBEGIN:
			if(compound_statement() == ERROR)
				return(ERROR);
			break;
		case TSEMI:
		case TEND:
			return(NORMAL);
			break;
		default:
			sprintf(mesg, "Statement is invalid at line %d", get_linenum());
			return(error(mesg));
			break;
	}
	return(NORMAL);
}

int condtion_statement() {	/* Parse Conditon statement */
	int cnst_label;
	int e_label;
	int is_first_if;

	if(else_label == 0) {
		is_first_if = TRUE;
	} else {
		is_first_if = FALSE;
	}
	token = scan();
	if(expression() == ERROR)
		return(ERROR);
	if(exptype != TPBOOL) {
		sprintf(mesg, "Experession-type must be boolean at line %d", get_linenum());
		return(error(mesg));
	}
	cnst_label = label++;
	fprintf(fp, "\tCPA\tgr1,gr0\n\tJZE\tL%04d\n", cnst_label);
	if(is_first_if == TRUE) {
		else_label = label++;
	}
	if(token != TTHEN) {
		sprintf(mesg, "Keyword 'then' is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(SPACE);
	pretty_print(TOKEN);
	token = scan();
	if(token == TSEMI || token == TEND) {
		indent--;
			if(is_first_if == TRUE) {
				fprintf(fp, "L%04d\n", cnst_label);
				else_label = 0;
				is_first_if = FALSE;
			} else {
				fprintf(fp, "L%04d\n", cnst_label);
			}
		return(NORMAL);
	}
	if(token != TELSE) {
		pretty_print(NEWLINE);
		if(statement() == ERROR)
			return(ERROR);
		if(token != TELSE) {
			if(is_first_if == TRUE) {
				fprintf(fp, "L%04d\n", cnst_label);
				else_label = 0;
				is_first_if = FALSE;
			} else {
				fprintf(fp, "L%04d\n", cnst_label);
			}
			return(NORMAL);
		}
	}
	indent--;
	if(token == TELSE) {
		fprintf(fp, "\tJUMP\tL%04d\nL%04d\n", else_label, cnst_label);
		cnst_label = label++;
		pretty_print(NEWLINE);
		pretty_print(INDENT);
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(token == TIF) {
			pretty_print(TOKEN);
			pretty_print(SPACE);
			indent++;
			if(condtion_statement() == ERROR)
				return(ERROR);
			if(is_first_if == TRUE) {
				else_label = 0;
				is_first_if == FALSE;
			}
			return(NORMAL);
		}
		if(token == TSEMI || token == TEND) {
			if(is_first_if == TRUE) {
				fprintf(fp, "L%04d\n", else_label);
				else_label = 0;
				is_first_if = FALSE;
			} else {
				fprintf(fp, "L%04d\n", cnst_label);
			}
			fprintf(fp, "L%04d\n", cnst_label);
			return(NORMAL);
		}
		pretty_print(NEWLINE);
		indent++;
		e_label = else_label;
		else_label = 0;
		if(statement() == ERROR)
			return(ERROR);
		indent--;
		else_label = e_label;
		fprintf(fp, "L%04d\n", else_label);
	}
	if(is_first_if == TRUE) {
		else_label = 0;
		is_first_if = FALSE;
	}
	return(NORMAL);
}

int iteration_statement() {	/* Parse iteration statement */
	int itst_label;

	pretty_print(INDENT);
	pretty_print(TOKEN);
	pretty_print(SPACE);
	indent++;
	loop++;
	token = scan();
	itst_label = label;
	fprintf(fp, "L%04d\n", itst_label);
	label = label + 2;
	if(expression() == ERROR)
		return(ERROR);
	if(exptype != TPBOOL) {
		sprintf(mesg, "Experession-type must be boolean at line %d", get_linenum());
		return(error(mesg));
	}
	fprintf(fp, "\tCPA\tgr1,gr0\n\tJZE\tL%04d\n", itst_label + 1);
	if(token != TDO) {
		sprintf(mesg, "Keyword 'do' is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(SPACE);
	pretty_print(TOKEN);
	token = scan();
	if(token == TSEMI || token == TEND) {
		indent--;
		fprintf(fp, "L%04d\n", itst_label + 1);
		return(NORMAL);
	}
	pretty_print(NEWLINE);
	break_label = itst_label + 1;
	if(statement() == ERROR)
		return(ERROR);
	indent--;
	loop--;
	fprintf(fp, "\tJUMP\tL%04d\nL%04d\n", itst_label, itst_label + 1);
	return(NORMAL);
}

int call_statement() {	/* Parse call statement */
	char pn[MAXSTRSIZE];

		pretty_print(INDENT);
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(token != TNAME) {
			sprintf(mesg, "Procedure name is not found at line %d", get_linenum());
			return(error(mesg));
		}
		pretty_print(TOKEN);
		strcpy(name, string_attr);
		strcpy(pn, name);
		if(!strcmp(name, procname)) {
				sprintf(mesg, "Recursive call of procedure '%s' at line %d", procname, get_linenum());
				return(error(mesg));
			}
			if(reflinenum_register() == ERROR) {
				return(ERROR);
			}
			token = scan();
			if(token == TLPAREN){
				pretty_print(TOKEN);
				token = scan();
				ispara = TRUE;
				if(expressions() == ERROR)
					return(ERROR);
				ispara = FALSE;
				if(token != TRPAREN) {
					sprintf(mesg, "Right parenthesis is not found at line %d", get_linenum());
					return(error(mesg));
				}
				pretty_print(TOKEN);
				token = scan();
			}
		fprintf(fp, "\tCALL\t$%s\n", pn);
		return(NORMAL);
	}

int expressions() {	/* Parse expressions */
	int first;

	get_parap();
	strcpy(procname, name);
	if(parap->paratp == NULL) {
		sprintf(mesg, "'%s' has not parameter at line %d", name, get_linenum());
		return(error(mesg));
	}
	parap = parap->paratp;
	first = token;
	isopr = FALSE;
	if(expression() == ERROR)
		return(ERROR);
	if(parap->ttype != exptype) {
		sprintf(mesg, "Type is invalid at line %d", get_linenum());
		return(error(mesg));
	}
	if(first == TNAME && isopr == FALSE) {
		fprintf(fp, "\tPUSH\t0,gr1\n");
	} else {
		fprintf(fp, "\tLAD\tgr2,L%04d\n\tST\tgr1,0,gr2\n\tPUSH\t0,gr2\n", label);
		register_strlb("");
	}
	while(token == TCOMMA) {
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(parap->paratp == NULL) {
			sprintf(mesg, "Too many arguments to function '%s' at line %d", procname, get_linenum());
			return(error(mesg));
		}
		parap = parap->paratp;
		first = token;
		isopr = FALSE;
		if(expression() == ERROR)
			return(ERROR);
		if(parap->ttype != exptype) {
			sprintf(mesg, "Type is invalid at line %d", get_linenum());
			return(error(mesg));
		}
		if(first == TNAME && isopr == FALSE) {
			fprintf(fp, "\tPUSH\t0,gr1\n");
		} else {
			fprintf(fp, "\tLAD\tgr2,L%04d\n\tST\tgr1,0,gr2\n\tPUSH\t0,gr2\n", label);
			register_strlb("");
		}
	}
	if(parap->paratp != NULL) {
		sprintf(mesg, "Too few arguments to function '%s' at line %d", procname, get_linenum());
		return(error(mesg));
	}
	procname[0] = '\0';
	return(NORMAL);
}

int assignment_statement() {	/* Parse assignment statement */
	int lefttype;

	pretty_print(INDENT);
	if(variable() == ERROR)
		return(ERROR);
	if(ttype != TPINT && ttype != TPCHAR && ttype != TPBOOL) {
		sprintf(mesg, "Type must be standard-type at line %d", get_linenum());
		return(error(mesg));
	}
	lefttype = ttype;
	if(token != TASSIGN) {
		sprintf(mesg, "Colon and equal sign is not found at line %d", get_linenum());
		return(error(mesg));
	}
	pretty_print(SPACE);
	pretty_print(TOKEN);
	pretty_print(SPACE);
	token = scan();
	fprintf(fp, "\tPUSH\t0,gr1\n");
	if(expression() == ERROR)
		return(ERROR);
	if(lefttype != exptype) {
		sprintf(mesg, "Right operand must be same type as left operand at line %d", get_linenum());
		return(error(mesg));
	}
	fprintf(fp, "\tPOP\tgr2\n\tST\tgr1,0,gr2\n");
	return(NORMAL);
}

int variable() {	/* Parse variable */
	int arrelmtype;

	pretty_print(TOKEN);
	strcpy(name, string_attr);
	if(reflinenum_register() == ERROR) {
		return(ERROR);
	}
	if(get_vartype() == ERROR) {
		return(error(mesg));
	}
	token = scan();
	if(token == TLSQPAREN) {
		if(get_arrelmtype() == ERROR) {
			sprintf(mesg, "'%s' is not array type at line %d", name, get_linenum());
			return(error(mesg));
		}
		arrelmtype = ttype;
		pretty_print(TOKEN);
		token = scan();
		fprintf(fp, "\tPUSH\t0,gr1\n");
		if(expression() == ERROR)
			return(ERROR);
		if(exptype != TPINT) {
			sprintf(mesg, "Element type must be integer at line %d", get_linenum());
			return(error(mesg));
		}
		fprintf(fp, "\tCPA\tgr0,gr2\n\tJPL\tL%04d\n\tLAD\tgr2,%d\n\tCPA\tgr1,gr2\n\tJMI\tL%04d\nL%04d\n\tCALL\tEROV\nL%04d\n", label, arraysize, label+1, label, label+1);
		fprintf(fp, "\tPOP\tgr2\n\tADDA\tgr1,gr2\n");
		label = label + 2;
		if(token != TRSQPAREN) {
			sprintf(mesg, "Right square bracket is not found at line %d", get_linenum());
			return(error(mesg));
		}
		pretty_print(TOKEN);
		token = scan();
		ttype = arrelmtype;
	}
	return(NORMAL);
}

int expression() {	/* Parse expression */
	int type;
	int opr;

	type = TPNONE;
	if(simple_expression() == ERROR)
		return(ERROR);
	type = sexptype;
	while(token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
		opr = token;
		isopr = TRUE;
		fprintf(fp, "\tPUSH\t0,gr1\n");
		pretty_print(SPACE);
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(simple_expression() == ERROR)
			return(ERROR);
		if(type != sexptype) {
			sprintf(mesg, "Right operand must be same type as left operand at line %d", get_linenum());
			return(error(mesg));
		}
		type = TPBOOL;
		fprintf(fp, "\tPOP\tgr2\n\tCPA\tgr2,gr1\n");
		switch(opr) {
			case TEQUAL:
				fprintf(fp, "\tJZE\tL%04d\n\tLD\tgr1,gr0\n\tJUMP\tL%04d\nL%04d\n\tLAD\tgr1,1\nL%04d\n", label, label + 1, label, label + 1);
				label = label + 2;
				break;
			case TNOTEQ:
				fprintf(fp, "\tJNZ\tL%04d\n\tLD\tgr1,gr0\n\tJUMP\tL%04d\nL%04d\n\tLAD\tgr1,1\nL%04d\n", label, label + 1, label, label + 1);
				label = label + 2;
				break;
			case TGR:
				fprintf(fp, "\tJPL\tL%04d\n\tLD\tgr1,gr0\n\tJUMP\tL%04d\nL%04d\n\tLAD\tgr1,1\nL%04d\n", label, label + 1, label, label + 1);
				label = label + 2;
				break;
			case TGREQ:
				fprintf(fp, "\tJZE\tL%04d\n\tJPL\tL%04d\n\tLD\tgr1,gr0\n\tJUMP\tL%04d\nL%04d\n\tLAD\tgr1,1\nL%04d\n", label, label, label + 1, label, label + 1);
				label = label + 2;
				break;
			case TLE:
				fprintf(fp, "\tJMI\tL%04d\n\tLD\tgr1,gr0\n\tJUMP\tL%04d\nL%04d\n\tLAD\tgr1,1\nL%04d\n", label, label + 1, label, label + 1);
				label = label + 2;
				break;
			case TLEEQ:
				fprintf(fp, "\tJPL\tL%04d\n\tLAD\tgr1,1\n\tJUMP\tL%04d\nL%04d\n\tLD\tgr1,gr0\nL%04d\n", label, label + 1, label, label + 1);
				label = label + 2;
				break;
			default:
				// not-reach
				exit(EXIT_FAILURE);
				break;
		}
	}
	exptype = type;
	return(NORMAL);
}

int simple_expression() {	/* Parse simple expression */
	int type;
	int opr;
	int isminus;

	isminus = FALSE;
	type = TPNONE;
	if(token == TPLUS || token == TMINUS) {
		if(token == TMINUS) {
			isminus = TRUE;
		}
		pretty_print(TOKEN);
		token = scan();
		isopr = TRUE;
		type = TPINT;
	}
	if(term() == ERROR)
		return(ERROR);
	if(type == TPINT && type != termtype) {
			sprintf(mesg, "Type is invalid at line %d", get_linenum());
		return(error(mesg));
	}
	if(isminus) {
		/* There is a bag concering XOR-operation on simulator??
		fprintf(fp, "\tXOR\tgr1,65535\n\tADDA\tgr1,1\n\tJOV\tEOVF\n");
		*/
		/* So prepare another method */
		fprintf(fp, "\tLAD\tgr2,65535\n\tSUBA\tgr2,gr1\n\tLAD\tgr1,1\n\tADDA\tgr1,gr2\n\tJOV\tEOVF\n");
	}
	type = termtype;
	while(token == TPLUS || token == TMINUS || token == TOR) {
		opr = token;
		isopr = TRUE;
		fprintf(fp, "\tPUSH\t0,gr1\n");
		if(type == TPCHAR) {
			sprintf(mesg, "Type is invalid at line %d", get_linenum());
			return(error(mesg));
		}
		if(type == TPINT && token == TOR) {
			sprintf(mesg, "Type is invalid at line %d", get_linenum());
			return(error(mesg));
		}
		if(type == TPBOOL && token != TOR) {
			sprintf(mesg, "Type is invalid at line %d", get_linenum());
			return(error(mesg));
		}
		pretty_print(SPACE);
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(term() == ERROR)
			return(ERROR);
		if(type != termtype) {
			sprintf(mesg, "Type is invalid at line %d", get_linenum());
			return(error(mesg));
		}
		fprintf(fp, "\tPOP\tgr2\n");
		switch(opr) {
			case TPLUS:
				fprintf(fp, "\tADDA\tgr1,gr2\n\tJOV\tEOVF\n");
				break;
			case TMINUS:
				fprintf(fp, "\tSUBA\tgr2,gr1\n\tJOV\tEOVF\n\tLD\tgr1,gr2\n");
				break;
			case TOR:
				fprintf(fp, "\tOR\tgr1,gr2\n");
				break;
			default:
				// not-reach
				exit(EXIT_FAILURE);
				break;
		}
		type = termtype;
	}
	sexptype = type;
	return(NORMAL);
}

int term() {	/* Parse term */
	int type;
	int opr;

	type = TPNONE;
	if(factor() == ERROR)
		return(ERROR);
	type = facttype;
	while(token == TSTAR || token == TAND || token == TDIV) {
		opr = token;
		isopr = TRUE;
		fprintf(fp, "\tPUSH\t0,gr1\n");
		if(type == TPCHAR) {
			sprintf(mesg, "Type is invalid at line %d", get_linenum());
			return(error(mesg));
		}
		if(type == TPINT && token == TAND) {
			sprintf(mesg, "Type is invalid at line %d", get_linenum());
			return(error(mesg));
		}
		if(type == TPBOOL && token != TAND) {
			sprintf(mesg, "Type is invalid at line %d", get_linenum());
			return(error(mesg));
		}
		pretty_print(SPACE);
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(factor() == ERROR)
			return(ERROR);
		fprintf(fp, "\tPOP\tgr2\n");
		switch(opr) {
			case TSTAR:
				fprintf(fp, "\tMULA\tgr1,gr2\n\tJOV\tEOVF\n");
				break;
			case TAND:
				fprintf(fp, "\tAND\tgr1,gr2\n");
				break;
			case TDIV:
				fprintf(fp, "\tDIVA\tgr2,gr1\n\tJOV\tE0DIV\n\tLD\tgr1,gr2\n");
				break;
			default:
				// not-reach
				exit(EXIT_FAILURE);
				break;
		}
	}
	termtype = type;
	return(NORMAL);
}

int factor() {	/* Parse factor */
	int type;

	type = TPNONE;
	switch(token) {
		case TNAME:
			if(variable() == ERROR)
				return(ERROR);
			if(!(ispara && (token == TCOMMA || token == TRPAREN) && !isopr)) {
				fprintf(fp, "\tLD\tgr1,0,gr1\n");
			}
			type = ttype;
			break;
		case TNUMBER:
		case TFALSE:
		case TTRUE:
			if(token == TNUMBER) {
				fprintf(fp, "\tLAD\tgr1,%d\n", num_attr);
				type = TPINT;
			} else {
				if(token == TFALSE) {
					fprintf(fp, "\tLAD\tgr1,0\n");
				} else {
					fprintf(fp, "\tLAD\tgr1,1\n");
				}
				type = TPBOOL;
			}
			pretty_print(TOKEN);
			token = scan();
			break;
		case TSTRING:
			if(strlen(string_attr) != 1) {
				sprintf(mesg, "String-length must be 1 at line %d", get_linenum());
				return(error(mesg));
			}
			fprintf(fp, "\tLAD\tgr1,%d\n", string_attr[0]);
			pretty_print(STRING);
			token = scan();
			type = TPCHAR;
			break;
		case TLPAREN:
			isopr = TRUE;
			pretty_print(TOKEN);
			token = scan();
			if(expression() == ERROR)
				return(ERROR);
			if(token != TRPAREN) {
				sprintf(mesg, "Right parenthesis is not found at line %d", get_linenum());
				return(error(mesg));
			}
			pretty_print(TOKEN);
			token = scan();
			type = exptype;
			break;
		case TNOT:
			isopr = TRUE;
			pretty_print(TOKEN);
			pretty_print(SPACE);
			token = scan();
			if(term() == ERROR)
				return(ERROR);
			if(termtype != TPBOOL) {
				sprintf(mesg, " Type is invalid at line %d", get_linenum());
				return(error(mesg));
			}
			fprintf(fp, "\tCPA\tgr1,gr0\n\tJZE\tL%04d\n\tLD\tgr1,gr0\n\tJUMP\tL%04d\nL%04d\n\tLAD\tgr1,1\nL%04d\n", label, label + 1, label, label + 1);
			label = label + 2;
			type = termtype;
			break;
		case TINTEGER:
		case TCHAR:
		case TBOOLEAN:
			isopr = TRUE;
			pretty_print(TOKEN);
			if(token == TINTEGER) {
				type = TPINT;
			} else if(token == TCHAR) {
				type = TPCHAR;
			} else {
				type = TPBOOL;
			}
			token = scan();
			if(token != TLPAREN) {
				sprintf(mesg, "Left parenthesis is not found at line %d", get_linenum());
				return(error(mesg));
			}
			pretty_print(TOKEN);
			token = scan();
			if(expression() == ERROR)
				return(ERROR);
			switch(exptype) {
				case TPINT:
					switch(type) {
						case TPINT:
							// Nothing
							break;
						case TPCHAR:
							fprintf(fp, "\tLAD\tgr2,127\n\tAND\tgr1,gr2\n");
							break;
						case TPBOOL:
							fprintf(fp, "\tCPA\tgr1,gr0\n\tJZE\tL%04d\n\tLAD\tgr1,1\nL%04d\n", label, label);
							label++;
							break;
						default:
							//not-reach
							exit(EXIT_FAILURE);
					}
					break;
				case TPCHAR:
					switch(type) {
						case TPINT:
						// Nothing
							break;
						case TPCHAR:
							// Nothing
							break;
						case TPBOOL:
							fprintf(fp, "\tCPA\tgr1,gr0\n\tJZE\tL%04d\n\tLAD\tgr1,1\nL%04d\n", label, label);
							label++;
							break;
						default:
							//not-reach
							exit(EXIT_FAILURE);
					}
					break;
				case TPBOOL:
					switch(type) {
						case TPINT:
							// Nothing
							break;
						case TPCHAR:
							fprintf(fp, "\tLAD\tgr2,127\n\tAND\tgr1,gr2\n");
							break;
						case TPBOOL:
							// Nothing
							break;
						default:
							//not-reach
							exit(EXIT_FAILURE);
					}
					break;
				default:
					//not-reach
					exit(EXIT_FAILURE);
			}
			if(token != TRPAREN) {
				sprintf(mesg, "Right parenthesis is not found at line %d", get_linenum());
				return(error(mesg));
			}
			pretty_print(TOKEN);
			token = scan();
			break;
		default:
			sprintf(mesg, "Factor is invalid at line %d", get_linenum());
			return(error(mesg));
	}
	facttype = type;
	return(NORMAL);
}

int input_statement() {	/* Parse input statement */
	int kind;

	kind = token;
	pretty_print(INDENT);
	pretty_print(TOKEN);
	token = scan();
	if(token == TLPAREN) {
		pretty_print(TOKEN);
		token = scan();
		if(token != TNAME) {
			sprintf(mesg, "Variable name is not found at line %d", get_linenum());
			return(error(mesg));
		}
		strcpy(name, string_attr);
		if(get_vartype() == ERROR) {
			return(error(mesg));
		}
		if(variable() == ERROR)
			return(ERROR);
		if(ttype != TPINT && ttype != TPCHAR) {
			sprintf(mesg, "Type must be integer or char at line %d", get_linenum());
			return(error(mesg));
		}
		if(ttype == TPINT) {
			fprintf(fp, "\tCALL\tREADINT\n");
		} else if(ttype == TPCHAR) {
			fprintf(fp, "\tCALL\tREADCHAR\n");
		} else {
			// not-reach
			exit(EXIT_FAILURE);
		}
		while(token == TCOMMA) {
			pretty_print(TOKEN);
			pretty_print(SPACE);
			token = scan();
			if(token != TNAME) {
				sprintf(mesg, "Variable name is not found at line %d", get_linenum());
				return(error(mesg));
			}
			strcpy(name, string_attr);
			if(get_vartype() == ERROR) {
				return(error(mesg));
			}
			if(variable() == ERROR)
				return(ERROR);
			if(ttype != TPINT && ttype != TPCHAR) {
				sprintf(mesg, "Type must be integer or char at line %d", get_linenum());
				return(error(mesg));
			}
			if(ttype == TPINT) {
				fprintf(fp, "\tCALL\tREADINT\n");
			} else if(ttype == TPCHAR) {
				fprintf(fp, "\tCALL\tREADCHAR\n");
			} else {
				// not-reach
				exit(EXIT_FAILURE);
			}
		}
		if(token != TRPAREN){
			sprintf(mesg, "Right parenthesis is not found at line %d", get_linenum());
			return(error(mesg));
		}
		if(kind == TREADLN) {
			fprintf(fp, "\tCALL\tREADLINE\n");
		}
		pretty_print(TOKEN);
		token = scan();
	}
	return(NORMAL);
}

int output_statement() {	/* Parse output statement */
	int kind;

	kind = token;
	pretty_print(INDENT);
	pretty_print(TOKEN);
	token = scan();
	if(token == TLPAREN) {
		pretty_print(TOKEN);
		if(output_format() == ERROR)
			return(ERROR);
		while(token == TCOMMA) {
			pretty_print(TOKEN);
			pretty_print(SPACE);
			if(output_format() == ERROR)
				return(ERROR);
		}
		if(token != TRPAREN){
			sprintf(mesg, "Right parenthesis is not found at line %d", get_linenum());
			return(error(mesg));
		}
		pretty_print(TOKEN);
		token = scan();
	}
	if(kind == TWRITELN) {
		fprintf(fp, "\tCALL\tWRITELINE\n");
	}
	return(NORMAL);
}

int output_format() {	/* Parse output format */
	token = scan();
	if(token == TSTRING && strlen(string_attr) != 1) {
		fprintf(fp, "\tLAD\tgr1,L%04d\n\tLD\tgr2,gr0\n\tCALL\tWRITESTR\n", label);
		register_strlb(string_attr);
		pretty_print(STRING);
		token = scan();
		return(NORMAL);
	}
	if(expression() == ERROR)
		return(ERROR);
	if(exptype != TPINT && exptype != TPCHAR && exptype != TPBOOL) {
		sprintf(mesg, "Type must be standard-type at line %d", get_linenum());
		return(error(mesg));
	}
	if(token == TCOLON) {
		pretty_print(TOKEN);
		pretty_print(SPACE);
		token = scan();
		if(token != TNUMBER) {
			sprintf(mesg, "Unsigned integer is not found at line %d", get_linenum());
			return(error(mesg));
		}
		fprintf(fp, "\tLAD\tgr2,%d\n", num_attr);
		pretty_print(TOKEN);
		token = scan();
	} else {
		fprintf(fp, "\tLD\tgr2,gr0\n");
	}
	switch(exptype) {
		case TPINT:
			fprintf(fp, "\tCALL\tWRITEINT\n");
			break;
		case TPCHAR:
			fprintf(fp, "\tCALL\tWRITECHAR\n");
			break;
		case TPBOOL:
			fprintf(fp, "\tCALL\tWRITEBOOL\n");
			break;
		default:
			// not-reach
			exit(EXIT_FAILURE);
	}
	return(NORMAL);
}

void pretty_print(int mode) {	/* Pretty-print */
	if(PRETTY_PRINT)
		switch(mode) {
			case TOKEN:
				printf("%s", string_attr);
				break;
			case STRING:
				printf("'%s'", string_attr);
				break;
			case SPACE:
				printf(" ");
				break;
			case NEWLINE:
				printf("\n");
				break;
			case INDENT:
				for(i = 0; i < indent; i++)
					printf("%s", "    ");
				break;
			default:
				error("Mode is invalid value in pretty_print");
				exit(EXIT_FAILURE);
		}
}
