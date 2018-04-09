#include "types.h"
#include "user.h"

static int alive = 3;

void
loop_child() {
	for(;;)
		sleep(500);
}

void
loop_parent(int pids[]) {
	int child;
	char buf[10];
	int flag;

	while (alive > 0) {
		flag = 1;
		printf(2, "Input a child id (0~2):\n");
		gets(buf, 10);
		switch(buf[0]) {
			case '0': child = 0; break;
			case '1': child = 1; break;
			case '2': child = 2; break;
			default: flag = 0; break;
		}

		if (flag && pids[child] == -1) {
			flag = 0;
			printf(2, "the process already dead.\n");
		}

		if (flag) {
			printf(2, "What signal to send: \n");
			gets(buf, 10);
			if (buf[1] == 10) {
				switch(buf[0]) {
					case '0': sigsend(pids[child], 0); pids[child] = -1; wait(); break;
					case '1': sigsend(pids[child], 1); break;
					case '2': sigsend(pids[child], 2); break;
					case '3': sigsend(pids[child], 3); break;
				}
			}
		}
	}
	return;
}

void
mySigchild() {
	alive--;
	printf(2, "in mySigchild. alive=%d\n", alive);
}

void
mySig3() {
	printf(2, "in mySig3 pid=%d\n", getpid());
}

int
main(int argc, char *argv[]) {
	int pids[3];

	signal(2, &mySigchild);
	signal(3, &mySig3);

	int cpid, i;
	for (i = 0; i < 3; i++) {
		if ((cpid = fork()) == 0) {
			printf(2, "child %d is inside loop, pid=%d\n", i, getpid());
			loop_child();
		}
		pids[i] = cpid;
	}

	loop_parent(pids);
	exit();
}