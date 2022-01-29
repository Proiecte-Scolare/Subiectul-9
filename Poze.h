#pragma once
#include "Common.h"
#define NR_MAX_POZE 3
#define NR_MAX_URL_POZA 201
enum class LoadStatus
{
	NotProccesed=0,
	Downloaded=1,
	LoadedOnGpu=2
};

struct Poza
{
	uint renderId{};
	char url[NR_MAX_URL_POZA]{};

	LoadStatus loadStatus = LoadStatus::NotProccesed;
	uchar* localBuffer{};
	int height{};
	int width{};
};

void IncarcaPozeleAsync(Poza** p, int len);

enum class ImgStatus
{
	NotLoaded = -1,
	PrevLoaded = 0,
	NowLoaded = 1
};
/// Returneaza un status care poate fi: imaginea nu s-a incarcat, imaginea a fost incarcata inainte ; imaginea a fost acum incarcata 
/// este folosita de AfiseazaPozele
ImgStatus AfiseazaPoza(Poza& p, const ImVec2& size);
/// Returneaza 1 daca pensiunile trebuie resortate
bool AfiseazaPozele(Poza* poze, const ImVec2& size, int len = NR_MAX_POZE);