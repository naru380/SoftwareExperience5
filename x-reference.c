/* x-reference.c */
#include "header.h"

char mesg[1024];	/* String of error message */
char typestr[1024];	/* String of procedure or variable type */
char name[], procname[];	/* String of variable & procedure name */
int ttype;	/* Type of variable */
int arraysize;	/* Size of array */
int scope;	/* Scope of procedure or variable */
int ispara;	/* Flag which show whether the procedure has parameters or not */
int isproc;	/* Flag which show whether the variable is procedure or not */
int isarr;	/* Flag which show whether the variable is array or not */
int  exptype, sexptype, termtype, facttype;	/* Types of expression & simple expression & term & factor */


/* <code-generate> */
int paranum;	/* Number of the procedure's parameters */
/* </code-generate> */

struct ID *globalidroot, *localidroot;	/* Pointers to root of global & local symbol tables */
struct ID *idroot;	/* Pointer to root of global symbol table adn local symbol tables Connected*/

/* <code-generate> */
struct ID *iparap;	/* Pointer to beginnig of procedure's parameter id */
/* </code-generate> */

struct TYPE *parap;	/* Pointer to beginning of procedure's parameter type */
struct LOCALIDS *localidroots;	/* List for storaging loval symboltables */

void init_idtab() {	/* Initilize global & local symbol tables and global variables */
	scope = GLOBAL;
	ispara = FALSE;
	isproc = FALSE;
	isarr = FALSE;
	globalidroot = NULL;
	localidroot = NULL;
	localidroots = NULL;
}

struct ID *search_idtab(char *np, int scope) {	/* Search the name pointed by np in the scope */
	struct ID *p;

	if(scope == GLOBAL) {
		for(p = globalidroot; p != NULL; p = p->nextp) {
			if(strcmp(np, p->name) == 0) {
				return(p);
			}
		}
	} else if(scope == LOCAL) {
		for(p = localidroot; p != NULL; p = p->nextp) {
			if(strcmp(np, p->name) == 0) {
				return(p);
			}
		}
	} else {	// ALL
		for(p = localidroot; p != NULL; p = p->nextp) {
			if(strcmp(np, p->name) == 0) {
				return(p);
			}
		}
		for(p = globalidroot; p != NULL; p = p->nextp) {
			if(strcmp(np, p->name) == 0) {
				return(p);
			}
		}
	}
	return(NULL);
}

int id_register() {	/* Register the name of procedure or variable with global or local symbol tables */
	struct ID *p;
	int deflinenum;
	char *cname, *cprocname;

	deflinenum = get_linenum();
	if((p = search_idtab(name, scope)) != NULL) {
		sprintf(mesg, "'%s' declared at line %d", string_attr, get_linenum());
		return(error(mesg));
	} else {
		if((p = (struct ID *)malloc(sizeof(struct ID))) == NULL) {
			sprintf(mesg, "Can not malloc in id_register");
			return(error(mesg));
		}
		if((cname = (char *)malloc(strlen(name)+1)) == NULL) {
			sprintf(mesg, "Can not malloc-2 in id_register");
			return(error(mesg));
		}
		strcpy(cname, name);
		p->name = cname;
		if(procname[0] != '\0') {
			if((cprocname = (char *)malloc(strlen(procname)+1)) == NULL) {
				sprintf(mesg, "Can not malloc-3 in id_register");
				return(error(mesg));
			}
			strcpy(cprocname, procname);
			p->procname = cprocname;
		} else {
			p->procname = NULL;
		}
		p->itp = NULL;
		if(ispara == FALSE) {
			p->ispara = FALSE;
		} else {
			p->ispara = TRUE;
		}
		p->deflinenum = deflinenum;
		p->irefp =NULL;
		if(scope == GLOBAL) {
			p->nextp = globalidroot;
			globalidroot = p;
		} else if(scope == LOCAL) {
			p->nextp = localidroot;
			localidroot = p;
		} else {
			sprintf(mesg, "Global-variable 'scope' is invalid value");
			return(error(mesg));
		}
		return(NORMAL);
	}
}

int type_register() {	/* Register the type of procedure or variable with global or local symbol tables */
	struct ID *p;
	struct TYPE *type, *fpara_type, *arr_type;

	if(scope == GLOBAL) {
		p = globalidroot;
	} else if(scope == LOCAL) {
		p = localidroot;
	} else {
		sprintf(mesg, "Global-variable 'scope' is invalid value");
		return(error(mesg));
	}
	for(; p != NULL; p = p->nextp) {
		if(p->itp == NULL) {
			if((type = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
				sprintf(mesg, "Can not malloc in type_register");
				return(error(mesg));
			}
			type->ttype = ttype;
			if(ttype == TPPROC) {
				parap = type;
			} else if(isproc) {
					if((fpara_type = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
						sprintf(mesg, "Can not malloc-2 in type_register");
						return(error(mesg));
					}
					fpara_type->ttype = ttype;
					parap->paratp = fpara_type;
					parap = fpara_type;
					fprintf(fp, "$%s%%%s\tDC\t0\n", p->name, p->procname);
			} else 	if(isarr) {
				type->ttype = TPARRAY;
				type->arraysize = arraysize;
				if((arr_type = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
					printf(mesg, "Can not malloc-3 in type_register");
					return(error(mesg));
				}
				arr_type->ttype = ttype;
				type->etp = arr_type;
				isarr = FALSE;
				fprintf(fp, "$%s\tDS\t%d\n", p->name, type->arraysize);
			}
			else {
				if(scope == LOCAL) {
					fprintf(fp, "$%s%%%s\tDC\t0\n", p->name, p->procname);
				} else {
					fprintf(fp, "$%s\tDC\t0\n", p->name);
				}
			}
			p->itp = type;
		}
	}
	return(NORMAL);
}

int reflinenum_register() {	/* Register the reference line number of procedure or variable with global or local symbol tables */
	struct ID *p;
	struct LINE *line, *lp;
	int reflinenum;

	reflinenum = get_linenum();
	if((p = search_idtab(name, scope)) == NULL) {
		sprintf(mesg, "'%s' undeclared at line %d", string_attr, get_linenum());
		return(error(mesg));
	} else {
		if((line = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) {
			sprintf(mesg, "Can not malloc in reflinenum_register");
			return(error(mesg));
		}
		line->reflinenum = reflinenum;
		line->nextlinep = NULL;
		if(p->irefp == NULL) {
			p->irefp = line;
		} else {
			for(lp = p->irefp; lp->nextlinep != NULL; lp = lp->nextlinep) {
		}
			lp->nextlinep = line;
		}
	}
	if(p->itp->ttype != TPPROC) {
		fprintf(fp, "\t");
		if(scope == GLOBAL || !(p->ispara)) {
			fprintf(fp, "LAD");
		} else {
			fprintf(fp, "LD");
		}
		fprintf(fp, "\tgr1,$%s", p->name);
		if(p->procname != NULL) {
			fprintf(fp, "%%%s", p->procname);
		}
		fprintf(fp, "\n");
	}
	return(NORMAL);
}

int get_vartype() {	/* Get variable type */
	struct ID *p;

	if((p = search_idtab(name, scope)) == NULL) {
		sprintf(mesg, "'%s' undeclared at line %d", string_attr, get_linenum());
		return(error(mesg));
	}
	ttype = p->itp->ttype;
	return(NORMAL);
}

int get_arrelmtype() {	/* Get type of array-element */
	struct ID *p;

	if((p = search_idtab(name, scope)) == NULL) {
		sprintf(mesg, "'%s' undeclared at line %d", string_attr, get_linenum());
		return(error(mesg));
	}
	if(p->itp->ttype != TPARRAY) {
		sprintf(mesg, "'%s' undeclared at line %d", string_attr, get_linenum());
		return(error(mesg));
	}
	ttype = p->itp->etp->ttype;
	arraysize = p->itp->arraysize;
	return(NORMAL);
}

int get_parap() {	/* Get pointer to beginning of the procedure's parameters */
	int i, n;
	struct ID *p;
	struct TYPE *tp;

	if((p = search_idtab(name, scope)) == NULL) {
		sprintf(mesg, "'%s' undeclared at line %d", string_attr, get_linenum());
		return(error(mesg));
	}
	parap = p->itp;
	for(p = localidroot, n = 0; p != NULL; p = p->nextp, n++) {
		// Nothing
	}
	for(tp = parap->paratp, paranum = 0; tp != NULL; tp = tp->paratp, paranum++) {
		// Nothing
	}
	iparap = localidroot;
	for(i = 0; i < n - paranum; i++) {
		iparap = iparap->nextp;
	}
	return(NORMAL);
}

int storage_localidtabs() {	/* Storage local symbol tables */
	struct LOCALIDS *p;

	if((p = (struct LOCALIDS *)malloc(sizeof(struct LOCALIDS))) == NULL) {
		sprintf(mesg, "Can not malloc in storage_localids()");
		return(error(mesg));
	}
	p->idroot = localidroot;
	p->nextp = localidroots;
	localidroots = p;
	localidroot = NULL;
	return(NORMAL);
}

void connect_idtabs() {	/* Connect global symbol table and local symbol tables */
	struct ID *top, *end, *p;
	struct LOCALIDS *q;
	
	top = end = NULL;
	for(p = globalidroot; p != NULL && p->nextp != NULL; p = p->nextp) {
		// Nothing
	}
	if(globalidroot != NULL) {
		idroot = globalidroot;
		end = p;
	}
	for(q = localidroots; q != NULL; q = q->nextp) {
		for(p = q->idroot; p != NULL && p->nextp != NULL; p = p->nextp) {
			// Nothing
		}
		if(idroot == NULL) {
			idroot = q->idroot;
		}
		if(p != NULL) {
			top = q->idroot;
			end->nextp = top;
			end = p;
		}
	}
}

struct ID *sort_idtab(struct ID *root) {	/* Sort symbol table */
	struct ID *p, *q, *r, *pp, *np;

	if(root == NULL || root->nextp == NULL) {
		return(root);
	}
	for(p = root->nextp, pp = root; p != NULL; p = np) {
		np = pp->nextp->nextp;
		if(strcmp(p->name, root->name) < 0) {
			pp->nextp = pp->nextp->nextp;
			r = p->nextp;
			p->nextp = root;
			root = p;
			np = r;
			continue;
		}
		for(q = root; q->nextp != p; q = q->nextp) {
			if(strcmp(p->name, q->nextp->name) < 0) {
				pp->nextp = pp->nextp->nextp;
				r = q->nextp;
				q->nextp = p;
				p->nextp = r;
				break;
			}
		}
		if(pp->nextp == NULL) {
			break;
		}
		if(pp->nextp == p) {
			pp = pp->nextp;
		}
	}
	return(root);
}

char *get_typestr(struct ID *p) {	/* Get type string for print_xreftab() */
	struct TYPE *pp;

	switch(p->itp->ttype) {
		case TPINT:
			sprintf(typestr, "integer");
			break;
		case TPCHAR:
			sprintf(typestr, "char");
			break;
		case TPBOOL:
			sprintf(typestr, "boolean");
			break;
		case TPARRAY:
			if(p->itp->etp->ttype == TPINT) {
				sprintf(typestr, "array[%d] of integer", p->itp->arraysize);
			} else if(p->itp->etp->ttype == TPCHAR) {
				sprintf(typestr, "array[%d] of char", p->itp->arraysize);
			} else if(p->itp->etp->ttype == TPBOOL) {
				sprintf(typestr, "array[%d] of char", p->itp->arraysize);
			} else {
				//not-reach
				exit(EXIT_FAILURE);
			}
			break;
		case TPPROC:
			sprintf(typestr, "procedure");
			if(p->itp->paratp == NULL) {
				break;
			}
			sprintf(typestr, "%s(", typestr);
			for(pp = p->itp->paratp; pp != NULL; pp = pp->paratp) {
				switch(pp->ttype) {
					case TPINT:
						sprintf(typestr, "%sinteger", typestr);
						break;
					case TPCHAR:
						sprintf(typestr, "%schar", typestr);
						break;
					case TPBOOL:
						sprintf(typestr, "%sboolean", typestr);
						break;
					default:
						//not-reach
						exit(EXIT_FAILURE);
						break;
				}
				if(pp->paratp != NULL) {
					sprintf(typestr, "%s,", typestr);
				}
			}
			sprintf(typestr, "%s)", typestr);
			break;
		default:
			//not-reach
			exit(EXIT_FAILURE);
			break;
	}
	return(typestr);
}

int print_xreftab() {	/* Output the registered data */
	struct ID *p1;
	struct LOCALIDS *p2;
	struct LINE *lp;
	char var_proc_str[MAXSTRSIZE*2];

	printf("%-8s %-30s %-s  %s\n", "Name", "Type", "Def.", "Ref.");
	connect_idtabs();
	idroot = sort_idtab(idroot);
	for(p1 = idroot; p1 != NULL; p1 = p1->nextp) {
		if(p1->procname != NULL) {
			sprintf(var_proc_str, "%s:%s", p1->name, p1->procname);
		} else {
			sprintf(var_proc_str, "%s", p1->name);
		}
		printf("%-8s ", var_proc_str);
		printf("%-30s ", get_typestr(p1));
		printf("%3d | ", p1->deflinenum);
		for(lp = p1->irefp; lp != NULL; lp = lp->nextlinep) {
			printf("%d", lp->reflinenum);
			if(lp->nextlinep != NULL) {
				printf(", ");
			}
		}
		printf("\n");
	}
}

void release_idtab() {	/* Release the data structure */
	struct ID *p, *q;
	struct TYPE *r, *s;
	struct LINE *t, *u;

	for(p = idroot; p != NULL; p = q) {
		free(p->name);
		free(p->procname);
		if(p->itp->ttype == TPARRAY) {
			free(p->itp->etp->etp);
			free(p->itp->etp->paratp);
			free(p->itp->etp);
			free(p->itp->paratp);
		} else if(p->itp->ttype == TPPROC) {
			for(r = p->itp; r != NULL; r = s) {
				free(r->etp);
				s = r->paratp;
				free(r);
			}
		} else {
			free(p->itp->etp);
			free(p->itp->paratp);
		}
		for(t = p->irefp; t != NULL; t = u) {
			u = t->nextlinep;
			free(t);
		}
		q = p->nextp;
		free(p);
	}
	init_idtab();
}
