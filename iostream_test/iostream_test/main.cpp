// iostream_test.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <iostream>
#include <sstream>
#include <string>

#include <fstream>

#include <memory>

using namespace std;

int main5()
{
	int length = 256;
	char buffer[256] = {0};

	ifstream is;
	string filename = "test.txt";
	is.open(filename.c_str(), ios::binary);

	ofstream out("out_test.txt", ios::app);
	out.close();
	out.open("out_test.txt", ios::binary);

	while (!is.eof())
	{
		is.getline(buffer, 256);
		out <<buffer<<endl;
	}
	

	is.close();
	
	out.flush();
	out.close();
	
	return 0;
}

int main()
{
	int length;

	ifstream is;
	is.open("test.txt", ios::binary);

	// get length of file:
	is.seekg(0, ios::end);
	length = is.tellg();
	is.seekg(0, ios::beg);


	// allocate memory:
    //std::shared_ptr<char>buffers(new char[length]);

    char *buffer = new char[length];

	// read data as a block:
	//is.read(buffer, length);
	while (!is.eof())
	{
		is.getline(buffer, 256);


		//cout.write(buffer, 256);
		cout << buffer << "\n";
	}

	is.close();
	return 0;
}
