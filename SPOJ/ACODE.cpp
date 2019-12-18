#include <cstring>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <sstream>
using ull = unsigned long long;

const int max_size = 5001;
ull dp[max_size];
char input[max_size];

inline bool can_append(char a, char b)
{
	if (a == '1') return true;
	if (a == '2' && b <= '6') return true;
	return false;
}
int n;
std::string s;
std::vector<int> m;

int calc(int i) {
	if (m[i] == -1) {
		if (i == n) {
			m[i] = 1;
		}
		else if (s[i] == '0') {
			m[i] = 0;
		}
		else {
			if (i + 1 == n) {
				m[i] = 1;
			}
			else if (10 * (s[i] - '0') + (s[i + 1] - '0') <= 26) {
				m[i] = calc(i + 1) + calc(i + 2);
			}
			else {
				m[i] = calc(i + 1);
			}
		}
	}
	return m[i];
}

int main()
{
	int len = 0;
	//while (true)
	for(int k = 1; k <= 10e9; ++k)
	{
		//fgets(input, max_size - 1, stdin);
		std::stringstream ss;
		ss << k;
		ss << '\n';
		strcpy(input, ss.str().c_str());
		len = strlen(input) - 1;
		if(len == 1)
		{
			if (input[0] == '0')
			{
				break;
			}

			dp[0] = 1;
		}
		else if (input[1] == '0' && input[0] != '1' && input[0] != '2')
		{
			dp[len - 1] = 0;
		}
		else 
		{
			std::fill(dp, dp + max_size, 0);
			dp[0] = 1;
			dp[1] = input[1] == '0' ? 1 : can_append(input[0], input[1]) ? 2 : 1;



			for (int i = 2; i < len; ++i)
			{
				if (input[i] == '0')
				{
					if (input[i - 1] != '1' && input[i - 1] != '2')
					{
						dp[len - 1] = 0;
						break;
					}

					dp[i] = dp[i - 2];
				}
				else if (input[i - 1] == '0')
				{
					dp[i] = dp[i - 1];
				}
				else if (can_append(input[i - 1], input[i]))
				{
					dp[i] = dp[i - 2] + dp[i - 1];
				}
				else
				{
					dp[i] = dp[i - 1];
				}
			}
		}

		s = std::string(input);
		s = s.substr(0, s.size() - 1);
		n = s.length();
		m = std::vector<int>(n + 1, -1);

		int my_sol = calc(0);
		if(my_sol != dp[len - 1])
		{
			printf("%llu\n", dp[len - 1]);
		}

	}

	return 0;
}