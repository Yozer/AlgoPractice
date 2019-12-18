#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct node : vector<int>
{
	int v = -1;
};

vector<node> tree;

int recur(int root, int source)
{
	if (tree[root].v != -1) return tree[root].v;
	if (tree[root].size() == 0) return 0;

	// take root
	int a = 1;
	for(auto x : tree[root])
	{
		if (x == source) continue;
		a += recur(x, root);
	}

	// don't take, take all children
	int b = 0;
	for (auto x : tree[root])
	{
		if (x == source) continue;
		for (auto y : tree[x])
		{
			if (y == root) continue;
			b += recur(y, x);
		}
		++b;
	}


	int best = min(a, b);
	tree[root].v = best;
	return best;
}

int main()
{
	ios::sync_with_stdio(false);
	int n, from, to;
	cin >> n;
	tree.resize(n);

	for (int i = 0; i < n - 1; ++i)
	{
		cin >> from >> to;
		--from; --to;
		tree[from].push_back(to);
		tree[to].push_back(from);
	}

	cout << recur(0, -1);
	
	return 0;
}