#include "CppUnitTest.h"
#include "CppUnitTestAssert.h"
#include <Multithreading\Task.h>
#include <Multithreading\Thread.h>
#include <Multithreading\ThreadManager.h>
#include <Multithreading\List.h>
#include <Exception\Exception.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <Windows.h>
#include <vector>


namespace RGE
{
namespace UnitTest
{

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Multithreading;

#undef max
#undef min

static SafeContainer<std::list<int>>	m_intVals;
static std::vector<int>					m_writerLog;

TEST_CLASS(MultithreadingTest){
public:
#pragma region TaskTest
	static void TaskFunction1(){
		printf("TestFunction1\n");
		throw "try exception in first function";
	}

	static int& TaskFunction2(int arg){
		static int ret = 10;
		ret += arg;
		printf("TestFunction2, arg = %i\n", arg);
		return ret;
	}

	static int TaskFunction3(int arg){
		if(arg<10){
			printf("TestFunction3 with good argument\n");
			return arg;
		}

		printf("TestFunction3 with bad argument. Exception\n");
		throw "bad arg in TestFunction3";
	}

	static void MultitaskFunction1(int rank, int size){
		printf("SubTask %i / %i\n", rank + 1, size);
	}

	TEST_METHOD(TaskTest){
		Task<void>		t1(TaskFunction1);
		Task<int&>		t2(std::bind(TaskFunction2, 2));
		Task<int>		t3(std::bind(TaskFunction3, 150));
		Multitask<void>	mt1(MultitaskFunction1, 3);

		auto subtask1 = mt1.GetSubtask(0);
		auto subtask2 = mt1.GetSubtask(1);
		auto subtask3 = mt1.GetSubtask(2);

		std::thread th([&]()
		{
			try{
				printf("th is started\n");
				t2.Perform();
				subtask1->Perform();
				subtask2->Perform();
				t1.GetResult();
			}
			catch(const char* e){
				printf("the exception thrown: %s\n", e);
			}
		});

		t2.Perform();	t3.Perform();
		try{
			printf("t2 result: %i\n", t2.GetResult());
			printf("t3 result: %i\n", t3.GetResult());
		}
		catch(const char* e){
			printf("the exception thrown: %s\n", e);
		}

		t1.Perform();
		th.join();

		subtask3->Perform();

		for(int i = 0; i<mt1.GetTaskCount(); i++)
			mt1.GetSubtask(i)->Wait();
	}
#pragma endregion

#pragma region ThreadTest
	static void SleepFor(int sec){
		std::mutex mutex;
		std::unique_lock<std::mutex> lock(mutex);
		std::condition_variable cv;
		cv.wait_for(lock, std::chrono::seconds(sec));
	}
	
	static void f1(){
		printf("f1 begin by %i. Sleep 1 sec\n", std::hash<std::thread::id>()(std::this_thread::get_id()));
		SleepFor(1);
	}

	static void f2(){
		printf("f2 begin by %i. Sleep 1 sec\n", std::hash<std::thread::id>()(std::this_thread::get_id()));
		SleepFor(1);
	}

	static void f3(){
		printf("f3 begin by %i. Sleep 1 sec\n", std::hash<std::thread::id>()(std::this_thread::get_id()));
		SleepFor(1);
	}

	TEST_METHOD(ThreadTest){
		static Thread th1;
		Thread th2;

		th1.Suspend();		th2.Suspend();

		th1.Perform(ITaskPtr(new Task<void>(f1)));	th2.Perform(ITaskPtr(new Task<void>(f2)));
		th1.Perform(ITaskPtr(new Task<void>(f3)));	th2.Perform(ITaskPtr(new Task<void>(f1)));
		th1.Perform(ITaskPtr(new Task<void>(f2)));	th2.Perform(ITaskPtr(new Task<void>(f3)));
		th1.Perform(ITaskPtr(new Task<void>(f1)));	th2.Perform(ITaskPtr(new Task<void>(f2)));
		th1.Perform(ITaskPtr(new Task<void>(f3)));	th2.Perform(ITaskPtr(new Task<void>(f1)));
		th1.Perform(ITaskPtr(new Task<void>(f2)));	th2.Perform(ITaskPtr(new Task<void>(f3)));

		printf("Threads resuming and sleep for 5 sec\n");
		th1.Resume();		th2.Resume();
		SleepFor(5);
		printf("Thread1 task count = %i\n", th1.GetTaskCount());
		printf("Thread2 task count = %i\n", th2.GetTaskCount());
		printf("Get th1 last task...");

		ITaskPtr task;
		if(th1.TryGetLastTask(task)){
			printf("OK\n");
			th2.Perform(task);
		}
		else
			printf("FAIL\n");
		printf("Suspending Threads and Sleep for 4 sec\n");
		th1.Suspend();		th2.Suspend();
		SleepFor(2);

		printf("Thread1 task count = %i, it is free(%i) and active(%i)\n", th1.GetTaskCount(), th1.IsFree(), th1.IsActive());
		printf("Thread2 task count = %i, it is free(%i) and active(%i)\n", th2.GetTaskCount(), th2.IsFree(), th2.IsActive());
		printf("Trying to steal task from th2 and give it to th1\n");
		if(th2.TryGetLastTask(task)){
			printf("OK\n");
			th1.Perform(task);
		}
		else
			printf("FAIL\n");

		printf("Resuming Threads\n");
		th1.Resume();		th2.Resume();


		if(!th1.IsFree()){
			printf("th1 isn't free. it has %i tasks. Sleep for 3 sec\n", th1.GetTaskCount());
			SleepFor(3);
			if(th1.IsFree())
				printf("th1 now is free\n");
			else
				printf("th1 isn't free yet\n");
		}
		else{
			printf("th1 is free. it has %i tasks\n", th1.GetTaskCount());
		}

		printf("th1 will be joined now\n");
		th1.Join();
		printf("th2 will be detached\n");
		th2.Detach();
		printf("IsAlive(%i, %i), IsFree(%i, %i), TaskCount(%i, %i)\n", th1.IsAlive(), th2.IsAlive(),
			th1.IsFree(), th2.IsFree(),
			th1.GetTaskCount(), th2.GetTaskCount());
	}
#pragma endregion

#pragma region ThreadManagerTest
	static void TM_f1(int){
		while(true){
			int i = rand() % 20000;
			if(i == 7)
				return;
		}
	}

	static void TM_f2(int _arg){
		std::list<double> doubleList;
		for(int i = 0; i<800; i++)
			doubleList.push_back(1.245*rand());
		doubleList.sort();
	}

	static void TM_f3(int){
		static std::atomic_int indx;

		std::string fname = "tmpFile";
		fname += std::to_string(std::max(1, indx.fetch_add(1)));
		fname += ".tmp";

		std::ofstream outFile(fname);
		outFile << "This temporary file was created for testing the ThreadManager class." << std::endl;
		outFile << "On this stage we try to simulate realistic load with input/output operations." << std::endl;
		outFile << "The file will be read and destroyed";
		outFile.close();

		std::ifstream inFile(fname);
		char lexeme[5];
		while(inFile.good()){
			inFile.get(lexeme, 5);
			memset(lexeme, 0, 5 * sizeof(char));
		}
		inFile.close();

		remove(fname.c_str());
	}

	static int TM_factorialPlus(int arg){
		if(arg <= 1) return 1;
		int* newArg = new int(arg - 1);
		int res = arg * TM_factorialPlus(*newArg);
		delete newArg;
		return res;
	}

	static void TM_f4(int arg){
		int res = TM_factorialPlus(arg);
	}

	
	static double DotKernel(std::vector<double>& v1, std::vector<double>& v2, int rank, int size){
		size_t n = v1.size();
		size_t loc_n = n / size;
		size_t shift = rank * loc_n;
		size_t elem_num = std::min(n - shift, loc_n);

		printf("Thread %i/%i begin: n(%i), loc_n(%i), shift(%i), elem_num(%i)\n", rank, size, n, loc_n, shift, elem_num);

		register double res = 0.0;
		for(size_t i = shift, elem = 0; elem<elem_num; i++, elem++)
			res += v1[i] * v2[i];

		printf("Thread %i/%i end\n", rank, size);
		return res;
	}

	static double Dot(std::vector<double>& v1, std::vector<double>& v2){
		uint8_t threadNum = ThreadManager::Instance().GetWorkersCount();

		auto dotTask = ThreadManager::Instance().Execute<double>([&](int rank, int size){return DotKernel(v1, v2, rank, size);}, threadNum);
		double res = 0.0;
		for(int i = 0; i<dotTask.GetTaskCount(); i++)
			res += dotTask.GetSubtask(i).GetResult();
		return res;
	}


	static void TM_WaitSignal(uint8_t* canProceed){
		std::condition_variable var;
		std::mutex m;

		std::unique_lock<std::mutex> lck(m);
		while((*canProceed) == 0)
			var.wait_for(lck, std::chrono::milliseconds(10));
	}


	void TM_ThreadsManipulation(){
		const ThreadManager& man = ThreadManager::Instance();

		uint8_t workersCount = ThreadManager::Instance().GetWorkersCount();
		uint8_t newWorkersCount = workersCount / 2;
		uint8_t f_num = 2 * workersCount;

		std::cout << "Total thread count in begining = " << (int)ThreadManager::Instance().GetThreadsCount()
			<< ": workers(" << (int)ThreadManager::Instance().GetWorkersCount()
			<< "), lent(" << (int)ThreadManager::Instance().GetLentThreadsCount()
			<< "), condemned(" << (int)ThreadManager::Instance().GetCondemnedThreadsCount() << ")\n";

		std::vector<uint8_t> canProceed(f_num, 0);
		for(uint8_t i = 0; i<f_num; i++)
			ThreadManager::Instance().Execute<void>(std::bind(TM_WaitSignal, &canProceed[i]));

		ThreadManager::Instance().SetWorkersCount(newWorkersCount);
		if(newWorkersCount != ThreadManager::Instance().GetWorkersCount())
			std::cout << "Failed on ThreadDecreasing\n";

		std::cout << "Total thread count after resizing = " << (int)ThreadManager::Instance().GetThreadsCount()
			<< ": workers(" << (int)ThreadManager::Instance().GetWorkersCount()
			<< "), lent(" << (int)ThreadManager::Instance().GetLentThreadsCount()
			<< "), condemned(" << (int)ThreadManager::Instance().GetCondemnedThreadsCount() << ")\n";

		ThreadPtr myThread = ThreadManager::Instance().LendThread();
		size_t i;
		for(i = 0; i<canProceed.size() && !myThread->IsFree(); i++){
			canProceed[i] = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}

		std::cout << "Total thread count after lending = " << (int)ThreadManager::Instance().GetThreadsCount()
			<< ": workers(" << (int)ThreadManager::Instance().GetWorkersCount()
			<< "), lent(" << (int)ThreadManager::Instance().GetLentThreadsCount()
			<< "), condemned(" << (int)ThreadManager::Instance().GetCondemnedThreadsCount() << ")\n";

		myThread->Execute([&](){for(; i<canProceed.size(); i++) canProceed[i] = true;});
		ThreadManager::Instance().ReturnThread(myThread);

		ThreadPtr falseThread;
		ThreadManager::Instance().ReturnThread(falseThread);

		ThreadManager::Instance().WaitAllTasksCompleted();

		std::cout << "Total thread count in the end = " << (int)ThreadManager::Instance().GetThreadsCount()
			<< ": workers(" << (int)ThreadManager::Instance().GetWorkersCount()
			<< "), lent(" << (int)ThreadManager::Instance().GetLentThreadsCount()
			<< "), condemned(" << (int)ThreadManager::Instance().GetCondemnedThreadsCount() << ")\n";
	}

	TEST_METHOD(ThreadManagerTest){
		std::function<void(int)> funcs[4] = {TM_f1, TM_f2, TM_f3, TM_f4};
		TaskPriority prioritys[] = {TP_MINIMAL, TP_NORMAL, TP_MAXIMAL};

		printf("====== ThreadManager Test Started ======\n");

		ThreadManager::Instance().SetWorkersCount(10);
		//ThreadManager::Instance().Execute<void>([](int rank, int){SetThreadAffinityMask(GetCurrentThread(), 1<<2*rank);}, 4);

		auto start = clock();

		for(int cycle = 0; cycle<250; cycle++){
			for(int i = 0; i<36; i++){
				ThreadManager::Instance().Execute<void>(std::bind(funcs[i % 4], rand() % 500), prioritys[i % 3]);
				//funcs[i%4](rand()%500);
			}
			printf("cycle %i\r", cycle);
			/*if(cycle%10 == 0){
			ThreadManager::Instance().WaitAllTasksCompleted();
			}*/
		}
		printf("\n");

		ThreadManager::Instance().WaitAllTasksCompleted();
		printf("All tasks performed\n");

		auto stop = clock();
		printf("time = %f\n\n", float(stop - start) / CLOCKS_PER_SEC);

		/*	std::vector<double> v(100, 1.0);
		double v_norm = Dot(v, v);
		printf("v_norm = %lf\n", v_norm);*/

		TM_ThreadsManipulation();

		printf("====== ThreadManager Test Finished ======\n\n");
	}
#pragma endregion

#pragma region RWLockTest
	static void WriterFunction(int rank, int size){
		int count = 100000;
		for(volatile int i = 0; i<count; i++){
			m_intVals.push_back(rank);
			m_writerLog[rank]++;

			int num = rand();
			if(num & 0x1101 && m_intVals.try_erase(rank))
				m_writerLog[rank]--;
			//std::this_thread::sleep_for( std::chrono::milliseconds(num % 5) );
		}
	}

	static void ReaderFunction(int rank, int size){
		const auto& constIntVals = m_intVals;
		volatile uint32_t sum = 100;

		int count = 500000;
		for(volatile int i = 0; i<count; i++){
			if(!constIntVals.empty()){
				if(constIntVals.size() & rank)
					sum += 5 * (constIntVals.back() + constIntVals.front());
				else
					sum -= 10 * constIntVals.back();
			}
			//std::this_thread::sleep_for( std::chrono::milliseconds(sum % 5) );
		}
	}

	TEST_METHOD(RWLockTest){
		uint8_t writerCount = 5;
		uint8_t readerCount = 5;
		ThreadManager::Instance().SetWorkersCount(writerCount + readerCount);

		m_writerLog.resize(writerCount);

		auto start = clock();
		auto wTask = ThreadManager::Instance().Execute<void>(WriterFunction, writerCount);
		auto rTask = ThreadManager::Instance().Execute<void>(ReaderFunction, readerCount);
		wTask.Wait();	rTask.Wait();
		auto stop = clock();

		printf("time = %i\n", static_cast<int>(stop - start));

		std::vector<int> writerRrealLog(writerCount);
		for(auto val : m_intVals.get_unsafe()){
			RGE_Assert(val < writerCount, Exception::TestFailed, "RWLockTest Failed");
			writerRrealLog[val]++;
		}

		for(int i = 0; i<writerCount; i++)
			RGE_Assert(writerRrealLog[i] == m_writerLog[i], Exception::TestFailed, "RWLockTest Failed");
	}
#pragma endregion

//TODO: решить проблему с компиляцией List<uint32_t>
#if 0 
// проверка правильности доступа к концам списка
	TEST_METHOD(SafeList_PushPopTest){
		//SafeContainer< std::list<uint64_t> > list;
		List<uint32_t>						list;
		std::vector<int>					pushCounter;
		SafeContainer< std::vector<int>	>	popCounter;

		int elemCount = 5000;
		uint8_t workersPerAction = 1;
		ThreadManager::Instance().SetWorkersCount(4 * workersPerAction);
		pushCounter.resize(2 * workersPerAction);
		popCounter.resize(2 * workersPerAction);

		//Logging::LogManager::Instance().DisableMessageType(Logging::MT_Debug);

		// push_front
		auto pushFrontTask = ThreadManager::Instance().Execute<void>([&](int rank, int){
			int globalRank = rank;
			for(int i = 0; i<elemCount; i++){
				list.push_front(globalRank);
				pushCounter[globalRank]++;
				std::this_thread::sleep_for(std::chrono::milliseconds(2));
			}
		}, workersPerAction);

		// push_back
		auto pushBackTask = ThreadManager::Instance().Execute<void>([&](int rank, int){
			/*int globalRank = rank + workersPerAction;
			for(int i=0; i<elemCount; i++){
			list.push_back(globalRank);
			pushCounter[globalRank]++;
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
			}*/
		}, workersPerAction);

		// pop_front
		auto popFrontTask = ThreadManager::Instance().Execute<void>([&](int, int){
			for(int i = 0; i<elemCount; i++){
				uint32_t val;
				while(!list.pop_front(&val))
					std::this_thread::sleep_for(std::chrono::milliseconds(2));
				popCounter[val]++;
				std::this_thread::sleep_for(std::chrono::milliseconds(2));
			}
		}, workersPerAction);

		// pop_back
		auto popBackTask = ThreadManager::Instance().Execute<void>([&](int, int){
			/*for(int i=0; i<elemCount; i++){
			uint32_t val;
			while(!list.pop_back(&val))
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
			popCounter[val]++;
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
			}*/
		}, workersPerAction);

		while(pushBackTask.GetTaskState() < TS_READY ||
			pushFrontTask.GetTaskState()< TS_READY ||
			popBackTask.GetTaskState()  < TS_READY ||
			popFrontTask.GetTaskState() < TS_READY)
		{
			for(int i = 0; i<2 * workersPerAction; i++){
				if(pushCounter[i] == popCounter[i])
					int a = 5;
				printf("%6i / %6i  ", pushCounter[i], popCounter[i]);
			}
			printf("\r");
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		for(int i = 0; i<2 * workersPerAction; i++)
			printf("%6i / %6i  ", pushCounter[i], popCounter[i]);
		printf("\n");

		try{
			for(int i = 0; i<workersPerAction; i++){
				pushBackTask.GetSubtask(i).GetResult();
				pushFrontTask.GetSubtask(i).GetResult();
				popBackTask.GetSubtask(i).GetResult();
				popFrontTask.GetSubtask(i).GetResult();
			}
		}
		catch(std::exception& e){
			printf("%s\n", e.what());
		}

		RGE_Assert(list.empty(), Exception::TestFailed, "List is not empty");
		for(int i = 0; i<workersPerAction; i++){
			RGE_Assert(pushCounter[i] == popCounter[i], Exception::TestFailed, "Not all values was processed correctly");
		}
	}

#endif
};


}
}