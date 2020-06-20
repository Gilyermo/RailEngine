#include "Timing.h"
#include <cstdint>


namespace RGE
{
namespace Timing
{

class KERNEL_API ITimer{
public:
	virtual ~ITimer(){}

	virtual void		Start() = 0;		// запустить отсчет
	virtual void		Stop() = 0;			// сбросить счетчк и поставить таймер на паузу
	virtual void		Pause() = 0;		// поставить таймер на паузу. Время не сбрасывается
	virtual void		Reset() = 0;		// сбросить счетчик таймера 
	
// замеряет время в текущий момент и возвращает количество миллисекунд от запуска таймера
	virtual uint32_t	MeasureTime() = 0;
	virtual uint32_t	GetElapsed() const = 0;

	virtual TimeStamp	GetTimeStamp() const = 0;
};

}
}