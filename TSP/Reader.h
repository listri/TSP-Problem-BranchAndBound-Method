#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

class Reader
{
public:
	int liczbaMiast;
	string caly, dane;
	vector<vector<int>> dist;
	string ZnajdzZnacznik(string znacznik);
	void WczytajFull();
	void WczytajUpperRow();
	bool WczytajMiasta(string path);
	vector<vector<int>>::iterator i1;
	vector<int>::iterator i2;
	Reader();
	~Reader();
};

