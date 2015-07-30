#include <assert.h>
#include <stdio.h>

int main(void)
{
	char buf;

	while (fread(&buf, 1, 1, stdin) > 0) {
		assert(!feof(stdin));
		assert(!ferror(stdin));

		if (fwrite(&buf, 1, 1, stdout) != 1) {
			assert(ferror(stdout));
			return 1;
		}
		assert(!feof(stdout));
		assert(!ferror(stdout));
	}
	if (ferror(stdin))
		return 1;
	assert(feof(stdin));

	return 0;
}
