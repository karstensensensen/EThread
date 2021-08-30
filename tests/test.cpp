#include <ETH.h>
#include <iostream>
#include <chrono>
#include <array>
#include <functional>

using namespace ETH;
using namespace std::chrono_literals;

#define RUN_TEST(test_name) { std::cout << "running test: [" << #test_name << "]\n"; \
auto[category, err_msg] = test_name(); \
std::cout << "result: " << category << '\n'; \
if(category != "SUCCESS") std::cout << "error message: " << err_msg << '\n';}
#define REQUIRE(condition, category, error_message) if(!(condition)) { return {category, error_message}; }

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

	REQUIRE(strcA.a == 0 && strcB.a == 0, "THREAD METHOD START ERROR", "Expected static variable to be unmodified before thread was started");

	threadA.start();

	threadA.join();

	REQUIRE(strcA.a == 1 && strcB.a == 1, "THREAD METHOD START ERROR", "Expected static variable to be modified only once after thread join");

	threadB.start();

	threadB.join();

	REQUIRE(strcA.a == 2 && strcB.a == 2, "THREAD METHOD START ERROR", "Expected static variable to be modified only once after thread join");
	
	threadA.start();
	threadB.start();

	threadA.join();
	threadB.join();

	REQUIRE(strcA.a == 4 && strcB.a == 4, "THREAD METHOD START ERROR", "Expected static variable to be modified twice after thread join");

	bool was_called = false;

	EThread thread(std::function([&was_called]() { was_called = true; }));

	REQUIRE(!was_called, "THREAD START ERROR", "Thread was started without calling start()");

	thread.start();
	thread.join();

	REQUIRE(was_called, "THREAD JOIN ERROR", "Thread was never started or joined");


	return SUCCESS;
}

std::pair<std::string, std::string> testLThread() 
{
	int call_count = 0;
	std::function call_counter([&call_count]() { call_count++; });

	LThread lthread(call_counter);

	lthread.start();

	REQUIRE(call_count == 0, "LOOP THREAD START ERROR", "Thread loop was started before startLoop() was called");

	lthread.startLoop();

	lthread.joinLoop();

	REQUIRE(call_count == 1, "LOOP THREAD LOOP ERROR", "Thread loop was not started on startLoop() call");

	lthread.startLoop();

	for (size_t i = 0; i < 99; i++)
	{
		lthread.restartLoop();
	}

	lthread.joinLoop();

	REQUIRE(call_count == 101, "LOOP THREAD RESTART ERROR", "Thread loop was not restarted correct amount on multiple restartLoop() calls");

	lthread.stop();

	REQUIRE(call_count == 101, "LOOP THREAD STOP ERROR", "Thread loop was started on stop() call");

	return SUCCESS;
}

int main()
{
	RUN_TEST(testEThread);
	RUN_TEST(testLThread);
}
