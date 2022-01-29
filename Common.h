#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include <curl/curl.h>
#include "StbImage/stb_image.h"

#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <streambuf>
#include <sstream> 
#include <locale>
#include <iomanip>
typedef unsigned char uchar;
typedef unsigned int uint;


#define ASSERT(x) if (!(x)) assert(false)
#define gc(x) GLClearError();\
    x;\
    ASSERT(GLCheckError())

bool GLCheckError();
void GLClearError();


ImVec4 Rgb(uchar r, uchar g, uchar b);
ImVec4 RandRgb();
ImVec4 FromHex(char const* hexColor);
extern ImFont* fontThin18, * fontRegular18, * fontBold18;
extern ImFont* fontThin24, * fontRegular24, * fontBold24;
extern bool isDoneLoadingImages;
void LoadFonts();