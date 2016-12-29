#include <ncurses.h>
#include <stdio.h>

#include "debug.h"

#define DEBUG_LOG "DEBUG.log"

int initDone = 0;

int debug = 1;

static int init()
{
	FILE *fp = fopen(DEBUG_LOG, "w");

	fprintf(fp, "===============\n");
	fprintf(fp, "     DEBUG     \n");
	fprintf(fp, "===============\n\n");

	fclose(fp);
	return 0;
}

int Debug::log(const char *format, ...)
{
	if (!debug)
		return 1;

	if (!initDone)
	{
		init();
		initDone = 1;
	}

	FILE *fp = fopen(DEBUG_LOG, "a");
	
	if (!fp)
	{
		fprintf(stderr, "debug: failed to open DEBUG.log\n");
		return 1;
	}

	char buffer[255];

	va_list argv;
	va_start(argv, format);
	vsprintf(buffer, format, argv);
	va_end(argv);

	fprintf(fp, "%s\n", buffer);

	fclose(fp);
	return 0;
}

