#include "../src/utils.h"

int main(int argc, char *argv[])
{
	struct nl_sock *h[1025];
	int i;

	h[0] = nl_handle_alloc();
	printf("Created handle with port 0x%x\n",
	       nl_socket_get_local_port(h[0]));
	nl_handle_destroy(h[0]);
	h[0] = nl_handle_alloc();
	printf("Created handle with port 0x%x\n",
	       nl_socket_get_local_port(h[0]));
	nl_handle_destroy(h[0]);

	for (i = 0; i < 1025; i++) {
		h[i] = nl_handle_alloc();

		if (h[i] == NULL)
			nl_perror("Unable to allocate socket");

		else
			printf("Created handle with port 0x%x\n",
			       nl_socket_get_local_port(h[i]));
	}

	return 0;
}
