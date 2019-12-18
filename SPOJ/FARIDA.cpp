#include <iostream>
#include <algorithm>
#include <string>

using namespace std;
int main()
{
	unsigned long long int t, a, b, c, tmp, coins;
	ios::sync_with_stdio(false);

	cin >> t;

	for (int i = 1; i <= t; i++)
	{
		cin >> coins;
		a = b = 0;
		for (int j = 0; j < coins; j++)
		{
			cin >> tmp;
			tmp = max(tmp + a, b);
			a = b;
			b = tmp;
		}
		cout << "Case " << i << ": " << b << "\n";
	}

	return 0;
}