#include "ErrorLogManager.h"

cException::cException(int i_errorNum, std::string i_errorDesc, std::string i_srcFile, int i_lineNum):
	m_errorNum(i_errorNum), m_errorDesc(i_errorDesc), m_errorSrcFile(i_srcFile), m_errorLineNum(i_lineNum)
{
	std::stringstream errorText; 
	errorText << "Exception Number: " << m_errorNum << " Description: " << m_errorDesc << " Source File: "
		<< m_errorSrcFile << " Line: " << m_errorLineNum;
	m_errorText = errorText.str();
}
const char* cException::what() {
	return m_errorText.c_str();
}



ErrorLogManager ErrorLogManager::m_instance;
ErrorLogManager::ErrorLogManager(){}
ErrorLogManager::~ErrorLogManager(){}

ErrorLogManager* ErrorLogManager::GetInstance() { return &m_instance; }


//creates the file 
void ErrorLogManager::createFile(const std::string& i_fileName) {
	m_LogFile.open(i_fileName.c_str());
}
//applies the changes
void ErrorLogManager::Flush() {
	m_LogFile << m_logBuffer.str();
	m_LogFile.flush();
	m_logBuffer.str("");
}
//close the file
void ErrorLogManager::Close() {
	m_LogFile.close();
}
//logs the changes 
void ErrorLogManager::LogException(cException e) {
	m_logBuffer << GetTimeString() << "\n" << e.what();
	Flush();
}
//return time in form hours:mins:secs
std::string ErrorLogManager::GetTimeString() {
	std::stringstream TimeStr;
	tm pTime;
	time_t ctTime; time(&ctTime);
	localtime_s(&pTime,&ctTime);

	TimeStr << std::setw(2) << std::setfill('0') << pTime.tm_hour << ":";
	TimeStr << std::setw(2) << std::setfill('0') << pTime.tm_min << ":";
	TimeStr << std::setw(2) << std::setfill('0') << pTime.tm_sec;
	return TimeStr.str();
}

///getters and setters 

std::stringstream& ErrorLogManager::GetLogBuffer() { return m_logBuffer; }