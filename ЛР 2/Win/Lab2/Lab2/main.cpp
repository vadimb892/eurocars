#include "general.h"

constexpr unsigned int hash(const char* s, int off = 0) {
	return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
}

int main(int argc, char* argv[]) {
	bool isMatched = false;
	std::string command;
	while (true) {
		isMatched = false;
		std::cin >> command;
		switch (hash(command.c_str())) {
		case hash("dir"): {
			task3();
			isMatched = true;
			break;
		}
		case hash("ldinfo"): {
			task4();
			isMatched = true;
			break;
		}
		case hash("exit"): {
			return 1;
		}
		}
		if (!isMatched)
			std::cout << "Error: Wrong command.\n";
	}
	return 1;
}