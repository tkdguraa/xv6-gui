#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// choi
int
main(int argc, char *argv[])
{
  int pid, pr;
	if (argc < 3) {
		printf(1, "not enough arguments\n");
		exit();
	}

	pid = atoi(argv[1]);
	pr = atoi(argv[2]);

	if (pr < 0 || pr > 20) {
		printf(1, "wrong priority value (0-20)");
		exit();
	}

	chpr(pid, pr);
	exit();
}