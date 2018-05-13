#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// choi
int
main(int argc, char *argv[])
{
	int k, n, id;
	double res, i, stride;

	if(argc < 2)
		n = 2;	// default child process num
	else
		n = atoi(argv[1]); // child process num
	if(n < 0 || n > 20)
		n = 2;
	if(argc < 3)
		stride = 0.01;	// default stride
	else	
		stride = atoi(argv[2]);	// stride

	res = 0;
	id = 0;
	for (k = 0; k < n; k++) {
		id = fork();
		if (id < 0)
			printf(1, "%d failed in fork\n", getpid());
		else if (id > 0) { // parent
			printf(1, "parent %d creating child %d\n", getpid(), id);
			wait();
		} else { // child
			printf(1, "Child %d created\n", getpid());
			for (i = 0; i < 10000000.0; i += stride)
				res += 1.52 * 3.65;
			break;
		}
	}

	exit();
}