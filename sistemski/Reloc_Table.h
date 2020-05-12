#pragma once

#include <string>;
#include <iostream>;

using namespace std;

class Reloc_Table
{

private:
	int rbr;
	int sym_table_entry;
	int adress;
	int linker;
	string way;

public:
	Reloc_Table* next;

	Reloc_Table(int r, int s, int a, string w) {
		int rbr = r;
		sym_table_entry = s;
		adress = a;
		way = w;
		next = nullptr;
		linker = 0;
	}

	int getSym() { return sym_table_entry; }
	string getWay() { return way; }
	int getAdress() { return adress; }
	int getRbr() { return rbr; }
	int getLinker() { return linker; }

	void setSym(int s) { sym_table_entry = s; }
	void setRbr(int r) { rbr = r; }
	void setWay(string w) { way = w; }
	void setAdress(int a) { adress = a; }
	void setLinker(int l) { linker = l; }

	~Reloc_Table();
};

