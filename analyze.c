#include <stddef.h>

extern int main(int argc, char *argv[]);

void analyze(void)
{
	char *argv[] = { "ffactor", "--", "input/00/in", "output/00/out",
			 NULL };
	int ret = main(sizeof(argv) / sizeof(argv[0]) - 1, argv);
}
