#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

class Blocksort
{
public:
	static std::string encode(const std::string &input, int &top);
	static std::string decode(const std::string &input, int top);
};