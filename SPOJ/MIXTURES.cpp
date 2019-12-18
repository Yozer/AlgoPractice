#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>

using namespace std;

struct res
{
	int result;
	int smoke;

	res(int result, int smoke)
		: result(result),
		  smoke(smoke)
	{
	}

	res() : res(-1, -1) {}

};

int mixtures[100];
res A[100][100];
int n;

res solve(int from, int to)
{
	if (from == to) return {mixtures[from], 0};
	else if (A[from][to].result != -1) return A[from][to];
	
	if(from + 1 == to)
	{
		A[from][to].smoke = mixtures[from] * mixtures[to];
		A[from][to].result = (mixtures[from] + mixtures[to]) % 100;
	}
	else
	{
		for(int i = from; i < to; ++i)
		{
			auto a = solve(from, i);
			auto b = solve(i + 1, to);
			auto c = res{ (a.result + b.result) % 100, a.smoke + b.smoke + a.result * b.result };
			if(A[from][to].result == -1 || A[from][to].smoke > c.smoke)
			{
				A[from][to] = c;
			}
		}
	}

	return A[from][to];
}

res solveDP()
{
	// cost for one mixture
	for (int i = 0; i < n; ++i) A[i][i] = { mixtures[i], 0 };

	for(int size = 2; size <= n; ++size)
	{
		for(int i = 0; i < n - size + 1; ++i)
		{
			int j = i + size - 1;

			A[i][j].smoke = INT_MAX;
			for(int k = i; k < j; ++k)
			{
				auto c = res{(A[i][k].result + A[k + 1][j].result) % 100, A[i][k].smoke + A[k+1][j].smoke + A[k + 1][j].result * A[i][k].result };
				if(A[i][j].smoke > c.smoke)
				{
					A[i][j] = c;
				}
			}
		}
	}
	return A[0][n - 1];
}

int main()
{
	ios::sync_with_stdio(false);

	while (cin >> n) {
		memset(A, -1, sizeof(res) * 100 * 100);
		for (int i = 0; i < n; ++i)
		{
			cin >> mixtures[i];
		}

		cout << solveDP().smoke << endl;
	}
	
	return 0;
}