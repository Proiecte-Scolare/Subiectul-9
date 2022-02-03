#include "Poze.h"
#include <future>
using namespace std;
bool isDoneLoadingImages = false;
size_t CallBackFunction(void* ptr, size_t size, size_t nmemb, void* userdata)
{
	stringstream* stream = (stringstream*)(userdata);
	stream->write((const char*)ptr, size * nmemb);
	return size * nmemb;
}
int imageLoadedSuccesfully = 0;
stringstream LoadImageIntoBuffer(CURL* curlCtx, const char* url)
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
#ifdef _DEBUG
		//printf("!!! Failed to download: %s\n", url);
#endif // _DEBUG
		imageLoadedSuccesfully = 0;
		return ss;
	}

	long res_code = 0;
	curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
	if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
	{


#ifdef _DEBUG
		//printf("!!! Response code: %d\n", res_code);
#endif // _DEBUG
		imageLoadedSuccesfully = 0;
		return ss;
	}
	imageLoadedSuccesfully = 1;
	return ss;
}
void LoadImageToMem(CURL* curlCtx,Poza& p, bool flip=false)
{
	auto buf = LoadImageIntoBuffer(curlCtx,p.url);
	if (imageLoadedSuccesfully ==0)
		return;
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
	if (tDownload != 0)
	{
		tDownload->join();
		delete tDownload;
		tDownload = 0;
	}
	isDoneLoadingImages = false;
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
			isDoneLoadingImages = true;
		});

}

ImgStatus AfiseazaPoza(Poza& p,const ImVec2& size)
{
	ImgStatus status = ImgStatus::PrevLoaded;
	if (strcmp(p.url, "") == 0 || p.height == 0 || p.width == 0)
		return ImgStatus::NotLoaded;
	if (p.renderId == 0)
	{
		if (p.loadStatus == LoadStatus::Downloaded)
		{
			LoadImageToGpu(p);
			p.loadStatus = LoadStatus::LoadedOnGpu;
			status = ImgStatus::NowLoaded;
		}
		else
			return ImgStatus::NotLoaded;
	}
	ImGui::Image((ImTextureID)p.renderId, size);
	return status;
}

bool AfiseazaPozele(Poza* poze, const ImVec2& size, int len)
{
	bool needsResorting = false;
	auto ip = ImGui::GetCursorPos();
	for (int i = 0; i < NR_MAX_POZE; i++)
	{
		auto pv = ImGui::GetCursorPos();
		if (AfiseazaPoza(poze[i], size) == ImgStatus::NowLoaded)
		{
			needsResorting = true;
		}

		ImGui::SetCursorPos({ pv.x + size.x + 10,pv.y });
	}
	ImGui::SetCursorPos({ ip.x,ip.y + size.y + 10 });
	return needsResorting;
}
