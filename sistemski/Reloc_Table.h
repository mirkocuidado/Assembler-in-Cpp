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
	int section;
	char sign;
	char size;

public:
	Reloc_Table* next;

	Reloc_Table(int r, int s, int a, string w) {
		int rbr = r;
		sym_table_entry = s;
		adress = a;
		way = w;
		next = nullptr;
		linker = 0;
		sign = '+';
		section = -10;
		size = 'w';
	}

	int getSym() { return sym_table_entry; }
	string getWay() { return way; }
	int getAdress() { return adress; }
	int getRbr() { return rbr; }
	int getLinker() { return linker; }
	char getSign() { return sign; }
	int getSection() { return section; }
	char getSize() { return size; }

	void setSize(char s) { size = s; }
	void setSym(int s) { sym_table_entry = s; }
	void setRbr(int r) { rbr = r; }
	void setWay(string w) { way = w; }
	void setAdress(int a) { adress = a; }
	void setLinker(int l) { linker = l; }
	void setSign(char c) { sign = c; }
	void setSection(int ss) { section = ss; }

	~Reloc_Table();
};

