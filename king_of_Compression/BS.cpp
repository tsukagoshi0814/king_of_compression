#include "BS.h"

std::string Blocksort::encode(const std::string &input, int &top)
{
	std::string buff, result;
	buff = input + input;
	std::vector<std::string> vtemp;
	//„‰ñ•\
	for (int i = 0; i < buff.size() / 2; ++i){
		std::string temp = buff.substr(i, input.size());
		vtemp.push_back(temp);
	}
	//sort
	std::stable_sort(vtemp.begin(), vtemp.end());
	//––”ö‚ğ‚Æ‚é
	for (int i = 0; i < vtemp.size(); ++i){
		if (input == vtemp[i]) top = i;
		result += *(vtemp[i].end() - 1);
	}
	return result;
}

std::string Blocksort::decode(const std::string &input, int top)
{
	std::string result, inputs = input;
	// only index sort
	std::vector<int> idx(input.size());
	for (int i = 0; i != idx.size(); ++i) idx[i] = i;
	std::stable_sort(idx.begin(), idx.end(),
		[inputs](int i1, int i2)	{return inputs[i1] < inputs[i2]; });
	//decode
	int p = top;
	for (int i = 0; i<input.size(); ++i) {
		p = idx[p];
		result += inputs[p];
	}

	return result;
}