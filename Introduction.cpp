#include <iostream>
#include <thread>
using namespace std;

void function_1(){
	cout << "Das bist du, mein wunderkeit." << endl;
}

int main(){
	thread t1(function_1);
	t1.join();
	
	return 0;
}
