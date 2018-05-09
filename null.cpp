#include "null.h"

int main (int argc, char* argv[]) {

	// variable declarations
	pid_t kid[2];
	int status = 0,
			cnt = 0;
	std::istringstream cmd;
	std::string input,
							arg,
							lhsCmd;
	char* lhsArgs[ARGSIZE];
	char*	rhsArgs[ARGSIZE];
	int pfd[2];

	// unbuffer output using unitbuf
	std::cout << std::unitbuf;

	// primary process
	while (lhsCmd != "q" && lhsCmd != "quit") {

		// istringstream refresh
		cmd.str("");
		cmd.clear();
		cnt = 0;

		// get input
		std::cout << PROMPT << " ";
		getline(std::cin, input);

		// begin tokenization
		cmd.str(input);

		while (cmd >> arg && cnt <= ARGSIZE) {
				// build lhs command
				lhsArgs[cnt] = new char(arg.length() + 1);
				strcpy(lhsArgs[cnt], arg.c_str());
				lhsArgs[cnt + 1] = nullptr;

				// checks for pipe
				if (arg == "|") {
					lhsArgs[cnt] = nullptr;
					cnt = 0;
					while (cmd >> arg && cnt <= ARGSIZE) {
						// build rhs command
						rhsArgs[cnt] = new char(arg.length() + 1);
						strcpy(rhsArgs[cnt], arg.c_str());
						rhsArgs[cnt + 1] = nullptr;

						cnt++;
					}
				}

			cnt++;
		}

		if (cnt > 0) {
			lhsCmd = lhsArgs[0];
		}

		// run commands if quit is not called
		if (lhsCmd != "q" && lhsCmd != "quit") {

			// create pipe
			if (pipe(pfd) == -1) {
				std::cerr << "pipe() failed!" << std::endl;
				exit(-1);
			}

			// parent forks first child
			kid[0] = fork();

			// checks for pipe
			if (cmd.str().find("|") > 0 && cmd.str().find("|") <= cmd.str().length()) {

				if (kid[0] == 0) { // first child process

					// close stdio out and replace with pipe; closes unneeded file descriptor
					close(1);
					dup(pfd[1]);
					close(pfd[1]);
					close(pfd[0]);

					// executes command in first child
					if (kid[0] == 0) {
						if (execvp(lhsArgs[0], lhsArgs) < 0) {
							std::cerr << "Couldn't execute '" << lhsArgs[0] << "'!" << std::endl;
							exit(127);
						}
					}

				} else if (kid[0] > 0) { // parent process

					// parent forks second child
					kid[1] = fork();

					if (kid[1] == 0) { // second child process

						// close stdio in and replace with pipe; closes unneeded file descriptor
						close(0);
						dup(pfd[0]);
						close(pfd[0]);
						close(pfd[1]);

						// execute command
						if (execvp(rhsArgs[0], rhsArgs) < 0) {
							std::cerr << "Couldn't execute '" << rhsArgs[0] << "'!" << std::endl;
							exit(127);
						}

					} else if (kid[1] < 0) { // checks for fork failure
						std::cerr << "fork() failed!" << std::endl;
						exit(-1);
					}

				} else if (kid[0] < 0) { // checks for fork failure
					std::cerr << "fork() failed!" << std::endl;
					exit(-1);
				}
			} else if (kid[0] == 0) {

				// executes command in first child
				if (execvp(lhsArgs[0], lhsArgs) < 0) {
					std::cerr << "Couldn't execute '" << lhsArgs[0] << "'!" << std::endl;
					exit(127);
				}
			}

			// close pipe
			close(pfd[0]);
			close(pfd[1]);

			// parent waits for children to finish
			while (wait(&status) > 0) {};
		}

		// clear arg arrays
		for (size_t i = 0; i < ARGSIZE; i++) {
	 		lhsArgs[i] = nullptr;
	 		rhsArgs[i] = nullptr;
		}
	}
	return 0;
}