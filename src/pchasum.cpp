#include <string.h>
#include <stdlib.h>
#include <argp.h>
#include "../lib/pcha/pcha.hpp"

#define CHUNK_SIZE 1024

static char doc[] = "Print PCHA checksums";
static char args_doc[] = "";
static struct argp_option options[] = {
    {"algorithm",  'a', 0, OPTION_ARG_OPTIONAL, "256 (default), 512"},
    {"capitalize", 'c', 0, OPTION_ARG_OPTIONAL, "print checksum in capital letters"},
	{"prettify",   'p', 0, OPTION_ARG_OPTIONAL, "prettify output"},
    {"raw", 'r', 0, OPTION_ARG_OPTIONAL, "print raw checksum instead of hex"},
    {"input",      'i', 0, OPTION_ARG_OPTIONAL, "take input from argument"},
    { 0 }
};

typedef struct Arguments {
	char * userInput;
	bool use512Bits;
	bool capitalize;
	bool displayRaw;
	bool prettify;
	bool canExecute;
} Arguments;

Arguments def_arguments = {0, false, false, false, false, true};

error_t parse_opt(int key, char * arg, struct argp_state *state) {

	// if (state->next < state->argc) {

		Arguments * arguments = (Arguments *) state->input;

		char * nextValue = state->argv[state->next];

		switch (key) {
			case 'a':
				switch (atoi(nextValue)) {
					case 512: arguments->use512Bits = true;
					case 256: break;
					default:
						printf("%s: Unrecognized algorithm\n\n", state->argv[0]);
						arguments->canExecute = false;
						break;
				}
				break;

			case 'c':
				arguments->capitalize = true;
				break;

			case 'p':
				arguments->prettify = true;
				break;

			case 'r':
				arguments->displayRaw = true;
				break;

			case 'i':
				arguments->userInput = nextValue;
				break;
			// default:
			// 	return ARGP_ERR_UNKNOWN;

		}
	// }

	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char *argv[]) {

	unsigned long int currentSize = CHUNK_SIZE;
	unsigned long int i = 0;
	unsigned long int inputLength = 0;

	PCHA * pcha = NULL;
	char * result = NULL;

	int displayLength = 0;

	Arguments arguments = def_arguments;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);


	if (arguments.canExecute == false) {
		argp_help(&argp, stdout, ARGP_HELP_STD_HELP, argv[0]);
		return 1;
	}


	if (arguments.userInput == 0) {
		arguments.userInput = new char[currentSize];

		char ch = 0;

		while ((ch=getchar()) != EOF) {
			// printf("%02hhx\n", ch);
			arguments.userInput[i++] = ch;

			if (i > currentSize) {
				// printf("Reallocating\n");
				currentSize += CHUNK_SIZE;
				arguments.userInput = (char *) realloc(arguments.userInput, currentSize);
			}
		}

		inputLength = i;
	} else {
		inputLength = strlen(arguments.userInput);
	}



	if (arguments.use512Bits) {
		pcha = new PCHA512();
	} else {
		pcha = new PCHA256();
	}

	displayLength = pcha->getDigestSize();

	if (arguments.displayRaw) {
		result = new char[displayLength];
		pcha->digest(result, arguments.userInput, inputLength);

	} else {
		displayLength = displayLength * 2 + 1;
		result = new char[displayLength];

		pcha->hexdigest(result, arguments.userInput, inputLength);

		if (arguments.capitalize) {
			for (i = 0; i < displayLength; i++)
				result[i] = toupper(result[i]);
		}
	}


	// Display result

	for (i = 0; i < displayLength; i++)
		putc(result[i], stdout);

	// Append new line if prettify is set

	if (arguments.prettify)
		putc('\n', stdout);

	return 0;
}
