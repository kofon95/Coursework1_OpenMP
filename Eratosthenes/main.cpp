#include <iostream>
#include <ctime>
#include <omp.h>

bool* eratosthenes(int n)
{
	int root = static_cast<int>(sqrt(n));
	auto sieve = static_cast<bool*>(calloc(n, sizeof(bool)));
	sieve[0] = sieve[1] = true;

	for (int i = 2; i <= root; i++)
	{
		if (sieve[i]) continue;
		for (int j = i*i; j < n; j+=i)
			sieve[j] = true;
	}

	return sieve;
}

bool* eratosthenes_mp(int n)
{
	int root = static_cast<int>(sqrt(n));
	auto sieve = static_cast<bool*>(calloc(n, sizeof(bool)));
	sieve[0] = sieve[1] = true;

//	omp_set_num_threads(2);
	for (int i = 2; i <= root; i++)
	{
		if (sieve[i]) continue;
#pragma omp parallel for
		for (int j = i*i; j < n; j += i)
			sieve[j] = true;
	}

	return sieve;
}

int main()
{
	const int N = 100000000;
	std::cout << "running...\n";
	int t = clock();
	auto composite = eratosthenes_mp(N);
	t = clock() - t;
	std::cout << "----\ntime: " << t << " mls.\n";
//	int sum = 0;
//	for (int i = 0; i < N; i++)
//	{
//		if (!composite[i]) sum++;
//	}
//	std::cout << "sum = " << sum << "\n";

#ifdef _DEBUG
	system("pause");
#endif
	return 0;
}
