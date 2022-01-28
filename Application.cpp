#include "Application.h"
#include "Common.h"

#define NR_MAX_STATIUNI 300
#define NR_MAX_PENSIUNI 300
using namespace std;

struct Pensiune
{
	char numePensiune[21];
	char categorie[21];

	int pretLocPeZi;
	int numarLocuri;

	int codPensiune;
	int codStatiune;

	Poza poze[NR_MAX_POZE]{};
}pensiuni[NR_MAX_PENSIUNI];

int pensN,statN;
struct Statiune
{
	char numeStatiune[21];
	char numeJudet[21];
	int codStatiune;
}statiuni[NR_MAX_STATIUNI];

void EliminaUnderscore(char * s)
{
	if (s!=0)
	for (char* p = s; *p; p++)
		if (*p == '_')
		
			*p = ' ';
}

stringstream LoadImageIntoBuffer(const char* url);

//Poza p = {321,0,"https://hips.hearstapps.com/hmg-prod.s3.amazonaws.com/images/beach-quotes-1559667853.jpg"};


void AppStart(GLFWwindow* window)
{
	Poza* pozeDeDescarcat[NR_MAX_POZE * NR_MAX_PENSIUNI];
	int pozeN = 0;

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
		for (int i = 0; i < NR_MAX_POZE && p; i++)
		{
			strcpy(pensiuni[pensN].poze[i].url, p);
			pozeDeDescarcat[pozeN++] = &pensiuni[pensN].poze[i];
			p = strtok(0, " ");
		}
		IncarcaPozeleAsync(pozeDeDescarcat,pozeN);
		pensN++;
	}

	finS.close();
	finP.close();
}


// Meniuri active
bool mainWindow = 1;
bool listaStatiuni = 0;
Statiune* statiuneSelectata = 0;

void AppRender()
{
	ImGui::Begin("Meniu", &mainWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::TextColored(Rgb(206, 220, 170),"Pensiunea perfecta");
	if (listaStatiuni)
	{
		if(statiuneSelectata ==0)
			for (int i = 0; i < statN; i++)
			{
				if (ImGui::Button(statiuni[i].numeStatiune))
				{
					statiuneSelectata = &statiuni[i];
				}
			}
		else 
			if (ImGui::Button("Inapoi"))
			{
				statiuneSelectata = 0;
			}
		if(statiuneSelectata != 0)
			for (int i = 0; i < pensN; i++)
			{
				if (pensiuni[i].codStatiune == statiuneSelectata->codStatiune)
				{
					ImGui::Text(pensiuni[i].numePensiune);
					AfiseazaPozele(pensiuni[i].poze,{100,100});
				}
			}
	}
	else if (ImGui::Button("##Statiuni"));
	{
		listaStatiuni = true;
	}

	
	ImGui::End();

}


void AppExit()
{

}
