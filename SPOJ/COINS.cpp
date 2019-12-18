#include <unordered_map>

using ull = unsigned long long;
std::unordered_map<int, ull> map(10000);

ull solve(int n)
{
	if(n <= 11)
	{
		return n;
	}

	auto it = map.find(n);
	if(it != map.end())
	{
		return it->second;
	}

	ull tmp = solve(n / 2) + solve(n / 3) + solve(n / 4);
	map[n] = tmp;
	return tmp;
}

int main()
{
	int n;

	while (scanf("%d", &n) != EOF) 
	{
		map.clear();
		ull sol = solve(n);
		printf("%llu\n", sol);
	}

	return 0;
}