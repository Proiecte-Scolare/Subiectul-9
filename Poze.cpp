#include "Poze.h"
#include <future>
using namespace std;
size_t CallBackFunction(void* ptr, size_t size, size_t nmemb, void* userdata)
{
	stringstream* stream = (stringstream*)(userdata);
	stream->write((const char*)ptr, size * nmemb);
	return size * nmemb;
}
stringstream LoadImageIntoBuffer(CURL* curlCtx,const char* url)
{
	stringstream ss;
	curl_easy_setopt(curlCtx, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(curlCtx, CURLOPT_URL, url);
	curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, &ss);
	curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, CallBackFunction);
	curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

	CURLcode rc = curl_easy_perform(curlCtx);
	if (rc)
	{
		printf("!!! Failed to download: %s\n", url);
		return ss;
	}

	long res_code = 0;
	curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
	if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
	{
		printf("!!! Response code: %d\n", res_code);
		return ss;
	}
	return ss;
}
void LoadImageToMem(CURL* curlCtx,Poza& p, bool flip=false)
{
	auto buf = LoadImageIntoBuffer(curlCtx,p.url);
	if (flip)
		stbi_set_flip_vertically_on_load(1);
	int ch;
	auto str = buf.str();
	auto data = (uchar*)&str[0];
	auto m_LocalBuffer = stbi_load_from_memory(data, str.length(), &p.width, &p.height, &ch, 4);
	p.localBuffer = m_LocalBuffer;
	p.loadStatus = LoadStatus::Downloaded;
}

void LoadImageToGpu(Poza& p)
{
	gc(glGenTextures(1, &p.renderId));
	gc(glBindTexture(GL_TEXTURE_2D, p.renderId));
	gc(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));

	gc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	gc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	gc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
	gc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));

	gc(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, p.width, p.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, p.localBuffer));
	if (p.localBuffer)
	{
		stbi_image_free(p.localBuffer);
		p.localBuffer = 0;
	}
}
thread* tDownload;
void IncarcaPozeleAsync(Poza** poze,int len)
{
	Poza** c_poze = new Poza*[len];
	for (int i = 0; i < len; i++)
		c_poze[i] = poze[i];
	tDownload = new thread([c_poze,len]()
		{
			CURL* curlCtx = curl_easy_init();
			
			for (int i = 0; i < len; i++)
				LoadImageToMem(curlCtx,*c_poze[i]);

			curl_easy_cleanup(curlCtx);
			delete[] c_poze;
		});

}

bool AfiseazaPoza(Poza& p,const ImVec2& size)
{
	if (strcmp(p.url, "") == 0 || p.height == 0 || p.width == 0)
		return false;
	if (p.renderId == 0)
	{
		if (p.loadStatus == LoadStatus::Downloaded)
		{
			LoadImageToGpu(p);
			p.loadStatus = LoadStatus::LoadedOnGpu;
		}
		else
			return false;
	}
	ImGui::Image((ImTextureID)p.renderId, size);
	return true;
}

void AfiseazaPozele(Poza* poze, const ImVec2& size, int len)
{
	for (int i = 0; i < NR_MAX_POZE; i++)
	{
		AfiseazaPoza(poze[i], size);
	}
}

void AfiseazaPoze()
{

}