#include <dcool/container.hpp>
#include <dcool/resource.hpp>

#include <iostream>

int main() {
	dcool::container::List<int> listNormal1;
	listNormal1.emplaceBack(1);
	listNormal1.emplaceBack(2);
	listNormal1.emplaceBack(3);
	for (int i: listNormal1) {
		std::cout << i << ", ";
	}
	std::cout << std::endl;
	dcool::container::List<int> listNormal2 = listNormal1;
	listNormal1.erase(listNormal1.begin() + 1);
	for (int i: listNormal1) {
		std::cout << i << ", ";
	}
	std::cout << std::endl;
	for (int i: listNormal2) {
		std::cout << i << ", ";
	}
	std::cout << std::endl;
	return 0;
}
