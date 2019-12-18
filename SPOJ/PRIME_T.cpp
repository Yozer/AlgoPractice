#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

// http://pl.spoj.com/problems/PRIME_T/

#define MAX 10001

bool sito[MAX];


int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	std::fill(sito, sito + MAX, true);
	sito[1] = false;

	for (int i = 2; i < MAX; i++)
	{
		if (!sito[i]) 
			continue;
		for (int j = i + i; j < MAX; j += i)
		{
			sito[j] = false;
		}
	}

	int n = 0;
	int tmp;
	cin >> n;

	for (int i = 0; i < n; ++i)
	{
		cin >> tmp;
		cout << (sito[tmp] ? "TAK" : "NIE") << endl;
	}

	return 0;
}
