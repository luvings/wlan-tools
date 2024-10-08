#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct _Buffer {
	char *buff;
	int  room, used;
} Buffer;

#if __STDC__ - 0
void InitBuffer(Buffer *b);
void AppendToBuffer(register Buffer *b, const char *str, register int len);
void ReadFile(register Buffer *buffer, FILE *input);
#endif

#define INIT_BUFFER_SIZE 10000

void InitBuffer(b)
Buffer *b;
{
	b->room = INIT_BUFFER_SIZE;
	b->used = 0;
	b->buff = (char *)malloc(INIT_BUFFER_SIZE * sizeof(char));
}

void AppendToBuffer(b, str, len)
register Buffer *b;
const char *str;
register int len;
{
	while (b->used + len > b->room) {
		b->buff = (char *)realloc(b->buff, 2 * b->room * (sizeof(char)));
		b->room *= 2;
	}

	strncpy(b->buff + b->used, str, len);
	b->used += len;
}

void ReadFile(buffer, input)
register Buffer *buffer;
FILE *input;
{
	char       buf[BUFSIZ + 1];
	register int        bytes;

	buffer->used = 0;

	while (!feof(input) && (bytes = fread(buf, 1, BUFSIZ, input)) > 0) {
		AppendToBuffer(buffer, buf, bytes);
	}

	AppendToBuffer(buffer, "", 1);
}

int main(int argc, char *argv[])
{
	char filename[] = "xbug.c";
	FILE *input;
	Buffer buffer;

	InitBuffer(&buffer);

	if (!freopen(filename, "r", stdin))
		fprintf(stderr, "cannot open file\n");

	unsetenv("LD_LIBRARY_PATH");

	if (!(input = popen("/bin/cat", "r")))
		fprintf(stderr, "cannot run \n");

	ReadFile(&buffer, input);
	pclose(input);

	return 0;
}
