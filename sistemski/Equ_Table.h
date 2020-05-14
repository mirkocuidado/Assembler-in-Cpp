#pragma once

#include <string>;
#include <iostream>;
#include <vector>;

using namespace std;

class Equ_Table
{

private:
	string symbol;
	vector<string> niz;
	vector<string> znaci;

	// igranje za indeks klasifikacije

	int indeks_klasifikacije;


public:
	Equ_Table* next;

	Equ_Table(string s, int br, string* ss, int br2, string * sz) {
		symbol = s;

		for (int i = 0; i < br; i++)
			niz.push_back(ss[i]);

		if (br > br2) {
			znaci.push_back("+");
			for (int i = 0; i < br2; i++)
				znaci.push_back(sz[i]);
		}
		else {
			for (int i = 0; i < br; i++)
				znaci.push_back(sz[i]);
		}

		indeks_klasifikacije = -1;

	}

	string getSymbol() { return symbol; }

	string getNiz() {
		string pom = "";
		for (int i = 0; i < niz.size(); i++)
			pom += niz[i] + " " ;

		return pom;
	}

	string getZnaci() {
		string pom = "";
		for (int i = 0; i < znaci.size(); i++)
			pom += znaci[i] + " ";

		return pom;
	}

	int getNizSize() { return niz.size(); }
	int getZnaciSize() { return znaci.size(); }

	vector<string> getNizBukvalno() {
		return niz;
	}

	vector<string> getZnaciBukvalno() {
		return znaci;
	}

	void setSymbol(string s, int a) { niz.push_back(s);}
	void setZnak(string s, int a) { znaci.push_back(s); }

	void loseSymbol(int a) { niz.erase(niz.begin() + a);}
	void loseZnak(int a) {
		znaci.erase(znaci.begin() + a);}

	~Equ_Table() {}

	// igranje za indeks klasifikacije

	void setIK(int a) { indeks_klasifikacije = a; }
	int getIK() { return indeks_klasifikacije; }

};
