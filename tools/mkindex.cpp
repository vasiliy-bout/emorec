
#include <iostream>
#include <cstdio>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		return 1;
	}
	for (int i = 2; i < argc; ++i) {
		std::cout << argv[1] << argv[i] << std::endl;
	}
	return 0;
}

