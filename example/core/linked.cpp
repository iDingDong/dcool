#include <dcool/core.hpp>

#include <iostream>

int main(void) {
	dcool::core::ForwardLinked<int> linked1;
	linked1.emplaceFront(1);
	linked1.emplaceFront(2);
	linked1.emplaceFront(3);
	for (int i: linked1) {
		std::cout << i << ", ";
	}
	std::cout << std::endl;
	dcool::core::ForwardLinked<int> linked2 = linked1;
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
