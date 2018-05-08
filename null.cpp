#include "null.h"

int main (int argc, char* argv[]) {

	// variable declarations
	pid_t mid,
	      kid;
	int status = 0,
			cnt = 0;
	std::istringstream cmd;
	std::string input,
							lhsCmd,
							rhsCmd,
							arg;
	char* lhsArgs[ARGSIZE];
	char*	rhsArgs[ARGSIZE];
	int pfd[2];

	// unbuffer output using unitbuff
	std::cout << std::unitbuf;

	// primary process
	while (lhsCmd != "q" && lhsCmd != "quit") {

		// istringstream refresh
		cmd.str("");
		cmd.clear();
		cnt = 0;

		// get input
		std::cout << "\033[0;31mØ\033[0m ";
		getline(std::cin, input);

		// begin tokenization
		cmd.str(input);

		while (cmd >> arg && cnt <= ARGSIZE) {
				// build lhs command
				lhsArgs[cnt] = new char(arg.length() + 1);
				strcpy(lhsArgs[cnt], arg.c_str());
				lhsArgs[cnt + 1] = nullptr;

				// checks for pipe
				if (arg == "||") {
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
				lhsCmd = lhsArgs[0];
		}

		if (lhsCmd != "q" && lhsCmd != "quit") {
			// initial fork
			mid = fork();

			// checks for pipe
			if (cmd.str().find("||") > 0 && cmd.str().find("||") <= cmd.str().length() && mid == 0) {

				if (pipe(pfd) == -1) {
					std::cout << "pipe() failed!" << std::endl;
					exit(-1);
				}

				// mid forks child
				kid = fork();

				if (kid == 0) {	// second child process
					// close stdio in and replace with pipe
					close(0);
					dup(pfd[0]);
					close(pfd[1]);
					// executes command
					if (execvp(rhsArgs[0], rhsArgs) < 0) {
						std::cout << "Couldn't execute '" << rhsArgs[0] << "'!" << std::endl;
						exit(127);
					}

					// close pipe
					close(pfd[0]);

				} else if (kid < 0) {	// checks for failure
					std::cout << "fork() failed!" << std::endl;
					exit(-1);
				}
			}

			if (mid == 0) { // first child process
				// checks for pipe
				if (kid > 0) {
					// close std out and replace with pipe
					close(1);
					dup(pfd[1]);
				}
				// executes command
				if (execvp(lhsArgs[0], lhsArgs) < 0) {
					std::cout << "Couldn't execute '" << lhsArgs[0] << "'!" << std::endl;
					exit(127);
				}		
				
				// close pipe
				close(pfd[1]);

			} else if (mid > 0) {	// parent process

				while (wait(&status) > 0) {};	// wait for children to finish

			} else if (mid < 0) { // checks for failure
				std::cout << "fork() failed!" << std::endl;
				exit(-1);
			}
		}

		// clear arg arrays
		for (size_t i = 0; i < ARGSIZE; i++) {
			lhsArgs[i] = nullptr;
			rhsArgs[i] = nullptr;
		}
	}
	return 0;
}
