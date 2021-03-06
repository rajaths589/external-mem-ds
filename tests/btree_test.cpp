#include "btree.h"
#include <random>
#include <functional>
#include <iostream>
#include <vector>

#define NUM_INSERT 1000

struct test_Comparator {
	test_Comparator() {}
	int operator()(int x, int y) const { return x > y; }

};

int main()
{
	BTree<int, int, test_Comparator>* btree_test = new BTree<int, int, test_Comparator>("./btree_test");

	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1,1000);

	auto dice = std::bind ( distribution, generator );

	for(auto i = 1; i <= NUM_INSERT; i++) {
		btree_test->insertElem(dice(), dice());
		std::cout << "INSERTED ELEMENT K : " << i << " VALUE : " << (i+1) << std::endl;
	}

	for(auto i = 1; i <= NUM_INSERT; i++) {
		int t = btree_test->searchElem(i);
		std::cout << "SEARCHED ELEMENT K : " << i << " VALUE : " << t << std::endl;
	}

	std::cout << "SIZE : " << btree_test->size() << std::endl;
	std::cout << std::endl;

	delete btree_test;
	return 0;
}
