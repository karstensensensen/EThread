#include <ETH.h>
#include <iostream>
#include <chrono>
#include <array>
#include <functional>
#include <string>
#include <Windows.h>
#include <cassert>

using namespace ETH;
using namespace std::chrono_literals;

#define RUN_TEST(test_name) { std::cout << "running test: [" << #test_name << "]\n"; \
auto[category, err_msg] = test_name(); \
std::cout << "result: " << category << '\n'; \
if(category != "SUCCESS") std::cout << "error message: " << err_msg << '\n';}
#define REQUIRE(value, condition, category, error_message) if(!(condition)) { std::string msg = std::string(error_message) + "\nvalue: " + std::to_string(value); return {category, msg}; }

#define SUCCESS {"SUCCESS", ""}

struct tEThrdHelper
{
	static int a;

	void hmeth()
	{
		a++;
	}
};

int tEThrdHelper::a = 0;

std::pair<std::string, std::string> testEThread()
{

	tEThrdHelper strcA;
	tEThrdHelper strcB;

	EThread threadA(&tEThrdHelper::hmeth, &strcA);
	EThread threadB(&tEThrdHelper::hmeth, &strcB);

	REQUIRE(tEThrdHelper::a, tEThrdHelper::a == 0, "THREAD METHOD START ERROR", "Expected static variable to be unmodified before thread was started");

	threadA.start();

	threadA.join();

	REQUIRE(tEThrdHelper::a, tEThrdHelper::a == 1, "THREAD METHOD START ERROR", "Expected static variable to be modified only once after thread join");

	threadB.start();

	threadB.join();

	REQUIRE(tEThrdHelper::a, tEThrdHelper::a == 2, "THREAD METHOD START ERROR", "Expected static variable to be modified only once after thread join");
	
	threadA.start();
	threadB.start();

	threadA.join();
	threadB.join();

	REQUIRE(tEThrdHelper::a, tEThrdHelper::a == 4, "THREAD METHOD START ERROR", "Expected static variable to be modified twice after thread join");

	bool was_called = false;

	EThread thread(std::function([&was_called]() { was_called = true; }));

	REQUIRE(was_called, !was_called, "THREAD START ERROR", "Thread was started without calling start()");

	thread.start();
	thread.join();

	REQUIRE(was_called, was_called, "THREAD JOIN ERROR", "Thread was never started or joined");


	return SUCCESS;
}

struct tLThrdHelper
{
	std::atomic<int> calls = 0;
	void incrCall() { calls++; }
};

std::pair<std::string, std::string> testLThread() 
{
	tLThrdHelper helper;

	EThread t(&tLThrdHelper::incrCall, &helper);
	LThread lthread(&tLThrdHelper::incrCall, &helper);

	lthread.start();

	size_t calls = 0;

	REQUIRE(helper.calls, helper.calls == calls, "LOOP THREAD START ERROR", "Thread loop was started before startLoop() was called");

	lthread.startLoop();
	calls++;

	lthread.joinLoop();

	REQUIRE(helper.calls, helper.calls == calls, "LOOP THREAD LOOP ERROR", "Thread loop was not started on startLoop() call");

	lthread.startLoop();
	calls++;

	for (size_t i = 0; i < 9999; i++)
	{
		lthread.restartLoop();
		calls++;
	}

	lthread.joinLoop();

	REQUIRE(helper.calls, helper.calls == calls, "LOOP THREAD RESTART ERROR", "Thread loop was not restarted correct amount on multiple restartLoop() calls");

	lthread.stop();

	REQUIRE(helper.calls, helper.calls == calls, "LOOP THREAD STOP ERROR", "Thread loop was started on stop() call");

	return SUCCESS;
}

int main()
{
	RUN_TEST(testEThread);
	RUN_TEST(testLThread);
}
