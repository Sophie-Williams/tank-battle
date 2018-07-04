#include "stdafx.h"
#include <Windows.h>

#include "CppUnitTest.h"
#include <thread>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;

namespace UnitTest
{		
	TEST_CLASS(StuffUnitTest)
	{
	public:

		static void loop() {
			int n = 100;
			while (n > 0)
			{
				this_thread::sleep_for(10ms);
				n--;
			}
		}

		static bool stopAndWait(thread& worker, int milisecond) {
			HANDLE hThread = (HANDLE)worker.native_handle();
			DWORD waitRes = WaitForSingleObject(hThread, (DWORD)milisecond);

			if (waitRes == WAIT_OBJECT_0) {
				worker.join();
				return true;
			}
			if (waitRes == WAIT_TIMEOUT) {
				bool res = TerminateThread(hThread, 0) != FALSE;
				worker.join();
				return res;
			}

			return false;
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// test terminate thread
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(WaitZero)
		{
			thread worker(loop);
			bool res = stopAndWait(worker, 0);
			Assert::IsTrue(res);
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// wait a mount of time that less than loop execution time
		/// but greater than zero
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(Wait100)
		{
			thread worker(loop);
			bool res = stopAndWait(worker, 100);
			Assert::IsTrue(res);
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// wait a mount of time that less than loop execution time
		/// but greater than zero
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(Wait2000)
		{
			thread worker(loop);
			bool res = stopAndWait(worker, 2000);
			Assert::IsTrue(res);
		}
	};
}