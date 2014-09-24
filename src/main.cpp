#include <iostream>
#include <string.h>

#include "Bounds.h"

#include "StoreNet.h"


using namespace std;



int main(int argc, const char **argv)

{
	if (argc == 1)
	{
		std::cout << "Usage: " << argv[0] << " [-prod] <FileName> [-i]" << std::endl;
		std::cout << "\t -i: specifies that calculus are performed using integer variables (slower, but more precise)" << std::endl;
		exit(1);
	}

	StoreNet N(argc, argv);
	bool is_integer = false;
	int ind = 0;

	if (argc == 3)
		ind = 2;
	else if (argc == 4)
		ind = 3;

	if ((argc == 3) || (argc == 4))
		if (strcmp(argv[ind], "-i") == 0)
			is_integer = true;

	Bounds b(N, is_integer);

	std::cout << b << std::endl;

    return 0;

}

