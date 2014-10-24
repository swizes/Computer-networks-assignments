#include "main.h"

int main(void) {
	loop_client();
	return 0;
}

void errorHandler(char * errMessage) {
	perror(errMessage);
	exit(1);
}
