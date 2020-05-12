#pragma once

#include <string>;
#include <iostream>;

using namespace std;

class Equ_Table
{

private:
	string symbol;
	string* niz;
	string *znaci;
	int size;
	int size2;

public:
	Equ_Table* next;

	Equ_Table(string s, int br, string* ss, int br2, string * sz) {
		symbol = s;
		size = br;
		size2 = br2;

		niz = new string[br];
		for (int i = 0; i < br; i++)
			niz[i] = ss[i];


		znaci = new string[br];
		if (br > br2) {
			size2 = br;
			znaci[0] = '+';
			for (int i = 1; i < br2 + 1; i++)
				znaci[i] = sz[i - 1];
		}
		else {
			for (int i = 0; i < br; i++)
				znaci[i] = sz[i];
		}
	}

	string getSymbol() { return symbol; }
	string getNiz() {
		string pom = "";
		for (int i = 0; i < size; i++)
			pom += niz[i] + " ";

		return pom;
	}
	string getZnaci() {
		string pom = "";
		for (int i = 0; i < size; i++)
			pom += znaci[i] + " ";

		return pom;
	}

	int getNizSize() { return size; }
	int getZnaciSize() { return size2; }

	string* getNizBukvalno() {
		return niz;
	}

	string* getZnaciBukvalno() {
		return znaci;
	}

	void setSymbol(string s, int a) { niz[a] = s; }

	~Equ_Table() {}
};
