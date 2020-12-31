#include <dcool/core.hpp>

#include <iostream>

struct Config {
	using Pool = dcool::core::ConcreteReferencePool<8, dcool::core::storageRequirement<8, 4>>;
};

int main(void) {
	dcool::core::ForwardLinked<int, Config> linked1;
	linked1.emplaceFront(1);
	linked1.emplaceFront(2);
	linked1.emplaceFront(3);
	for (int i: linked1) {
		std::cout << i << ", ";
	}
	std::cout << std::endl;
	dcool::core::ForwardLinked<int, Config> linked2 = linked1;
	linked1.eraseAfter(linked1.begin());
	for (int i: linked1) {
		std::cout << i << ", ";
	}
	std::cout << std::endl;
	for (int i: linked2) {
		std::cout << i << ", ";
	}
	std::cout << std::endl;
	return 0;
}
