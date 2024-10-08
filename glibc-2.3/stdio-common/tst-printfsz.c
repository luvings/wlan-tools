/* Based on code by Larry McVoy <lm@neteng.engr.sgi.com>.  */
#include <printf.h>
#include <stdio.h>
#include <string.h>

#define V       12345678.12345678


int main(int argc, char *argv[])
{
	char buf[1024];
	int result = 0;

	/* Register the printf handlers.  */
	register_printf_function('b', printf_size, printf_size_info);
	register_printf_function('B', printf_size, printf_size_info);


	sprintf(buf, "%g %b %B %.0b %.0B %.1b %.1B %8.0b %08.0B",
	        V, 1025., V, V, V, V, V, V, V, V);
	fputs(buf, stdout);

	if (strcmp(buf, "\
1.23457e+07 1.001k 12.346M 12m 12M 11.8m 12.3M      12m 0000012M")) {
		result = 1;
		fputs(" -> WRONG\n", stdout);

	} else
		fputs(" -> OK\n", stdout);

	sprintf(buf, "%b|%B|%-20.2b|%-10.0b|%-10.8b|%-10.2B|",
	        V, V, V, V, V, V, V, V, V, V, V);
	fputs(buf, stdout);

	if (strcmp(buf, "\
11.774m|12.346M|11.77m              |12m       |11.77375614m|12.35M    |")) {
		result = 1;
		fputs(" -> WRONG\n", stdout);

	} else
		fputs(" -> OK\n", stdout);

	sprintf(buf, "%#.0B %*.0b %10.*b %*.*B %10.2B",
	        V, 2, V, 2, V, 10, 2, V, V);
	fputs(buf, stdout);

	if (strcmp(buf, "12.M 12m     11.77m     12.35M     12.35M")) {
		result = 1;
		fputs(" -> WRONG\n", stdout);

	} else
		fputs(" -> OK\n", stdout);

	sprintf(buf, "%6B %6.1B %b %B %b %B",
	        V, V, 1000.0, 1000.0, 1024.0, 1024.0);
	fputs(buf, stdout);

	if (strcmp(buf, "12.346M  12.3M 1000.000  1.000K 1.000k 1.024K")) {
		result = 1;
		fputs(" -> WRONG\n", stdout);

	} else
		fputs(" -> OK\n", stdout);

	return result;
}
