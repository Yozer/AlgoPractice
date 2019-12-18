#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

// http://pl.spoj.com/problems/PA05_POT/



int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	unsigned long long x, a, b, w;
	cin >> x;

	for (int i = 0; i < x; ++i)
	{
		cin >> a >> b;
		w = 1;
		while (b > 0)
		{
			if (b & 1 == 1)
			{
				w *= a;
				w %= 10;
			}

			a *= a;
			a %= 10;
			b >>= 1;

		}

		cout << w << endl;
	}

	return 0;
}
