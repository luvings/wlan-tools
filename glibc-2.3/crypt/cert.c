
/*
 * This crypt(3) validation program shipped with UFC-crypt
 * is derived from one distributed with Phil Karns PD DES package.
 *
 * @(#)cert.c	1.8 11 Aug 1996
 */

#include <stdio.h>
#include <stdlib.h>
#include "crypt.h"

int totfails = 0;

#if __STDC__ - 0
int main(int argc, char *argv[]);
void get8(char *cp);
void put8(char *cp);
void good_bye(void) __attribute__((noreturn));
#else
void get8(), put8();
#endif

void good_bye()
{
	if (totfails == 0) {
		printf("Passed DES validation suite\n");
		exit(0);

	} else {
		printf("%d failures during DES validation suite!!!\n", totfails);
		exit(1);
	}
}

int main(argc, argv)
int argc;
char *argv[];
{
	char key[64], plain[64], cipher[64], answer[64];
	int i;
	int test;
	int fail;

	for (test = 0; !feof(stdin); test++) {

		get8(key);
		printf(" K: ");
		put8(key);
		setkey(key);

		get8(plain);
		printf(" P: ");
		put8(plain);

		get8(answer);
		printf(" C: ");
		put8(answer);

		for (i = 0; i < 64; i++)
			cipher[i] = plain[i];

		encrypt(cipher, 0);

		for (i = 0; i < 64; i++)
			if (cipher[i] != answer[i])
				break;

		fail = 0;

		if (i != 64) {
			printf(" Encrypt FAIL");
			fail++;
			totfails++;
		}

		encrypt(cipher, 1);

		for (i = 0; i < 64; i++)
			if (cipher[i] != plain[i])
				break;

		if (i != 64) {
			printf(" Decrypt FAIL");
			fail++;
			totfails++;
		}

		if (fail == 0)
			printf(" OK");

		printf("\n");
	}

	good_bye();
}

void get8(cp)
char *cp;
{
	int i, j, t;

	for (i = 0; i < 8; i++) {
		scanf("%2x", &t);

		if (feof(stdin))
			good_bye();

		for (j = 0; j < 8 ; j++) {
			*cp++ = (t & (0x01 << (7 - j))) != 0;
		}
	}
}

void put8(cp)
char *cp;
{
	int i, j, t;

	for (i = 0; i < 8; i++) {
		t = 0;

		for (j = 0; j < 8; j++)
			t = (t << 1) | *cp++;

		printf("%02x", t);
	}
}
