#include <fstream>
#include <string>
#include <iostream>
#include <regex>
#include <iomanip>
#include <string.h>
#include <sstream>
#include <exception>
#include <vector>

#include "Symbol_Table.h"
#include "For_Table.h"
#include "Reloc_Table.h"
#include "Equ_Table.h"

using namespace std;

int brojR = 1;
int brojRR = 1;

Symbol_Table* prvi = new Symbol_Table(0, "UND", 0, 'l', true, 0, 0);
Symbol_Table *posl = prvi;

Reloc_Table* prviR, *poslR = nullptr;
For_Table* prviF, *poslF = nullptr;
Equ_Table* prviE, *poslE = nullptr;

string binary_hexa(string s) {
	string pom = "";
	string binToHex = "";
	for (int j = 0; j < s.size(); j += 4) {
		pom = s.substr(j, 4);
		if (!pom.compare("0000")) binToHex += "0";
		else if (!pom.compare("0001")) binToHex += "1";
		else if (!pom.compare("0010")) binToHex += "2";
		else if (!pom.compare("0011")) binToHex += "3";
		else if (!pom.compare("0100")) binToHex += "4";
		else if (!pom.compare("0101")) binToHex += "5";
		else if (!pom.compare("0110")) binToHex += "6";
		else if (!pom.compare("0111")) binToHex += "7";
		else if (!pom.compare("1000")) binToHex += "8";
		else if (!pom.compare("1001")) binToHex += "9";
		else if (!pom.compare("1010")) binToHex += "A";
		else if (!pom.compare("1011")) binToHex += "B";
		else if (!pom.compare("1100")) binToHex += "C";
		else if (!pom.compare("1101")) binToHex += "D";
		else if (!pom.compare("1110")) binToHex += "E";
		else if (!pom.compare("1111")) binToHex += "F";
		else throw "Error in converting binary-hexa! ";

		pom = "";
	}
	return binToHex;
}

string lose_extra_white_space(string s) {
	while (s[0] == ' ' || s[0] == '\t') {
		s.erase(0, 1);
	}

	string pom = "";
	for (int j = 0; j < s.size(); j++) {

		pom += s[j];

		if ((j != (s.size() - 1)) && ((s[j + 1] == ' ') || (s[j + 1] == '\t'))) {
			while ((j != (s.size() - 1)) && ((s[j + 1] == ' ') || (s[j + 1] == '\t'))) {
				j++;
			}
			pom += " ";
		}
	}
	return pom + '\r';
}

string decToBinary(int n, int p)
{
	string s = "";
	if (p == 1) {
		for (int i = 7; i >= 0; i--) {
			int k = n >> i;
			if (k & 1)
				s += "1";
			else
				s += "0";
		}
		return s;
	}
	else if (p == 2) {
		for (int i = 15; i >= 0; i--) {
			int k = n >> i;
			if (k & 1)
				s += "1";
			else
				s += "0";
		}

		string ss = "";
		for (int i = 8; i < 16; i++) ss += s[i];
		for (int i = 0; i < 8; i++) ss += s[i];
		return ss;
	}
}

string getReg(string s2) {
	if (s2[1] == '0') return "00000";
	else if (s2[1] == '1') return "00010";
	else if (s2[1] == '2') return "00100";
	else if (s2[1] == '3') return "00110";
	else if (s2[1] == '4') return "01000";
	else if (s2[1] == '5') return "01010";
	else if (s2[1] == '6') return "01100";
	else if (s2[1] == '7' || s2 == "pc" || s2 == "PC") return "01110";
	else if (s2 == "PSW" || s2 == "psw") return "11110";
	else throw "Error in converting binary-hexa! ";
}

string getRegKRACI(string s2) {
	string luck = "";
	if (s2[1] == '0') luck += "0000";
	else if (s2[1] == '1') luck += "0001";
	else if (s2[1] == '2') luck += "0010";
	else if (s2[1] == '3') luck += "0011";
	else if (s2[1] == '4') luck += "0100";
	else if (s2[1] == '5') luck += "0101";
	else if (s2[1] == '6') luck += "0110";
	else if (s2[1] == '7' || s2 == "PC" || s2 == "pc") luck += "0111";
	else if (s2 == "PSW" || s2 == "psw") luck += "1111";
	else throw "Error in converting binary-hexa! ";

	if (s2[s2.size() - 1] == 'h') luck += "1";
	else luck += "0";

	return luck;
}

Symbol_Table* isInSymbol_Table(string s) {
	for (Symbol_Table* tek = prvi; tek != nullptr; tek = tek->next) {
		if (tek->getName() == s) {
			return tek;
		}
	}
	return nullptr;
}

Symbol_Table* getCodeNumberMain(int r) {
	for (Symbol_Table* tek = prvi; tek != nullptr; tek = tek->next) {
		if (tek->getRbr() == r) {
			return tek;
		}
	}
	return nullptr;
}

void add(string n, int brSek, char l, bool def, int val, int s) {
	posl->next = new Symbol_Table(brojR, n, brSek, l, def, val, s);
	posl = posl->next;

	brojR++;
}

void addR(int s, int a, string w) {
	if (prviR == nullptr) {
		prviR = new Reloc_Table(brojRR, s, a, w);
		poslR = prviR;
	}
	else {
		poslR->next = new Reloc_Table(brojRR, s, a, w);
		poslR = poslR->next;
	}
	poslR->setRbr(brojRR++);
}

void addE(string s, int a, string* ss, int b, string* sz) {
	if (prviE == nullptr) {
		prviE = new Equ_Table(s, a, ss, b, sz);
		poslE = prviE;
	}
	else {
		poslE->next = new Equ_Table(s, a, ss, b, sz);
		poslE = poslE->next;
	}
}

void addF(int p, string s, int sn, string sek, char ss = ' ') {
	if (prviF == nullptr) {
		prviF = new For_Table(p, s, sn, sek, ss);
		poslF = prviF;
	}
	else {
		poslF->next = new For_Table(p, s, sn, sek, ss);
		poslF = poslF->next;
	}
}

bool circle() {
	regex numberr("^(\\+|-)?[0-9]+|(\\+|-)?(0x|0X)[0-9A-Fa-f]{2}|(\\+|-)?(0x|0X)[0-9A-Fa-f]{4}$");
	for (Equ_Table* tek = prviE; tek != nullptr; tek = tek->next) {
		vector<string> s1 = tek->getNizBukvalno();
		for (int i = 0; i < s1.size(); i++) {
			if (regex_match(s1[i], numberr) == false) {
				for (Equ_Table* tek2 = prviE; tek2 != nullptr; tek2 = tek2->next) {
					if (tek2 != tek) {
						if (tek2->getSymbol() == s1[i]) {
							vector<string> s2 = tek2->getNizBukvalno();
							int size2 = tek2->getNizSize();
							for (int j = 0; j < size2; j++) {
								if (s2[j] == tek->getSymbol()) {
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

void rewriteCode(Symbol_Table* pom, For_Table* tek, Symbol_Table* c) {
	char vel = tek->getSize();
	if (vel == 'b') {
		if (pom->getValue() > 127 || pom->getValue() < -129) throw "Byte forward table element with size of Word. ";
		string s = decToBinary(pom->getValue(), 1);
		s = binary_hexa(s);
		c->changeCode(tek->getPatch(), s);
	}
	else {
		string s = decToBinary(pom->getValue(), 2);
		s = binary_hexa(s);
		string s1 = "";
		string s2 = "";

		for (int i = 0; i < s.size(); i++)
			if (i <= 1) s1 += s[i];
			else s2 += s[i];

		c->changeCode(tek->getPatch(), s1);
		c->changeCode(tek->getPatch() + 1, s2);
	}
}

void process_move(string s4, int LC, int BROJ_SEKCIJE, string SEKCIJA, int pomerajLC) {
	Symbol_Table* pom = isInSymbol_Table(s4);
	if (pom == nullptr) {
		add(s4, -5, 'l', false, 0, 0);
	}
	int av = LC + pomerajLC;
	addF(av, s4, BROJ_SEKCIJE, SEKCIJA, 'w');
}

int main(int argc, char* argv[]) {

	try {

		if (argc != 4) throw "Not enough parameters!";

		string o = argv[1];
		if (o != "-o") throw "Write -o in your command line call as a second argument!";

		cout << argv[argc - 1] << endl;
		ifstream inFile;
		//inFile.open(argv[argc-1]);
		inFile.open("test.txt");
		if (inFile.fail()) {
			throw "FILE ERROR! ";
			exit(1);
		}
		vector<string> globalni;
		vector<string> eksterni;
		vector<string> item;
		string pom;

		string codeC = "";
		int LC = 0;

		string SEKCIJA = "";
		int BROJ_SEKCIJE = 0;

		string LABELA = "";

		bool bilo_end = false;

		string niz[50];
		string znak[50];
		int nizI = 0;
		int nizZ = 0;

		regex numberr("^(\\+|-)?[0-9]+|(\\+|-)?(0x|0X)[0-9A-Fa-f]{2}|(\\+|-)?(0x|0X)[0-9A-Fa-f]{4}$");

		while (getline(inFile, pom)) {
			if (pom != "") item.push_back(pom);
			else continue;

			int brr = item.size();
			int i = brr - 1;
			item[i] = lose_extra_white_space(item[i]);

			/*for (Symbol_Table*tek = prvi; tek != nullptr; tek = tek->next) {
				//if (tek->getlg() == 'l' && tek->getSection() == -5 && tek->getDefined() == false) throw "ERROR! LOCAL AND UNDEFINED! ";
				cout << left;
				cout << setw(10) << tek->getRbr() << setw(10) << tek->getSection() << setw(10) << tek->getlg() << setw(10) << tek->getDefined()
					<< setw(10) << tek->getValue() << setw(10) << tek->getSize() << setw(10) << tek->getName() << setw(10) << tek->getEqu() << setw(10) << endl;
			}
			cout << endl;
			*/

			int br = 0;
			int jj = br;
			string s1 = "";

			bool isInstruction = false;
			bool daLiImamLabelu = false;
			bool reseno = false;
			int flag = 2;

			if (item[i][0] != '.') {
				while (item[i][jj] != '\r') {
					if (item[i][jj] == ':') {
						jj++; // preskace :

						while (item[i][jj] == ' ') jj++;
						if (item[i][jj] == '\r') flag = 1;
						else if (item[i][jj] == '.') { flag = 2; isInstruction = false; daLiImamLabelu = true; }
						else { flag = 2; isInstruction = true; daLiImamLabelu = true; }
						reseno = true;
						break;
					}
					else {
						s1 += item[i][jj];
					}
					jj++;
				}

				if (reseno == false) {
					bool samoZaPetLinija = false;
					for (int i = 0; i < s1.size(); i++) if (s1[i] == '.') {
						flag = 2; daLiImamLabelu = false; isInstruction = false; samoZaPetLinija = true; break;
					}
					if (samoZaPetLinija == false) {
						flag = 2; daLiImamLabelu = false; isInstruction = true;
					}
				}
				while (item[i][jj] == ' ') jj++;
			}
			else { flag = 2; isInstruction = false; daLiImamLabelu = false; }

			if (true) {
				// SAMO LABELA
				if (flag == 1) {
					if (BROJ_SEKCIJE == 0) throw "NOT IN SECTION! " + item[i];
					LABELA = s1;
					Symbol_Table* pomm = isInSymbol_Table(LABELA);
					if (pomm != nullptr) {
						for (int j = 0; j < eksterni.size(); j++) if (eksterni[j] == LABELA) { throw "CAN'T BE LABEL AND EXTERN! " + item[i]; break; }
						if (pomm->getDefined()) { throw "label: already defined ERROR " + item[i]; }
						else if (pomm->getDefined() == false) {
							pomm->setName(LABELA);
							pomm->setDefined(true);
							pomm->setValue(LC);
							pomm->setSection(BROJ_SEKCIJE);
						}
					}
					else {
						add(LABELA, BROJ_SEKCIJE, 'l', true, LC, 0);
					}
				}
				else if (flag == 2) {

					if (daLiImamLabelu == true) {
						LABELA = s1;
						if (BROJ_SEKCIJE == 0) throw "NOT IN SECTION! " + item[i];
						Symbol_Table* pomm = isInSymbol_Table(LABELA);
						if (pomm != nullptr) {
							for (int j = 0; j < eksterni.size(); j++) if (eksterni[j] == LABELA) { throw "CAN'T BE LABEL AND EXTERN! " + item[i]; break; }
							if (pomm->getDefined()) { throw "label: already defined ERROR " + item[i]; }
							else if (pomm->getDefined() == false) {
								pomm->setName(LABELA);
								pomm->setDefined(true);
								pomm->setValue(LC);
								pomm->setSection(BROJ_SEKCIJE);
							}
						}
						else {
							add(LABELA, BROJ_SEKCIJE, 'l', true, LC, 0);
						}
						br = jj;
					}
					else br = 0;
					while (item[i][br] == ' ') br++;

					if (flag == 2 && isInstruction == false) {
						string posleTacke = "";
						br++;

						while (item[i][br] != ' ' && item[i][br] != '\r') {
							posleTacke += item[i][br];
							br++;
						}
						while (item[i][br] == ' ')
							br++; // preskoci ' '

						// END:
						if (posleTacke == "end" || posleTacke == "end\r") {
							bilo_end = true;
							Symbol_Table* sec = isInSymbol_Table(SEKCIJA);
							if (sec != nullptr) {
								sec->setSize(LC);
								for (int i = 0; i < codeC.size(); i += 2) {
									string u = "";
									u += codeC[i]; u += codeC[i + 1];
									sec->setCode(u);
								}
								codeC = "";
							}
							break;
						}
						// SECTION:
						if (posleTacke == "section") {
							string s1 = "";
							while (item[i][br] != ':') {
								if (item[i][br] == ' ') throw "SECTION MUST BE ONE WORD!";
								s1 += item[i][br];
								br++;
							}
							br++; // preskoci ':'

							while (item[i][br] == ' ') br++;
							if (item[i][br] != '\r') throw "Error while reading .section " + item[i];

							Symbol_Table* pom = isInSymbol_Table(SEKCIJA);
							if (pom != nullptr) {
								if (pom->getRbr() != pom->getSection()) throw "SYMBOL !!! SECTION " + item[i];
								pom->setSize(LC);
								for (int i = 0; i < codeC.size(); i += 2) {
									string u = "";
									u += codeC[i]; u += codeC[i + 1];
									pom->setCode(u);
								}
								codeC = "";
								LC = 0;
							}

							SEKCIJA = s1;
							for (int j = 0; j < eksterni.size(); j++) if (eksterni[j] == SEKCIJA) { throw "SECTION CAN'T BE EXTERN!!! " + item[i]; break; }
							Symbol_Table* sec = isInSymbol_Table(SEKCIJA);
							if (sec != nullptr) {
								if (sec->getRbr() != sec->getSection()) throw "THERE IS A SYMBOL OF THIS NAME! " + item[i];
								LC = sec->getSize();
								BROJ_SEKCIJE = sec->getRbr();
								codeC = "";
							}
							else {
								add(SEKCIJA, -1, 'l', true, 0, 0);
								posl->setSection(posl->getRbr());
								codeC = "";
								BROJ_SEKCIJE = posl->getSection();
							}
						}
						// BYTE:
						else if (posleTacke == "byte") {
							if (BROJ_SEKCIJE == 0) throw "NOT IN SECTION!";

							string s1 = "";
							bool ima_param = false;
							bool zarez = false;

							while (item[i][br] != '\r') {
								while (item[i][br] == ' ') br++;
								while (item[i][br] != ',' && item[i][br] != '\r') {
									if (s1 != "" && item[i][br] == ' ') {
										while (item[i][br] == ' ') br++;
										if (item[i][br] == ',' || item[i][br] == '\r') break;
										else throw "ERROR! " + item[i];
									}
									s1 += item[i][br];
									zarez = false;
									br++;
								}

								if (item[i][br] == ',' && s1 == "") throw " ',' error! " + item[i];
								if (item[i][br] == ',' && s1 != "") zarez = true;
								if (item[i][br] == '\r' && zarez == true && ima_param == true) throw "ERROR!!! " + item[i];
								if (item[i][br] != '\r')
									br++;

								if (s1 != "") {
									ima_param = true;
									if (regex_match(s1, numberr)) {
										if (s1.size() > 4 && s1[0] == '0' && s1[1] == 'x') throw "ERROR WITH HEXA! " + item[i];
										if (stoi(s1, nullptr, 0) < -129 || stoi(s1, nullptr, 0) > 127) throw "ERROR OF SIZE FOR BYTE!";
										else {
											string s = decToBinary(stoi(s1, nullptr, 0), 1);
											codeC += binary_hexa(s);
										}
									}
									else {
										Symbol_Table* pomm = isInSymbol_Table(s1);
										codeC += "00";
										if (pomm != nullptr) {
											if (pomm->getDefined() == true && pomm->getlg() == 'g') {
												addR(pomm->getRbr(), LC, "direct"); poslR->setSize('b'); poslR->setSection(BROJ_SEKCIJE);
											}
											else if (pomm->getlg() == 'l') addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'b');
										}
										else {
											add(s1, -5, 'l', false, 0, 0);
											addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'b');
										}
									}
									LC += 1;
								}
								s1 = "";
							}
							if (ima_param == false) throw ".byte ERROR " + item[i];
							if (item[i][br - 1] == ',') throw ".byte ERROR " + item[i];
						}
						// WORD:
						else if (posleTacke == "word") {

							if (BROJ_SEKCIJE == 0) throw "NOT IN SECTION!";

							string s1 = "";

							bool ima_param = false;
							bool zarez = false;

							while (item[i][br] != '\r') {
								while (item[i][br] == ' ') br++;
								while (item[i][br] != ',' && item[i][br] != '\r') {
									if (s1 != "" && item[i][br] == ' ') {
										while (item[i][br] == ' ') br++;
										if (item[i][br] == ',' || item[i][br] == '\r') break;
										else throw "ERROR! " + item[i];
									}
									s1 += item[i][br];
									zarez = false;
									br++;
								}

								if (item[i][br] == ',' && s1 == "") throw " ',' error! " + item[i];
								if (item[i][br] == ',' && s1 != "") zarez = true;
								if (item[i][br] == '\r' && zarez == true && ima_param == true) throw "ERROR!!! " + item[i];
								if (item[i][br] != '\r')
									br++;

								if (s1 != "") {
									ima_param = true;
									if (regex_match(s1, numberr)) {
										if (s1.size() > 6 && s1[0] == '0' && s1[1] == 'x') throw "ERROR WITH HEXA! " + item[i];
										if (stoi(s1, nullptr, 0) < -32769 || stoi(s1, nullptr, 0) > 32767) throw "ERROR OF SIZE FOR WORD!";
										string s = decToBinary(stoi(s1, nullptr, 0), 2);
										codeC += binary_hexa(s);
									}
									else {
										Symbol_Table* pomm = isInSymbol_Table(s1);
										codeC += "0000";
										if (pomm != nullptr) {
											if (pomm->getDefined() == true && pomm->getlg() == 'g') addR(pomm->getRbr(), LC, "direct");
											else if (pomm->getlg() == 'l') addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'w');
										}
										else {
											add(s1, -5, 'l', false, 0, 0);
											addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'w');
										}
									}
									LC += 2;
								}
								s1 = "";
							}
							if (ima_param == false) throw ".word ERROR " + item[i];
							if (item[i][br - 1] == ',') throw ".word ERROR " + item[i];
						}
						// SKIP:
						else if (posleTacke == "skip") {
							if (BROJ_SEKCIJE == 0) throw "NOT IN SECTION!";

							string s1 = "";
							while (item[i][br] != '\r') {
								while (item[i][br] != '\r' && item[i][br] != ' ') {
									if (item[i][br] != ' ')
										s1 += item[i][br];
									br++;
								}
								while (item[i][br] == ' ')
									br++;
								if (item[i][br] != '\r') throw ".skip symbol ERROR " + item[i];

								if (s1 != "") {
									if (regex_match(s1, numberr)) {
										int a = stoi(s1, nullptr, 0); if (a < 0) throw "NEGATIVE NUMBER!";
										LC = LC + a;

										for (int i = 0; i < a; i++) {
											codeC += "00";
										}
									}
									else throw "NOT A NUMBER + .skip " + item[i];
								}
								else throw ".skip symbol ERROR " + item[i];
							}
							if (s1 == "") throw ".skip symbol ERROR " + item[i];
						}
						// GLOBAL:
						else if (posleTacke == "global") {
							string s1 = "";

							bool ima_param = false;
							bool zarez = false;

							while (item[i][br] != '\r') {
								while (item[i][br] == ' ') br++;
								while (item[i][br] != ',' && item[i][br] != '\r') {
									if (s1 != "" && item[i][br] == ' ') {
										while (item[i][br] == ' ') br++;
										if (item[i][br] == ',' || item[i][br] == '\r') break;
										else throw "ERROR! " + item[i];
									}
									s1 += item[i][br];
									zarez = false;
									br++;
								}

								if (item[i][br] == ',' && s1 == "") throw " ',' error! " + item[i];
								if (item[i][br] == ',' && s1 != "") zarez = true;
								if (item[i][br] == '\r' && zarez == true && ima_param == true) throw "ERROR!!! " + item[i];
								if (item[i][br] != '\r')
									br++;

								if (regex_match(s1, numberr)) throw ".global + number ERROR " + item[i];

								if (s1 != "") {
									globalni.push_back(s1);
									ima_param = true;
									Symbol_Table* pom = isInSymbol_Table(s1);
									if (pom != nullptr) {
										if (pom->getRbr() == pom->getSection()) throw "SECTION CAN'T BE GLOBAL" + item[i];
										for (int i = 0; i < eksterni.size(); i++) { if (eksterni[i] == s1) throw "CAN'T BE EXTERN AND GLOBAL!" + item[i]; }
										pom->setlg('g');
									}
									else add(s1, -5, 'g', false, 0, 0);
								}
								s1 = "";
							}
							if (ima_param == false) throw ".global ERROR " + item[i];
							if (item[i][br - 1] == ',') throw ".global ERROR " + item[i];
						}  // dodati jos LC i broj sekcije
						// EXTERN:
						else if (posleTacke == "extern") {
							string s1 = "";

							bool ima_param = false;
							bool zarez = false;

							while (item[i][br] != '\r') {
								while (item[i][br] == ' ') br++;
								while (item[i][br] != ',' && item[i][br] != '\r') {
									if (s1 != "" && item[i][br] == ' ') {
										while (item[i][br] == ' ') br++;
										if (item[i][br] == ',' || item[i][br] == '\r') break;
										else throw "ERROR! " + item[i];
									}
									s1 += item[i][br];
									zarez = false;
									br++;
								}

								if (item[i][br] == ',' && s1 == "") throw " ',' error! " + item[i];
								if (item[i][br] == ',' && s1 != "") zarez = true;
								if (item[i][br] == '\r' && zarez == true && ima_param == true) throw "ERROR!!! " + item[i];
								if (item[i][br] != '\r')
									br++;

								if (regex_match(s1, numberr)) throw ".extern + number ERROR " + item[i];

								if (s1 != "") {
									eksterni.push_back(s1);
									ima_param = true;
									Symbol_Table* pom = isInSymbol_Table(s1);
									if (pom != nullptr) {
										if (pom->getRbr() == pom->getSection()) throw "SECTION CAN'T BE EXTERN" + item[i];
										if (pom->getDefined() == true) throw "SYMBOL CAN'T BE DEFINED!" + item[i];
										for (int i = 0; i < globalni.size(); i++) { if (globalni[i] == s1) throw "CAN'T BE EXTERN AND GLOBAL!" + item[i]; }
										pom->setlg('g');
									}
									else add(s1, 0, 'g', false, 0, 0);

								}
								s1 = "";
							}
							if (ima_param == false) throw ".extern ERROR " + item[i];
							if (item[i][br - 1] == ',') throw ".extern ERROR " + item[i];
						}
						// EQU:
						else if (posleTacke == "equ") {
							string s2 = "";
							string s1 = "";

							while (item[i][br] != ',') {
								if (item[i][br] != ' ')
									s1 += item[i][br];
								br++;
							}

							br++; // preskoci ','
							while (item[i][br] == ' ') br++;

							Symbol_Table* pom = isInSymbol_Table(s1);
							if (pom != nullptr && pom->getDefined() == true) throw ".equ ERROR " + item[i];
							for (int i = 0; i < eksterni.size(); i++) if (s1 == eksterni[i]) throw "IT's EXTERN, CAN'T BE .equ! " + item[i];

							while (item[i][br] != '\r') {
								s2 += item[i][br];
								br++;
							}

							string pomocni = "";
							bool flagg = false;

							for (int j = 0; j < s2.size(); j++) {
								if (s2[j] != '+' && s2[j] != '-' && s2[j] != '\r') {
									if (s2[j] != ' ') {
										pomocni += s2[j];
									}
								}
								else {
									if (pomocni == "" && j != 0) throw "ERROR! " + item[i];
									else if (pomocni != "") {
										niz[nizI++] = pomocni;
										if (flagg == true || (flagg == false && nizZ == 0)) flagg = false; else throw "EXPRESSION ERROR 2! " + item[i];
										pomocni = "";
									}

									znak[nizZ++] = s2[j];
									if (flagg == false) flagg = true; else throw "EXPRESSION ERROR 1! " + item[i];
								}
							}
							if (pomocni != "" && pomocni != " ") {
								niz[nizI++] = pomocni;
								flagg = false;
							}

							if (flagg == true) throw "ERROR EXPRESSION 3! " + item[i];


							int izraz = 0;
							int flag = 0;

							for (int i = 0; i < nizI; i++) {
								if (regex_match(niz[i], numberr)) izraz += stoi(niz[i]);
								else {
									flag = 1;
									Symbol_Table* pompom = isInSymbol_Table(niz[i]);
									if (pompom == nullptr) add(niz[i], 0, 'l', false, 0, 0);
								}
							}

							// AKO JE SAMO OD BROJEVA DODAJ/AZURIRAJ VREDNOST U TABELI SIMBOLA I RECI LINKERU DA NE MENJA, A
							// AKO JE I OD SIMBOLA, ONDA DODAJ NOVI ZAPIS ZA EQU TABELU
							if (flag == 0) {
								if (pom == nullptr) {
									add(s1, -5, 'l', true, izraz, 0);
									posl->setEqu();
								}
								else {
									pom->setDefined(true);
									pom->setValue(izraz);
									pom->setSection(-5);
									pom->setEqu();
								}

								addE(s1, nizI, niz, nizZ, znak);
							}
							else {
								if (pom == nullptr) {
									add(s1, -5, 'l', true, 0, 0);
									posl->setEqu();
								}
								else {
									pom->setEqu(); //NE TREBA OVO JA MSM
									pom->setDefined(true);
									pom->setSection(-5);
								}
								addE(s1, nizI, niz, nizZ, znak);
							}

							izraz = 0;
							nizI = 0;
							nizZ = 0;
						}
						else throw "NOT RECOGNISED! " + item[i];
					}

					if (flag == 2 && isInstruction == true) {

						if (BROJ_SEKCIJE == 0) throw "NOT IN SECTION! " + item[i];

						string lucky = "";
						string s1 = "";
						string s2 = "";
						string s3 = "";
						string s4 = "";
						string s5 = "";
						int pazi = 0;

						while (item[i][br] != ' ') s1 += item[i][br++];
						while (item[i][br] == ' ') br++;

						if (s1 == "HALT") {
							LC++;
							codeC += "00";

							while (item[i][br] == ' ' || item[i][br] == '\t') br++;
							if (item[i][br] == '\r') continue; else throw "ERROR WITH OPERANDS! " + item[i];
						}
						else if (s1 == "IRET") {
							LC++;
							codeC += "08";

							while (item[i][br] == ' ' || item[i][br] == '\t') br++; if (item[i][br] == '\r') continue; else throw "ERROR WITH OPERANDS! " + item[i];
						}
						else if (s1 == "RET") {
							LC++;
							codeC += "10";

							while (item[i][br] == ' ' || item[i][br] == '\t') br++; if (item[i][br] == '\r') continue; else throw "ERROR WITH OPERANDS! " + item[i];
						}
						else {

							if (s1.size() == 4 || s1.size() == 5) {
								if (s1[s1.size() - 1] == 'B') pazi = 1; else if (s1[s1.size() - 1] == 'W') pazi = 2;
							}
							else if (s1.size() == 3) {
								if (s1 != "SUB") {
									if (s1[s1.size() - 1] == 'B') pazi = 1; else if (s1[s1.size() - 1] == 'W') pazi = 2;
								}
							}
							if (pazi != 0) {
								string pom = "";
								for (int i = 0; i < s1.size() - 1; i++) pom += s1[i];
								s1 = pom;
							}

							if (s1 == "PUSH") lucky += "01001";
							else if (s1 == "POP") lucky += "01010";

							// obrada za push i pop
							if (lucky != "") {
								//immed
								if (item[i][br] == '$') {
									if (s1 == "POP") throw "POP AND IMMED ERROR! " + item[i];
									if (pazi == 1) throw "CAN'T USE B WITH THIS! " + item[i];

									br++;
									while (item[i][br] == ' ') br++;
									while (item[i][br] != ' ' && item[i][br] != '\r') s2 += item[i][br++];
									while (item[i][br] == ' ') br++;
									if (item[i][br] != '\r') throw "ERROR WITH SPACE! " + item[i];

									if (pazi == 2) lucky += "1"; //ako je w, tada je size = 1
									else lucky += "0"; // za b ili nista je size=0, pa ako bude >255, menja se

									if (regex_match(s2, numberr)) {
										if (stoi(s2, nullptr, 0) > 32767 || stoi(s2, nullptr, 0) < -32768) throw "OPERAND TOO BIG! " + item[i];

										lucky += "0000000000";
										lucky += decToBinary(stoi(s2, nullptr, 0), 2);
										lucky[5] = '1';
									}
									else {
										lucky += "0000000000";
										lucky[5] = '1';
										lucky += decToBinary(0, 2);
										int a = LC + 2;
										addF(a, s2, BROJ_SEKCIJE, SEKCIJA, 'w');
									}

									LC += (lucky.size() / 8);
									codeC += binary_hexa(lucky);
								}
								//regdir
								else if (item[i][br] == '%') {

									if (pazi == 1) throw "CAN'T BE B AND REGDIR! " + item[i];

									br++;
									while (item[i][br] == ' ') br++;
									while (item[i][br] != ' ' && item[i][br] != '\r') s2 += item[i][br++];
									br++;
									if (br < item[i].size()) { while (item[i][br] == ' ') br++; if (item[i][br] != '\r') throw "ERROR WITH SPACE - regdir! " + item[i]; }
									else if (item[i][br - 1] != '\r') throw "ERROR WITH SPACE - regdir! " + item[i];
									if (s2 == "") throw "NO s2 HERE!" + item[i];

									if (s2.size() == 2 || s2 == "PSW") {
										lucky += "100";
										lucky += "001";
										lucky += getReg(s2);
									}
									else if (s2.size() == 3 || s2.size() == 4) { throw "CAN'T USE SUFIXES h or l HERE! " + item[i]; } // Ri high/low ili PC high/low ili PSW high/low
									else throw "NOT GOOD s2 HERE!" + item[i];

									LC += (lucky.size() / 8);
									codeC += binary_hexa(lucky);
								}
								//regind
								else if (item[i][br] == '(') {

									if (pazi == 1) throw "CAN'T BE B AND REGDIR! " + item[i];

									br++;
									if (item[i][br] == '%') {
										br++;
										while (item[i][br] != ' ' && item[i][br] != ')' && item[i][br] != '\r') { s2 += item[i][br]; br++; }
										if (item[i][br] == ' ') throw "ERROR WITH SPACE - regind! " + item[i];
										if (item[i][br] == '\r') throw "ERROR WITH NEW LINE - regind! " + item[i];
										else {
											br++;
											while (item[i][br] == ' ') br++;
											if (item[i][br] != '\r') throw "ERROR WITH SPACE - regind! " + item[i];
										}

										lucky += "100"; lucky += "010";

										if (s2.size() == 2 || s2 == "PSW") lucky += getReg(s2); else throw "CAN'T USE h or l HERE! " + item[i];
									}
									else throw "( AND NO %! " + item[i];

									LC += (lucky.size() / 8);
									codeC += binary_hexa(lucky);
								}
								else {
									if (pazi == 1) throw "CAN'T BE B AND REGDIR! " + item[i];

									while (item[i][br] != '(' && item[i][br] != '\r' && item[i][br] != ' ') s4 += item[i][br++];
									// memdir
									if (item[i][br] == ' ' || item[i][br] == '\r') {
										while (item[i][br] == ' ' && item[i][br] == '\t') br++; if (item[i][br] != '\r') throw "SPACE ERROR - memdir!";

										if (regex_match(s4, numberr) == false && s4[0] != '0' && s4[1] != 'x')
											process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, 2);

										lucky += "100";
										lucky += "10000000";

										if (regex_match(s4, numberr)) lucky += decToBinary(stoi(s4, nullptr, 0), 2);
										else lucky += "0000000000000000";

									}
									//regindpom
									else if (item[i][br] == '(') {
										br++;

										if (regex_match(s4, numberr) == false && s4[0] != '0' && s4[1] != 'x')
											process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, 2);

										lucky += "100011";
										if (item[i][br] == '%') {
											br++;
											while (item[i][br] != ')') if (item[i][br] != ' ' && item[i][br] != '\r') s2 += item[i][br++]; else throw "CAN'T USE SPACE! " + item[i];
											if (s2.size() == 2 || s2 == "PSW")
												lucky += getReg(s2);
											else throw "NOT GOOD s2 HERE!" + item[i];
										}
										else throw "MUST BE WITHOUT SPACE! " + item[i];

										if (regex_match(s4, numberr)) {
											lucky += decToBinary(stoi(s4, nullptr, 0), 2);
											int m = stoi(s4, nullptr, 0);
											if (s2 == "PC" || s2 == "pc" || s2 == "r7" || s2 == "R7") throw "PC only with non NUMBERS! " + item[i];
										}
										else {
											if (s2 == "PC" || s2 == "pc" || s2 == "r7" || s2 == "R7") {

												poslF->setLinker(-2);
												poslF->setIma_Pomeraj();
											}
											lucky += "0000000000000000";
										}

									}
									else throw "NOTHING! " + item[i];

									LC += (lucky.size() / 8);
									codeC += binary_hexa(lucky);
								}
							}

							//obrada za MOV i aritmeticke
							if (lucky == "") {
								if (s1 == "XCHG") lucky += "01011";
								else if (s1 == "MOV") lucky += "01100";
								else if (s1 == "ADD") lucky += "01101";
								else if (s1 == "SUB") lucky += "01110";
								else if (s1 == "MUL") lucky += "01111";
								else if (s1 == "DIV") lucky += "10000";
								else if (s1 == "CMP") lucky += "10001";
								else if (s1 == "NOT") lucky += "10010";
								else if (s1 == "AND") lucky += "10011";
								else if (s1 == "OR")  lucky += "10100";
								else if (s1 == "XOR") lucky += "10101";
								else if (s1 == "TEST") lucky += "10110";
								else if (s1 == "SHL") lucky += "10111";
								else if (s1 == "SHR") lucky += "11000";

								if (lucky != "") {
									// IMMED
									if (item[i][br] == '$') {

										if (s1 == "XCHG") throw "XCHG can't go with immed! " + item[i];

										br++;
										if (item[i][br] == ' ') throw "CAN'T SEPERATE $ AND OPERAND! " + item[i];

										while (item[i][br] != ' ' && item[i][br] != ',') s2 += item[i][br++];
										if (item[i][br] == ' ') {
											while (item[i][br] == ' ') br++;
											if (item[i][br] != ',') throw "ERROR IMMED AND NO , " + item[i];
											else {
												br++;  while (item[i][br] == ' ') br++;
											}// preskoci ' ' sve do sledeceg
										}
										else {
											br++;
											while (item[i][br] == ' ') br++;  // preskoci ' ' sve do sledeceg
										}

										if (pazi == 2) lucky += "1";
										else lucky += "0";

										if (regex_match(s2, numberr)) {
											if (stoi(s2, nullptr, 0) < 128 && stoi(s2, nullptr, 0) > -129) {
												lucky += "0000000000";
												if (pazi == 0) lucky += "";
												else if (pazi == 1) lucky += decToBinary(stoi(s2, nullptr, 0), 1);
												else if (pazi == 2) lucky += decToBinary(stoi(s2, nullptr, 0), 2);
											}
											else {
												if (stoi(s2, nullptr, 0) > 32767 || stoi(s2, nullptr, 0) < -32768) throw "OPERAND TOO BIG! " + item[i];
												if (pazi == 1) throw "Instruction with sufix B and >255 number ERROR!" + item[i];
												lucky[5] = '1';
												lucky += "0000000000";
												lucky += decToBinary(stoi(s2, nullptr, 0), 2);
											}
										}
										else {

											lucky += "0000000000";
											Symbol_Table* pom = isInSymbol_Table(s2);
											if (pom == nullptr)
												add(s2, -5, 'l', false, 0, 0);
											int a = LC + 2;
											if (pazi == 1) {
												lucky += "00000000";
												addF(a, s2, BROJ_SEKCIJE, SEKCIJA, 'b');
											}
											else {
												lucky += "0000000000000000";
												addF(a, s2, BROJ_SEKCIJE, SEKCIJA, 'w');
											}
										}

										if (item[i][br] == '$') throw "CAN'T USE IMMED HERE! " + item[i];
										// immed - regdir
										if (item[i][br] == '%') {
											br++;
											if (item[i][br] == ' ') throw "YOU CAN'T HAVE % AND THEN ' ' " + item[i];
											while (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++];
											br++;
											if (br < item[i].size()) { while (item[i][br] == ' ') br++; if (item[i][br] != '\r') throw "ERROR WITH SPACE - regdir! " + item[i]; }
											else if (item[i][br - 1] != '\r') throw "ERROR WITH SPACE - regdir! " + item[i];
											if (s3 == "") throw "NO s2 HERE!" + item[i];

											if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") { //s3 je registar bez h ili l

												if (pazi == 1) throw "CAN'T USE B SUFIX AND WHOLE REGISTER! " + item[i];

												if (regex_match(s2, numberr)) {
													if (pazi == 0 && stoi(s2, nullptr, 0) < 128 && stoi(s2, nullptr, 0) > -129)
														lucky += decToBinary(stoi(s2, nullptr, 0), 2);
												}

												lucky += "001"; //regdir
												lucky += getReg(s3); //registar + 0 za high/low
											}
											else if (s3.size() == 3 || s3 == "PSWh" || s3 == "PSWH" || s3 == "PSWl" || s3 == "PSWL") { // s3 je registar sa h ili l
												lucky[5] = '0';
												if (pazi == 2) throw "Instruction with sufix W and h/l ERROR!" + item[i]; // ne sme MOVW sa high.low
												if (regex_match(s2, numberr)) {
													if (stoi(s2, nullptr, 0) < -128 || stoi(s2, nullptr, 0) > 127) throw "h/l and too big number ERROR!" + item[i]; // ne sme npr. MOV $1000, %R1 high/low
													if (pazi == 0) lucky += decToBinary(stoi(s2, nullptr, 0), 1);
												}
												else throw "Address is 16b and here you have high/low" + item[i];

												lucky += "001"; //regdir
												lucky += getRegKRACI(s3); //registar + 0 za high/low
											}
											else {
												throw "Register size ERROR!" + item[i];
											}
										}
										// immed - regind
										else if (item[i][br] == '(') {

											br++;

											if (pazi == 0 && regex_match(s2, numberr)) {
												if (stoi(s2, nullptr, 0) < 128 && stoi(s2, nullptr, 0) > -129) { lucky += decToBinary(stoi(s2, nullptr, 0), 1); lucky[5] = '0'; }
												else {
													lucky += decToBinary(stoi(s2, nullptr, 0), 2); lucky[5] = '1';
												}
											}

											lucky += "010";
											if (item[i][br] == '%') {
												br++;
												while (item[i][br] != ' ' && item[i][br] != ')' && item[i][br] != '\r') { s3 += item[i][br]; br++; }
												if (item[i][br] == ' ') throw "ERROR WITH SPACE - regind! " + item[i];
												if (item[i][br] == '\r') throw "ERROR WITH NEW LINE - regind! " + item[i];
												else {
													br++;
													while (item[i][br] == ' ') br++;
													if (item[i][br] != '\r') throw "ERROR WITH SPACE - regind! " + item[i];
												}

												if (s3.size() == 2 || s3 == "PSW" || s3 == "psw")
													lucky += getReg(s3);
												else throw "ERROR WITH REGISTER! " + item[i];
											}
											else throw "regind but no % ERROR!" + item[i];
										}
										// immed - regindpom/memdir
										else {
											while (item[i][br] != '(' && item[i][br] != '\r' && item[i][br] != ' ') s4 += item[i][br++];

											//memdir
											if (item[i][br] == ' ' || item[i][br] == '\r') {

												if (pazi == 0 && regex_match(s2, numberr)) {
													if (stoi(s2, nullptr, 0) < 128 && stoi(s2, nullptr, 0) > -129) { lucky += decToBinary(stoi(s2, nullptr, 0), 1); lucky[5] = '0'; }
												}

												while (item[i][br] == ' ' || item[i][br] == '\t') br++; if (item[i][br] != '\r') throw "SPACE ERROR - memdir!";

												if (regex_match(s4, numberr) == false)
													process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, (lucky.size() / 8 + 1));

												lucky += "10000000";
												if (regex_match(s4, numberr)) {
													lucky[5] = '1';
													lucky += decToBinary(stoi(s4, nullptr, 0), 2);
												}
												else {
													lucky += "0000000000000000";
												}
											}
											//regindpom
											else if (item[i][br] == '(') {

												if (pazi == 0 && regex_match(s2, numberr)) {
													if (stoi(s2, nullptr, 0) < 128 && stoi(s2, nullptr, 0) > -129) { lucky += decToBinary(stoi(s2, nullptr, 0), 1); lucky[5] = '0'; }
												}

												br++;

												if (regex_match(s4, numberr) == false)
													process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, (lucky.size() / 8 + 1));

												lucky += "011";
												if (item[i][br] == '%') {
													br++;
													while (item[i][br] != ')') if (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++]; else throw "CAN'T USE SPACE! " + item[i];
													if (item[i][br] == ')') br++;
													while (item[i][br] == ' ') br++;
													if (item[i][br] != '\r') throw "SOME GARBAGE AFTER REGINDPOM! " + item[i];

													if (s3.size() == 2 || s3 == "PSW" || s3 == "psw")
														lucky += getReg(s3);
													else throw "NOT GOOD s2 HERE!" + item[i];
												}
												else throw "MUST BE WITHOUT SPACE! " + item[i];

												if (regex_match(s4, numberr)) {
													lucky += decToBinary(stoi(s4, nullptr, 0), 2);
												}
												else {
													if (s3 == "PC" || s3 == "pc" || s3 == "r7" || s3 == "R7") {
														poslF->setLinker(-2);
														poslF->setIma_Pomeraj();
														lucky += "0000000000000000";
													}
													else lucky += "0000000000000000";
												}
											}
											else {
												throw "NOTHING! " + item[i];
											}
										}
										LC += (lucky.size() / 8);
										codeC += binary_hexa(lucky);
									}
									// REGDIR
									else if (item[i][br] == '%') {
										br++;
										if (item[i][br] == ' ') throw "YOU CAN'T HAVE % AND THEN ' ' " + item[i];
										while (item[i][br] != ' ' && item[i][br] != ',') s2 += item[i][br++];
										if (item[i][br] == ' ') {
											while (item[i][br] == ' ') br++; if (item[i][br] != ',') throw "ERROR, NO COMA!";
										}
										else {
											br++;
											while (item[i][br] == ' ') br++;
										}

										if (s2.size() == 2 || s2 == "PSW" || s2 == "psw") {
											if (pazi == 2) lucky += "100";
											else lucky += "000";

											lucky += "001";

											if (pazi == 1) throw "Instruction with sufix B and regdir ERROR!" + item[i];
											lucky += getReg(s2);
										}
										else if (s2.size() == 3 || s2 == "PSWh" || s2 == "PSWH" || s2 == "PSWl" || s2 == "PSWL") {

											lucky += "000";
											lucky += "001";
											if (pazi == 2) throw "Instruction with sufix W and h/l ERROR!" + item[i];

											lucky += getRegKRACI(s2);
										}
										else throw "reg size ERROR!" + item[i];

										while (item[i][br] == ' ' || item[i][br] == ',') br++; //ovo ne mora, ali bolje leciti, nego spreciti!

										if (item[i][br] == '$') throw "CAN'T USE IMMED HERE! " + item[i];
										// regdir - regdir
										if (item[i][br] == '%') {
											br++;
											if (item[i][br] == ' ') throw "YOU CAN'T HAVE % AND THEN ' ' " + item[i];
											while (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++];
											if (item[i][br] == ' ') {
												while (item[i][br] == ' ') br++; if (item[i][br] != '\r') throw "ERROR, NO COMA!";
											}

											if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") {
												if (pazi == 1) throw "ERROR!" + item[i];
												lucky[5] = '1';

												if (s2.size() == 3)  throw "ERROR!" + item[i];

												lucky += "001";
												lucky += getReg(s3);
											}
											else if (s3.size() == 3 || s3 == "PSWh" || s3 == "PSWH" || s3 == "PSWl" || s3 == "PSWL") {

												lucky += "001";

												if (pazi == 2) throw "ERROR!" + item[i];
												if (pazi == 0 && s2.size() == 2)  throw "ERROR!" + item[i];

												if (s2.size() == 2)  throw "ERROR!" + item[i];

												lucky += getRegKRACI(s3);
											}
										}
										// regdir - regind
										else if (item[i][br] == '(') {
											br++;
											if (item[i][br] == '%') {
												br++;
												while (item[i][br] != ' ' && item[i][br] != ')' && item[i][br] != '\r') { s3 += item[i][br]; br++; }
												if (item[i][br] == ' ') throw "ERROR WITH SPACE - regind! " + item[i];
												if (item[i][br] == '\r') throw "ERROR WITH NEW LINE - regind! " + item[i];
												else {
													br++;
													while (item[i][br] == ' ') br++;
													if (item[i][br] != '\r') throw "ERROR WITH SPACE - regind! " + item[i];
												}

												if (pazi == 0 && s2.size() == 2) lucky[5] = '1';

												lucky += "010";

												if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") lucky += getReg(s3);
												else throw "ERROR!" + item[i];
											}
											else throw "ERROR!" + item[i];
										}
										// regdir - regindpom/memdir
										else {
											while (item[i][br] != '(' && item[i][br] != '\r' && item[i][br] != ' ') s4 += item[i][br++];

											//memdir
											if (item[i][br] == ' ' || item[i][br] == '\r') {
												while (item[i][br] == ' ' || item[i][br] == '\t') br++; if (item[i][br] != '\r') throw "SPACE ERROR - memdir!";

												if (regex_match(s4, numberr) == false)
													process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, 3);

												lucky += "10000000";
												if (pazi == 0 && s2.size() == 2) lucky[5] = '1';

												if (regex_match(s4, numberr)) lucky += decToBinary(stoi(s4, nullptr, 0), 2);
												else lucky += "0000000000000000";

											}
											// regindpom
											else if (item[i][br] == '(') {
												br++;

												if (regex_match(s4, numberr) == false)
													process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, 3);

												lucky += "011";

												if (item[i][br] == '%') {
													br++;
													while (item[i][br] != ')') if (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++]; else throw "CAN'T USE SPACE! " + item[i];
													if (item[i][br] == ')') br++;
													while (item[i][br] == ' ') br++;
													if (item[i][br] != '\r') throw "SOME GARBAGE AFTER REGIND! " + item[i];

													if (s3.size() == 2 || s3 == "PSW" || s3 == "psw")
														lucky += getReg(s3);
													else throw "NOT GOOD s2 HERE!" + item[i];
												}
												else throw "MUST BE WITHOUT SPACE! " + item[i];

												if (pazi == 0 && s2.size() == 2) lucky[5] = '1';

												if (regex_match(s4, numberr)) {
													lucky += decToBinary(stoi(s4), 2);
												}
												else {
													if (s3 == "PC" || s3 == "pc" || s3 == "r7" || s3 == "R7") {
														poslF->setLinker(-2);
														poslF->setIma_Pomeraj();
													}
													lucky += "0000000000000000";
												}

											}
											else {
												throw "UNKNOWN! " + item[i];
											}
										}
										LC += lucky.size() / 8;
										codeC += binary_hexa(lucky);
									}
									// REGIND
									else if (item[i][br] == '(') {
										br++;
										if (item[i][br] == '%') {
											br++;
											while (item[i][br] != ' ' && item[i][br] != ')') { s2 += item[i][br]; br++; }
											if (item[i][br] == ' ') throw "ERROR WITH SPACE - regind! " + item[i];
											else {
												br++;
												while (item[i][br] == ' ') br++;
												if (item[i][br] != ',') throw "ERROR WITH SPACE - regind! " + item[i];
												else {
													br++;  while (item[i][br] == ' ') br++;
												}
											}
										}
										else throw "SPACE NOT ALLOWED! " + item[i];

										if (pazi == 2) lucky += "100";
										else lucky += "000";

										lucky += "010";

										if (s2.size() == 2 || s2 == "PSW" || s2 == "psw") lucky += getReg(s2);
										else throw "ERROR!" + item[i];

										if (item[i][br] == '$') throw "CAN'T USE IMMED HERE! " + item[i];
										// regind - regdir
										if (item[i][br] == '%') {
											br++;
											if (item[i][br] == ' ') throw "YOU CAN'T HAVE % AND THEN ' ' " + item[i];
											while (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++];
											if (item[i][br] == ' ') {
												while (item[i][br] == ' ') br++; if (item[i][br] != '\r') throw "ERROR, NO COMA!";
											}

											lucky += "001";

											if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") {
												if (pazi == 0) lucky[5] = '1';
												if (pazi == 1) throw "ERROR!" + item[i];
												lucky += getReg(s3);
											}
											else if (s3.size() == 3 || s3 == "PSWh" || s3 == "PSWH" || s3 == "PSWl" || s3 == "PSWL") {
												if (pazi == 2) throw "ERROR!" + item[i];
												lucky += getRegKRACI(s3);
											}
										}
										// regind - regind
										else if (item[i][br] == '(') {
											br++;
											if (item[i][br] == '%') {
												br++;
												while (item[i][br] != ' ' && item[i][br] != ')' && item[i][br] != '\r') { s3 += item[i][br]; br++; }
												if (item[i][br] == ' ') throw "ERROR WITH SPACE - regind! " + item[i];
												if (item[i][br] == '\r') throw "ERROR WITH NEW LINE - regind! " + item[i];
												else {
													br++;
													while (item[i][br] == ' ') br++;
													if (item[i][br] != '\r') throw "ERROR WITH SPACE - regind! " + item[i];
												}
											}
											else throw "CAN'T USE SPACE! " + item[i];

											lucky += "010";

											if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") lucky += getReg(s3);
											else throw "ERROR!" + item[i];
										}
										// regind - regindpom / memdir
										else {
											while (item[i][br] != '(' && item[i][br] != '\r' && item[i][br] != ' ') s4 += item[i][br++];

											// memdir
											if (item[i][br] == ' ' || item[i][br] == '\r') {
												while (item[i][br] == ' ' || item[i][br] == '\t') br++; if (item[i][br] != '\r') throw "SPACE ERROR - memdir!";

												if (regex_match(s4, numberr) == false)
													process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, 3);

												lucky += "10000000";

												if (regex_match(s4, numberr)) lucky += decToBinary(stoi(s4, nullptr, 0), 2);
												else lucky += "0000000000000000";

											}
											// regindpom
											else if (item[i][br] == '(') {
												br++;

												if (regex_match(s4, numberr) == false)
													process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, 3);

												lucky += "011";

												if (item[i][br] == '%') {
													br++;
													while (item[i][br] != ')') if (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++]; else throw "CAN'T USE SPACE! " + item[i];
													if (item[i][br] == ')') br++;
													while (item[i][br] == ' ') br++;
													if (item[i][br] != '\r') throw "SOME GARBAGE AFTER! " + item[i];

													if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") lucky += getReg(s3);
													else throw "NOT GOOD s2 HERE!" + item[i];
												}
												else throw "MUST BE WITHOUT SPACE! " + item[i];

												if (regex_match(s4, numberr)) {
													lucky += decToBinary(stoi(s4), 2);
												}
												else {
													if (s3 == "PC" || s3 == "pc" || s3 == "r7" || s3 == "R7") {
														poslF->setLinker(-2);
														poslF->setIma_Pomeraj();
													}
													lucky += "0000000000000000";
												}
											}
											else {
												throw "UNKNOWN! " + item[i];
											}
										}
										LC += lucky.size() / 8;
										codeC += binary_hexa(lucky);
									}
									// REGINDPOM/MEMDIR
									else {
										while (item[i][br] != '(' && item[i][br] != ',' && item[i][br] != ' ') s4 += item[i][br++];

										//REGINDPOM
										if (item[i][br] == '(') {
											br++;

											if (regex_match(s4, numberr) == false)
												process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, 2);

											For_Table* pomocni = poslF;

											lucky += "000011";
											if (pazi == 2) lucky[5] = '1';

											if (item[i][br] == '%') {
												br++;
												while (item[i][br] != ')') if (item[i][br] != ' ' && item[i][br] != '\r') s2 += item[i][br++]; else throw "CAN'T USE SPACE! " + item[i];
												if (item[i][br] == ')') br++;
												while (item[i][br] == ' ') br++;
												if (item[i][br] != ',') throw "NO COMA HERE! " + item[i];
												else {
													br++;  while (item[i][br] == ' ') br++;
												}

												if (s2.size() == 2 || s2 == "PSW" || s2 == "psw") lucky += getReg(s2);
												else throw "NOT GOOD s2 HERE!" + item[i];
											}
											else throw "MUST BE WITHOUT SPACE! " + item[i];

											int SPECIJALNI_FLAG = 0;
											if (s2 == "PC" || s2 == "pc" || s2 == "R7" || s2 == "r7") SPECIJALNI_FLAG = 1;

											if (regex_match(s2, numberr)) {
												lucky += decToBinary(stoi(s4, nullptr, 0), 2);
											}
											else {
												if (SPECIJALNI_FLAG == 0)
													lucky += "0000000000000000";
											}

											if (item[i][br] == '$') throw "CAN'T USE IMMED AS SECOND OPERAND! " + item[i];
											// regindpom - regdir
											else if (item[i][br] == '%') {
												br++;
												if (item[i][br] == ' ') throw "YOU CAN'T HAVE % AND THEN ' ' " + item[i];
												while (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++];
												if (item[i][br] == ' ') {
													while (item[i][br] == ' ') br++; if (item[i][br] != '\r') throw "ERROR, NO COMA!";
												}

												if (SPECIJALNI_FLAG == 1) { // bio PC gore
													pomocni->setLinker(-3);
													pomocni->setIma_Pomeraj();
													lucky += "0000000000000000";
												}

												lucky += "001";
												if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") {
													if (pazi == 0) lucky[5] = '1';
													if (pazi == 1) throw "ERROR!" + item[i];
													lucky += getReg(s3);
												}
												else if (s3.size() == 3 || s3 == "PSWh" || s3 == "PSWH" || s3 == "PSWl" || s3 == "PSWL") {
													if (pazi == 2) throw "ERROR!" + item[i];
													lucky += getRegKRACI(s3);
												}
											}
											// regindpom - regind
											else if (item[i][br] == '(') {
												br++;

												if (item[i][br] == '%') {
													br++;
													while (item[i][br] != ' ' && item[i][br] != ')' && item[i][br] != '\r') { s3 += item[i][br]; br++; }
													if (item[i][br] == ' ') throw "ERROR WITH SPACE - regind! " + item[i];
													if (item[i][br] == '\r') throw "ERROR WITH NEW LINE - regind! " + item[i];
													else {
														br++;
														while (item[i][br] == ' ') br++;
														if (item[i][br] != '\r') throw "ERROR WITH SPACE - regind! " + item[i];
													}
												}
												else throw "CAN'T USE SPACE! " + item[i];

												if (SPECIJALNI_FLAG == 1) { //bio PC gore
													pomocni->setLinker(-3);
													pomocni->setIma_Pomeraj();
													lucky += "0000000000000000";
												}

												lucky += "010";

												if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") lucky += getReg(s3);
												else throw "ERROR!" + item[i];
											}
											else {
												while (item[i][br] != '(' && item[i][br] != '\r' && item[i][br] != ' ') s5 += item[i][br++];

												// regindpom - memdir
												if (item[i][br] == ' ' || item[i][br] == '\r') {
													while (item[i][br] == ' ' || item[i][br] == '\t') br++; if (item[i][br] != '\r') throw "SPACE ERROR - memdir!";

													if (regex_match(s5, numberr) == false) {

														Symbol_Table* pom = isInSymbol_Table(s5);
														if (pom == nullptr) {
															add(s5, -5, 'l', false, 0, 0);
														}
														if (SPECIJALNI_FLAG == 0) {
															int av = LC + 5;
															addF(av, s5, BROJ_SEKCIJE, SEKCIJA, 'w');
														}
														else {
															int av = LC + 5;
															addF(av, s5, BROJ_SEKCIJE, SEKCIJA, 'w');
															pomocni->setLinker(-5);
															pomocni->setIma_Pomeraj();
														}
													}

													if (SPECIJALNI_FLAG == 1) { //bio PC gore
														pomocni->setLinker(-5);
														pomocni->setIma_Pomeraj();
														lucky += "0000000000000000";
													}

													lucky += "10000000";

													if (regex_match(s5, numberr)) lucky += decToBinary(stoi(s5, nullptr, 0), 2);
													else lucky += "0000000000000000";
												}
												// regindpom - regindpom
												else if (item[i][br] == '(') {
													br++;

													if (regex_match(s5, numberr) == false)
														process_move(s5, LC, BROJ_SEKCIJE, SEKCIJA, 5);

													if (item[i][br] == '%') {
														br++;
														while (item[i][br] != ')') if (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++]; else throw "CAN'T USE SPACE! " + item[i];
														if (item[i][br] == ')') br++;
														while (item[i][br] == ' ') br++;
														if (item[i][br] != '\r') throw "SOME GARBAGE AFTER! " + item[i];
													}
													else throw "MUST BE WITHOUT SPACE! " + item[i];

													if (SPECIJALNI_FLAG == 1) {
														pomocni->setLinker(-5);
														pomocni->setIma_Pomeraj();
														lucky += "0000000000000000";
													}

													lucky += "011";

													if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") lucky += getReg(s3);
													else throw "REG SIZE ERROR! " + item[i];

													if (regex_match(s5, numberr))
														lucky += decToBinary(stoi(s5, nullptr, 0), 2);
													else {
														if (s3 == "PC" || s3 == "pc" || s3 == "R7" || s3 == "r7") {
															poslF->setLinker(-2);
															poslF->setIma_Pomeraj();
														}
														lucky += "0000000000000000";
													}
												}
												else throw "UNKNOWN! " + item[i];
											}
											LC += lucky.size() / 8;
											codeC += binary_hexa(lucky);
										}
										//MEMDIR
										else if (item[i][br] == ' ' || item[i][br] == ',') {
											while (item[i][br] == ' ' || item[i][br] == '\t') br++; if (item[i][br] != ',') throw "SPACE ERROR - memdir!";
											br++;
											while (item[i][br] == ' ') br++;
											if (item[i][br] == '\r') throw "NO SECOND OPERAND! " + item[i];

											if (pazi == 2) lucky += "100";
											else lucky += "000";

											lucky += "10000000";

											if (regex_match(s4, numberr))
												lucky += decToBinary(stoi(s4, nullptr, 0), 2);
											else {
												process_move(s4, LC, BROJ_SEKCIJE, SEKCIJA, 2);
												lucky += "0000000000000000";
											}

											if (item[i][br] == '$') throw "CAN'T USE IMMED HERE! " + item[i];
											// memdir - regdir
											if (item[i][br] == '%') {
												br++;
												if (item[i][br] == ' ') throw "YOU CAN'T HAVE % AND THEN ' ' " + item[i];
												while (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++];
												if (item[i][br] == ' ') {
													while (item[i][br] == ' ') br++; if (item[i][br] != '\r') throw "ERROR, NO COMA!";
												}

												lucky += "001";

												if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") {
													if (pazi == 0) lucky[5] = '1';
													if (pazi == 1) throw "ERROR!" + item[i];
													lucky += getReg(s3);
												}
												else if (s3.size() == 3 || s3 == "PSWh" || s3 == "PSWH" || s3 == "PSWl" || s3 == "PSWL") {
													if (pazi == 2) throw "ERROR!" + item[i];
													lucky += getRegKRACI(s3);
												}
												else throw "REG SIZE ERROR!" + item[i];
											}
											//memdir - regind
											else if (item[i][br] == '(') {
												br++;

												if (item[i][br] == '%') {
													br++;
													while (item[i][br] != ' ' && item[i][br] != ')'  && item[i][br] != '\r') { s3 += item[i][br]; br++; }
													if (item[i][br] == ' ') throw "ERROR WITH SPACE - regind! " + item[i];
													if (item[i][br] == '\r') throw "ERROR WITH NEW LINE - regind! " + item[i];
													else {
														br++;
														while (item[i][br] == ' ') br++;
														if (item[i][br] != '\r') throw "ERROR WITH SPACE - regind! " + item[i];
													}
												}
												else throw "CAN'T USE SPACE! " + item[i];

												lucky += "010";

												if (s3.size() == 2 || s3 == "PSW" || s3 == "psw") lucky += getReg(s3);
												else throw "ERROR!" + item[i];
											}
											// memdir - regindpom/memdir
											else {
												while (item[i][br] != '(' && item[i][br] != '\r' && item[i][br] != ' ') s5 += item[i][br++];

												// memdir
												if (item[i][br] == ' ' || item[i][br] == '\r') {
													while (item[i][br] == ' ' || item[i][br] == '\t') br++; if (item[i][br] != '\r') throw "SPACE ERROR - memdir!";

													if (regex_match(s5, numberr) == false)
														process_move(s5, LC, BROJ_SEKCIJE, SEKCIJA, 5);

													lucky += "10000000";

													if (regex_match(s5, numberr)) lucky += decToBinary(stoi(s5, nullptr, 0), 2);
													else lucky += "0000000000000000";
												}
												// regindpom
												else if (item[i][br] == '(') {
													br++;

													if (regex_match(s5, numberr) == false)
														process_move(s5, LC, BROJ_SEKCIJE, SEKCIJA, ((lucky.size() / 8) + 1));

													if (item[i][br] == '%') {
														br++;
														while (item[i][br] != ')') if (item[i][br] != ' ' && item[i][br] != '\r') s3 += item[i][br++]; else throw "CAN'T USE SPACE! " + item[i];
														if (item[i][br] == ')') br++;
														while (item[i][br] == ' ') br++;
														if (item[i][br] != '\r') throw "SOME GARBAGE AFTER! " + item[i];
													}
													else throw "MUST BE WITHOUT SPACE! " + item[i];

													lucky += "011";

													if (s3.size() == 2 || s3 == "PSW" || s3 == "psw")
														lucky += getReg(s3);
													else throw "REG SIZE ERROR! " + item[i];

													if (regex_match(s5, numberr))
														lucky += decToBinary(stoi(s5, nullptr, 0), 2);
													else {
														if (s3 == "PC" || s3 == "pc" || s3 == "R7" || s3 == "r7") {
															poslF->setLinker(-2);
															poslF->setIma_Pomeraj();
														}
														lucky += "0000000000000000";
													}
												}
												else throw "UNKNOWN! " + item[i];
											}
											LC += lucky.size() / 8;
											codeC += binary_hexa(lucky);
										}
									}
								}
							}

							// BRANCHES
							if (lucky == "") {

								if (s1 == "INT") lucky += "00011100";
								else if (s1 == "CALL") lucky += "00100100";
								else if (s1 == "JMP") lucky += "00101100";
								else if (s1 == "JEQ") lucky += "00110100";
								else if (s1 == "JNE") lucky += "00111100";
								else if (s1 == "JGT") lucky += "01000100";

								if (lucky != "") {
									if (pazi == 1 && s1 != "INT") throw "You can't use branches with B sufix! " + item[i];
									if (BROJ_SEKCIJE == 0) throw "NOT IN SECTION! " + item[i];

									// IMMED
									if (item[i][br] != '*') {
										while (item[i][br] == ' ') br++;
										while (item[i][br] != ' ' && item[i][br] != '\r') s2 += item[i][br++];
										if (item[i][br] != '\r') throw "ERROR WITH SPACE! " + item[i];
										br++;
										while (item[i][br] == ' ') br++;

										lucky += "00000000"; // 2. bajt

										if (regex_match(s2, numberr)) {
											if (s1 == "INT" && pazi == 1 && (stoi(s2, nullptr, 0) < 128 && stoi(s2, nullptr, 0) >= 0)) { lucky[5] = '0'; lucky += decToBinary(stoi(s2, nullptr, 0), 1); }
											else if (s1 == "INT" && pazi == 1 && (stoi(s2, nullptr, 0) > 128 || stoi(s2, nullptr, 0) < 0)) throw "CAN'T USE SUFIX b HERE!";

											if (stoi(s2, nullptr, 0) > 0 && pazi != 1) {
												if (stoi(s2, nullptr, 0) < 32767)
													lucky += decToBinary(stoi(s2, nullptr, 0), 2);
												else throw "OPERAND TOO BIG! " + item[i];
											}
											else if (pazi == 1) {}
											else throw "CAN'T USE NEGATIVE NUMBERS! " + item[i];
										}
										else {
											process_move(s2, LC, BROJ_SEKCIJE, SEKCIJA, 2);
											poslF->setSkok();
											lucky += "0000000000000000";
										}
									}
									// NOT IMMED
									else if (item[i][br] == '*') {
										br++;
										// REGDIR
										if (item[i][br] == '%') {
											br++;
											while (item[i][br] == ' ') br++;
											while (item[i][br] != ' ' && item[i][br] != '\r') s2 += item[i][br++];
											br++;
											if (br < item[i].size()) { while (item[i][br] == ' ') br++; if (item[i][br] != '\r') throw "ERROR WITH SPACE - regdir! " + item[i]; }
											else if (item[i][br - 1] != '\r') throw "ERROR WITH SPACE - regdir! " + item[i];
											if (s2 == "") throw "NO s2 HERE!" + item[i];

											lucky += "001"; //regdir

											if (s2.size() == 2 || s2 == "PSW" || s2 == "psw") {
												if (pazi == 1) throw "ERROR! " + item[i];
												lucky += getReg(s2);
											}
											else if (pazi == 1 && (s2.size() == 3 || s2.size() == 4)) {
												lucky[5] = '0';
												lucky += getRegKRACI(s2);
											}
											else throw "ILLEGAL REGISTER! " + item[i];
										}
										// REGIND
										else if (item[i][br] == '(') {
											br++;
											if (item[i][br] == '%') {
												br++;
												while (item[i][br] != ' ' && item[i][br] != ')'  && item[i][br] != '\r') { s2 += item[i][br]; br++; }
												if (item[i][br] == ' ') throw "ERROR WITH SPACE - regind! " + item[i];
												if (item[i][br] == '\r') throw "ERROR WITH NEW LINE - regind! " + item[i];
												else {
													br++;
													while (item[i][br] == ' ') br++;
													if (item[i][br] != '\r') throw "ERROR WITH SPACE - regind! " + item[i];
												}

												lucky += "010"; //regind

												if (pazi != 2) lucky[5] = '0'; // za INT

												if (s2.size() == 2 || s2 == "PSW" || s2 == "psw") lucky += getReg(s2);
												else throw "ILLEGAL REGISTER! " + item[i];
											}
											else throw "ERROR with '(' " + item[i];
										}
										// REGINDPOM / MEMDIR
										else {
											while (item[i][br] != '(' && item[i][br] != '\r' && item[i][br] != ' ') s3 += item[i][br++];

											// MEMDIR
											if (item[i][br] == ' ' || item[i][br] == '\r') {
												while (item[i][br] == ' ' || item[i][br] == '\t') br++; if (item[i][br] != '\r') throw "SPACE ERROR - memdir!";

												lucky += "10000000";

												if (regex_match(s3, numberr) == false) {
													process_move(s3, LC, BROJ_SEKCIJE, SEKCIJA, 3);
													poslF->setSkok();
													lucky += decToBinary(0, 2);
												}
												else {
													if (s1 == "INT" && pazi == 1 && stoi(s3, nullptr, 0) < 128 && stoi(s3, nullptr, 0) > 0) {
														lucky[5] = '0';
														lucky += decToBinary(stoi(s3, nullptr, 0), 1);
													}
													else if (s1 == "INT" && pazi == 1 && stoi(s3, nullptr, 0) >= 128) {
														throw "CAN'T USE SUFIX B AND THIS VALUE! " + item[i];
													}
													else if (stoi(s3, nullptr, 0) > 32767) throw "OPERAND TOO BIG! " + item[i];
													else lucky += decToBinary(stoi(s3, nullptr, 0), 2);

												}
											}
											// REGINDPOM
											else if (item[i][br] == '(') {
												br++;

												lucky += "011";

												if (pazi == 1) throw "CAN'T USE SUFIXES HERE! " + item[i]; // za INT

												if (item[i][br] == '%') {
													br++;
													while (item[i][br] != ')') if (item[i][br] != ' ' && item[i][br] != '\r') s2 += item[i][br++]; else throw "CAN'T USE SPACE! " + item[i];
													if (s2.size() == 2 || s2 == "PSW" || s2 == "psw")
														lucky += getReg(s2);
													else throw "NOT GOOD s2 HERE!" + item[i];
												}
												else throw "MUST BE WITHOUT SPACE! " + item[i];

												if (regex_match(s3, numberr) == false) {
													process_move(s3, LC, BROJ_SEKCIJE, SEKCIJA, 2);
													poslF->setSkok();
													lucky += decToBinary(0, 2);

													if (s2 == "PC" || s2 == "pc" || s2 == "r7" || s2 == "R7") {
														poslF->setLinker(-2);
													}
												}
												else lucky += decToBinary(stoi(s3, nullptr, 0), 2);
											}
											else {
												throw "Unknown ( attempt BRANCH-REGINDPOM )? " + item[i];
											}
										}
									}
									LC += lucky.size() / 8;
									codeC += binary_hexa(lucky);
								}
								// ERROR
								else {
									throw "ERROR unknown opcode! " + item[i];
								}
							}
						}
						cout << lucky << " " << lucky.size() << endl;
						cout << LC << endl;
					}
				}
			}
		}

		if (bilo_end == false) throw "NO .end!";

		cout << "C" << endl;

		// PROVERA KRUZNE ZAVISNOSTI I ZAMENA VREDNOSTI ZA EQU!

		for (Equ_Table* tek = prviE; tek != nullptr; tek = tek->next) {
			string symbol = tek->getSymbol();
			vector<string> sMain1 = tek->getNizBukvalno();
			vector<string> sMain2 = tek->getZnaciBukvalno();
			if (circle() == true) throw "CIRCLE ERROR!";
			else {
				for (Equ_Table* tek2 = prviE; tek2 != nullptr; tek2 = tek2->next) {
					if (tek2 != tek) {
						vector<string> s1 = tek2->getNizBukvalno();
						vector<string> s2 = tek2->getZnaciBukvalno();

						for (int i = 0; i < s1.size(); i++) {
							if (s1[i] == symbol) {
								int flegZaMinusIspredZagrade = 0;
								if (s2[i] == "-") { flegZaMinusIspredZagrade = 1; }

								tek2->loseSymbol(i);
								tek2->loseZnak(i);

								for (int j = 0; j < sMain1.size(); j++) {
									tek2->setSymbol(sMain1[j], i);

									if (sMain2[j] == "+") {
										if (flegZaMinusIspredZagrade == 1) {
											tek2->setZnak("-", i);
										}
										else tek2->setZnak("+", i);
									}
									else if (sMain2[j] == "-") {
										if (flegZaMinusIspredZagrade == 1) {
											tek2->setZnak("+", i);
										}
										else tek2->setZnak("-", i);
									}
								}
							}
						}
					}
				}
			}
		}


		//PROVERA INDEKSA KLASIFIKACIJE I BELEZENJE ZA KOJU SEKCIJU TREBA ICI RELOKACIONI
		for (Equ_Table* tek = prviE; tek != nullptr; tek = tek->next) {
			vector<string> s1 = tek->getNizBukvalno();
			vector<string> s2 = tek->getZnaciBukvalno();

			vector<int> s3;
			for (int i = 0; i < 10; i++)
				s3.push_back(0);

			int vrednost = 0;
			for (int i = 0; i < s1.size(); i++) {
				Symbol_Table* pom = isInSymbol_Table(s1[i]);

				if (pom != nullptr) {
					if (pom->getSection() == 0) {
						s3[0] += 1;
					}
					else {
						if (s2[i] == "-") s3[pom->getSection()] -= 1;
						else s3[pom->getSection()] += 1;
					}
				}
			}

			bool doublee = false;
			for (int i = 0; i < 10; i++) {
				if (s3[i] != 0) {
					if (doublee == false) doublee = true;
					else throw "ERROR EQU 1!";

					tek->setIK(i);
				}
				if (s3[i] != 1 && s3[i] != 0) throw "ERROR EQU 2!"; // mozda bolje na kraju
			}
		}

		/* UBACIVANJE VREDNOSTI I RACUNANJE VREDNOSTI SIMBOLA I PRAVLJENJE ZAPISA
		   indeks_klasifikacije == 0 => simbol je extern => relokacioni ide ka njemu => u kod 0
		   indeks_klasifikacije == 1 => simbol je global => relokacioni ka sekciji => u kod njegova vrednost
									 => simbol je local => relokacioni nema uopste */


		for (Equ_Table* tek = prviE; tek != nullptr; tek = tek->next) {
			vector<string> s1 = tek->getNizBukvalno();
			vector<string> s2 = tek->getZnaciBukvalno();
			Symbol_Table* blabla = isInSymbol_Table(tek->getSymbol());
			int ulaz_za_simbol = blabla->getRbr();

			int vrednost = 0;
			for (int i = 0; i < s1.size(); i++) {
				Symbol_Table* pom = isInSymbol_Table(s1[i]);

				if (regex_match(s1[i], numberr) == false) {

					int v = pom->getValue();
					int sekcija = pom->getSection();

					if (s2[i] == "-") vrednost -= v;
					else vrednost += v;

					if (blabla->getlg() == 'l' && tek->getIK() == 0) {} // preskoci obradu 
					else {
						if (sekcija == 0) {
							addR(pom->getRbr(), ulaz_za_simbol, "direct");
							poslR->setSign(s2[i][0]);
						}
					}
				}
				else {
					int v = stoi(s1[i]);
					if (s2[i] == "-") vrednost -= v;
					else vrednost += v;
				}
			}

			if (tek->getIK() != 0 && tek->getIK() != -1) { //ako nije apsolutan i ako nije extern
				if (blabla->getlg() != 'l')
					addR(tek->getIK(), ulaz_za_simbol, "direct");
			}
			if (tek->getIK() == -1) { blabla->setSection(-5); } //ako je apsolutan, sekcija izmisljena kao znak -5
			else {
				blabla->setSection(tek->getIK()); // u suprotnom sekcija zavisi od izraza
			}

			blabla->setValue(vrednost);
		}

		for (int i = 0; i < globalni.size(); i++) {
			Symbol_Table* pom = isInSymbol_Table(globalni[i]);
			if (pom->getDefined() == false)
				throw "ERROR! CAN'T BE LIKE THIS! GLOBAL AND NOT DEFINED!";
		}

		// RAZRESAVANJE TABELE OBRACANJA UNAPRED!
		for (For_Table* tek = prviF; tek != nullptr; tek = tek->next) {
			Symbol_Table* pom = isInSymbol_Table(tek->getSymbol());
			if (pom == nullptr) {
				throw "ERROR! NOT IN TABLE! ";
			}
			else if (pom->getlg() == 'l' && pom->getEqu() && pom->getSection() == 0) {
				for (Equ_Table* naca = prviE; naca != nullptr; naca = naca->next) {
					if (naca->getSymbol() == pom->getName()) {
						int a = tek->getSectionNumber();
						Symbol_Table* c = getCodeNumberMain(a);

						vector<string> s1 = naca->getNizBukvalno();
						vector<string> s2 = naca->getZnaciBukvalno();

						for (int i = 0; i < s1.size(); i++) {
							Symbol_Table* pompom = isInSymbol_Table(s1[i]);
							if (pompom != nullptr) {
								if (pompom->getSection() == 0) {

									if (tek->getLinker() != 0 || tek->getIma_Pomeraj()) {
										addR(pompom->getRbr(), tek->getPatch(), "PCrel");
										poslR->setLinker(tek->getLinker());
										poslR->setSection(tek->getSectionNumber());
										poslR->setSign(s2[i][0]);
										poslR->setSize(tek->getSize());
									}
									else {
										addR(pompom->getRbr(), tek->getPatch(), "direct");
										poslR->setSection(tek->getSectionNumber());
										poslR->setSign(s2[i][0]);
										poslR->setSize(tek->getSize());
									}

									break;
								}
							}
						}
						rewriteCode(pom, tek, c);
						break;
					}
				}
			}
			else if (pom->getlg() == 'g' && pom->getEqu() && pom->getSection() == 0) {
				int b = pom->getRbr();
				addR(b, tek->getPatch(), "direct");
				poslR->setSection(tek->getSectionNumber());
				poslR->setSize(tek->getSize());
			}
			else if (pom->getlg() == 'l') {
				int a = tek->getSectionNumber();
				Symbol_Table* c = getCodeNumberMain(a);

				rewriteCode(pom, tek, c);

				if (pom->getlg() == 'l' && pom->getSection() != -5) {
					if (tek->getLinker() != 0 || tek->getIma_Pomeraj()) {
						addR(a, tek->getPatch(), "PCrel");
						poslR->setLinker(tek->getLinker());
						poslR->setSection(tek->getSectionNumber());
						poslR->setSize(tek->getSize());
					}
					else {
						addR(a, tek->getPatch(), "direct");
						poslR->setSection(tek->getSectionNumber());
						poslR->setSize(tek->getSize());
					}
				}
			}
			else if (pom->getSection() != -5) {
				if (tek->getLinker() != 0 || tek->getIma_Pomeraj()) {
					addR(pom->getRbr(), tek->getPatch(), "PCrel");
					poslR->setLinker(tek->getLinker());
					poslR->setSection(tek->getSectionNumber());
					poslR->setSize(tek->getSize());
				}
				else {
					addR(pom->getRbr(), tek->getPatch(), "direct");
					poslR->setSection(tek->getSectionNumber());
					poslR->setSize(tek->getSize());
				}
			}
			else if (pom->getlg() == 'g') { // PROVERI DA LI OVO TREBA
				addR(pom->getRbr(), tek->getPatch(), "direct");
				poslR->setSection(tek->getSectionNumber());
				poslR->setSize(tek->getSize());
			}
		}

		ofstream outdata;
		outdata.open(argv[2]); // opens the file
		if (!outdata) { // file couldn't be opened
			cerr << "Error: file could not be opened" << endl;
			exit(1);
		}

		outdata << left;
		outdata << setw(10) << "Number" << setw(10) << "Section" << setw(10) << "L/G" << setw(10) << "Defined"
			<< setw(10) << "Value" << setw(10) << "Size" << setw(10) << "Name" << setw(10) << "Equ" << setw(10) << endl;


		// ISPIS SYMBOL TABLE
		for (Symbol_Table*tek = prvi; tek != nullptr; tek = tek->next) {
			if (tek->getlg() == 'l' && tek->getSection() == -5 && tek->getDefined() == false) throw "ERROR! LOCAL AND UNDEFINED! " + tek->getName();
			outdata << left;
			outdata << setw(10) << tek->getRbr() << setw(10) << tek->getSection() << setw(10) << tek->getlg() << setw(10) << tek->getDefined()
				<< setw(10) << tek->getValue() << setw(10) << tek->getSize() << setw(10) << tek->getName() << setw(10) << tek->getEqu() << setw(10) << endl;
		}
		outdata << endl;

		outdata << left;
		outdata << setw(10) << "Number" << setw(20) << "Section/Symbol" << setw(20) << "Address/For Symbol" << setw(20) << "Way" << setw(10) << "Linker" << setw(15) << "For Section" << setw(10) << "Sign" << setw(10) << "Size" << endl;

		// ISPIS RELOCATION TABLE
		for (Reloc_Table* tek = prviR; tek != nullptr; tek = tek->next) {
			outdata << left;
			outdata << setw(10) << tek->getRbr() << setw(20) << tek->getSym() << setw(20) << tek->getAdress() << setw(20) << tek->getWay() << setw(10) << tek->getLinker() << setw(15); if (tek->getSection() == -10) outdata << "EQU"; else outdata << tek->getSection(); outdata << setw(10) << tek->getSign() << setw(10) << tek->getSize() << endl;
		}
		outdata << endl;

		// ISPIS CODE-ova
		for (Symbol_Table*tek = prvi; tek != nullptr; tek = tek->next) {
			if (tek->getSize() != 0) {
				outdata << tek->getName() << ": ";
				for (int i = 0; i < tek->kod.size(); i++) {
					outdata << tek->kod[i];
					if (i % 25 == 0 && i != 0) outdata << endl;
				}
				outdata << endl;
			}
		}
		outdata << endl;

		outdata.close();
		inFile.close();
	}
	/*Simbol koji je EQU, zavistan od extern simbola i globalan -> .extern b .global a .equ a, 5-b -> ide direct zapis
		=> LINKERU, menjaj b, nemoj mene pomerati, pusti da se na osnovu te promene i ja promenim.

	Simbol koji je EQU, zavistan od extern simbola i lokalan -> .extern b .equ a, 5-b -> nema EQU zapisa
		=> a je lokalni simbol i njegova vrednost linkeru nista ne znaci => ne treba je azurirati

	Simbol koji je EQU, zavistan od lokalnog simbola i lokalan -> b: .equ a, 5-b -> nema EQU zapisa
		=> a je lokalni simbol i njegova vrednost linkeru nista ne znaci => ne treba je azurirati

	Simbol koji je EQU, zavistan od lokalnog simbola i globalan -> b:.global a .equ a, 5-b -> ima EQU zapis
		=> LINKERU, menjaj sekciju kojoj pripada b, nemoj mene pomerati, pusti da se na osnovu te promene i ja promenim.

	Simbol koji je EQU i apsolutan -> sekcija -5
		=> LINKERU, znaj da sam ja EQU i da sam apsolutan i nemoj da me diras */
	catch (const char* m) { cout << m << endl; }
	catch (string m) { cout << m << endl; }
	return 0;
}