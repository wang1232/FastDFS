#include <iostream>
#include "MemcachedClient.h"
using std::cout;
using std::endl;

int main()
{
	MemCachedClient mc;
	mc.Insert("kingcat","value123");
	cout << mc.Get("kingcat") << endl;
	
	return 0;
};