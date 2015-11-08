#include <Application.h>


//static variables
std::string App::nextScene = std::string("");

void TW_CALL App::Callback(void *clientData)
{
	// do something
	nextScene = static_cast<App*>(clientData)->m_appName;
}