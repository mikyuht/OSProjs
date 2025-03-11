#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
	int parent_to_child[2];
	int child_to_parent[2];
	char buf[1];

	if (pipe(parent_to_child) < 0){
		fprintf(2, "pipe parent_to_child failed\n");
		exit(1);
	}

	if (pipe(child_to_parent) < 0){
		fprintf(2, "pipe child_to_parent failed\n");
		exit(1);
	}

	int pid = fork();
	if (pid < 0){
		fprintf(2, "fork failed\n");
		exit(1);
	}

	if (pid == 0){
		close(parent_to_child[1]);
		close(child_to_parent[0]);
	
		if (read(parent_to_child[0], buf, 1) != 1){
			fprintf(2, "child: read failed\n");
			exit(1);
		}
		
		printf("%d: received ping\n", getpid());

		if (write(child_to_parent[1], buf, 1) != 1){
			fprintf(2, "child: write failed\n");
			exit(1);
		}

		close(parent_to_child[0]);
		close(child_to_parent[1]);
		exit(0);
	} else {
		close(parent_to_child[0]);
		close(child_to_parent[1]);

		buf[0] = 'x';
		if (write(parent_to_child[1], buf, 1) != 1){
			fprintf(2, "parent: write failed\n");
			exit(1);
		}
		
		if (read(child_to_parent[0], buf, 1) != 1){
			fprintf(2, "parent: read failed\n");
			exit(1);
		}

		printf("%d: received pong\n", getpid());

		close(parent_to_child[1]);
		close(child_to_parent[0]);
		
		wait(0);
		exit(0);
	}
	
	return 0;

}




