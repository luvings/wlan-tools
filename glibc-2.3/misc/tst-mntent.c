/* Test case by Horst von Brand <vonbrand@sleipnir.valparaiso.cl>
   and Ulrich Drepper <drepper@cygnus.com>.  */
#include <mntent.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
	int result = 0;
	struct mntent mef;
	struct mntent *mnt = &mef;
	char *name;
	FILE *fp;

	mef.mnt_fsname = strdupa("/dev/hda1");
	mef.mnt_dir = strdupa("/some dir");
	mef.mnt_type = strdupa("ext2");
	mef.mnt_opts = strdupa("defaults");
	mef.mnt_freq = 1;
	mef.mnt_passno = 2;

	if (hasmntopt(mnt, "defaults"))
		printf("Found!\n");

	else {
		printf("Didn't find it\n");
		result = 1;
	}

	name = tmpnam(NULL);
	fp = fopen(name, "w+");

	if (fp == NULL) {
		printf("Cannot open temporary file: %m\n");
		result = 1;

	} else {
		char buf[1024];

		/* Write the name entry.  */
		addmntent(fp, &mef);

		/* Prepare for reading.  */
		rewind(fp);

		/* First, read it raw.  */
		if (fgets(buf, sizeof(buf), fp) == NULL) {
			printf("Cannot read temporary file: %m");
			result = 1;

		} else if (strcmp(buf, "/dev/hda1 /some\\040dir ext2 defaults 1 2\n") != 0) {
			puts("Raw file data not correct");
			result = 1;
		}

		/* Prepare for reading, part II.  */
		rewind(fp);

		/* Now read it cooked.  */
		mnt = getmntent(fp);

		if (strcmp(mnt->mnt_fsname, "/dev/hda1") != 0
		                || strcmp(mnt->mnt_dir, "/some dir") != 0
		                || strcmp(mnt->mnt_type, "ext2") != 0
		                || strcmp(mnt->mnt_opts, "defaults") != 0
		                || mnt->mnt_freq != 1
		                || mnt->mnt_passno != 2) {
			puts("Error while reading written entry back in");
			result = 1;
		}

		remove(name);
	}

	return result;
}
