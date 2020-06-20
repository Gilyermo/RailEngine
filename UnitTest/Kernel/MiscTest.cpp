#include "CppUnitTest.h"
#include "CppUnitTestAssert.h"
#include <Misc\FormatString.h>


namespace RGE
{
namespace UnitTest
{

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Misc;


TEST_CLASS(MiscTest){
public:
	TEST_METHOD(FormatStringTest){
		std::string str;
		char buf[120];
		int ten = 10;
		unsigned int five = 5;
		unsigned long three = 3;

		//str.resize(50);
		FormatString(buf, 120, "some numbers: ten = %i, five = %u, three = %u", ten, five, three);
		FormatString(str, "some numbers: ten = %i, five = %u, three = %u", ten, five, three);
		const char* exact = "some numbers: ten = 10, five = 5, three = 3";

		Assert::AreEqual(str.c_str(), exact, L"Strings are incorrect");
		Assert::AreEqual(buf, exact, L"Strings are incorrect");
	}
};

}
}