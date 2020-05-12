#pragma once
#include <string>
#include <iostream>
#include <array>

#include "For_Table.h";

using namespace std;

class Symbol_Table
{
private:
	int rbr;
	string name;
	int section;
	char lg;
	bool defined;
	int value;
	int size;
	For_Table* flink;
	int codeINDEX;

	bool equ;


public:
	Symbol_Table* next;
	std::array<string, 1000> kod;

public:
	Symbol_Table() {
		rbr = 0;
		name = "";
		section = 0;
		lg = 'N';
		defined = false;
		value = 0;
		size = 0;
		equ = false;
		next = nullptr;
		flink = nullptr;
	};

	Symbol_Table(int r, string s, int br, char l, bool d, int v, int ss) {
		rbr = r;
		name = s;
		section = br;
		lg = l;
		defined = d;
		value = v;
		size = ss;
		equ = false;
		next = nullptr;
		flink = nullptr;
		codeINDEX = 0;
	}

	~Symbol_Table();

	int getRbr() { return rbr; }
	string getName() { return name; }
	char getlg() { return lg; }
	bool getDefined() { return defined; }
	int getValue() { return value; }
	int getSize() { return size; }
	int getSection() { return section; }
	bool getEqu() { return equ; }

	void setEqu() { equ = true; }
	void setRbr(int r) { rbr = r; }
	void setName(string s) { name = s; }
	void setSection(int s) { section = s; }
	void setlg(char l) { lg = l; }
	void setDefined(bool b) { defined = b; }
	void setValue(int v) { value = v; }
	void setSize(int s) { size = s; }

	void setCode(string s) {
		s += " ";
		kod[codeINDEX++] = s;
	}

	void changeCode(int i, string s) {
		if (s.size() < 1) {
			s = "0" + s + " ";
		}
		else s += " ";
		kod[i] = s;
	}

};

