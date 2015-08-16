#include <stdio.h>
#include <err.h>

int main(void)
{
	char buf;

	while (fread(&buf, 1, 1, stdin) == 1)
		if (fwrite(&buf, 1, 1, stdout) != 1)
			errx(1, "Write error");

	if (ferror(stdin))
		errx(1, "Read error");

	return 0;
}
