#include "Exception.h"
#include <sstream>


namespace RGE
{
namespace Exception
{

BaseException::BaseException(const std::string& type, const std::string& desc, const std::string& file, uint32_t line){
	m_type		=	type;
	m_desc		=	desc;
	m_file		=	file;
	m_line		=	line;
}

BaseException::BaseException(const BaseException& e){
	m_type		=	e.m_type;
	m_desc		=	e.m_desc;
	m_file		=	e.m_file;
	m_line		=	e.m_line;
}


std::string& BaseException::GetDescription(){
	return m_desc;
}

const std::string& BaseException::GetDescription() const{
	return m_desc;
}

const std::string& BaseException::GetType() const{
	return m_type;
}

const std::string& BaseException::GetFile() const{
	return m_file;
}

const uint32_t BaseException::GetLine() const{
	return m_line;
}


const char* BaseException::What() const throw(){
	if(m_what.empty()){
		std::ostringstream msg;
		msg << m_type << " exception was thrown from " 
			<< m_file << " (" << m_line << "). " << m_desc;
		m_what = msg.str();
	}
	return m_what.c_str();
}

const char* BaseException::what() const throw(){
	return What();
}


BaseException& BaseException::operator=(const BaseException& e) throw(){
	if(this != &e){
		m_desc		=	e.m_desc;
		m_type		=	e.m_type;
		m_file		=	e.m_file;
		m_line		=	e.m_line;
	}
	return *this;
}

}
}