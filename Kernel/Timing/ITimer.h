#include "Timing.h"
#include <cstdint>


namespace RGE
{
namespace Timing
{

class KERNEL_API ITimer{
public:
	virtual ~ITimer(){}

	virtual void		Start() = 0;		// ��������� ������
	virtual void		Stop() = 0;			// �������� ������ � ��������� ������ �� �����
	virtual void		Pause() = 0;		// ��������� ������ �� �����. ����� �� ������������
	virtual void		Reset() = 0;		// �������� ������� ������� 
	
// �������� ����� � ������� ������ � ���������� ���������� ����������� �� ������� �������
	virtual uint32_t	MeasureTime() = 0;
	virtual uint32_t	GetElapsed() const = 0;

	virtual TimeStamp	GetTimeStamp() const = 0;
};

}
}