#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sys/utsname.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#ifndef NULL_H
#define NULL_H

// Begin Config Settings

// Maximum Argument Size
const int ARGSIZE = 10;

// Prompt Display
const std::string PROMPT("\033[0;31mØ\033[0m");

#endif