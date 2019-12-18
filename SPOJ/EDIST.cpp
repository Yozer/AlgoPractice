#include <algorithm>
#include <cstdio>
#include <cstring>
#include <numeric>

using namespace std;
const int max_size = 3001;
int x0[max_size], x1[max_size];
char a[max_size], b[max_size];

int main()
{
	int t, a_size, b_size;
	int *v0 = x0, *v1 = x1;

	scanf("%d\n", &t);

	while (t--)
	{
		fgets(a, max_size - 1, stdin);
		fgets(b, max_size - 1, stdin);
		a_size = strlen(a) - 1;
		b_size = strlen(b) - 1;

		iota(v0, v0 + a_size, 0);

		for (int i = 0; i < a_size; ++i)
		{
			v1[0] = i + 1;

			for (int j = 0; j < b_size; ++j)
			{
				int cost = a[i] == b[j] ? 0 : 1;
				v1[j + 1] = std::min(v1[j] + 1, std::min(v0[j + 1] + 1, v0[j] + cost));
			}

			std:swap(v0, v1);
		}

		printf("%d\n", v0[b_size]);
	}

	return 0;
}