/* main.c  */
#include "header.h"

//#define PRINT_X_REFERENCE_TABLE

int main(int nc, char *np[]) { /* main routine */
    if(nc < 2) {
		printf("File name id not given.\n");
		return 0;
    }
    if(init_scan(np[1]) < 0) { /* Initialize the scanner */
		printf("File %s can not open.\n", np[1]);
		return 0;
    }

	/* Initialize the cross-reference table */
	init_idtab();

	/* Initilize object-code generater */
	if(init_generate(np[1]) == ERROR) {
		return(ERROR);
	}

	/* Get first token and start parsing */
    token = scan();
	if(parse_program() == ERROR)
		return(ERROR);

#ifdef PRINT_X_REFERENCE_TABLE
	/* Display cross-reference table */
	if(print_xreftab() == ERROR) {
		return(ERROR);
	}
#endif

	/* End scanning */
    end_scan();

	/* Release memory for cross-reference table */
	release_idtab();

	/* End object-code generater */
	end_generate();

    return(NORMAL);
}

