#include <algorithm>
#include <cstdio>
#include <cstring>

const int max_size = 6002;
int x0[max_size], x1[max_size], x2[max_size];
char a[max_size];

int main()
{
	int t, len;
	int *v0 = x0, *v1 = x1, *v2 = x2;
	int* tmp = NULL;

	scanf("%d\n", &t);

	while (t--)
	{
		fgets(a, max_size - 1, stdin);
		len = strlen(a) - 1;
		memset(v0, 0, sizeof(x0));
		memset(v1, 0, sizeof(x1));

		for (int j = 2; j <= len; ++j)
		{
			for (int i = 0; i + j <= len; ++i)
			{
				if(a[i] == a[i + j - 1])
				{
					v2[i] = v0[i + 1];
				}
				else
				{
					v2[i] = std::min(v1[i], v1[i + 1]) + 1;
				}
			}

			tmp = v0;
			v0 = v1;
			v1 = v2;
			v2 = v0;
		}
		

		printf("%d\n", v1[0]);
	}

	return 0;
}