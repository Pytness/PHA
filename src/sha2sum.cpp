#include <string.h>
#include <stdlib.h>
#include <argp.h>
#include "sha2/sha2.hpp"

#define CHUNK_SIZE 1024

static char doc[] = "Print SHA2 checksums";
static char args_doc[] = "";

static struct argp_option options[] = {
	{"capitalize", 'c', 0, OPTION_ARG_OPTIONAL, "print checksum in capital letters"},
	{"newline",    'l', 0, OPTION_ARG_OPTIONAL, "append a \\n to the output"},
	{"raw",        'r', 0, OPTION_ARG_OPTIONAL, "print raw checksum instead of hex"},
	{"input",      'i', 0, OPTION_ARG_OPTIONAL, "take input from argument"},
	{ 0 }
};

typedef struct Arguments {
	char * userInput;
	bool capitalize;
	bool displayRaw;
	bool newline;
	bool canExecute;
} Arguments;

Arguments def_arguments = {nullptr, false, false, false, true};

error_t parse_opt(int key, char * arg, struct argp_state *state) {

	// if (state->next < state->argc) {

		Arguments * arguments = (Arguments *) state->input;

		const char * nextValue = state->next < state->argc ?
			state->argv[state->next] : nullptr;

		switch (key) {
			case 'c':
				arguments->capitalize = true;
				break;

			case 'l':
				arguments->newline = true;
				break;

			case 'r':
				arguments->displayRaw = true;
				break;

			case 'i':
				if (nextValue == nullptr) {
					arguments->canExecute = false;
					break;
				}

				arguments->userInput = (char *) nextValue;
				break;
			// default:
			// 	return ARGP_ERR_UNKNOWN;

		}
	// }

	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char * argv[]) {

	unsigned long int currentSize = CHUNK_SIZE;
	unsigned long int i = 0;
	unsigned long int inputLength = 0;

	Arguments arguments = def_arguments;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	if (arguments.canExecute == false) {
		argp_help(&argp, stdout, ARGP_HELP_STD_HELP, argv[0]);
		return 1;
	}

	if (arguments.userInput == nullptr) {
		arguments.userInput = new char[currentSize];

		char ch = 0;

		while ((ch = getchar()) != EOF) {
			// printf("%02hhx\n", ch);
			arguments.userInput[i++] = ch;

			if (i > currentSize) {
				currentSize += CHUNK_SIZE;
				arguments.userInput = (char *) realloc(arguments.userInput, currentSize);
			}
		}

		inputLength = i;
	} else {
		inputLength = strlen(arguments.userInput);
	}

	uint8_t displayLength = SHA256_DIGEST_SIZE;
	char * result = NULL;

	if (arguments.displayRaw) {
		result = new char[displayLength];

		sha256((const unsigned char *) arguments.userInput,
			inputLength, (unsigned char *) result );
	} else {
		char * tempResult = new char[displayLength];

		displayLength *= 2;
		result = new char[displayLength + 1];

		sha256((const unsigned char *) arguments.userInput,
			inputLength, (unsigned char *) tempResult);

		for (uint32_t i = 0; i < SHA256_DIGEST_SIZE; i++)
			snprintf(&result[i * 2], 3, "%02hhx", tempResult[i]);

		if (arguments.capitalize) {

			for (i = 0; i < displayLength; i++)
				result[i] = toupper(result[i]);
		}
	}

	// Display result

	for (i = 0; i < displayLength; i++)
		putchar(result[i]);

	// Append new line if newline is set

	if (arguments.newline)
		putchar('\n');

	return 0;
}
