#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[]) {
	if(argc < 2){
    printf(2, "Usage: test signal...\n");
    exit();
  }

	int proc_num = atoi(argv[1]);
	int pid, i;

	for (i = 0; i < proc_num; i++) {
		if ((pid = fork()) < 0) // Fork failed
			printf(2, "Fork failed\n");
		else if (pid == 0) {		// Child process
			printf(2, "Child Process was created");
			sleep(10 * 1000);
			sigsend(getpid(), 0);
		}
	}

	for(; i > 0; i--){
    if(wait() < 0){
      printf(2, "Wait stopped early\n");
      exit();
    }
  }

	exit();
}