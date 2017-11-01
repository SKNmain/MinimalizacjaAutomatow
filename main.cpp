#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <vector>

using namespace std;

class automat
{
	private:	
		int type; //0 - mealy | 1 - moore
		int states; //stany czyli kolejne wiersze
		int inputs; //wejscia
		int **stateArr; //tablica stanów
		char **output; //tablica wyjsæ
		vector< vector < vector<int> > > ungerPaull;
		vector< vector<int> > Qmax;
	public:
		automat() : type(0), states(0), inputs(0) {}
		automat(int ty, int st, int in)
		{
			AllocAutomat(ty, st, in);
		}
		void AllocAutomat(int ty, int st, int in)
		{
			type = ty;
			states = st;
			inputs = in;
			
			//alokacja pamiêci na stany automatu dla poszczególnych wejsæ
			stateArr = new int*[st];
			for(int i = 0; i < st; ++i)
				stateArr[i] = new int[in];
			
			//alokacja pamiêci na tablice wyjscia
			if(type == 0) //mealy
			{
				output = new char*[st];
				for(int i = 0; i < st; ++i)
					output[i] = new char[in];
			}
			else //moore
			{
				output = new char*[st];
				for(int i = 0; i < st; ++i)
					output[i] = new char[1];
			}
		}

		~automat()
		{

			for (int i = 0; i < states; ++i)
				delete[] stateArr[i];
			delete[] stateArr;

			if (type == 0)
			{
				for (int i = 0; i < states; ++i)
					delete[] output[i];
				delete[] output;
			}
			else
			{
				for (int i = 0; i < states; ++i)
					delete[] output[i];
				delete[] output;
			}
		}
		void ReadAutomatFromFile(FILE *f)
		{
			fscanf(f, "%d%d%d", &type, &states, &inputs); //wczytaj dane startowe z pliku
			AllocAutomat(type, states, inputs);
			
			cout << type << " " << states << " " << inputs << "\n";
			if(type == 0) cout << "automat mally'ego\n";
			else cout << "automat moore'a\n";

			for(int i = 0; i < states; ++i)
			{
				for(int j = 0; j < inputs; ++j)
				{
					char temp[3];
					fscanf(f, "%s", &temp);
					if(temp[0] == '-')
					{
						stateArr[i][j] = -1;
					}
					else stateArr[i][j] = atoi(temp);	
				}
				if(type == 0)
				{
					for(int j = 0; j < inputs; ++j)
					{
						fscanf(f, "%s", &output[i][j]);	
					}
				}
				else
				{
					fscanf(f, "%s", &output[i][0]);	
				}
			}
			
			for(int i = 0; i < states; ++i) //wyœwietlanie tabeli z danymi
			{
				for(int j = 0; j < inputs; ++j)
				{
					if(stateArr[i][j] == -1) cout << "- ";
					else cout << stateArr[i][j] << " ";
				}
				if(type == 0)
				{
					for(int j = 0; j < inputs; ++j)
					{
						cout << output[i][j] << " ";
					}
					cout << "\n";
				}
				else
				{
					cout << output[i][0] << "\n";
				}
			}
		}
		
		void CreateUngerPaullTable()
		{
			for(int i = 0; i < states - 1; ++i) //inicjalizacja tablicy
			{
				vector <vector<int> > tempPaull;
				for(int j = 0; j < states; ++j)
				{
					vector <int> tempInt;
					//tempInt.push_back(-3);
					tempPaull.push_back(tempInt);
				}
				ungerPaull.push_back(tempPaull);	
			}
			
			for(int i = 0; i < states - 1; ++i) ungerPaull[i][0].push_back(-3); //pierwsza linia to same 3
			
			for(int i = 0; i < states; ++i) //wype³nianie
			{
				for(int j = i + 1; j < states; ++j)
				{
					bool match = false;
					//sprawdzenie dopasowania wyjœcia
					for(int k = 0; k < inputs; ++k)
					{
						if((output[i][k] == output[j][k]) || (output[i][k] == '-') || (output[j][k] == '-'))
						{
							match = true;
						}
						else
						{
							match = false;
							break;
						}
					}
					if(match)
					{
						int tempMatch = 0;
						for(int k = 0; k < inputs; ++k)
						{
							//wrzucenie pary do tablicy
							if(stateArr[i][k] != -1 && stateArr[j][k] != -1 && stateArr[j][k] != stateArr[i][k] )
							{
								if(stateArr[i][k] > stateArr[j][k]) //segregacja wyników dla u³atwienia
								{
									ungerPaull[i][j].push_back(stateArr[j][k]); 
									ungerPaull[i][j].push_back(stateArr[i][k]);
								}
								else
								{
									ungerPaull[i][j].push_back(stateArr[i][k]); 
									ungerPaull[i][j].push_back(stateArr[j][k]);
								}
							}
							else
							{
								++tempMatch;
							}
						}
						if(tempMatch == inputs) ungerPaull[i][j].push_back(-4);
					}
					else ungerPaull[i][j].push_back(-2);
				}
			}
			
			//pozbycie siê z tablicy sprzecznoœci
			int iteration = (((states - 1)*(states - 1)) / 2) + states - 1;
			for(int times = 0; times < iteration; ++times)
			{
				for(int i = 0; i < states; ++i)
				{
					for(int j = 0; j < states - 1; ++j)
					{
						if(ungerPaull[j][i].size() > 0 && ungerPaull[j][i][0] == -2) //gdy znajdzie siê sprzecznoœæ trzeba poszukaæ pól które
						{
							for(int m = 0; m < states; ++m) //przechodzenie przez tablice paulliego
							{
								for(int n = 0; n < states - 1; ++n)
								{
									for(int k = 0; k < ungerPaull[n][m].size(); ++k)
									{
										if(ungerPaull[n][m][0] == -3) break; //nic tu nie ma
										if(ungerPaull[n][m][0] == -2) break; //jeœli pole ju¿ jest zaznaczone jako sprzecznoœæ to pomija
										if(ungerPaull[n][m][0] == -4) break; //pomijamy ca³kowicie niesprzeczne
										if(ungerPaull[n][m][k] < ungerPaull[n][m][k + 1])
										{
											//cout << i  << " " << j << " ; "<< n << " " << m << "\n";
											if(ungerPaull[n][m][k] - 1 == j)
											{
												if(ungerPaull[n][m][k + 1] - 1 == i)
												{
													ungerPaull[n][m].clear();
													ungerPaull[n][m].push_back(-2);
													break;
												}
											}
											++k;
										}
										else
										{
											if(ungerPaull[n][m][k] - 1 == i)
											{
												if(ungerPaull[n][m][k + 1] - 1 == j)
												{
													ungerPaull[n][m].clear();
													ungerPaull[n][m].push_back(-2);
													break;
												}
											}
											++k;
										}			
									}
								}
							}	
						}
					}
				} //koniec pozbywania siê niesprzecznoœci
			}
			
			for(int i = 0; i < states; ++i)
			{
				for(int j = 0; j < states - 1; ++j)
				{
					for(int k = 0; k < ungerPaull[j][i].size(); k+=2)
					{
						if(ungerPaull[j][i][0] == -3) break; //nic tu nie ma
						if(ungerPaull[j][i][0] == -2) break; //jeœli pole ju¿ jest zaznaczone jako sprzecznoœæ to pomija
						if(ungerPaull[j][i][0] == -4) break; //pomijamy ca³kowicie niesprzeczne
						if(i < j)
						{
							if(ungerPaull[j][i][k] - 1 == i && ungerPaull[j][i][k + 1] - 1 == j)
							{
								for(int m = ungerPaull[j][i].size() - 1; m >= 0; --m)
								{
									ungerPaull[j][i].erase(ungerPaull[j][i].begin() + m);
								}
								ungerPaull[j][i].push_back(-4);
								break;
							}
						}
						else
						{
							if(ungerPaull[j][i][k] - 1 == j && ungerPaull[j][i][k + 1] - 1 == i)
							{
								for(int m = ungerPaull[j][i].size() - 1; m >= 0; --m)
								{
									ungerPaull[j][i].erase(ungerPaull[j][i].begin() + m);
								}
								ungerPaull[j][i].push_back(-4);
								break;
							}
						}
						
						
					}
				}
			}
			
			cout << "\nTablica: Unger-Paull'a\n";
			for(int i = 1; i < states; ++i) //wyœwietlanie tablicy paulliego
			{
				for(int j = 0; j < states - 1; ++j)
				{
					for(int k = 0; k < ungerPaull[j][i].size(); ++k)
					{
						//-3 nic nie oznacza | -4 to V czyli wszystki zgodne | -2 to X czyli sprzecznoœæ
						if(ungerPaull[j][i][k] == -3)
						{
							cout << " ";
						}
						else if(ungerPaull[j][i][k] == -2)
						{
							cout << "X ";
							if(k > 0 && k%2 == 1 && k < ungerPaull[j][i].size() - 1) cout << "; ";
						}
						else if(ungerPaull[j][i][k] == -4)
						{
							cout << "V ";
							if(k > 0 && k%2 == 1 && k < ungerPaull[j][i].size() - 1) cout << "; ";
						}
						else
						{
							cout << ungerPaull[j][i][k] << " ";
							if(k > 0 && k%2 == 1 && k < ungerPaull[j][i].size() - 1) cout << "; ";
						}
					}
					if(ungerPaull[j][i].size() > 0) cout << "| ";
				}
				cout << "\n";
			}
			
			
		}
		
		int OrganizeFamily() //³¹czy stany w jaknajwiêksze grupy oraz zlicza iloœæ po³¹czonych grup by je zwróciæ
		{
			for(int i = 0; i < Qmax.size(); ++i)
			{
				for(int j = 0; j < Qmax[i].size(); ++j)
				{
					cout << Qmax[i][j];
					if(j%2 == 0) cout << " ; ";
				}
				cout << " | ";
			}
			cout << "\n";
			
			return 0; //zwraca iloœæ liczb które trzeba siê skreœli³o
		}
		
		void FindMaxFamily()
		{
			vector<int> temp;
			int actualFamily = -1;
			cout << "\n";
			
			for(int i = states - 2; i >= 0; --i) //wyœwietlanie tablicy paulliego
			{
				for(int j = states - 1; j >= 1; --j)
				{
					if(ungerPaull[i][j].size() > 0)
					{
						if(ungerPaull[i][j][0] != -3)
						{
							if(ungerPaull[i][j][0] != -2)
							{
								++actualFamily;
								Qmax.push_back(temp);
								Qmax[actualFamily].push_back(i + 1);
								Qmax[actualFamily].push_back(j + 1);
							}
						}
					}
				}
				actualFamily -= OrganizeFamily();

			}
			
			
			cout << "\n\nMaksymalne rodziny stanow niesprzecznych\n";
			for(int i = 0; i < Qmax.size(); ++i)
			{
				for(int j = 0; j < Qmax[i].size(); ++j)
				{
					cout << Qmax[i][j];
					if(j%2 == 0) cout << " ; ";
				}
				cout << " | ";
			}
			
		}
		
		void CreateMinTable()
		{
			
		}
		
		void Minimalize()
		{
			CreateUngerPaullTable();
			FindMaxFamily();
			CreateMinTable();
		}
		
		inline int GetType()
		{
			return type;
		}
		inline int GetStateCount()
		{
			return states;
		}
		inline int GetInputCount()
		{
			return inputs;
		}
};

int main(int argc, char** argv)
{
	FILE *f = fopen("automaty.txt", "r");
	
	if(f == NULL)
	{
		printf("can not find file");
		return 1;
	}
	
	
	automat a;
	a.ReadAutomatFromFile(f);
	a.Minimalize();
	
	
	fclose(f);
	
	return 0;
}
