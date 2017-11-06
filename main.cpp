#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

class automat
{
	private:
		int type; //0 - mealy | 1 - moore
		int states; //stany czyli kolejne wiersze
		int inputs; //wejscia
		int **stateArr; //tablica stanï¿½w
		char **output; //tablica wyjsï¿½
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
			printf("ty = %d\n", ty);
			printf("st = %d\n", st);
			printf("in = %d\n", in);

			type = ty;
			states = st;
			inputs = in;

			//alokacja pamiï¿½ci na stany automatu dla poszczegï¿½lnych wejsï¿½
			stateArr = new int*[st];
			for(int i = 0; i < st; ++i)
				stateArr[i] = new int[in];

			//alokacja pamiï¿½ci na tablice wyjscia
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
			while(!isdigit(fgetc(f)));
			fseek(f, -1, SEEK_CUR);
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

			for(int i = 0; i < states; ++i) //wyï¿½wietlanie tabeli z danymi
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

			for(int i = 0; i < states; ++i) //wypeï¿½nianie
			{
				for(int j = i + 1; j < states; ++j)
				{
					bool match = false;
					//sprawdzenie dopasowania wyjï¿½cia
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
								if(stateArr[i][k] > stateArr[j][k]) //segregacja wynikï¿½w dla uï¿½atwienia
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

			//pozbycie siï¿½ z tablicy sprzecznoï¿½ci
			int iteration = (((states - 1)*(states - 1)) / 2) + states - 1;
			for(int times = 0; times < iteration; ++times)
			{
				for(int i = 0; i < states; ++i)
				{
					for(int j = 0; j < states - 1; ++j)
					{
						if(ungerPaull[j][i].size() > 0 && ungerPaull[j][i][0] == -2) //gdy znajdzie siï¿½ sprzecznoï¿½ï¿½ trzeba poszukaï¿½ pï¿½l ktï¿½re
						{
							for(int m = 0; m < states; ++m) //przechodzenie przez tablice paulliego
							{
								for(int n = 0; n < states - 1; ++n)
								{
									for(int k = 0; k < ungerPaull[n][m].size(); ++k)
									{
										if(ungerPaull[n][m][0] == -3) break; //nic tu nie ma
										if(ungerPaull[n][m][0] == -2) break; //jeï¿½li pole juï¿½ jest zaznaczone jako sprzecznoï¿½ï¿½ to pomija
										if(ungerPaull[n][m][0] == -4) break; //pomijamy caï¿½kowicie niesprzeczne
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
				} //koniec pozbywania siï¿½ niesprzecznoï¿½ci
			}

			for(int i = 0; i < states; ++i)
			{
				for(int j = 0; j < states - 1; ++j)
				{
					for(int k = 0; k < ungerPaull[j][i].size(); k+=2)
					{
						if(ungerPaull[j][i][0] == -3) break; //nic tu nie ma
						if(ungerPaull[j][i][0] == -2) break; //jeï¿½li pole juï¿½ jest zaznaczone jako sprzecznoï¿½ï¿½ to pomija
						if(ungerPaull[j][i][0] == -4) break; //pomijamy caï¿½kowicie niesprzeczne
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
			for(int i = 1; i < states; ++i) //wyï¿½wietlanie tablicy paulliego
			{
				for(int j = 0; j < states - 1; ++j)
				{
					for(int k = 0; k < ungerPaull[j][i].size(); ++k)
					{
						//-3 nic nie oznacza | -4 to V czyli wszystki zgodne | -2 to X czyli sprzecznoï¿½ï¿½
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
		
		bool removeRedundant(vector<int>& redundant)
		{
			int j = 0;
			unsigned int redundantSize = redundant.size();
			for(int i = 0; i < redundantSize; ++i)
			{
				while(((++i != redundantSize) && (redundant[j] == redundant[i])));
				//getchar();
				redundant[++j] = redundant[i];
			}
			redundant.resize(j);
			
			for(int i = 0; i < redundant.size(); ++i)
				cout << "tas " << redundant[i] << endl;
		}

		int OrganizeFamily() //ï¿½ï¿½czy stany w jaknajwiï¿½ksze grupy oraz zlicza iloï¿½ï¿½ poï¿½ï¿½czonych grup by je zwrï¿½ciï¿½
		{
			for(int i = 0; i < Qmax.size(); ++i)
			{
				for(int j = 0; j < Qmax[i].size(); ++j)
				{
					cout << Qmax[i][j];
					if(j%2 == 0) cout << "  ";
				}
				cout << " | ";
			}
			cout << "\n";
			
			int Qsize = Qmax.size();
			vector<int> matching;
			vector<int> blackList;
			bool wypier = false;
			
			cout << "Qsize " << Qsize << "\n";
			
			//getchar();
			
			for(int i = 0; i < Qsize; ++i)
			{
				for(int j = i; j < Qmax.size(); ++j)
				{
					if(i != j)
					{
						cout << "Qmax[" << i << "] = " << Qmax[i].size() << endl;
						cout << "Qmax[" << j << "] = " << Qmax[j].size() << endl;
						for(int m = 0; m < Qmax[i].size() && !wypier; ++m)
						{
							for(int n = 0; n < Qmax[j].size(); ++n)
							{
								if(Qmax[i][m] == Qmax[j][n])
								{
									cout << " a = " << Qmax[i][m] << "\n";
									matching.push_back(Qmax[i][m]);
									continue;
								}
								else if(Qmax[i][m] < Qmax[j][n])
								{
									cout << "\nagh";
									if(ungerPaull[Qmax[i][m] - 1][Qmax[j][n] - 1][0] == -2)
									{
										cout << "a\n";
										wypier = true;
										break;
									}
									else
									{
										cout << "b\n";
										matching.push_back(Qmax[i][m]);
										matching.push_back(Qmax[j][n]);
									}
								}
								else if(Qmax[i][m] > Qmax[j][n])
								{
									if(ungerPaull[Qmax[j][n] - 1][Qmax[i][m] - 1][0] == -2)
									{
										cout << "c\n";
										wypier = true;
										break;
									}
									else
									{
										cout << "d\n";
										matching.push_back(Qmax[j][n]);
										matching.push_back(Qmax[i][m]);
									}
								}
							}
						}
						cout << wypier << " " << matching.size() << "\n";
						if(!wypier)
						{
							//matching przeci¹gn¹ przez unique
							sort(matching.begin(), matching.end());
							removeRedundant(matching);
							blackList.push_back(i);
							blackList.push_back(j);
							
							for(int o = 0; o < matching.size(); ++o) cout << "lol " << matching[o] << "\n";
							
							Qmax.push_back(matching);
						}
						matching.clear();
						wypier = false;
					}	
				}
			}
			
			int deleteImtes = 0;
			
			if(blackList.size() > 0)
			{
				sort(blackList.begin(), blackList.end());
				removeRedundant(blackList);
				
				deleteImtes = blackList.size();
				
				for(int i = blackList.size() - 1; i >= 0; --i)
				{
					Qmax.erase(Qmax.begin() + blackList[i]);
				}
			}
			
			return deleteImtes; //zwraca iloœæ liczb ktï¿½re trzeba siï¿½ skreï¿½liï¿½o
		}

		void FindMaxFamily()
		{
			vector<int> temp;
			int actualFamily = -1;
			cout << "\n";

			for(int i = states - 2; i >= 0; --i) //wyï¿½wietlanie tablicy paulliego
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
	FILE *f;
	
	if(argc != 2)
	{
		printf("Wrong number of arguments\n");
		f = fopen("automaty.txt", "r");
	}
	else
	{
		f = fopen(argv[1], "r");
	}

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
