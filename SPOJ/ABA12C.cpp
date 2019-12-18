#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>

using namespace std;


int n, k;
int prices[101];
int mem[101];

int solve(int kg)
{
	if (kg == 0) return 0;
	if (kg < 0) return INT_MIN;
	if (mem[kg] != -1) return mem[kg];

	int best = INT_MAX;
	for(int weight = 1; weight <= k; weight++)
	{
		if (prices[weight - 1] == -1) continue;
		int x = prices[weight - 1] + solve(kg - weight);
		if (x < 0) continue;
		best = min(x, best);
	}

	mem[kg] = best;
	return best;
}

int solveDP()
{
	mem[0] = 0;
	for(int kg = 1; kg <= k; kg++)
	{
		int best = INT_MAX;
		for (int weight = 1; weight <= k; weight++)
		{
			// can't buy this or we will buy too much or we will have problem with buying (kg - weight) (because of two earlier conditions)
			if (prices[weight - 1] == -1 || kg - weight < 0 || mem[kg - weight] == INT_MAX) continue;
			int x = prices[weight - 1] + mem[kg - weight];
			best = min(x, best);
		}

		mem[kg] = best;
	}

	return mem[k];
}

int main()
{
	int t;
	ios::sync_with_stdio(false);
	cin >> t;

	while (cin >> n) {
		cin >> k;
		for (int i = 0; i < k; ++i)
		{
			cin >> prices[i];
		}

		int result = solveDP();
		cout << (result == INT_MAX ? -1 : result) << endl;
	}
	
	return 0;
}