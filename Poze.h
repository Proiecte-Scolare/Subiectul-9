#pragma once
#include "Common.h"
#define NR_MAX_POZE 3
#define NR_MAX_URL_POZA 201
enum class LoadStatus
{
	NotProccesed,
	Downloaded,
	LoadedOnGpu
};

struct Poza
{
	int codPensiune{};
	uint renderId{};
	char url[NR_MAX_URL_POZA]{};

	LoadStatus loadStatus = LoadStatus::NotProccesed;
	uchar* localBuffer=0;
	int height{};
	int width{};
};

void IncarcaPozeleAsync(Poza** p, int len);

/// Reaturneaza 1 daca a reusit sa dea display la poza si 0 in caz contrar 
bool AfiseazaPoza(Poza& p, const ImVec2& size);

void AfiseazaPozele(Poza* poze, const ImVec2& size, int len = NR_MAX_POZE);