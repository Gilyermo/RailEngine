/****************************************
*	базовый класс для описания задания	*
****************************************/
#pragma once

#include "Multithreading.h"
#include <memory>


namespace RGE
{
namespace Multithreading
{

enum TaskState{
	TS_QUEUED = 0,
	TS_PROCESSING,
	TS_READY,
	TS_DISCARDED,
	TS_COUNT
};

enum TaskPriority{
	TP_MINIMAL = 0,
	TP_NORMAL,
	TP_MAXIMAL,
	TP_COUNT
};

class ITask{
public:
	virtual ~ITask(){}
	
	virtual void			Perform() = 0;
	virtual void			Wait() const = 0;
	virtual TaskState		GetState() const = 0;
	virtual void			SetState(TaskState state) = 0;
	virtual TaskPriority	GetPriority() const = 0;
	virtual void			SetPriority(TaskPriority priority) = 0;
};

typedef std::shared_ptr<ITask> ITaskPtr;

}
}