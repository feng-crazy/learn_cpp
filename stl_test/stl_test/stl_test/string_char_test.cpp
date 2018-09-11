#include "stdafx.h"
#pragma warning(disable:4996)
#include <string.h>
#include <iostream>
#include <map>
using namespace std;

int mainstr()
{
    string key = "hello";
    cout << key.size() << " " << key.length() << endl;
    string value = "world";
    char buffer[256] = { 0 };
    strncpy(&buffer[5], key.c_str(), key.length());

    char char_value[256] = { 0 };
    strncpy(char_value, &buffer[5], 5);
    value = char_value;
    cout << "value:" << value.c_str() << endl;
    return 0;
}