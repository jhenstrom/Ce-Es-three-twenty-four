#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char *argv[]) {
	int pid;
	int returnstatus;
	FILE *f;
	int pvar[2];

	f = fopen("fork-output.txt", "w");
	fprintf(f, "BEFORE FORK\n");
	printf("Starting program; process has pid %d\n", getpid());
	pipe(pvar);
	

	if ((pid = fork()) < 0) {
		fprintf(stderr, "Could not fork()");
		exit(1);
	}

	/* BEGIN SECTION A */
	fprintf(f, "SECTION A\n");

	printf("Section A;  pid %d\n", getpid());
	//sleep(30);

	/* END SECTION A */
	if (pid == 0) {
		/* BEGIN SECTION B */
		close(pvar[0]);
		FILE *fd = fdopen(pvar[1], "w");
		fputs("hello from Section B\n", fd);
		fprintf(f, "SECTION B\n");
		printf("Section B\n");
		//sleep(30);
		//sleep(30);
		printf("Section B done sleeping\n");

		char *newenviron[] = { NULL };

		printf("Program \"%s\" has pid %d. Sleeping.\n", argv[0], getpid());

		if (argc <= 1) {
			printf("No program to exec.  Exiting...\n");
			exit(0);
		}

		printf("Running exec of \"%s\"\n", argv[1]);
		dup2(fileno(f), fileno(stdout));
		execve(argv[1], &argv[1], newenviron);
		printf("End of program \"%s\".\n", argv[0]);

		exit(0);

		/* END SECTION B */
	} else {
		/* BEGIN SECTION C */
		
		waitpid(pid, &returnstatus, 0);
		close(pvar[1]);
		FILE *fs = fdopen(pvar[0], "r");
		int max = 50;
		char *temp = malloc(max);		
		fgets(temp, max, fs);
		printf("%s", temp);
		fprintf(f, "SECTION C\n");
		printf("Section C\n");
		//sleep(30);
		//sleep(30);		
		printf("Section C done sleeping\n");

		exit(0);

		/* END SECTION C */
	}
	/* BEGIN SECTION D */

	fprintf(f, "SECTION D\n");
	printf("Section D\n");
	//sleep(30);

	/* END SECTION D */
}
