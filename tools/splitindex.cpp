
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>

int main(int argc, char *argv[]) {
	if (argc < 4) {
		return 1;
	}

	srand(time(0));

	FILE *f = fopen(argv[1], "rt");
	if (f == 0) {
		return 1;
	}

	std::vector<FILE*> files;

	for (int i = 2; i < argc; ++i) {
		FILE *out = fopen(argv[i], "wt");
		if (out == 0) {
			return 1;
		}
		files.push_back(out);
	}

	char buf[1000];
	while (fgets(buf, 1000, f)) {
		int n = rand() % files.size();
		fputs(buf, files[n]);
	}

	fclose(f);
	for (size_t i = 0; i < files.size(); ++i) {
		fclose(files[i]);
	}

	return 0;
}

