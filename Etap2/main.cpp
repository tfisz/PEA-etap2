#include<iostream>
#include<fstream>
#include<string>
#include<conio.h>
#include<algorithm>
#include<ctime>
#include<iomanip>
#include<windows.h>
#include<list>
#include<vector>

using namespace std;

int cities;
int** matrix;
int optimum;
string name;
int cityA, cityB;

//zmienne i funkcje wykorzystywane przy pomiarze czasu dzia³ania algorytmów
double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}

// klasa TabuList i metody z ni¹ zwi¹zane

class TabuList {
	struct ListElement {
		int cityA;
		int cityB;
		
		ListElement(int a, int b) {
			cityA = a;
			cityB = b;
		}
	};
	

	int maxListSize;
	list<ListElement> tabuList;
	bool** tabuMatrix;

public:
	TabuList(int maxListSize);
	bool contains(int cityA,int cityB);
	void insertFront(int cityA, int cityB);
	void clear();
};

TabuList::TabuList(int maxSize) {
	maxListSize = maxSize;

	tabuMatrix = new bool*[cities];
	for (int i = 0; i < cities; i++) {
		tabuMatrix[i] = new bool[cities];
		for (int j = 0; j < cities;j++)
			tabuMatrix[i][j] = false;
	}
}

bool TabuList::contains(int cityA, int cityB) {
	return tabuMatrix[cityA][cityB];
}

void TabuList::insertFront(int a, int b) {
	if (tabuList.size() == maxListSize) {
		ListElement oldCities = tabuList.back();
		tabuMatrix[oldCities.cityA][oldCities.cityB] = false;
		tabuMatrix[oldCities.cityB][oldCities.cityA] = false;
		tabuList.pop_back();
	}

	tabuMatrix[a][b] = true;
	tabuMatrix[b][a] = true;
	ListElement newCities(a, b);
	tabuList.push_front(newCities);
}

void TabuList::clear() {
	while (!tabuList.empty()) {
		ListElement cities = tabuList.back();
		tabuMatrix[cities.cityA][cities.cityB] = false;
		tabuMatrix[cities.cityB][cities.cityA] = false;
		tabuList.pop_back();
	}
}

//dalsza czesc programu

void readFromFile(string fileName) {
	ifstream file;
	string dir = "C:\\Users\\TOSHIBA\\Documents\\semestr_5\\PEA\\Etap2\\Etap2\\instancje\\" + fileName;
	const char* fName = dir.c_str();
	file.open(fName);
	if (!file) {
		cout << "Nazwa pliku jest bledna!\n";
		return;
	}

	file >> name;
	file >> cities;

	matrix = new int*[cities];

	for (int i = 0; i < cities; i++) {
		matrix[i] = new int[cities];
		for (int j = 0; j < cities; j++)
			file >> matrix[i][j];
	}
	file >> optimum;
	file.close();
}

void displayMatrix() {
	cout << endl << "   ";
	for (int i = 0; i < cities; i++) {
		cout << setw(3) << "[" << i << "]";

	}
	cout << endl;
	for (int i = 0; i < cities; i++) {
		cout << "[" << i << "] ";
		for (int j = 0; j < cities; j++) {
			cout << setw(4) << matrix[i][j] << " ";
		}
		cout << endl;
	}
}

vector<int> getOrder() {
	vector<int> order;
	int counter = cities*cities;
	int pos;
	int* tab = new int[cities];
	for (int i = 0; i < cities; i++)
		tab[i] = i;

	while (counter > 0) {
		pos = rand() % cities;
		swap(tab[pos], tab[cities-1 - pos]);
		counter--;
	}
	
	for (int i = 0; i < cities; i++)
		order.push_back(tab[i]);
	return order;
}

int getPathCost(vector<int> order) {
	int cost = 0;
	for (int i = 0; i < cities - 1; i++)
		cost += matrix[order[i]][order[i+1]];
	cost += matrix[order.back()][order.front()];
	
	return cost;
}

vector<int> getBestSwap(vector<int> order, TabuList* tabuList, int cost) {
	vector<int> nextOrder(order);
	vector<int> newOrder;
	vector<int> bestOfTheWorst(order);
	int a, b;
	int newCost;

	int counter = cities;
	while (counter > 0) {
		newOrder = order;

		a = rand() % cities;
		b = rand() % cities;
		if (a == b)
			continue;

		swap(newOrder[a], newOrder[b]);
		newCost = getPathCost(newOrder);
		if (!tabuList->contains(a, b) && newCost < getPathCost(nextOrder)) {
			cityA = newOrder[a];
			cityB = newOrder[b];
			nextOrder = newOrder;
		}
		else if (!tabuList->contains(a, b) && getPathCost(newOrder) > cost){
			if (bestOfTheWorst == order)
				bestOfTheWorst = newOrder;
			else if (getPathCost(bestOfTheWorst) > newCost) 
				bestOfTheWorst = newOrder;
		}
		
		counter--;
	}
	if (nextOrder == order)
		return bestOfTheWorst;
	return nextOrder;
}

int TabuSearch() {
	int loopCycles = 10*cities;
	int noChanges = 0;
	TabuList* tabuList = new TabuList(cities);
	int iterations = 0, counter = 0;
	double time;

	vector<int> currentOrder = getOrder();
	vector<int> bestOrder = currentOrder;
	
	int currentCost = getPathCost(currentOrder);
	int bestCost = currentCost;
	while (loopCycles > 0) {
		currentOrder = getBestSwap(currentOrder, tabuList, currentCost);
		currentCost = getPathCost(currentOrder);
		
		if (currentCost < bestCost) {
			bestCost = currentCost;
			bestOrder = currentOrder;
			noChanges = 0;
		}
		else {
			noChanges++;
			tabuList->insertFront(cityA, cityB);
		}

		if (noChanges == cities) {
			currentOrder = getOrder();
			tabuList->clear();
			noChanges = 0;
		}

		loopCycles--;
		iterations++;
	}

	return bestCost;
}

//funkcje dla SA
vector<int> findRandomNext(vector<int> currentOrder) {
	vector<int> nextOrder(currentOrder);
	int a, b;
	a = rand() % cities;
	b = rand() % cities;
	if (a != b)
		swap(nextOrder[a], nextOrder[b]);

	return nextOrder;
}


vector<int> findBetterNext(vector<int> currentOrder) { // metoda nieefektywna
	vector<int> nextOrder(currentOrder);
	vector<int> newOrder;
	int a, b;

	int counter = cities;
	while (counter > 0) {
		newOrder = nextOrder;
		a = rand() % cities;
		b = rand() % cities;
		if (a == b)
			continue;

		swap(newOrder[a], newOrder[b]);
		if (getPathCost(newOrder) < getPathCost(nextOrder))
			nextOrder = newOrder;
		counter--;
	}
	return nextOrder;
}

double getRandDouble() {
	double x;
	x = rand() % 999 + 1;
	return x / 1000.0;
}

int SimulatedAnnealing() {
	double maxTemperature = cities*10;
	double temperature = maxTemperature;
	double coolRate = 0.9999;
	int iterations = 0;

	vector<int> currentOrder = getOrder();
	vector<int> bestOrder = currentOrder;
	
	int currentCost = getPathCost(currentOrder);
	int bestCost = currentCost;
	int delta, counter = 0;
	double random, time;

	vector<int> newOrder;
	int newCost;

	while (temperature > 0.0001) {
		newOrder = findRandomNext(currentOrder);
		newCost = getPathCost(newOrder);

		if (newCost < bestCost) {
			bestOrder = newOrder;
			bestCost = newCost;
		}

		delta = newCost - currentCost;
		if (delta < 0) {
			currentOrder = newOrder;
			currentCost = newCost;
		}
		else {
			random = getRandDouble();
			if (random < exp(-delta / temperature)) {
				currentOrder = newOrder;
				currentCost = newCost;
			}
		}

		temperature *= coolRate;
	
		iterations++;

	}

	return bestCost;
}

int main() {
	srand(time(NULL));
	char  opt;
	string fileName;
	int sum;
	double time;
	double difference;

	do {
		cout << "\n==MENU==";
		cout << "\n1. Wczytaj z pliku";
		cout << "\n2. Wyswietl macierz";
		cout << "\n3. Poszukiwanie z zakazami";
		cout << "\n4. Symulowane wyzarzanie";
		cout << "\n0. Wyjdz\n";
		opt = _getche();
		switch (opt) {
		case '1': //wczytywanie z pliku
			cout << "\nPodaj nazwe pliku: ";
			cin >> fileName;
			readFromFile(fileName);
			break;

		case '2': //wyœwietlanie macierzy
			displayMatrix();
			break;

		case '3': //wywo³anie Tabu Searcha
			StartCounter();
			sum = TabuSearch();
			time = GetCounter();
			cout << "\nUkonczono Tabu Search, wynik wynosi: " << sum;
			cout << "\nOptimum ma wartosc: " << optimum;
			difference = (double)(sum - optimum) / optimum;
			cout << "\nRoznica wynosi: " << difference*100 << "%" << " Czas trwania programu: " << time;

			break;

		case '4': //wywo³anie SA
			StartCounter();
			sum = SimulatedAnnealing();
			time = GetCounter();
			//cout << "\nUkonczono algorytm SA, wynik wynosi: " << sum;
			//cout << "\nOptimum ma wartosc: " << optimum;
			difference = (double)(sum - optimum) / optimum;
			cout << "\nRoznica wynosi: " << difference * 100 << "%" << " Czas trwania programu: " << time;
			break;
		}
	} while (opt != '0');
	return 0;
}
