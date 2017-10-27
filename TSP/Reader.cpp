#include "Reader.h"

string Reader::ZnajdzZnacznik(string znacznik) {
	string tmp = caly;
	if (int pos = tmp.find(znacznik)) {		//znalezienie znacznika
		tmp = tmp.substr(pos + znacznik.size() + 1);
	}

	while (isspace(tmp[0])) {	//usuniecie bia³ych znaków
		tmp = tmp.substr(1);
	}

	return tmp;
}

void Reader::WczytajFull() {	
	stringstream ss(dane);
	int tmpint;
	vector <int> newColumn;
	for (int i = 0; i < liczbaMiast; ++i) {
		dist.push_back(newColumn);
		for (int j = 0; j < liczbaMiast; ++j) {
			ss >> tmpint;
			dist[i].push_back(tmpint);
		}
	}
}

void Reader::WczytajUpperRow() {
	stringstream ss(dane);
	int tmpint;

	dist.resize(liczbaMiast);
	for (int i = 0; i < liczbaMiast; ++i) {
		dist[i].resize(liczbaMiast, 0);
	}

	for (int i = 0; i < liczbaMiast - 1; i++) {
		for (int j = i + 1; j < liczbaMiast; ++j) {
			ss >> tmpint;
			dist[i][j] = tmpint;
			dist[j][i] = tmpint;
		}
	}
}

bool Reader::WczytajMiasta(string path)
{
	ifstream file(path);
	string tmp;

	while (getline(file, tmp)) {	//wczytaj caly string
		caly += tmp;
	}

	tmp = ZnajdzZnacznik("DIMENSION");

	string ilosc;
	while (tmp[0] > 47 && tmp[0] < 58 )		//wczytanie wymiaru
	{
		ilosc += tmp[0];
		tmp = tmp.substr(1);
	}	
	sscanf_s(ilosc.c_str(), "%d", &liczbaMiast);

	dane = ZnajdzZnacznik("EDGE_WEIGHT_SECTIO");
	int eof = dane.find("EOF");
	dane = dane.substr(0, eof);

	tmp = ZnajdzZnacznik("EDGE_WEIGHT_FORMAT");
	if (tmp.find("FULL_MATRIX") == 0) {
		WczytajFull();
	}
	else if (tmp.find("UPPER_ROW") == 0) {
		WczytajUpperRow();
	}

	//sprawdzenie poprawnosci wczytania ( wyrzuca na ekran)
	i1 = dist.begin();
	for (i1; i1 < dist.end(); ++i1) {
		i2 = i1->begin();
		for (i2; i2 < i1->end(); ++i2) {
			cout << *i2 << " ";
		}
		cout << endl;
	}

	return true;
}

Reader::Reader()
{
}


Reader::~Reader()
{
}
