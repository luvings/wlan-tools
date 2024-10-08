#include <dlfcn.h>
#include <stdio.h>

int main(void)
{
	int result = 0;

	/* First try to load an object which is a dependency.  This should
	   succeed.  */
	if (dlopen("testobj1.so", RTLD_LAZY | RTLD_NOLOAD) == NULL) {
		printf("cannot open \"testobj1.so\": %s\n", dlerror());
		result = 1;

	} else
		puts("loading \"testobj1.so\" succeeded, OK");

	/* Now try loading an object which is not already loaded.  */
	if (dlopen("testobj5.so", RTLD_LAZY | RTLD_NOLOAD) != NULL) {
		puts("succeeded in loading \"testobj5.so\"");
		result = 1;

	} else {
		/* Load the object and run the same test again.  */
		void *p;

		puts("\"testobj5.so\" wasn't loaded and RTLD_NOLOAD prevented it, OK");

		p = dlopen("testobj5.so", RTLD_LAZY);

		if (p == NULL) {
			printf("cannot open \"testobj5.so\" without RTLD_NOLOAD: %s\n",
			       dlerror());
			result = 1;

		} else {
			puts("loading \"testobj5.so\" succeeded, OK");

			if (dlopen("testobj5.so", RTLD_LAZY | RTLD_NOLOAD) == NULL) {
				printf("cannot open \"testobj5.so\": %s\n", dlerror());
				result = 1;

			} else
				puts("loading \"testobj5.so\" with RTLD_NOLOAD succeeded, OK");

			if (dlclose(p) != 0) {
				printf("cannot close \"testobj5.so\": %s\n", dlerror());
				result = 1;

			} else
				puts("closing \"testobj5.so\" succeeded, OK");
		}
	}

	return result;
}


extern int foo(int a);
int foo(int a)
{
	return 42 + a;
}
