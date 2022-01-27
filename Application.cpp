#include "Common.h"
#include "Application.h"
#include <fstream>
using namespace std;
struct Pensiune
{
	char numePensiune[21];
	char categorie[21];

	int pretLocPeZi;
	int numarLocuri;

	int codPensiune;
	int codStatiune;
}pensiuni[300];

int pensN,statN;
struct Statiune
{
	char numeStatiune[21];
	char numeJudet[21];
	int codStatiune;
}statiuni[300];
void EliminaUnderscore(char * s)
{
	for (char* p = s; *p; p++)
		if (*p == '_')
			*p = ' ';
}
void AppStart(GLFWwindow* window)
{
	ifstream finS("Statiune.db");
	ifstream finP("Pensiune.db");
	char buf[301];
	while (finS.getline(buf,300))
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

	while (finP.getline(buf, 300))
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
		pensN++;
	}

	finS.close();
	finP.close();
}

ImVec4 Rgb(uchar r,uchar g,uchar b)
{
	return { 255.0f / r,255.0f / g,255.0f / b,1.0f };
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
