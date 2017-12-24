#include <iostream>           
#include <thread>             
#include <mutex>              
#include <condition_variable> 
#include <map>
#include <ctime>
using namespace std;

mutex mtx;
condition_variable cv1;
condition_variable cv2;

//buffer;
int a = 0;
int b = 0;
int c = 0;
static int j = 0;
/* Consumer */

int random() {
	srand(j++);
	return rand();
}

void productWorker(int id) {
	unique_lock<mutex> lck(mtx);
	while (a + b + c == 0) cv1.wait(lck);
	int pickupA = (random() % 4), pickupB, pickupC;
	if (pickupA == 0)
	{
		pickupB = (random() % 3) + 1;
		if (pickupB == 3) {
			pickupC = 1;
		}
		else {
			pickupC = random() % (3 - pickupB) + 1;
		}
	}
	else
	{
		int boolvalue = random() % 2;
		if (boolvalue == 0) {
			pickupB = 0;
			pickupC = 4 - pickupA;
		}
		else
		{
			pickupB = 4 - pickupA;
			pickupC = 0;
		}

	}

pickup:
	cout << "ProductWorker ID: " << id + 1 << endl;
	cout << "Buffer State: (" << a << "," << b << "," << c << ")" << endl;

	cout << "Pickup Request: (" << pickupA << "," << pickupB << "," << pickupC << ")" << endl;
	if (pickupA != 0) {
		if ((a - pickupA) >= 0) {
			a = a - pickupA;
			pickupA = 0;
		}
		else
		{
			pickupA = -(a - pickupA);
			a = 0;
		}
	}

	if (pickupB != 0) {
		if ((b - pickupB) >= 0) {
			b = b - pickupB;
			pickupB = 0;
		}
		else
		{
			pickupB = -(b - pickupB);
			b = 0;
		}
	}

	if (pickupC != 0) {
		if ((c - pickupC) >= 0) {
			c = c - pickupC;
			pickupC = 0;
		}
		else
		{
			pickupC = -(c - pickupC);
			c = 0;
		}
	}

	cout << "Updated Buffer State: (" << a << "," << b << "," << c << ")" << endl;
	cout << "Updated Pickup Request: (" << pickupA << "," << pickupB << "," << pickupC << ")" << endl << endl;

	if (pickupA != 0) {
		while (a <pickupA) cv1.wait(lck);
		goto pickup;
	}
	else if (pickupB != 0) {
		while (b<pickupB) cv1.wait(lck);
		goto pickup;
	}
	else if (pickupC != 0) {
		while (c <pickupC) cv1.wait(lck);
		goto pickup;
	}
	cv2.notify_all();
}

/* Producer */
void partWorker(int id) {

	unique_lock<mutex> lck(mtx);
	while ((a + b + c) == 15) cv2.wait(lck); //max buffer size is 15.
	int placeA = (random() % 4), placeB, placeC;
	if (placeA == 3) {
		placeB = 0;
		placeC = 0;
	}
	else if (placeA == 0)
	{
		placeB = random() % 4;
		placeC = 3 - placeB;
	}
	else
	{
		placeB = (random() % (3 - placeA));
		placeC = 3 - placeA - placeB;
	}

place:
	cout << "PartWorker ID: " << id + 1 << endl;
	cout << "Buffer State: (" << a << "," << b << "," << c << ")" << endl;
	cout << "Place Request: (" << placeA << "," << placeB << "," << placeC << ")" << endl;

	if (a + placeA < 6) {
		a = a + placeA;
		placeA = 0;
	}
	else
	{
		placeA = (a + placeA) % 6;
		a = 6;
	}

	if (b + placeB < 5) {
		b = b + placeB;
		placeB = 0;
	}
	else
	{
		placeB = (b + placeB) % 5;
		b = 6;
	}

	if (c + placeC< 4) {
		c = c + placeC;
		placeC = 0;
	}
	else
	{
		placeC = (c + placeC) % 4;
		c = 4;
	}

	cout << "Updated Buffer State: (" << a << "," << b << "," << c << ")" << endl;
	cout << "Updated Place Request: (" << placeA << "," << placeB << "," << placeC << ")" << endl << endl;
	if (placeA != 0) {
		while (6 - a <placeA) cv2.wait(lck);
		goto place;
	}
	else if (placeB != 0) {
		while (5 - b<placeB) cv2.wait(lck);
		goto place;
	}
	else if (placeC != 0) {
		while (4 - c <placeC) cv2.wait(lck);
		goto place;
	}
	cv1.notify_all();
}

int main() {
	int j = 12;
	srand(j++);
	thread partW[10];
	thread prodW[7];
	for (int i = 0; i < 7; i++) {
		partW[i] = thread(partWorker, i);
		prodW[i] = thread(productWorker, i);
	}
	for (int i = 7; i<10; i++) {
		partW[i] = thread(partWorker, i);
	}
	/* Join the threads to the main threads */
	for (int i = 0; i < 7; i++) {
		partW[i].join();
		prodW[i].join();
	}
	for (int i = 7; i<10; i++) {
		partW[i].join();
	}
	cout << "Finish!" << endl;

	getchar();
	getchar();
	return 0;
}