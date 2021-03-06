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
	string menjaj_me_linkeru;

public:
	Symbol_Table* next;
	vector<string> kod;

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
		menjaj_me_linkeru = "MENJAJ ME";
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
	string getMenjaj_Me_Linekru() { return menjaj_me_linkeru; }

	void setEqu() { equ = true; }
	void setRbr(int r) { rbr = r; }
	void setName(string s) { name = s; }
	void setSection(int s) { section = s; }
	void setlg(char l) { lg = l; }
	void setDefined(bool b) { defined = b; }
	void setValue(int v) { value = v; }
	void setSize(int s) { size = s; }
	void setMenjaj_Me_Linekru(string s) { menjaj_me_linkeru = s; }

	void setCode(string s) {
		s += " ";
		kod.push_back(s);
	}

	void changeCode(int i, string s) {
		if (s.size() < 1) {
			s = "0" + s + " ";
		}
		else s += " ";
		kod[i] = s;
	}

};

