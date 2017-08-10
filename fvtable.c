#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define NUMROWS 50
#define NUMCOLS 6
#define MAXBYTES 80

/* enum type definition */
enum compounding { A, S, Q, M, D, C };
typedef enum compounding compounding;

/* function prototypes */
void parse_form_data(char *buf, double *a, double *i);
void incore_format(char core[], double f);
void get_header(char header[], compounding period);
void get_column(double table[][NUMCOLS], 
		        double (*pf)(double i, int m, double n), 
				double a, compounding period, double i);
double md1(double i, int m, double n);
double md2(double i, int m, double n);
double md3(double i, int m, double n);

/* personal finance calculations */

int main()  /* calculate future value of a series of monthly deposits */
{
   int n;  						/* table row counter */
   double i;    					/* interest rate */
   double a;    					/* amount of each monthly payment */
	int bufsz;						/* data buffer size */
	char *buf;						/* form input data buffer */
   char header[16];				/* table header */
   char incore[16];			    /* U. S. currency formatted output buffer */
   double table[NUMROWS][NUMCOLS]; /* future value table */
	compounding period;				/* enum variable */

	/* pointer to function declaration */
	double (*pf)(double i, int m, double n);

	/* get cgi-bin stdin POST form input data */
	bufsz = atoi(getenv("CONTENT_LENGTH"));
	if (bufsz >= MAXBYTES)
		return(1);	/* positive failure */
	printf("Content-Type:text/html;charset=iso-8859-1\n\n");
	buf = malloc(bufsz * sizeof(char));
	fread(buf, bufsz, 1, stdin);

	parse_form_data(buf, &a, &i);

	free(buf);

	/* error trap */
	if (a <= 0 || a > 1000000 || i <= 0 || i > 0.5) {
		printf("ERROR - processing terminated");
		return(1);
	}		
	
   printf("\n<!DOCTYPE html>");
	printf("\n<html>");
	printf("\n<head>");
	printf("\n<meta charset=\"US-ASCII\">");
	printf("\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
	printf("\n<link rel=\"stylesheet\" media=\"all\"  href=\"/includes/style.css\">");
	printf("\n</head>");
	printf("\n<body>");
	
	printf("\n<header><p>cgi-bin programming with C</p></header>");
	
	/* output navigation links */
	printf("\n<div><a href=\"/index.php\">Home</a>");
   printf("| <a href=\"/fvtable.html\">Back</a></div>");

	printf("\n<h1>future value table</h1>");	

	incore_format(incore, a);
	printf("\n<div>A = %s</div>", incore);
   printf("\n<div>i = %.2f%%</div>", 100 * i); 
    
	/* print html table headers */
	printf("\n<table class = \"right-nowrap\">\n"); 
	printf("\n<tr><th>n</th>");
	for (period = A; period < NUMCOLS; ++period) {
 		get_header(header, period);
		printf("<th>%s</th>", header); 
    }
	printf("</tr>");

   /* compute each column */
   for (period = A; period < NUMCOLS; ++period) {
		if (period == A || period == S || period == Q)
	  		pf = &md1;
		else if (period == D)
	  		pf = &md2;
		else if (period == C)
      	pf = &md3;
  		get_column(table, pf, a, period, i);
 	}
 	/* print table using incore formatting - then out to the world */
 	for (n = 0; n < NUMROWS; ++n) {
		printf("\n<tr><td class = \"right\">%d</td>", n + 1);
  		for (period = A; period < NUMCOLS; ++period) {
			incore_format(incore, table[n][period]);
			printf("<td class = \"right\">%s</td>", incore);
  		}
		printf("</tr>");
 	}
	printf("\n</table>");

	/* output navigation links */
	printf("\n<div><a href=\"/index.php\">Home</a>");
   printf(" | <a href=\"/fvtable.html\">Back</a></div>");
	
	printf("\n<footer><p>CopyLeft 2016 Josh Roybal - all wrongs reserved</p></footer>");
   printf("\n</body>");
	printf("\n</html>\n");
   return(0);
}

void parse_form_data(char *buf, double *a, double *i)
{
	char *token;
	char *astr;
	char *istr;

	/* tokenize amount=var & r=var out of buf */	
	astr = strtok(buf, "&");
	istr = strtok(NULL, "&");

	/* tokenize a out of astr */
	token = strtok(astr, "=");
	token = strtok(NULL, "=");
	if (token == NULL)
		*a = 0;	/* error trap blank and some other invalid fields */
	else
		*a = atof(token);	/* amount of monthly deposit */

	/* tokenize i out of istr */
	token = strtok(istr, "=");
	token = strtok(NULL, "=");
	if (token == NULL)
		*i = 0;	/* error trap blank and some other invalid fields */
	else
		*i = atof(token) / 100;	/* interest rate */
}

/* incore formatting function */
void incore_format(char core[], double f)
{
	int j, n;
	char tmp[16];

	memset(tmp, '\0', 16);
	memset(core, '\0', 16);

	if (f > 999999999.99) { /* number to big for field error trap */
		strcpy(core, "$**************");
		return;
	}
	snprintf(tmp, 16, "%c%-14.2f", '$', f);
	j = 14;
	while (tmp[j] == ' ') {
		tmp[j] = '\0';
		--j;
		if (j < 0) 
			break;
	} 
	memset(core, ' ', 15);
	j = 14;
	n = strlen(tmp) - 1;
	while (tmp[n] != '$') {
		if (j == 8 || j == 4)
			core[j] = ',';
		else {
			core[j] = tmp[n];
			--n;
		}
		--j;
	}
	core[j] = tmp[n];

	n = 0;
	for (j = 0; j < 15 && core[n] != '\0'; ++j)
		if (core[j] != ' ') {
			tmp[n] = core[j];
			++n;
		}
	strcpy(core, tmp);
}

void get_header(char header[], compounding period)
{
	switch(period) {
 		case A:
  			strcpy(header, "Annual");
  			break;
 		case S:
  			strcpy(header, "Semiannual");
  			break;
 		case Q:
  			strcpy(header, "Quarterly");
  			break;
 		case M:
  			strcpy(header, "Monthly");
  			break;
 		case D:
  			strcpy(header, "Daily");
  			break;
 		case C:
  			strcpy(header, "Continuous");
  			break;
 		default:
  			strcpy(header, "ERROR");
 	}
}

void get_column (double table[][NUMCOLS], 
		            double (*pf)(double r, int m, double n), 
				      double a, compounding period, double i)
/* table jth column generator
   (this function accepts a pointer to another function as an argument)

   NOTE: the formal argument

         double (*pf)(double i, int m, double n)

         is a POINTER TO A FUNCTION */
{
    int n;   	/* row counter */
    int m;   	/* periods per year */

	/* period equals the kth column of the future value table */
	switch (period) {
		case A:	/* Annual compounding */
  			m = 1;
  			break;
 		case S: /* Semiannual compounding */
  			m = 2;
  			break;
 		case Q: /* Quarterly compounding */
  			m = 4;
  			break;
 		case M: /* Monthly compounding */
  			m = 12;
  			break;
 		case D: /* Daily compounding */
  			m = 360;
  			break;
 		case C: /* Continuous compounding */
  			m = 0;
  			break;
 		default:
  			printf("\n ERROR\n");
	}

    for (n = 0; n < NUMROWS; ++n)
  		table[n][period] = a * (*pf)(i, m, n + 1);
}

double md1(double i, int m, double n)
/* monthly deposits, periodic compounding */

{
    double factor, ratio;

    factor = 1 + (i / m);
    ratio = 12 * (pow(factor, m * n) - 1) / i;
    return(ratio);
}

double md2(double i, int m, double n)
/* monthly deposits, daily compounding */

{
    double factor, ratio;

    factor = 1 + (i / m);
    ratio = (pow(factor, m * n) - 1) / (pow(factor, m / 12) - 1);
    return(ratio);
}

double md3(double i, int dummy, double n)
/* monthly deposits, continuous compounding */

{
    double ratio;

    ratio = (exp(i * n) - 1) / (exp(i / 12) - 1);
    return(ratio);
}
