#include <iostream>
#include <chrono>
#include <thread>
#include <omp.h>
#include <vector>
#include <cassert>
#include <iso646.h>
using namespace std;

// returns 2^N (where N - natural number) which is >= real number of proccessors
int get_num_procs()
{
	static int i = 1;
	if (i != 1) return i;
	int n = omp_get_num_procs();
	while (i < n) i <<= 1;
	return i;
}

template <typename T>
void merge(T *a, const size_t &size_a, T *b, const size_t &size_b, T *result)
{
	size_t pa = 0, pb = 0, pr = 0;
	while (pa < size_a && pb < size_b)
		result[pr++] = a[pa] <= b[pb] ? a[pa++] : b[pb++];

	while (pa < size_a) result[pr++] = a[pa++];
	while (pb < size_b) result[pr++] = b[pb++];
}


template <typename T>
void _merge_sort(T *a, const size_t &size, T* const buffer)
{
	if (size <= 1)
	{
		if (size == 1) *buffer = *a;
		return;
	}
	auto half = size >> 1;
	_merge_sort(a, half, buffer);
	_merge_sort(a + half, size - half, buffer);

	merge(a, half, a + half, size - half, buffer);
	memcpy(a, buffer, size*sizeof(T));
}
template <typename T>
void merge_sort(T *a, const size_t &size)
{
	T* const buffer = new int[size];
	_merge_sort(a, size, buffer);
	delete[] buffer;
}


template <typename T>
void merge_sort_mp(T *a, const int &size)
{
	int procs = get_num_procs();
	if (size <= procs)
	{
		return merge_sort(a, size);
	}
	T* const buffer = new T[size];
	omp_set_num_threads(procs);
	int part = size / procs;
	int start = part + size%procs;
#pragma omp parallel for
	for (int to = start; to <= size; to+=part)
	{
		int from = to == start ? 0 : to - part;
		_merge_sort(a+from, to-from, buffer+from);
	}
	for (int i = 2; i <= procs; i <<= 1)
	{
		int step = part*i;
		int stepHalf = step >> 1;
		start += stepHalf;
#pragma omp parallel for
		for (int to = start; to <= size; to += step)
		{
			int from = to == start ? 0 : to - step;
			int firstLen = to - stepHalf-from;
			int secondLen = to - firstLen-from;
			merge(buffer + from, firstLen, buffer + from + firstLen, secondLen, a+from);
			memcpy(buffer + from, a+from, (firstLen + secondLen) * sizeof(T));
		}
	}
	delete[] buffer;
}

int main()
{
	// initializing
	int n = 1000000;
	int *a = new int[n], *b = new int[n];
	for (int i = 0; i < n; i++)
	{
		a[i] = b[i] = rand()%100;
	}
	///////////////

	int t = clock();
	merge_sort(a, n);
	cout << clock()-t << " mls. - sync\n";
	t = clock();
	merge_sort_mp(b, n);
	cout << clock()-t << " mls. - OpenMP \n";

	// checking
	for (int i = 0; i < n; i++)
	{
		assert(a[i] == b[i]);
	}
	cout << "\n";
	system("pause");
	return 0;
}