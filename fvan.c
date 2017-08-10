#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define MAXBYTES 80 

/* function prototypes */
void parse_form_data(char *buf, double *amt, int *n);
void incore_format(char core[], double f);
void table (double (*pf)(double r, int m, double n), double a, char freq, double r);
double md1(double r, int m, double n);
double md2(double r, int m, double n);
double md3(double r, int m, double n);

int main()
{
   int bufsz;        /* data buffer size */
   char buf[MAXBYTES];  /* form input data buffer */
   char core[16];    /* incore formatting buffer */
   int n;            /* number of years */
   int i;            /* interest rate */
   double amt;       /* amount of each monthly payment */

   bufsz = atoi(getenv("CONTENT_LENGTH"));
   if (bufsz >= MAXBYTES)
      return(1);  /* positive failure */
   printf("Content-Type:text/html;charset=iso-8859-1\n\n");
   fread(buf, bufsz, 1, stdin);

   parse_form_data(buf, &amt, &n);

   /* error trap */
   if (amt <= 0 || amt > 1000000 || n <= 0 || n > 50) {
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
   printf("| <a href=\"/fvan.html\">Back</a></div>"); 
   
   printf("\n<h1>future value table</h1>");  
   incore_format(core, amt);
   printf("\n<div>A = %s</div>", core);   
   printf("\n<div>n = %d</div>", n);   

   printf("\n<table class = \"right-nowrap\">\n");
   printf("\n<tr><th>Interest rate</th>");
   for (i = 5; i <= 15; ++i) 
      printf("<th> %10d%%</th>", i);
   printf("</tr>\n");
   printf("\n<tr><th>Frequency of compounding</th></tr>");
   table(md1, amt, 'A', n);
   table(md1, amt, 'S', n);
   table(md1, amt, 'Q', n);
   table(md1, amt, 'M', n);
   table(md2, amt, 'D', n);
   table(md3, amt, 'C', n);
   printf("\n</table>");

   /* output navigation links */
   printf("\n<div><a href=\"/index.php\">Home</a>");
   printf(" | <a href=\"/fvan.html\">Back</a></div>");
   
   printf("\n<footer><p>CopyLeft 2016 Josh Roybal - all wrongs reserved</p></footer>");
   printf("\n</body>");
   printf("\n</html>");
   return(0);
}

void parse_form_data(char *buf, double *amt, int *n)
{
   char *token;
   char *amtstr;
   char *nstr;

   /* tokenize amount=var & r=var out of buf */ 
   amtstr = strtok(buf, "&");
   nstr = strtok(NULL, "&");


   /* tokenize amt out of amtstr */
   token = strtok(amtstr, "=");
   token = strtok(NULL, "=");
   if (token == NULL) 
      *amt = 0;   /* error traps blank field */
   else
      *amt = atof(token);

   /* tokenize r out of istr */
   token = strtok(nstr, "=");
   token = strtok(NULL, "=");
   if (token == NULL)
      *n = 0;     /* error traps blank field */
   else
      *n = atoi(token);
}

void table (double (*pf)(double i, int m, double n),
                         double a, char freq, double n)
/* table generator
   (this function accepts a pointer to another function as an argument)

   NOTE: the formal argument

         double (*pf)(double i, int m, double n)

         is a POINTER TO A FUNCTION */
{
   int count;  /* loop counter */
   int m;      /* periods per year */
   int j;
   double i;   /* annual interest rate */
   double f;   /* future value */
   char dollar_sign[] = "$";
   char core[14]; 

   printf("\n<tr><th>");
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
      printf("Continuous");
   }
   printf("</th> ");

   for (count = 5; count <= 15; ++count)   {
      i = 0.01 * count;
      f = a * (*pf)(i, m, n); /* ACCESS THE FUNCTION PASSED AS A POINTER */
      incore_format(core, f);
      printf("<td>%s</td>", core);

    }
    printf("\n</tr>\n");
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
