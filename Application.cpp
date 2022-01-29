#include "Application.h"
#include "Common.h"
#include <random>
#define POZA_SIZE_X 460
#define POZA_SIZE_Y 300
#define NR_MAX_STATIUNI 300
#define NR_MAX_PENSIUNI 300
#define NR_MAX_NUME 21
using namespace std;

struct Pensiune
{
	char numePensiune[NR_MAX_NUME];
	char categorie[NR_MAX_NUME];

	int pretLocPeZi;
	int numarLocuri;

	int codPensiune;
	int codStatiune;

	Poza poze[NR_MAX_POZE]{};
}pensiuni[NR_MAX_PENSIUNI];

int pensN,statN;
struct Statiune
{
	char numeStatiune[NR_MAX_NUME];
	char numeJudet[NR_MAX_NUME];
	int codStatiune;
	int nrLocuriLibere=0;
}statiuni[NR_MAX_STATIUNI];


/// Variabile Globale
GLFWwindow* window;
int display_w, display_h;
Poza background = { 0,"https://calatorinbascheti.ro/wp-content/uploads/2019/11/61662090-1024x576.jpg",LoadStatus::NotProccesed };

#pragma region Utility functions

enum ModSortareStatiuni
{
	DJudetDStat,
	DStat,
	DNr,
};
void SortareStatiuni(ModSortareStatiuni mod)
{
	int cmp;
	switch (mod)
	{
	case DJudetDStat:
		for(int i =0;i<statN;i++)
			for (int j = i + 1; j < statN; j++)
			{
				cmp = strcmp(statiuni[i].numeJudet, statiuni[j].numeJudet);
				if (cmp > 0)
					swap(statiuni[i], statiuni[j]);
				else if (cmp == 0)
				{
					cmp = strcmp(statiuni[i].numeStatiune, statiuni[j].numeStatiune);
					if(cmp > 0)
						swap(statiuni[i], statiuni[j]);
				}
			}
		break;
	case DStat:
		for (int i = 0; i < statN; i++)
			for (int j = i + 1; j < statN; j++)
			{
				cmp = strcmp(statiuni[i].numeStatiune, statiuni[j].numeStatiune);
				if (cmp > 0)
					swap(statiuni[i], statiuni[j]);
			}
		break;
	case DNr:
		for (int i = 0; i < statN; i++)
			for (int j = i + 1; j < statN; j++)
			{
				if (statiuni[i].nrLocuriLibere < statiuni[j].nrLocuriLibere)
					swap(statiuni[i], statiuni[j]);
			}
		break;
	default:
		break;
	}
}


void EliminaUnderscore(char * s)
{
	if (s!=0)
	for (char* p = s; *p; p++)
		if (*p == '_')
		
			*p = ' ';
}

void AdaugaLocLiberLaStatiune(int codStatiune,int nr)
{
	for (size_t i = 0; i < statN; i++)
	{
		if (statiuni[i].codStatiune == codStatiune)
		{
			statiuni[i].nrLocuriLibere += nr;
			return;
		}
	}
}

void AddPensiune(const char* numePens,const char* categorie,int pretLocPeZi,int codPensiune,int numarLocuri,int codStatiune, const char* poze[NR_MAX_POZE])
{
	Pensiune p;
	strcpy(p.numePensiune, numePens);
	strcpy(p.categorie, categorie);
	p.pretLocPeZi=pretLocPeZi;
	p.codPensiune = codPensiune;
	p.codStatiune=codStatiune;
	p.numarLocuri=numarLocuri;
	//for (int i = 0; i < NR_MAX_POZE; i++)
		//strcpy(p.poze[i].url, poze[i]); TODO
	pensiuni[pensN++] = p;
}

void AddStatiune(const char* numeStatiune,const char* numeJudet,int codStatiune,int nrLocuriLibere)
{
	Statiune s;
	strcpy(s.numeStatiune, numeStatiune);
	strcpy(s.numeJudet, numeJudet);
	s.codStatiune = codStatiune;
	s.nrLocuriLibere = nrLocuriLibere;
	statiuni[statN++] = s;
}

int GetCodPensiune()
{
	bool b = 0;
	int r;
	do
	{
		b = 0;
		r = rand();
		for (int i = 0; i < statN; ++i)
			if (r == pensiuni[i].codPensiune) b = 1;
	} while (b);
	return r;
}

int GetCodStatiuneFromName(const char* nume)
{
	for (size_t i = 0; i < statN; i++)
	{
		if (strcmp(statiuni[i].numeStatiune, nume) == 0)
		{
			return statiuni[i].codStatiune;
		}
	}
}

void CenterNextWindow(int n = 3, int m = 3)
{
	float sx = display_w / m, sy = display_h / n;
	ImGui::SetNextWindowSize({ (m - 2) * sx,(n - 2) * sy });
	ImGui::SetNextWindowPos({ sx,sy });
}

void SetRelPos(float x, float y)
{
	ImVec2 screen_pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos({ screen_pos.x + x,screen_pos.y + y });
}

void InputTextTable(const char** lable, char** buf,int rows,float voff=-3, ImGuiInputTextFlags_ flags= ImGuiInputTextFlags_None)
{
	if (ImGui::BeginTable("table2", 2))
	{
		for (int row = 0; row < 4; row++)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text(lable[row]);
			ImGui::TableNextColumn();
			ImGui::InputText(("##" + string(lable[row])).c_str(), buf[row], NR_MAX_NUME, flags);
		}
		ImGui::EndTable();
	}
}
#pragma endregion

stringstream LoadImageIntoBuffer(const char* url);

void AppStart(GLFWwindow* _window)
{
	window = _window;
	// Incarca fonturile
	LoadFonts();
	// Contine ref la poze care trebuie descarcate de pe net
	Poza* pozeDeDescarcat[NR_MAX_POZE * NR_MAX_PENSIUNI];
	int pozeN = 0;

	// Adaugam background
	pozeDeDescarcat[pozeN++] = &background;

	// Citire din fisiere
	ifstream finS("Statiune.db");
	ifstream finP("Pensiune.db");
	char buf[1001];
	while (finS.getline(buf, 1000))
	{
		char* p = strtok(buf, " ");
		EliminaUnderscore(p);
		strcpy(statiuni[statN].numeStatiune, p);
		p = strtok(0, " ");
		EliminaUnderscore(p);
		strcpy(statiuni[statN].numeJudet, p);
		p = strtok(0, " ");
		statiuni[statN].codStatiune = atoi(p);
		statN++;
	}

	while (finP.getline(buf, 1000))
	{
		char* p = strtok(buf, " ");
		EliminaUnderscore(p);
		strcpy(pensiuni[pensN].numePensiune, p);
		p = strtok(0, " ");
		EliminaUnderscore(p);
		strcpy(pensiuni[pensN].categorie, p);
		p = strtok(0, " ");
		pensiuni[pensN].pretLocPeZi = atoi(p);
		p = strtok(0, " ");
		pensiuni[pensN].numarLocuri = atoi(p);
		p = strtok(0, " ");
		pensiuni[pensN].codPensiune = atoi(p);
		p = strtok(0, " ");
		pensiuni[pensN].codStatiune = atoi(p);
		p = strtok(0, " ");
		AdaugaLocLiberLaStatiune(pensiuni[pensN].codStatiune, pensiuni[pensN].numarLocuri);
		for (int i = 0; i < NR_MAX_POZE && p; i++)
		{
			strcpy(pensiuni[pensN].poze[i].url, p);
			pozeDeDescarcat[pozeN++] = &pensiuni[pensN].poze[i];
			p = strtok(0, " ");
		}
		pensN++;
	}

	IncarcaPozeleAsync(pozeDeDescarcat,pozeN);

	finS.close();
	finP.close();
}


// Meniuri active
bool mainWindow = 1;
bool adaugaPensiune = 0;
bool isInMenu1 = 1;
bool veziPensiuni = 0;
bool veziStatiuni = 0;
Statiune* statiuneSelectata = 0;


void AppRender()
{
	glfwGetFramebufferSize(window, &display_w, &display_h);
	/// Background


	ImGui::SetNextWindowSize({ 5000, 5000 });
	ImGui::SetNextWindowPos({ -100,-100  });
	ImGui::Begin("Background", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::SetCursorPos({ 100,100 });
	AfiseazaPoza(background, { (float)display_w, (float)display_h });
	ImGui::End();

	/// Style
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowRounding = { 10 };
	/// MainWindow
	ImGui::PushFont(fontBold24);

	CenterNextWindow(10, 10);

	ImGui::Begin("     Meniu", &mainWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize );
	ImGui::TextColored(Rgb(206, 220, 170),"Pensiunea perfecta");
	
	SetRelPos(0, 10);
	
	ImGui::PopFont();
	ImGui::PushFont(fontRegular18);
	
	auto pv1 = ImGui::GetCursorPos();

	style->FramePadding = { 4,4 };
	if (veziPensiuni)
	{
		if (ImGui::Button("Inapoi##2"))
			veziPensiuni = false, isInMenu1 = 1;
		SetRelPos(0, 20);
		for (int i = 0; i < pensN; i++)
		{
			stringstream ss;
			ss << setw(20)<<left<< pensiuni[i].numePensiune;
			ss << setw(20)<<left<< pensiuni[i].categorie;
			ss << setw(20)<<left<< "Pret: " + to_string(pensiuni[i].pretLocPeZi) + " lei";
			ss << setw(20)<<left<< "Locuri ramase: " + to_string(pensiuni[i].numarLocuri);
			ss << setw(20)<<left<< "CodPensiune: " + to_string(pensiuni[i].codPensiune);
			ss << setw(20)<<left<< "CodStatiune: " + to_string(pensiuni[i].codStatiune);
			ImGui::TextColored(FromHex("F6F5F3"), ss.str().c_str());
		}
		ImGui::PopFont();
		ImGui::End();
		return;
	}
	else
	{
		if (isInMenu1)
		{
			if (ImGui::Button("Vezi Toate Pensiunile din Tara"))
			{
				veziPensiuni = true;
				isInMenu1 = 0;
			}
			SetRelPos(0, 5);
		}
	}
	if (veziStatiuni && veziPensiuni == false)
	{
		if (statiuneSelectata == 0)
		{
			ImGui::SetCursorPos({ ImGui::GetWindowWidth() - 80, 45 });
			if (ImGui::Button("Inapoi##3"))
				veziStatiuni = false, isInMenu1 = 1;
			ImGui::SetCursorPos({ ImGui::GetWindowWidth() - 400, 45 });
			ImGui::Text("Sortare: ");
			ImGui::SameLine();
			static const char* sortOp[] = { "Nume Judet","Nume Statiune","Nr. locuri libere" };
			static const char* sortCurrent = "";
			
			ImGui::SetNextItemWidth(200);
			if (ImGui::BeginCombo("##Sortcombo", sortCurrent, ImGuiComboFlags_HeightSmall))
			{
				for (int i = 0; i < 3; i++)
				{
					bool is_selected = (sortCurrent == sortOp[i]);
					if (ImGui::Selectable(sortOp[i], is_selected,0))
					{
						sortCurrent = sortOp[i];
						SortareStatiuni((ModSortareStatiuni)i);
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::SetCursorPos(pv1);
			SetRelPos(0, 20);

			for (int i = 0; i < statN; i++)
			{
				auto pv = ImGui::GetCursorPos();
				if (ImGui::Button(statiuni[i].numeStatiune))
				{
					statiuneSelectata = &statiuni[i];
				}
				ImGui::SetCursorPos({ pv.x + display_w / 4,pv.y });
				stringstream ss;
				ss << setfill(' ') << setw(20) << left << "Locuri libere:";
				ss << setw(10) << left << to_string(statiuni[i].nrLocuriLibere);
				ss << setw(10) << left << "Judet:" << statiuni[i].numeJudet;

				ImGui::Text(ss.str().c_str());
				SetRelPos(0, 10);
			}
		}
		else {
			ImGui::SetCursorPos({ImGui::GetWindowWidth()-100, 45});
			if (ImGui::Button("Inapoi"))
				statiuneSelectata = 0;
			ImGui::SetCursorPos(pv1);
			SetRelPos(0, 20);
		}
		bool anteriorStatiune = false;
		if (statiuneSelectata != 0)
		{
			SetRelPos(0, -30);
			ImGui::PushFont(fontBold24);
			ImGui::Text("Statiunea"); ImGui::SameLine();
			ImGui::TextColored(FromHex("FBAF00"), statiuneSelectata->numeStatiune);
			ImGui::PopFont();
			SetRelPos(0, 10);
			
			auto pv = ImGui::GetCursorPos();
			ImGui::SetCursorPos({ pv.x + display_w / 3, pv.y });
			ImGui::SetCursorPos(pv);
			if (ImGui::Button("Adauga") && adaugaPensiune == false)
			{
				adaugaPensiune = 1;
			}
			if (adaugaPensiune)
			{


				static char numePens[NR_MAX_NUME]{};
				static char categorie[NR_MAX_NUME]{};
				static char pretStr[NR_MAX_NUME]{};
				static char nrLocuriStr[NR_MAX_NUME]{};
				static int nrLocuri=0, pret=0;
				static const char* statiuneCbx = 0;

				ImGui::SetNextWindowSize({ 420,250 }, ImGuiCond_FirstUseEver);
				ImGui::SetNextWindowPos({ ImGui::GetWindowWidth() / 2,ImGui::GetWindowHeight() / 2 }, ImGuiCond_FirstUseEver);

				ImGui::Begin("Adauga o noua pensiune",&adaugaPensiune, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
			
				const char* c1[] = {"Nume pensiune: ","Statiune: ","Categorie: ","Pret: ","Locuri libere: "};
				char* c2[] = {numePens,0,categorie,pretStr,nrLocuriStr};
				int flags[] = { 0,0,0,ImGuiInputTextFlags_CharsDecimal,ImGuiInputTextFlags_CharsDecimal };
				// ComboBox cu statiuni
				if (ImGui::BeginTable("table2", 2))
				{
					for (int row = 0; row < 5; row++)
					{
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text(c1[row]);
						ImGui::TableNextColumn();
						if (row == 1)
						{
							if (ImGui::BeginCombo("##combo", statiuneCbx))
							{
								for (int i = 0; i < statN; i++)
								{
									bool is_selected = (statiuneCbx == statiuni[i].numeStatiune);
									if (ImGui::Selectable(statiuni[i].numeStatiune, is_selected))
									{
										statiuneCbx = statiuni[i].numeStatiune;
									}
									if (is_selected)
									{
										ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndCombo();
							}
							else
								ImGui::SetWindowFocus();
						}
						else
							ImGui::InputText(("##" + string(c1[row])).c_str(), c2[row], NR_MAX_NUME, flags[row]);
					}
					ImGui::EndTable();
				}

				nrLocuri = atoi(nrLocuriStr);
				pret = atoi(pretStr);
				if (ImGui::Button("Gata") && statiuneCbx !=0)
				{
					adaugaPensiune = false;
					AddPensiune(numePens, categorie, pret, rand(), nrLocuri, GetCodStatiuneFromName(statiuneCbx), 0);
				}
				ImGui::End();
			}
			for (int i = 0; i < pensN; i++)
			{
				if (pensiuni[i].codStatiune == statiuneSelectata->codStatiune)
				{
					anteriorStatiune = true;
					stringstream ss;
					ss << pensiuni[i].numePensiune <<"\t" << pensiuni[i].categorie << "\t" << pensiuni[i].pretLocPeZi << " lei pe noapte\t" << "Locuri ramase: " << pensiuni[i].numarLocuri;
					ImGui::TextColored(FromHex("F6F5F3"), ss.str().c_str());
				
					AfiseazaPozele(pensiuni[i].poze, { POZA_SIZE_X,POZA_SIZE_Y });
					if (ImGui::Button(("Delete##" + to_string(i)).c_str()))
					{
						for (int j = i; j < pensN - 1; ++j)
							pensiuni[j] = pensiuni[j + 1];
						pensN--;
						ImGui::PopFont();
						ImGui::End();
						return;
					}

				}
				if (anteriorStatiune)
					SetRelPos(0, 20);
			}
		}
	}
	else
	{
		if (isInMenu1)
		{
			SetRelPos(0, 5);
			if (ImGui::Button("Vezi Statiuni") && isInMenu1)
			{
				veziStatiuni = true;
				isInMenu1 = 0;
			}
		}
	}
	ImGui::PopFont();
	ImGui::End();
}


void AppExit()
{

}
