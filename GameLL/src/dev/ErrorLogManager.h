#pragma once
#include <exception>
#include <string>
#include <fstream>
#include <time.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#ifndef THROW_EXCEPTION
#define THROW_EXCEPTION(ErrorNum,ErrorDesc) throw cException(ErrorNum, ErrorDesc, __FILE__, __LINE__);
#endif
class cException : public std::exception
{
public:
	cException(int i_errorNum, std::string i_errorDesc, std::string i_srcFile, int i_lineNum);
	~cException() noexcept {};
	const char* what();
protected:
private:
	int m_errorNum;
	int m_errorLineNum;
	std::string m_errorText;
	std::string m_errorDesc;
	std::string m_errorSrcFile;

};


class ErrorLogManager {
public:
	ErrorLogManager(const ErrorLogManager&) = delete;
	static ErrorLogManager * GetInstance();
	//creating a log file
	void createFile(const std::string& i_fileName);
	//commits changes to the log file
	void Flush();
	//close the file 
	void Close();
	//logs an exception to the log file
	void LogException(cException e);
	void LogMessage(const std::string& i_message);
	//return a time in a string 
	std::string GetTimeString();

	//getters and setters 

	std::stringstream& GetLogBuffer();
protected:
	ErrorLogManager();
	virtual ~ErrorLogManager();
	static ErrorLogManager m_instance;
private:
	std::stringstream m_logBuffer;
	std::ofstream m_LogFile;
	
	
};