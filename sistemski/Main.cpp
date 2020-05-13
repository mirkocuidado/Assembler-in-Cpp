#include <fstream>
#include <string>
#include <iostream>
#include <regex>
#include <iomanip>
#include <string.h>
#include <sstream>
#include <exception>

#include "Symbol_Table.h";
#include "For_Table.h";
#include "Reloc_Table.h";
#include "Equ_Table.h";

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
		//cout << pom << endl;
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
	return pom += '\n';
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
	else throw "Error in converting binary-hexa! ";

	if (s2[s2.size() - 1] == 'h') luck += "1";
	else luck += "0";

	return luck;
}

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
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

int main() {

	try{


		ifstream inFile;
		inFile.open("input.txt");
		if (inFile.fail()) {
			throw "FILE ERROR! ";
			exit(1);
		}

		string item[100] = {};
		string pom;
		int i = 0;

		string codeC = "";
		int LC = 0;

		string SEKCIJA = "";
		int BROJ_SEKCIJE = 0;

		string LABELA = "";

		string niz[50];
		string znak[50];
		int nizI = 0;
		int nizZ = 0;

		while (getline(inFile, pom)) {
			if (pom != "") {
				item[i++] = pom;
			}
		}

		int br = i;

		for (int i = 0; i < br; i++) {
			item[i] = lose_extra_white_space(item[i]);
		}

		for (int i = 0; i < br; i++)
			cout << item[i];

		cout << endl;

		regex section("section");
		regex end(".end\n");
		regex byte("byte");
		regex word("word");
		regex skip("skip");
		regex REGglobal("global");
		regex REGextern("extern");
		regex equ("equ");

		for (int i = 0; i < br; i++) {
			if (item[i][0] == '.') {
				if (regex_match(item[i], end)) {
					Symbol_Table* sec = isInSymbol_Table(SEKCIJA);
					if (sec != nullptr) {
						sec->setSize(LC);
						for (int i = 0; i < codeC.size(); i += 2) {
							string u = "";
							u += codeC[i];
							u += codeC[i + 1];
							sec->setCode(u);
						}
						codeC = "";
					}
					break;
				}
				else {
					string posleTacke = "";
					int br = 1;
					while (item[i][br] != ' ') {
						posleTacke += item[i][br];
						br++;
					}

					br++; // preskoci ' '

					if (posleTacke == "section") {
						string s1 = "";
						while (item[i][br] != ':') {
							s1 += item[i][br];
							br++;
						}
						br++; // preskoci ':'

						while (item[i][br] == ' ') br++;

						Symbol_Table* pom = isInSymbol_Table(SEKCIJA);
						if (pom != nullptr) {
							pom->setSize(LC);
							for (int i = 0; i < codeC.size(); i += 2) {
								string u = "";
								u += codeC[i];
								u += codeC[i + 1];
								pom->setCode(u);
							}
							codeC = "";
							LC = 0;
						}

						SEKCIJA = s1;
						Symbol_Table* sec = isInSymbol_Table(SEKCIJA);
						if (sec != nullptr) {
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

						if (item[i][br] == '\n') {
							cout << "Sekcija " << s1 << endl;
						}
						else {
							throw "Error while reading .section " + item[i];
						}
					}
					else {
						if (regex_match(posleTacke, byte)) {
							string s1 = "";
							while (item[i][br] != '\n') {
								while (item[i][br] != ',' && item[i][br] != ' ' && item[i][br] != '\n') {
									s1 += item[i][br];
									br++;
								}
								if (item[i][br] != '\n')
									br++;
								// vidi sta ces sa s1

								if (s1 != "") {
									if (is_number(s1)) {
										if (atoi(s1.c_str()) > 255) throw "Operand too big! " + item[i];
										else {
											string s = decToBinary(atoi(s1.c_str()), 1);
											codeC += binary_hexa(s);
										}
									}
									else {
										Symbol_Table* pomm = isInSymbol_Table(s1);
										if (pomm != nullptr) {
											if (pomm->getDefined() == true) {
												if (pomm->getlg() == 'g') {
													codeC += "00";
													addR(pomm->getRbr(), LC, "direct");
												}
												else {
													codeC += "00";
													addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'b');
												}
											}
											else {
												codeC += "00";
												addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'b');
											}
										}
										else {
											add(s1, 0, 'l', false, 0, 0);
											addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'b');
											codeC += "00";
										}
									}
									LC += 1;
								}

								s1 = "";
							}
						}
						else if (regex_match(posleTacke, word)) {
							string s1 = "";
							while (item[i][br] != '\n') {
								while (item[i][br] != ',' && item[i][br] != ' ' && item[i][br] != '\n') {
									s1 += item[i][br];
									br++;
								}
								if (item[i][br] != '\n')
									br++;
								// vidi sta ces sa s1
								if (s1 != "") {
									if (is_number(s1)) {
										string s = decToBinary(atoi(s1.c_str()), 2);
										codeC += binary_hexa(s);
									}
									else {
										Symbol_Table* pomm = isInSymbol_Table(s1);
										if (pomm != nullptr) {
											if (pomm->getDefined() == true) {
												if (pomm->getlg() == 'g') {
													codeC += "0000";

													addR(pomm->getRbr(), LC, "direct");
												}
												else {
													codeC += "0000";
													addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'w');
												}
											}
											else {
												codeC += "0000";
												if (pomm->getlg() != 'g') {
													addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'w');
												}
											}
										}
										else {
											add(s1, 0, 'l', false, 0, 0);
											codeC += "0000";
											addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'w');
										}
									}
									LC += 2;
								}
								s1 = "";
							}
						}
						// SKIP: ubaciti kod jos za sve, za sad u char* code bleji
						else if (regex_match(posleTacke, skip)) {
							string s1 = "";
							while (item[i][br] != '\n') {
								while (item[i][br] != ',' && item[i][br] != ' ' && item[i][br] != '\n') {
									s1 += item[i][br];
									br++;
								}
								if (item[i][br] != '\n')
									br++;

								if (is_number(s1) == false) {
									throw ".skip symbol ERROR " + item[i];
								}

								if (s1 != "") {
									LC = LC + stoi(s1);

									for (int i = 0; i < stoi(s1); i++) {
										codeC += "00";
									}

									cout << "LC = " << LC << endl;
								}

								s1 = "";
							}
						}
						// GLOBAL: dodati jos LC i broj sekcije
						else if (regex_match(posleTacke, REGglobal)) {
							string s1 = "";
							while (item[i][br] != '\n') {
								while (item[i][br] != ',' && item[i][br] != ' ' && item[i][br] != '\n') {
									s1 += item[i][br];
									br++;
								}
								if (item[i][br] != '\n')
									br++;

								if(is_number(s1)==true) throw ".global + number ERROR " + item[i];

								if (s1 != "") {
									Symbol_Table* pom = isInSymbol_Table(s1);
									if (pom != nullptr) {
										pom->setlg('g');
									}
									else {
										add(s1, 0, 'g', false, 0, 0);
									}
								}

								s1 = "";
							}
						}  // dodati jos LC i broj sekcije
						// EXTERN: dodati jos LC i broj sekcije
						else if (regex_match(posleTacke, REGextern)) {
							string s1 = "";
							while (item[i][br] != '\n') {
								while (item[i][br] != ',' && item[i][br] != ' ' && item[i][br] != '\n') {
									s1 += item[i][br];
									br++;
								}
								if (item[i][br] != '\n')
									br++;

								if (is_number(s1) == true) throw ".extern + number ERROR " + item[i];

								if (s1 != "") {
									Symbol_Table* pom = isInSymbol_Table(s1);
									if (pom != nullptr) {
										pom->setlg('g');
									}
									else {
										add(s1, 0, 'g', false, 0, 0);
									}
								}

								s1 = "";
							}
						}
						else if (regex_match(posleTacke, equ)) {
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

							while (item[i][br] != '\n') {
								s2 += item[i][br];
								br++;
							}

							string pomocni = "";

							for (int i = 0; i < s2.size(); i++) {
								if (s2[i] != '+' && s2[i] != '-' && s2[i] != '\n') {
									if (s2[i] != ' ')
										pomocni += s2[i];
								}
								else {
									znak[nizZ++] = s2[i];
									if (pomocni == "" && i != 0) cout << "GRESKA!" << endl;
									else if (pomocni != "") {
										niz[nizI++] = pomocni;
										pomocni = "";
									}
								}
							}

							niz[nizI++] = pomocni;

							int izraz = 0;

							int flag = 0;

							for (int i = 0; i < nizI; i++) {
								if (is_number(niz[i]) == true) {
									izraz += atoi(niz[i].c_str());
								}
								else {
									flag = 1;
									Symbol_Table* pompom = isInSymbol_Table(niz[i]);
									if (pompom == nullptr) {
										add(niz[i], 0, 'l', false, 0, 0);
									}
								}
							}

							if (flag == 0) {
								if (pom == nullptr)
									add(s1, BROJ_SEKCIJE, 'l', true, izraz, 0);
								else {
									pom->setDefined(true);
									pom->setValue(izraz);
									pom->setSection(BROJ_SEKCIJE);
								}

								cout << izraz << endl;
							}
							else {
								if (pom == nullptr) add(s1, BROJ_SEKCIJE, 'l', false, 0, 0);

								addE(s1, nizI, niz, nizZ, znak);
								posl->setEqu();

							}

							izraz = 0;
							nizI = 0;
							nizZ = 0;

							/*cout << "EEE" << endl;
							for (Equ_Table* tek = prviE; tek != nullptr; tek = tek->next)
								cout << tek->getSymbol() << " " << tek->getNiz() << endl;
							cout << "FFF" << endl;

							cout << s1 << " = ";
							cout << s2 << endl;*/

						}
					}
				}
			}
			else {
				int br = 0;
				int jj = br;
				int flag = 5;
				string s1 = "";
				while (item[i][jj] != '\n') {
					if (item[i][jj] == ':') {
						if (item[i][jj + 1] == ' ') {
							if (item[i][jj + 2] == '\n') {
								flag = 1;
							}
							else flag = 2;
						}
						else if (item[i][jj + 1] == '\n') {
							flag = 1;
						}
						else flag = 2;
						break;
					}
					else {
						s1 += item[i][jj];
					}
					jj++;
				}

				jj++; // preskace :

				if (flag == 1) {
					LABELA = s1;
					Symbol_Table* pomm = isInSymbol_Table(LABELA);
					if (pomm != nullptr) {
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
				else if (flag == 2) { // ovde nije item[i][br] u obradi, nego pom[br]

					LABELA = s1;
					Symbol_Table* pomm = isInSymbol_Table(LABELA);
					if (pomm != nullptr) {
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
					string pom = "";

					while (item[i][br] == ' ') br++;

					while (item[i][br] != '\n') {
						pom += item[i][br++];
					}

					pom += '\n';

					if (pom[0] == '.') {
						if (regex_match(pom, end)) {
							Symbol_Table* sec = isInSymbol_Table(SEKCIJA);
							if (sec != nullptr) {
								sec->setSize(LC);
								for (int i = 0; i < codeC.size(); i += 2) {
									string u = "";
									u += codeC[i];
									u += codeC[i + 1];
									sec->setCode(u);
								}
								codeC = "";
							}
							break;
						}
						else {
							string posleTacke = "";
							int br = 1;
							while (pom[br] != ' ') {
								posleTacke += pom[br];
								br++;
							}
							br++; // preskoci ' '

							if (regex_match(posleTacke, section)) {
								string s1 = "";
								while (pom[br] != ':') {
									s1 += pom[br];
									br++;
								}
								br++; // preskoci ':'

								while (pom[br] == ' ') br++;

								Symbol_Table* pommm = isInSymbol_Table(SEKCIJA);
								if (pommm != nullptr) {
									pommm->setSize(LC);
									for (int i = 0; i < codeC.size(); i += 2) {
										string u = "";
										u += codeC[i];
										u += codeC[i + 1];
										pommm->setCode(u);
									}
									codeC = "";
									LC = 0;
								}

								SEKCIJA = s1;
								Symbol_Table* sec = isInSymbol_Table(SEKCIJA);
								if (sec != nullptr) {
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

								if (pom[br] != '\n') {
									throw ".section ERROR " + item[i];
								}
							}
							else {
								if (regex_match(posleTacke, byte)) {
									string s1 = "";
									while (pom[br] != '\n') {
										while (pom[br] != ',' && pom[br] != ' ' && pom[br] != '\n') {
											s1 += pom[br];
											br++;
										}
										if (pom[br] != '\n')
											br++;
										// vidi sta ces sa s1

										if (s1 != "") {
											if (is_number(s1)) {
												if (atoi(s1.c_str()) > 255) {
													throw ".byte number too big ERROR " + item[i];
												}
												else codeC += "55";
											}
											else {
												Symbol_Table* pomm = isInSymbol_Table(s1);
												if (pomm != nullptr) {
													if (pomm->getDefined() == true) {
														if (pomm->getlg() == 'g') {
															codeC += "00";
															addR(pomm->getRbr(), LC, "direct");
														}
														else {
															codeC += "00";
															addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'b');
														}
													}
													else {
														codeC += "00";
														addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'b');
													}
												}
												else {
													add(s1, 0, 'l', false, 0, 0);
													addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'b');
													codeC += "00";
												}
											}
											LC += 1;
										}
										s1 = "";
									}
								}
								else if (regex_match(posleTacke, word)) {
									string s1 = "";
									while (pom[br] != '\n') {
										while (pom[br] != ',' && pom[br] != ' ' && pom[br] != '\n') {
											s1 += pom[br];
											br++;
										}
										if (pom[br] != '\n')
											br++;
										// vidi sta ces sa s1

										if (s1 != "") {
											if (is_number(s1)) {
												string s = decToBinary(atoi(s1.c_str()), 2);
												codeC += binary_hexa(s);
											}
											else {
												Symbol_Table* pomm = isInSymbol_Table(s1);
												if (pomm != nullptr) {
													if (pomm->getDefined() == true) {
														if (pomm->getlg() == 'g') {
															codeC += "0000";

															addR(pomm->getRbr(), LC, "direct");
														}
														else {
															codeC += "0000";
															addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'w');
														}
													}
													else {
														codeC += "0000";
														if (pomm->getlg() != 'g') {
															addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'w');
														}
													}
												}
												else {
													add(s1, 0, 'l', false, 0, 0);
													codeC += "0000";
													addF(LC, s1, BROJ_SEKCIJE, SEKCIJA, 'w');
												}
											}
											LC += 2;
										}
										s1 = "";
									}
								}
								// SKIP: ubaciti kod jos za sve, za sad u char* code bleji
								else if (regex_match(posleTacke, skip)) {
									string s1 = "";
									while (pom[br] != '\n') {
										while (pom[br] != ',' && pom[br] != ' ' && pom[br] != '\n') {
											s1 += pom[br];
											br++;
										}
										if (pom[br] != '\n')
											br++;

										if (is_number(s1) == false) {
											throw ".skip symbol ERROR " + item[i];
										}

										if (s1 != "") {
											LC = LC + stoi(s1);

											for (int i = 0; i < stoi(s1); i++) {
												codeC += "00";
											}

											cout << "LC = " << LC << endl;
										}

										s1 = "";
									}
								}
								// GLOBAL: dodati jos LC i broj sekcije
								else if (regex_match(posleTacke, REGglobal)) {
									string s1 = "";
									while (pom[br] != '\n') {
										while (pom[br] != ',' && pom[br] != ' ' && pom[br] != '\n') {
											s1 += pom[br];
											br++;
										}
										if (pom[br] != '\n')
											br++;

										if (is_number(s1) == true) throw ".global + number ERROR " + item[i];

										if (s1 != "") {
											Symbol_Table* pom = isInSymbol_Table(s1);
											if (pom != nullptr) {
												pom->setlg('g');
											}
											else {
												add(s1, -1, 'g', false, 0, 0);
											}
										}

										s1 = "";
									}
								}  // dodati jos LC i broj sekcije
								// EXTERN: dodati jos LC i broj sekcije
								else if (regex_match(posleTacke, REGextern)) {
									string s1 = "";
									while (pom[br] != '\n') {
										while (pom[br] != ',' && pom[br] != ' ' && pom[br] != '\n') {
											s1 += pom[br];
											br++;
										}
										if (pom[br] != '\n')
											br++;

										if (is_number(s1) == true) throw ".extern + number ERROR " + item[i];

										if (s1 != "") {
											Symbol_Table* pom = isInSymbol_Table(s1);
											if (pom != nullptr) {
												pom->setlg('g');
											}
											else {
												add(s1, 0, 'g', false, 0, 0);
											}
										}
										s1 = "";
									}
								}
								else if (regex_match(posleTacke, equ)) {
									string s2 = "";
									string s1 = "";

									while (pom[br] != ',') {
										if (pom[br] != ' ')
											s1 += pom[br];
										br++;
									}

									br++; // preskoci ','
									while (pom[br] == ' ') br++;

									Symbol_Table* poom = isInSymbol_Table(s1);
									if (poom != nullptr && poom->getDefined() == true) if (is_number(s1) == true) throw ".equ already defined ERROR " + item[i];

									while (pom[br] != '\n') {
										s2 += pom[br];
										br++;
									}

									string pomocni = "";

									for (int i = 0; i < s2.size(); i++) {
										if (s2[i] != '+' && s2[i] != '-' && s2[i] != '\n') {
											if (s2[i] != ' ')
												pomocni += s2[i];
										}
										else {
											znak[nizZ++] = s2[i];
											if (pomocni == "" && i != 0) cout << "GRESKA!" << endl;
											else if (pomocni != "") {
												niz[nizI++] = pomocni;
												pomocni = "";
											}
										}
									}

									niz[nizI++] = pomocni;

									int izraz = 0;

									int flag = 0;

									for (int i = 0; i < nizI; i++) {
										if (is_number(niz[i]) == true) {
											izraz += atoi(niz[i].c_str());
										}
										else {
											flag = 1;
											Symbol_Table* pompom = isInSymbol_Table(niz[i]);
											if (pompom == nullptr) {
												add(niz[i], 0, 'l', false, 0, 0);
											}
										}
									}

									if (flag == 0) {
										if (poom == nullptr)
											add(s1, BROJ_SEKCIJE, 'l', true, izraz, 0);
										else {
											poom->setDefined(true);
											poom->setValue(izraz);
											poom->setSection(BROJ_SEKCIJE);
										}

										cout << izraz << endl;
									}
									else {
										if (poom == nullptr) add(s1, BROJ_SEKCIJE, 'l', false, 0, 0);

										addE(s1, nizI, niz, nizZ, znak);
										posl->setEqu();

									}

									izraz = 0;
									nizI = 0;
									nizZ = 0;
									/*cout << "EEE" << endl;
									for (Equ_Table* tek = prviE; tek != nullptr; tek = tek->next)
										cout << tek->getSymbol() << " " << tek->getNiz() << endl;
									cout << "FFF" << endl;*/

								}
							}
						}

					}
					else {
						cout << "Instrukcija! " << pom << endl;
					}
				}
				else if (flag == 5) {
					string lucky = "";
					string s1 = "";
					string s2 = "";
					string s3 = "";
					string s4 = "";
					string s5 = "";
					int pazi = 0;

					while (item[i][br] != ' ') if (item[i][br] != ' ') s1 += item[i][br++];
					br++; // preskoci ' '

					if (s1 == "HALT") {
						LC++;
						codeC += "00";
					}
					else if (s1 == "IRET") {
						LC++;
						codeC += "08";
					}
					else if (s1 == "RET") {
						LC++;
						codeC += "10";
					}
					else {

						if (s1.size() == 4 || s1.size() == 5) {
							if (s1[s1.size() - 1] == 'B') pazi = 1;
							else if (s1[s1.size() - 1] == 'W') pazi = 2;
						}
						else if (s1.size() == 3) {
							if (s1 != "SUB") {
								if (s1[s1.size() - 1] == 'B') pazi = 1;
								else if (s1[s1.size() - 1] == 'W') pazi = 2;
							}
						}
						if (pazi != 0) {
							string pom = "";
							for (int i = 0; i < s1.size() - 1; i++) {
								pom += s1[i];
							}

							s1 = pom;
						}

						if (s1 == "PUSH") lucky += "01001";
						else if (s1 == "POP") lucky += "01010";

						// obrada za push i pop
						if (lucky != "") {
							if (item[i][br] == '$') {
								br++;
								while (item[i][br] != '\n') s2 += item[i][br++];
								br++; // preskoci ','

								if (pazi == 2) lucky += "1"; //ako je w, tada je size = 1
								else lucky += "0"; // za b ili nista je size=0, pa ako bude >255, menja se

								if (is_number(s2)) {
									if (atoi(s2.c_str()) < 255) {
										lucky += "0000000000";
										if (pazi == 2) lucky += decToBinary(atoi(s2.c_str()), 2);
										else lucky += decToBinary(atoi(s2.c_str()), 1);
									}
									else {
										if (pazi == 1) cout << "ERRORE! " << item[i] << endl;
										lucky[5] = '1';
										lucky += "0000000000";
										lucky += decToBinary(atoi(s2.c_str()), 2);
									}
								}
								else cout << "EURORA! " << item[i] << endl;

								LC += (lucky.size() / 8);;
								codeC += binary_hexa(lucky);

							}

							else if (item[i][br] == '%') {
								br++; //preskoci %
								while (item[i][br] != '\n') if (item[i][br] != ' ') s2 += item[i][br++];

								br++; //preskoci ,

								if (s2.size() == 2) {
									if (pazi == 2) lucky += "100";
									else lucky += "000";

									lucky += "001";

									if (pazi == 1) cout << "ERROREE! " << item[i] << endl;

									lucky += getReg(s2);
								}
								else if (s2.size() == 3) {

									if (pazi == 2) cout << "ERRORE! " << item[i] << endl;
									else lucky += "000";

									lucky += "001";
									lucky += getRegKRACI(s2);

								}

								LC += (lucky.size() / 8);
								codeC += binary_hexa(lucky);
							}

							else if (item[i][br] == '(') {
								br++;
								if (item[i][br] == '%') {
									br++;
									while (item[i][br] != ')') if (item[i][br] != ' ') s2 += item[i][br++];

									br++; // za zagradu;

									if (pazi == 2) lucky += "100";
									else lucky += "000";

									lucky += "010";

									if (s2.size() == 2)
										lucky += getReg(s2);
									else cout << "ERRUFL! " << item[i] << endl;
								}

								LC += (lucky.size() / 8);
								codeC += binary_hexa(lucky);

							}

							else {
								while (item[i][br] != '(' && item[i][br] != '\n') if (item[i][br] != ' ') s4 += item[i][br++];

								if (is_number(s4) == false) {

									Symbol_Table* pom = isInSymbol_Table(s4);
									if (pom == nullptr) {
										add(s4, 0, 'l', false, 0, 0);
									}

									int av = LC + 2;
									addF(av, s4, BROJ_SEKCIJE, SEKCIJA, 'w');
								}

								if (item[i][br] == '(') { //regindpom
									br++;

									lucky += "000011";
									if (pazi == 2) lucky[5] = '1';

									if (item[i][br++] == '%') {
										while (item[i][br] != ')') if (item[i][br] != ' ') s2 += item[i][br++];

										lucky += getReg(s2);
									}
									else cout << "ERRORETRA! " << item[i] << endl;

									if (is_number(s4)) {
										lucky += decToBinary(atoi(s4.c_str()), 2);
									}
									else {
										if (s2 == "PC" || s2 == "pc" || s2 == "r7" || s2 == "R7") {
											poslF->setLinker(-2);
											poslF->setIma_Pomeraj();
											lucky += "0000000000000000";
										}
										/*lucky += decToBinary(-2, 2);*/
										else lucky += "0000000000000000";
									}
								}
								else if (item[i][br] == '\n') { //memdir
									br++;

									if (pazi == 2) lucky += "100";
									else lucky += "000";

									lucky += "10000000";

									if (is_number(s4)) {
										lucky += decToBinary(atoi(s4.c_str()), 2);
									}
									else {
										lucky += "0000000000000000";
									}
								}
								else cout << "ERORIRIRI! " << item[i] << endl;

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
									br++;
									while (item[i][br] != ',') s2 += item[i][br++];
									br++; // preskoci ','
									while (item[i][br] == ' ') br++; // preskoci ' ' sve do sledeceg

									if (pazi == 2) lucky += "1"; //ako je w, tada je size = 1
									else lucky += "0"; // za b ili nista je size=0, pa ako bude >255, menja se

									if (is_number(s2)) {
										if (atoi(s2.c_str()) < 255) {
											lucky += "0000000000";
											if (s1 == "MOV") lucky += "";
											else if (pazi == 2) lucky += decToBinary(atoi(s2.c_str()), 2);
											else lucky += decToBinary(atoi(s2.c_str()), 1);
										}
										else {
											if (pazi == 0) lucky[5] = '1';
											if (pazi == 1) cout << "ERRORE! " << item[i] << endl;
											lucky[5] = '1';
											lucky += "0000000000";
											lucky += decToBinary(atoi(s2.c_str()), 2);
										}

										// immed - regdir
										if (item[i][br] == '%') {
											br++;

											while (item[i][br] != '\n' && item[i][br] != ' ') s3 += item[i][br++];
											if (s3.size() == 2) { //s1 je registar bez h ili l

												if (s1 == "MOV" && s3.size() == 2 && atoi(s2.c_str()) < 255)
													lucky += decToBinary(atoi(s2.c_str()), 2);


												lucky += "001";
												lucky += getReg(s3);
											}
											else if (s3.size() == 3) { // s1 je registar sa h ili l

												if (pazi == 2) cout << "ERRORE! " << item[i] << endl; // ne sme MOVW sa high.low
												if (atoi(s2.c_str()) > 255) cout << "ERRORE! " << item[i] << endl; // ne sme npr. MOV $2000, %R1 high/low

												lucky += "001";

												if (s3[1] == '0') lucky += "0000";
												else if (s3[1] == '1') lucky += "0001";
												else if (s3[1] == '2') lucky += "0010";
												else if (s3[1] == '3') lucky += "0011";
												else if (s3[1] == '4') lucky += "0100";
												else if (s3[1] == '5') lucky += "0101";
												else if (s3[1] == '6') lucky += "0110";
												else if (s3[1] == '7') lucky += "0111";
												else cout << "ERRORE! " << item[i] << endl;

												if (s3[2] == 'h') lucky += "1";
												else if (s3[2] == 'l') lucky += "0";
												else cout << "ERRORE! " << item[i] << endl;

											}
											else {
												cout << "ERRORE! " << item[i] << endl;
											}

											LC += (lucky.size() / 8);;
											codeC += binary_hexa(lucky);

										}
										// immed - regind
										else if (item[i][br] == '(') {
											br++;
											lucky += "010";
											if (item[i][br++] == '%') {
												while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];
												lucky += getReg(s3);

												LC += (lucky.size() / 8);
												codeC += binary_hexa(lucky);
											}
											else cout << "ERRORE! " << item[i] << endl;
										}
										// immed - regindpom/memdir
										else {
											while (item[i][br] != '(' && item[i][br] != '\n') if (item[i][br] != ' ') s4 += item[i][br++];

											if (is_number(s4) == false) {

												Symbol_Table* pom = isInSymbol_Table(s4);
												if (pom == nullptr) {
													add(s4, 0, 'l', false, 0, 0);
												}

												int av = LC + (lucky.size() / 8) + 1;
												addF(av, s4, BROJ_SEKCIJE, SEKCIJA, 'w');
											}

											if (item[i][br] == '(') {
												br++;
												lucky += "011";
												if (item[i][br++] == '%') {

													while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];
													lucky += getReg(s3);
												}
												else cout << "ERRORE! " << item[i] << endl;

												if (is_number(s4)) {
													lucky += decToBinary(atoi(s4.c_str()), 2);
												}
												else {
													if (s3 == "PC" || s3 == "pc" || s3 == "r7" || s3 == "R7") {
														poslF->setLinker(-2);
														poslF->setIma_Pomeraj();
														lucky += "0000000000000000";
													}
													//lucky += decToBinary(-2, 2);
													else lucky += "0000000000000000";
												}
											}
											else {
												if (item[i][br] == '\n') {
													lucky += "10000000";
													if (is_number(s4)) {
														lucky += decToBinary(atoi(s4.c_str()), 2);
													}
													else {
														lucky += "0000000000000000";
													}
												}
											}

											LC += (lucky.size() / 8);
											codeC += binary_hexa(lucky);
										}
									}
									else {
										cout << "ERROR! " << item[i] << endl;
									}
								}
								// REGDIR
								else if (item[i][br] == '%') {
									br++; //preskoci %
									while (item[i][br] != ',') if (item[i][br] != ' ') s2 += item[i][br++];

									br++; //preskoci ,

									if (s2.size() == 2) {
										if (pazi == 2) lucky += "100";
										else lucky += "000";

										lucky += "001";

										if (pazi == 1) cout << "ERROREE! " << item[i] << endl;

										lucky += getReg(s2);
									}
									else if (s2.size() == 3) {

										if (pazi == 2) lucky += "100";
										else lucky += "000";

										lucky += "001";

										if (pazi == 2) cout << "ERRORE! " << item[i] << endl;

										lucky += getRegKRACI(s2);

									}
									else cout << "ERRORE! " << item[i] << endl;

									while (item[i][br] == ' ' || item[i][br] == ',') br++;

									// regdir - regdir
									if (item[i][br] == '%') {
										br++;
										while (item[i][br] != '\n') if (item[i][br] != ' ' && item[i][br] != '\n') s3 += item[i][br++];

										if (s3.size() == 2) {

											if (pazi == 1 && s1 == "MOV") cout << "ERRORE! " << item[i] << endl;
											if (s2.size() == 3 && s1 == "MOV") cout << "ERRORE! " << item[i] << endl;
											if (pazi == 1 && s2.size() == 3 && s1 == "MOV") cout << "ERRORE! " << item[i] << endl;

											lucky += "001";

											lucky += getReg(s3);
										}
										else if (s3.size() == 3) {

											lucky += "001";
											if (pazi == 0 && s2.size() == 2) lucky[5] = '1';

											if (pazi == 2) cout << "ERRORE! " << item[i] << endl;
											if (pazi == 0 && s2.size() == 2)  cout << "ERRORE! " << item[i] << endl;

											lucky += getRegKRACI(s3);
										}

										LC += lucky.size() / 8;
										codeC += binary_hexa(lucky);


									}
									// regdir - regind
									else if (item[i][br] == '(') {
										br++;
										if (item[i][br] == '%') {
											br++;

											if (pazi == 0 && s2.size() == 2) lucky[5] = '1';

											lucky += "010";

											while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];

											if (s3.size() == 2) {
												lucky += getReg(s3);
											}
											else cout << "ERRORA! " << item[i] << endl;
										}
										else cout << "ERRORISMO! " << item[i] << endl;

										LC += lucky.size() / 8;
										codeC += binary_hexa(lucky);
									}
									// regdir - regindpom/memdir
									else {
										while (item[i][br] != '(' && item[i][br] != '\n') if (item[i][br] != ' ') s4 += item[i][br++];

										if (is_number(s4) == false) {

											Symbol_Table* pom = isInSymbol_Table(s4);
											if (pom == nullptr) {
												add(s4, 0, 'l', false, 0, 0);
											}

											int av = LC + (lucky.size() / 8) + 1;
											addF(av, s4, BROJ_SEKCIJE, SEKCIJA, 'w');
										}

										if (item[i][br] == '(') { // regdir - regindpom PAZI PC
											br++;

											lucky += "011";

											if (pazi == 0 && s2.size() == 2) lucky[5] = '1';

											if (item[i][br++] == '%') {
												while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];
												lucky += getReg(s3);
											}
											else cout << "ERRORETRA! " << item[i] << endl;

											if (is_number(s4)) {
												lucky += decToBinary(atoi(s4.c_str()), 2);
											}
											else {
												if (s3 == "PC" || s3 == "pc" || s3 == "r7" || s3 == "R7") {
													poslF->setLinker(-2);
													poslF->setIma_Pomeraj();
													lucky += "0000000000000000";
												}
												//lucky += decToBinary(-2, 2);
												else lucky += "0000000000000000";
											}
										}
										else {
											if (item[i][br] == '\n') {
												lucky += "10000000";
												if (pazi == 0 && s2.size() == 2) lucky[5] = '1';

												if (is_number(s4)) {
													lucky += decToBinary(atoi(s4.c_str()), 2);
												}
												else {
													lucky += "0000000000000000";
												}
											}
										}

										LC += lucky.size() / 8;
										codeC += binary_hexa(lucky);


									}
								}
								// REGIND
								else if (item[i][br] == '(') {
									br++;
									if (item[i][br] == '%') {
										br++;
										while (item[i][br] != ')') if (item[i][br] != ' ') s2 += item[i][br++];

										br++; // za zagradu;

										if (pazi == 2) lucky += "100";
										else lucky += "000";

										lucky += "010";

										if (s2.size() == 2)
											lucky += getReg(s2);
										else cout << "ERRUFL! " << item[i] << endl;

										while (item[i][br] == ' ') br++;

										if (item[i][br] == ',') br++;
										else cout << "ERONDOFIL" << endl;

										while (item[i][br] == ' ') br++;

										// regind - regdir
										if (item[i][br] == '%') {
											br++;

											lucky += "001";

											while (item[i][br] != '\n') if (item[i][br] != ' ') s3 += item[i][br++];

											if (s3.size() == 2) {

												if (pazi == 0) lucky[5] = '1';

												if (pazi == 1) cout << "ERRORUROS! " << item[i] << endl;

												lucky += getReg(s3);
											}
											else if (s3.size() == 3) {

												if (pazi == 2) cout << "ERRORUROSS! " << item[i] << endl;

												lucky += getRegKRACI(s3);
											}

											LC += lucky.size() / 8;
											codeC += binary_hexa(lucky);

										}
										// regind - regind
										else if (item[i][br] == '(') {
											br++;
											if (item[i][br] == '%') {
												br++;
												while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];

												br++; // za zagradu;

												lucky += "010";
												if (pazi == 0) lucky[5] = '1';

												if (s3.size() == 2) {
													lucky += getReg(s3);
												}
												else cout << "ERRUFLIO! " << item[i] << endl;
											}
											else cout << "ERRURRR! " << item[i] << endl;

											LC += lucky.size() / 8;
											codeC += binary_hexa(lucky);

										}
										// regind - regindpom / memdir
										else {
											while (item[i][br] != '(' && item[i][br] != '\n') if (item[i][br] != ' ') s4 += item[i][br++];

											if (is_number(s4) == false) {

												Symbol_Table* pom = isInSymbol_Table(s4);
												if (pom == nullptr) {
													add(s4, 0, 'l', false, 0, 0);
												}

												int av = LC + (lucky.size() / 8) + 1;
												addF(av, s4, BROJ_SEKCIJE, SEKCIJA, 'w');
											}

											if (item[i][br] == '(') { //regind - regindpom
												br++;

												lucky += "011";

												if (pazi == 0) lucky[5] = '1';

												if (item[i][br++] == '%') {
													while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];

													lucky += getReg(s3);
												}
												else cout << "ERRORETRA! " << item[i] << endl;

												if (is_number(s4)) {
													lucky += decToBinary(atoi(s4.c_str()), 2);
												}
												else {
													if (s3 == "PC" || s3 == "pc" || s3 == "r7" || s3 == "R7") {
														lucky += "0000000000000000";
														poslF->setLinker(-5);
														poslF->setIma_Pomeraj();
													}
													//lucky += decToBinary(-2, 2);
													else lucky += "0000000000000000";
												}
											}
											else { //regind - memdir
												if (item[i][br] == '\n') {
													lucky += "10000000";
													if (pazi == 0) lucky[5] = '1';

													if (is_number(s4)) {
														lucky += decToBinary(atoi(s4.c_str()), 2);
													}
													else {
														lucky += "0000000000000000";
													}
												}
											}

											LC += lucky.size() / 8;
											codeC += binary_hexa(lucky);
										}
									}
								}
								// REGINDPOM/MEMDIR
								else {
									while (item[i][br] != '(' && item[i][br] != ',') if (item[i][br] != ' ') s4 += item[i][br++];


									if (is_number(s4) == false) {

										Symbol_Table* pom = isInSymbol_Table(s4);
										if (pom == nullptr) {
											add(s4, 0, 'l', false, 0, 0);
										}

										int av = LC + 2;
										addF(av, s4, BROJ_SEKCIJE, SEKCIJA, 'w');
									}

									For_Table* pomocni = poslF;

									//REGINDPOM
									if (item[i][br] == '(') {
										br++;

										lucky += "000011";
										if (pazi == 2) lucky[5] = '1';

										if (item[i][br++] == '%') {
											while (item[i][br] != ')') if (item[i][br] != ' ') s2 += item[i][br++];

											lucky += getReg(s2);
										}
										else cout << "ERRORETRA! " << item[i] << endl;

										int SPECIJALNI_FLAG = 0;
										if (s2 == "PC" || s2 == "pc" || s2 == "R7" || s2 == "r7") SPECIJALNI_FLAG = 1;

										if (is_number(s4)) {
											lucky += decToBinary(atoi(s4.c_str()), 2);
										}
										else {
											if (SPECIJALNI_FLAG == 0)
												lucky += "0000000000000000";
										}

										br++; //preskoci ')'
										while (item[i][br] == ' ') br++;
										if (item[i][br] == ',') {
											br++;
											while (item[i][br] == ' ') br++;
											// regindpom - regdir
											if (item[i][br] == '%') {
												br++;

												while (item[i][br] != '\n') if (item[i][br] != ' ') s3 += item[i][br++];

												if (SPECIJALNI_FLAG == 1) {

													pomocni->setLinker(-3);
													pomocni->setIma_Pomeraj();
													lucky += "0000000000000000";
													//lucky += decToBinary(-3, 2);
												}

												lucky += "001";

												if (s3.size() == 2) {

													if (pazi == 0) lucky[5] = '1';

													if (pazi == 1) cout << "ERRORUROS! " << item[i] << endl;

													lucky += getReg(s3);
												}
												else if (s3.size() == 3) {

													if (pazi == 2) cout << "ERRORUROSS! " << item[i] << endl;

													lucky += getRegKRACI(s3);
												}

												LC += lucky.size() / 8;
												codeC += binary_hexa(lucky);
											}
											// regindpom - regind
											else if (item[i][br] == '(') {
												br++;
												if (item[i][br] == '%') {
													br++;
													while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];

													br++; // za zagradu;

													if (SPECIJALNI_FLAG == 1) {
														pomocni->setLinker(-3);
														pomocni->setIma_Pomeraj();
														lucky += "0000000000000000";
														//lucky += decToBinary(-3, 2);
													}

													lucky += "010";
													if (pazi == 0) lucky[5] = '1';

													if (s3.size() == 2) {
														lucky += getReg(s3);
													}
													else cout << "ERRUFLIO! " << item[i] << endl;
												}
												else cout << "ERRURRR! " << item[i] << endl;

												LC += lucky.size() / 8;
												codeC += binary_hexa(lucky);
											}
											// regindpom - regindpom/memdir
											else {
												while (item[i][br] != '(' && item[i][br] != '\n') if (item[i][br] != ' ') s5 += item[i][br++];

												if (is_number(s5) == false) {

													Symbol_Table* pom = isInSymbol_Table(s5);
													if (pom == nullptr) {
														add(s5, 0, 'l', false, 0, 0);
													}

													if (SPECIJALNI_FLAG == 0) {
														int av = LC + (lucky.size() / 8) + 1;
														addF(av, s5, BROJ_SEKCIJE, SEKCIJA, 'w');
													}
													else {
														int av = LC + 5;
														addF(av, s5, BROJ_SEKCIJE, SEKCIJA, 'w');
														pomocni->setLinker(-5);
														pomocni->setIma_Pomeraj();
														lucky += "0000000000000000";
													}
												}

												// regindpom - regindpom
												if (item[i][br] == '(') {
													br++;

													if (pazi == 0) lucky[5] = '1';

													if (item[i][br++] == '%') {
														while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];

														if (SPECIJALNI_FLAG == 1) {
															pomocni->setLinker(-5);
															pomocni->setIma_Pomeraj();
														}
														//lucky += decToBinary(-5, 2);

														lucky += "011";

														lucky += getReg(s3);
													}
													else cout << "ERRORETRA! " << item[i] << endl;

													if (is_number(s4)) {
														lucky += decToBinary(atoi(s4.c_str()), 2);
													}
													else {
														if (s3 == "PC" || s3 == "pc" || s3 == "R7" || s3 == "r7") {
															poslF->setLinker(-2);
															poslF->setIma_Pomeraj();
															lucky += "0000000000000000";
														}
														//lucky += decToBinary(-2, 2);
														else lucky += "0000000000000000";
													}
												}
												// regindpom - memdir
												else {
													if (item[i][br] == '\n') {

														if (SPECIJALNI_FLAG == 1) {
															pomocni->setLinker(-5);
															pomocni->setIma_Pomeraj();
														}
														//lucky += decToBinary(-5, 2);

														lucky += "10000000";
														if (pazi == 0) lucky[5] = '1';

														if (is_number(s4)) {
															lucky += decToBinary(atoi(s4.c_str()), 2);
														}
														else {
															lucky += "0000000000000000";
														}
													}
												}

												LC += lucky.size() / 8;
												codeC += binary_hexa(lucky);
											}
										}
										else cout << "ERRDUCATI! " << item[i] << endl;


									}
									//MEMDIR
									else if (item[i][br] == ',') {
										br++;

										if (pazi == 2) lucky += "100";
										else lucky += "000";

										lucky += "10000000";

										if (is_number(s4)) {
											lucky += decToBinary(atoi(s4.c_str()), 2);
										}
										else {
											Symbol_Table* pom = isInSymbol_Table(s4);
											if (pom == nullptr) add(s4, 0, 'l', false, 0, 0);
											lucky += "0000000000000000"; // dodaj decToBinary(TABELA SIMBOLA-VREDNOST)
										}

										while (item[i][br] == ' ') br++;

										// memdir - regdir
										if (item[i][br] == '%') {
											br++;
											lucky += "001";

											while (item[i][br] != '\n') if (item[i][br] != ' ') s3 += item[i][br++];

											if (s3.size() == 2) {

												if (pazi == 0) lucky[5] = '1';

												if (pazi == 1) cout << "ERRORUROS! " << item[i] << endl;

												lucky += getReg(s3);
											}
											else if (s3.size() == 3) {

												if (pazi == 2) cout << "ERRORUROSS! " << item[i] << endl;

												lucky += getRegKRACI(s3);
											}
											else cout << "ERROTROFIL! " << item[i] << endl;

											LC += lucky.size() / 8;
											codeC += binary_hexa(lucky);
										}
										// memdir - regind
										else if (item[i][br] == '(') {
											br++;
											if (item[i][br] == '%') {
												br++;
												while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];

												br++; // za zagradu;

												if (pazi == 2) lucky += "100";
												else lucky += "000";

												lucky += "010";
												if (pazi == 0) lucky[5] = '1';

												if (s3.size() == 2) {
													lucky += getReg(s3);
												}
												else cout << "ERRUFLIO! " << item[i] << endl;
											}
											else cout << "ERRURRR! " << item[i] << endl;

											LC += lucky.size() / 8;
											codeC += binary_hexa(lucky);
										}
										// memdir - regindpom/memdir
										else {
											while (item[i][br] != '(' && item[i][br] != '\n') if (item[i][br] != ' ') s5 += item[i][br++];

											if (is_number(s5) == false) {

												Symbol_Table* pom = isInSymbol_Table(s5);
												if (pom == nullptr) {
													add(s5, 0, 'l', false, 0, 0);
												}

												int av = LC + (lucky.size() / 8) + 1;
												addF(av, s5, BROJ_SEKCIJE, SEKCIJA, 'w');
											}

											if (item[i][br] == '(') {
												br++;

												lucky += "011";

												if (pazi == 0) lucky[5] = '1';

												if (item[i][br++] == '%') {
													while (item[i][br] != ')') if (item[i][br] != ' ') s3 += item[i][br++];

													lucky += getReg(s3);
												}
												else cout << "ERRORETRA! " << item[i] << endl;

												if (is_number(s5)) {
													lucky += decToBinary(atoi(s5.c_str()), 2);
												}
												else {
													if (s3 == "PC" || s3 == "pc" || s3 == "R7" || s3 == "r7") {
														poslF->setLinker(-5);
														poslF->setIma_Pomeraj();
														lucky += "0000000000000000";
													}
													//lucky += decToBinary(-2, 2);
													else lucky += "0000000000000000";
												}
											}
											else {
												if (item[i][br] == '\n') {
													lucky += "10000000";
													if (pazi == 0) lucky[5] = '1';

													if (is_number(s5)) {
														lucky += decToBinary(atoi(s5.c_str()), 2);
													}
													else {
														Symbol_Table* pom = isInSymbol_Table(s5);
														if (pom == nullptr) add(s5, 0, 'l', false, 0, 0);
														lucky += "0000000000000000";
													}
												}
											}

											LC += lucky.size() / 8;
											codeC += binary_hexa(lucky);
										}
									}
								}
							}
						}

						// BRANCHES
						if (lucky == "") {
							if (pazi == 1) throw "You can't use branches with B sufix! " + item[i];

							if (s1 == "CALL") lucky += "00100100";
							else if (s1 == "JMP") lucky += "00101100";
							else if (s1 == "JEQ") lucky += "00110100";
							else if (s1 == "JNE") lucky += "00111100";
							else if (s1 == "JGT") lucky += "01000100";

							// IMMED
							if (item[i][br] != '*') {
								while (item[i][br] != '\n') if (item[i][br] != ' ') s2 += item[i][br++];

								lucky += "00000000"; // 2. bajt

								if (is_number(s2) == true)
									lucky += decToBinary(atoi(s2.c_str()), 2);
								else {

									Symbol_Table* pom = isInSymbol_Table(s2);
									if (pom == nullptr) {
										add(s2, 0, 'l', false, 0, 0);
									}

									int av = LC + (lucky.size() / 8);

									addF(av, s2, BROJ_SEKCIJE, SEKCIJA, 'w');
									poslF->setSkok();

									lucky += decToBinary(0, 2);
								}

								LC += lucky.size() / 8;
								codeC += binary_hexa(lucky);

							}
							// NOT IMMED
							else if (item[i][br] == '*') {
								br++;
								while (item[i][br] == ' ') br++;

								// REGDIR
								if (item[i][br] == '%') {
									br++;
									while (item[i][br] == ' ') br++;

									lucky += "001"; //regdir

									while (item[i][br] != '\n') if (item[i][br] != ' ') s2 += item[i][br++];

									lucky += getReg(s2);

									LC += lucky.size() / 8;
									codeC += binary_hexa(lucky);
								}
								// REGIND
								else if (item[i][br] == '(') {
									br++;
									while (item[i][br] == ' ') br++;
									if (item[i][br] == '%') {
										br++;
										lucky += "010"; //regind

										while (item[i][br] != ')') if (item[i][br] != ' ') s2 += item[i][br++];

										lucky += getReg(s2);

										LC += lucky.size() / 8;
										codeC += binary_hexa(lucky);
									}
								}
								// REGINDPOM / MEMDIR
								else {
									while (item[i][br] != '(' && item[i][br] != '\n') if (item[i][br] != ' ') s3 += item[i][br++];

									if (item[i][br] == '(') { // REGINDPOM
										br++;
										if (item[i][br] == '%') {
											br++;
											while (item[i][br] != ')') if (item[i][br] != ' ') s2 += item[i][br++];

											lucky += "011";
											lucky += getReg(s2);

											if (is_number(s3) == true) {
												lucky += decToBinary(atoi(s3.c_str()), 2);
											}
											else {

												Symbol_Table* pom = isInSymbol_Table(s3);
												if (pom == nullptr) {
													add(s3, 0, 'l', false, 0, 0);
												}

												int av = LC + (lucky.size() / 8);

												addF(av, s3, BROJ_SEKCIJE, SEKCIJA, 'w');
												poslF->setSkok();
												poslF->setLinker(-2);

												if (s2 == "PC" || s2 == "pc" || s2 == "r7" || s2 == "R7") {
													lucky += decToBinary(0, 2);
												}
												else lucky += decToBinary(0, 2);
											}

											LC += lucky.size() / 8;
											codeC += binary_hexa(lucky);

										}
										else {
											throw "Unknown ( attempt BRANCH-REGINDPOM )? " + item[i];
										}

									}
									else if (item[i][br] == '\n') { //MEMDIR
										// u s3 je adresa

										lucky += "10000000";

										if (is_number(s3) == true) {
											lucky += decToBinary(atoi(s3.c_str()), 2);
										}
										else {

											Symbol_Table* pom = isInSymbol_Table(s3);
											if (pom == nullptr) {
												add(s3, 0, 'l', false, 0, 0);
											}

											int av = LC + (lucky.size() / 8);

											addF(av, s3, BROJ_SEKCIJE, SEKCIJA, 'w');
											poslF->setSkok();

											lucky += decToBinary(0, 2);
										}

										LC += lucky.size() / 8;
										codeC += binary_hexa(lucky);
									}
									else throw "Unknow type of add. " + item[i];
								}
							}
							// ERROR
							else {
							throw "ERROR! " + item[i];
							}

						}
					}
					cout << lucky << " " << lucky.size() << endl;
					cout << LC << endl;
				}
			}
		}

		cout << endl;

		for (Equ_Table* tek = prviE; tek != nullptr; tek = tek->next) {
			string* s1 = tek->getNizBukvalno();
			string *s2 = tek->getZnaciBukvalno();

			int s3[50];
			for (int i = 0; i < 50; i++)
				s3[i] = 0;

			int vrednost = 0;
			for (int i = 0; i < tek->getNizSize(); i++) {
				Symbol_Table* pom = isInSymbol_Table(s1[i]);

				if (pom != nullptr)
					if (s2[i] == "-") {
						s3[pom->getSection()] -= pom->getSection();
					}
					else s3[pom->getSection()] += pom->getSection();
			}

			for (int i = 0; i < 50; i++)
				if (s3[i] != i && s3[i] != 0 && s3[i] != -i) cout << "ERROR!" << endl;

			for (int i = 0; i < tek->getNizSize(); i++) {
				if (is_number(s1[i]) == false) {
					Symbol_Table * pom = isInSymbol_Table(s1[i]);
					if (pom != nullptr && pom->getEqu() == false) { int n = pom->getValue(); string s = to_string(n);  tek->setSymbol(s, i); }
					else tek->setSymbol(s1[i], i);
				}
			}
		}

		for (Equ_Table* tek = prviE; tek != nullptr;) {
			string* s1 = tek->getNizBukvalno();
			string *s2 = tek->getZnaciBukvalno();
			int vr = 0;

			int skroz = 0;
			for (int i = 0; i < tek->getNizSize(); i++) {
				string pomocSvima = s1[i];
				string pomocSvima2 = "";

				if (pomocSvima[0] == '-') {
					for (int i = 1; i < pomocSvima.size(); i++)
						pomocSvima2 += pomocSvima[i];
				}
				else pomocSvima2 = pomocSvima;

				if (is_number(pomocSvima2) == true) {
					if(s2[i]=="-")
						vr -= atoi(pomocSvima.c_str());
					else vr += atoi(pomocSvima.c_str());
				}
				else {
					skroz = 1;
					break;
				}
			}

			if (skroz == 0) {
				Symbol_Table * pom = isInSymbol_Table(tek->getSymbol());
				pom->setValue(vr);

				for (Equ_Table* tek2 = prviE; tek2 != nullptr; tek2 = tek2->next) {
					if (tek2 != tek) {
						string* s1 = tek2->getNizBukvalno();
						for (int i = 0; i < tek2->getNizSize(); i++) {
							if (s1[i] == tek->getSymbol()) {
								int n = pom->getValue();
								string s = to_string(n);
								tek2->setSymbol(s, i);
							}
						}
					}
				}

				for (Equ_Table* tek3 = prviE; tek3 != nullptr; tek3 = tek3->next) {
					Equ_Table* last3=nullptr;
					if (tek3->getSymbol() == tek->getSymbol()) {
						if (last3 == nullptr) {
							prviE = prviE->next;
							tek = tek->next;
							delete tek3;
							break;
						}
						else if (tek3->next == nullptr) {
							poslE = last3;
							tek = tek->next;
							delete tek3;
							break;
						}
					}
					else {
						last3 = tek3;
						tek3 = tek3->next;
					}
				}
			}
			else {
				tek = tek->next;
			}


		}

		for (Equ_Table* tek = prviE; tek != nullptr; tek = tek->next)
			cout << tek->getSymbol() << " | " << tek->getNiz() << " | " << tek->getZnaci() << endl;

		cout << endl;

		for (Symbol_Table*tek = prvi; tek != nullptr; tek = tek->next) {
			if (tek->getlg() == 'l' && tek->getSection() == 0 && tek->getName() != "UND") cout << "ERROR! LOCAL AND UNDEFINED! " << endl;
			cout << tek->getRbr() << " | " << tek->getSection() << " | " << tek->getlg() << " | " << tek->getDefined() << " | " << tek->getValue() << " | " <<
				tek->getSize() << " | " << tek->getName() << " | " << tek->getEqu() << endl;
		}
		cout << endl;

		for (Reloc_Table* tek = prviR; tek != nullptr; tek = tek->next) {
			cout << tek->getRbr() << " | " << tek->getSym() << " | " << tek->getAdress() << " | " << tek->getWay() << endl;
		}
		cout << endl;

		for (For_Table* tek = prviF; tek != nullptr; tek = tek->next) {
			cout << tek->getPatch() << " | " << tek->getSize() << " | " << tek->getSymbol() << " | " << tek->getSekcija_U_Kojoj_Je_Nastao_Skok() << " | " << tek->getIsSkok() << " | " << tek->getIma_Pomeraj() << " | " << tek->getLinker() << endl;
		}
		cout << endl;

		for (Symbol_Table*tek = prvi; tek != nullptr; tek = tek->next) {
			if (tek->getSize() != 0) {
				cout << tek->getName() << ": ";
				for (int i = 0; i < tek->kod.size(); i++) {
					cout << tek->kod[i];
				}
				cout << endl;
			}
		}
		cout << endl;
		for (For_Table* tek = prviF; tek != nullptr; tek = tek->next) {
			Symbol_Table* pom = isInSymbol_Table(tek->getSymbol());
			if (pom == nullptr) {
				cout << "ERROR! NOT IN TABLE!" << endl;
			}
			else if (pom->getlg() == 'l') {
				int a = tek->getSectionNumber();
				Symbol_Table* c = getCodeNumberMain(a);

				char vel = tek->getSize();
				if (vel == 'b') {
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

				if (tek->getLinker() != 0 || tek->getIma_Pomeraj()) {
					addR(a, tek->getPatch(), "PCrel");
					poslR->setLinker(tek->getLinker());
				}
				else addR(a, tek->getPatch(), "direct");
			}
			else {
				if (tek->getLinker() != 0 || tek->getIma_Pomeraj()) {
					addR(pom->getRbr(), tek->getPatch(), "PCrel");
					poslR->setLinker(tek->getLinker());
				}
				else addR(pom->getRbr(), tek->getPatch(), "direct");
			}
		}

		for (Reloc_Table* tek = prviR; tek != nullptr; tek = tek->next) {
			cout << tek->getRbr() << " | " << tek->getSym() << " | " << tek->getAdress() << " | " << tek->getWay() << " | " << tek->getLinker() << endl;
		}
		cout << endl;

		for (Symbol_Table*tek = prvi; tek != nullptr; tek = tek->next) {
			if (tek->getSize() != 0) {
				cout << tek->getName() << ": ";
				for (int i = 0; i < tek->kod.size(); i++) {
					cout << tek->kod[i];
				}
				cout << endl;
			}
		}
		cout << endl;
		inFile.close();
	
	}
	catch (string m) {
		cout << m << endl;
	}
	system("pause");
	return 0;
}