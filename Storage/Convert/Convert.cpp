///////////////////////////////////////////////////////////////////////
// Convert.cpp -											         //
// ver 1.0                                                           //
// Tianxiang Yi, CSE687 - Object Oriented Design, Spring 2019        //
///////////////////////////////////////////////////////////////////////

#include "Convert.h"


#ifdef TEST_CONVERT

int main() {
	//Convert c("554hw3.txt");
	//c.readfile();
	//c.translate();
	//c.show();
	//c.inserttag();
	//c.show();
	//c.inserttemp("head", "tail");
	//c.show();
	Convert c("554hw3.txt");
	c.convertFile();
	getchar();
	getchar();
}

#endif