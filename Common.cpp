#include "Common.h"
ImFont* fontThin18{}, * fontRegular18{}, * fontBold18{};
ImFont* fontThin24{}, * fontRegular24{}, * fontBold24{};
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLCheckError()
{
	while (GLenum error = glGetError())
	{

		std::cout << "[OpenGL Error] ";
		switch (error) {
		case GL_INVALID_ENUM:
			std::cout << "GL_INVALID_ENUM : An unacceptable value is specified for an enumerated argument.";
			break;
		case GL_INVALID_VALUE:
			std::cout << "GL_INVALID_OPERATION : A numeric argument is out of range.";
			break;
		case GL_INVALID_OPERATION:
			std::cout << "GL_INVALID_OPERATION : The specified operation is not allowed in the current state.";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION : The framebuffer object is not complete.";
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "GL_OUT_OF_MEMORY : There is not enough memory left to execute the command.";
			break;
		case GL_STACK_UNDERFLOW:
			std::cout << "GL_STACK_UNDERFLOW : An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			std::cout << "GL_STACK_OVERFLOW : An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		default:
			std::cout << "Unrecognized error" << error;
		}
		std::cout << std::endl;
		return false;
	}
	return true;
}

ImVec4 Rgb(uchar r, uchar g, uchar b)
{
	return { r/255.0f ,g/255.0f,b/255.0f,1.0f };
}
ImVec4 FromHex(char const* hexColor)
{
	int r, g, b;
	if (*hexColor == '#')
		hexColor++;
	sscanf(hexColor, "%02x%02x%02x", &r, &g, &b);
	return { (float)r / 255,(float)g / 255,(float)b / 255,1 };
}
void LoadFonts()
{	// Setup fiont 
	ImGuiIO& io = ImGui::GetIO();

	fontThin18 = io.Fonts->AddFontFromFileTTF("Fonts/Roboto-Thin.ttf", 18);
	fontRegular18 = io.Fonts->AddFontFromFileTTF("Fonts/Roboto-Medium.ttf", 18);
	fontBold18 = io.Fonts->AddFontFromFileTTF("Fonts/Roboto-Bold.ttf", 18);

	fontThin24 = io.Fonts->AddFontFromFileTTF("Fonts/Roboto-Thin.ttf", 24);
	fontRegular24 = io.Fonts->AddFontFromFileTTF("Fonts/Roboto-Medium.ttf", 24);
	fontBold24 = io.Fonts->AddFontFromFileTTF("Fonts/Roboto-Bold.ttf", 24);
}

