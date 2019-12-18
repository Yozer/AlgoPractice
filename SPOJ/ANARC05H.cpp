#include <cstdio>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>

using ull = unsigned long long;

std::unordered_map<std::string, int> map;

ull sum(const std::string& num, int from, int to)
{
	ull x = 0;
	for(int i = from; i < to; ++i)
	{
		x += num[i] - '0';
	}

	return x;
}

ull solve(const std::string& num)
{
	if (num.size() == 2)
	{
		return num[0] < num[1] ? 1 : 0;
	}
	else if (num.size() == 1)
	{
		return 0;
	}

	if(map.find(num) != map.end())
	{
		return map[num];
	}
	

	ull res = 0;
	ull sum_so_far = 0;

	for(int i = 0; i < num.size() - 1; ++i)
	{
		sum_so_far += num[i] - '0';
		ull min_grp;
		auto right = num.substr(i + 1);
		int tmp = solve(right) + 1;;

		if (min_grp <= sum_so_far)
			res += tmp;

		min_param = std::min(min_param, std::min(sum_so_far, min_grp));
	}

	map[num] = res;
	return res;
}

int main()
{
	int t = 0;
	std::ios::sync_with_stdio(false);

	while(++t)
	{
		std::string str;
		getline(std::cin, str);
		if(str == "bye")
		{
			break;
		}
		
		ull x;
		ull res = solve(str, x) + 1;
		printf("%d. %llu\n", t, res);

	}

	return 0;
}