#include <cstdio>
#include <algorithm>

const int max_size = 10001;
int x0[max_size], x1[max_size];
int x0_len, x1_len;

int solve()
{
	int i = 0, j = 0;
	int sumPiece0 = 0, sumPiece1 = 0;
	int result = 0;

	while (i < x0_len && j < x1_len)
	{
		if (x0[i] == x1[j])
		{
			result += std::max(sumPiece0 + x0[i], sumPiece1 + x1[j]);
			++i;
			++j;
			sumPiece0 = sumPiece1 = 0;
		}
		else if (x0[i] > x1[j])
		{
			sumPiece1 += x1[j];
			++j;
		}
		else
		{
			sumPiece0 += x0[i];
			++i;
		}
	}

	while (i < x0_len) sumPiece0 += x0[i++];
	while (j < x1_len) sumPiece1 += x1[j++];

	return result + std::max(sumPiece0, sumPiece1);
}

int main()
{

	while (true) 
	{
		scanf("%d", &x0_len);
		if (x0_len == 0)
			break;

		for (int i = 0; i < x0_len; ++i)
			scanf("%d", x0 + i);

		scanf("%d", &x1_len);
		for (int i = 0; i < x1_len; ++i)
			scanf("%d", x1 + i);

		int result = solve();
		printf("%d\n", result);
	}

	return 0;
}