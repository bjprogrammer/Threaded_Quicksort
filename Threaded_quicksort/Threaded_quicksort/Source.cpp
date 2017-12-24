#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
using namespace std;

int partition(vector<int> &V, int low, int high)
{
	if (high < 0)
	{
		return low;
	}
	else if (low > high)
	{
		return high;
	}
	else
	{
		int pivot = V[high];
		int i = low - 1;
		for (int j = low; j <= high - 1; j++)
		{
			if (V[j] < pivot)
			{
				i++;
				swap(V[i], V[j]);
			}
		}
		swap(V[i + 1], V[high]);
		return (i + 1);
	}
}

void swap(int a, int b)
{
	int t = a;
	a = b;
	b = t;
}

//The following funciton will be the work for each thread
void quick_sort(vector<int> &V, int i, int j) {
	//sort elements of vector V from position i to position j
	if (i < j)
	{
		int ti = partition(V, i, j);
		quick_sort(V, i, ti - 1);
		quick_sort(V, ti + 1, j);
	}

}

int main() {
	int n = 100000;
	vector<int> V(n);//V has n elements.
	for (int i = 0; i < 100000; i++) V[i] = rand() % 5000;
	//You need to create four threads to help perform sorting

	ofstream out("output.txt");
	if (n == 1)
	{
		out << V[0] << " ";
		out << endl;
		return 0;
	}

	int low = 0, high = n - 1;
	if (low < high)
	{
		int pi = partition(V, low, high);
		int pi2 = partition(V, low, pi - 1);
		int pi3 = partition(V, pi + 1, high);

		thread t1(quick_sort, ref(V), low, pi2 - 1);
		thread t2(quick_sort, ref(V), pi2 + 1, pi - 1);
		thread t3(quick_sort, ref(V), pi + 1, pi3 - 1);
		thread t4(quick_sort, ref(V), pi3 + 1, high);
		t1.join();
		t2.join();
		t3.join();
		t4.join();

		for (int i = 0; i < n; i++) out << V[i] << endl;
		out << endl;
	}

	return 0;
}

