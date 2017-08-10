#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define MAXBYTES 80

/* personal finance calculations  given amount and interest web cgi version */

/* function prototypes */
void parse_form_data(char *buf, double *a, double *i);
void incore_format(char core[], double f);
void table (double (*pf)(double i, int m, double n), double a, char freq, double i);
void incore_format(char core[], double f);
double md1(double i, int m, double n);
double md2(double i, int m, double n);
double md3(double i, int m, double n);


int main()  /* calculate future value of a series of monthly deposits - given A and i */
{
	int bufsz;	/* data buffer size */
	char *buf;	/* form input data buffer */
	int n;		/* number of years */
    double i;   /* interest rate */
    double a; 	/* amount of each monthly payment */
    char freq;  /* frequency of compounding indicator */
	char amtstr[13];	/* amount display string */
	

	bufsz = atoi(getenv("CONTENT_LENGTH"));
	if (bufsz >= MAXBYTES)
		return(1);	/* positive failure */
	buf = malloc(bufsz * sizeof(char));
	printf("Content-Type:text/html;charset=iso-8859-1\n\n");
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
	printf("\n<style>th {text-align:center;} td {text-align:right;}</style>"); 
	printf("\n</head>");
	printf("\n<body>");
		
	printf("\n<header><p>cgi-bin programming with C</p></header>");

	/* output navigation links */
	printf("\n<div><a href=\"/index.php\">Home</a>");
    printf("| <a href=\"/fvai.html\">Back</a></div>");	
	
	printf("\n<h1>future value of a series of monthly deposits</h1>");
	incore_format(amtstr, a);
 	
	printf("\n<div>A = %s</div>", amtstr);
	printf("\n<div>i = %.2f%%</div>", i * 100); 	
    
	printf("\n<table class = \"right-nowrap\">\n");
	printf("\n <tr> <th>Time period (n)</th>");
   	for (n = 1; n <= 10; ++n) 
        printf("<th>%d</th>", n);
	printf("\n<tr><th>Frequency of compounding</th></tr>");
    table(md1, a, 'A', i);
    table(md1, a, 'S', i);
    table(md1, a, 'Q', i);
    table(md1, a, 'M', i);
    table(md2, a, 'D', i);
    table(md3, a, 'C', i);
	printf("\n</table>");

	/* output navigation links */
	printf("\n<div><a href=\"/index.php\">Home</a>");
    printf(" | <a href=\"/fvai.html\">Back</a></div>");
	
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
		*a = 0;				/* error trap blank and some invalid fields */
	else
		*a = atof(token);	/* amount of monthly deposit */

	/* tokenize i out of istr */
	token = strtok(istr, "=");
	token = strtok(NULL, "=");
	if (token == NULL)
		*i = 0;					/* error trap blank and some invalid fields */
	else
		*i = atof(token) / 100;	/* interest rate */
}

/* incore formatting function */
void incore_format(char core[], double f)
{
	int j, n;
	char tmp[12];

	memset(tmp, '\0', 12);
	memset(core, '\0', 14);

	if (f > 9999999.99) { /* number to big for field error trap */
		strcpy(core, "$************");
		return;
	}
	snprintf(tmp, 12, "%c%-10.2f", '$', f);
	j = 10;
	while (tmp[j] == ' ') {
		tmp[j] = '\0';
		--j;
		if (j < 0) 
			break;
	} 
	memset(core, ' ', 13);
	j = 12;
	n = strlen(tmp) - 1;
	while (tmp[n] != '$') {
		if (j == 6 || j == 2)
			core[j] = ',';
		else {
			core[j] = tmp[n];
			--n;
		}
		--j;
	}
	core[j] = tmp[n];
}

void table (double (*pf)(double i, int m, double n), double a, char freq, double i)
/* table generator - one row of an html row
   (this function accepts a pointer to another function as an argument)

   NOTE: the formal argument

         double (*pf)(double i, int m, double n)

         is a POINTER TO A FUNCTION */

{
    int count;  /* loop counter */
    int m;      /* periods per year */
    double n;   /* no. of years */
    double f;   /* future value */
	char core[14];	

	printf("\n<tr><th>");	/* html table header row start */
    if (freq == 'A') {
        m = 1;
        printf("Annual");
    }
    if (freq == 'S')   {
        m = 2;
        printf("Semiannual");
    }
    if (freq == 'Q')   {
        m = 4;
        printf("Quarterly");
    }
    if (freq == 'M')   {
        m = 12;
        printf("Monthly");
    }
    if (freq == 'D')   {
        m = 360;
        printf("Daily");
    }
    if (freq == 'C')   {
        m = 0;
        printf("Continuously");
    }
	printf("</th>"); 

	/* html table row data */
    for (count = 1; count <= 10; ++count)   {
        f = a * (*pf)(i, m, count); /* access the function passed as a pointer */
		incore_format(core, f);
		printf("<td>%s</td>", core);
    }
	printf("\n</tr>\n");
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
