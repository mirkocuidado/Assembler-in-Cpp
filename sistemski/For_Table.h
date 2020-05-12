#pragma once

#include <string>;
#include <iostream>;

using namespace std;

class For_Table
{

private:
	int patch;
	string symbol_name;
	int section_number;
	char size;
	bool skok;
	string sekcija_u_kojoj_je_nastao_skok;
	int linker;
	bool ima_pomeraj;

public:
	For_Table* next;

	For_Table() {
		next = nullptr;
	};

	For_Table(int p, string s, int n, string sek, char ss = ' ') {
		patch = p;
		symbol_name = s;
		next = nullptr;
		section_number = n;
		size = ss;
		skok = false;
		ima_pomeraj = false;
		sekcija_u_kojoj_je_nastao_skok = sek;
		linker = 0;
	}

	int getPatch() { return patch; }
	string getSymbol() { return symbol_name; }
	char getSize() { return size; }
	int getSectionNumber() { return section_number; }
	string getSekcija_U_Kojoj_Je_Nastao_Skok() { return sekcija_u_kojoj_je_nastao_skok; }
	bool getIsSkok() { return skok; }
	int getLinker() { return linker; }
	bool getIma_Pomeraj() { return ima_pomeraj; }

	void setIma_Pomeraj() { ima_pomeraj = true; }
	void setSize(char s) { size = s; }
	void setSkok() { skok = true; }
	void setLinker(int l) { linker = l; }
	~For_Table();
};

