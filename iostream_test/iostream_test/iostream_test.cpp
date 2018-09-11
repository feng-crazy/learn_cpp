// iostream_test.cpp: 定义控制台应用程序的入口点。
//测试iostream的代码
//

#include "stdafx.h"


#include <iostream>
#include <sstream>
#include <string>

#include <fstream>

using namespace std;

int main3() {
	char ch;
	ifstream infile;
	ofstream outfile;
	filebuf *inbuf, *outbuf;

	infile.open("test.txt");
	outfile.open("copy.txt");

	inbuf = infile.rdbuf();
	outbuf = outfile.rdbuf();

	ch = inbuf->sgetc();
	while (ch != EOF)
	{
		outbuf->sputc(ch);
		ch = inbuf->snextc();
		inbuf->sputc(ch);
	}

	outfile.close();
	infile.close();

	return 0;
}


int main2() {
	int length;
	char * buffer;

	ifstream is;
	is.open("test.txt", ios::binary);

	// get length of file:
	is.seekg(0, ios::end);
	length = is.tellg();
	is.seekg(0, ios::beg);

	// allocate memory:
	buffer = new char[length];

	// read data as a block:
	is.read(buffer, length);
	is.close();

	cout.write(buffer, length);

	delete[] buffer;
	return 0;
}


int main1()
{
	ostringstream ostr;
	//ostr.str("abc");//如果构造的时候设置了字符串参数,那么增长操作的时候不会从结        尾开始增加,而是修改原有数据,超出的部分增长 
	ostr.put('d');
	ostr.put('e');
	ostr << "fg";

	string gstr = ostr.str();
	cout << gstr;

	
	string tmp = "152 456 471 458 250";
	istringstream istr(tmp);

	for (int i = 0; i < 5; i++)
	{
		int count;
		istr >> count;
		string substr = istr.str();
		cout << i << " count:"<<(count) <<" str size:"<<istr.gcount()<< endl;
	}


	system("pause");
	return 0;
}


