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

/// Reaturneaza 1 daca a reusit sa dea display la poza si 0 in caz contrar 
bool AfiseazaPoza(Poza& p, const ImVec2& size);

void AfiseazaPozele(Poza* poze, const ImVec2& size, int len = NR_MAX_POZE);