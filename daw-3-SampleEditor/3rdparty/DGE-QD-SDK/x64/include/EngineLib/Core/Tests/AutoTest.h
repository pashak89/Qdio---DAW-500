/* Copyright (C) 2025, Abbas Aliakbari - All Rights Reserved
 * SPDX-License-Identifier: MIT
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: a.aliakbari91@gmail.com
 *
 * Written by Abbas Aliakbari
 *
 * @section DESCRIPTION
 *
 * Test utility file for using in tests project of the engine projects
 */
/* Copyright (C) 2025, Abbas Aliakbari - All Rights Reserved
 * SPDX-License-Identifier: MIT
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: a.aliakbari91@gmail.com
 *
 * Written by Abbas Aliakbari
 *
 * @section DESCRIPTION
 *
 * API Configurator header file is using for detecting available features and libraries
 */
#pragma once
#include <Core/IO/LogSystem/Log.h>
#include <Core/Utility/Singleton/DebugSingleton.h>
#include <Core/System/Event.h>

#ifdef DGE_USE_GOOGLE_TEST
#if defined(DGE_Track_Memory) || defined(DGE_UseCRTDebugMemory)
#undef new
#undef delete
#endif

#ifdef DGE_USE_QT
#include <QGuiApplication>
#include <QtTest/QTest>
#endif
#include <gtest/gtest.h>

#ifdef DGE_Track_Memory
#define new(param, ...) DBG_NEW(param, DGE_FileCodeInfo, DGE_LineCodeInfo)
#define delete(param, ...) DBG_Delete(param, DGE_FunctionInfo, DGE_FileCodeInfo, DGE_LineCodeInfo)
#elif defined(DGE_UseCRTDebugMemory)
#define _New DBG_NEW
#define new DBG_NEW
#endif
#endif
#if DGE_Platform == DGE_Windows_Platform
#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h> //for malloc and free
#include <stdlib.h>
#include <windows.h>
#endif
namespace DGE
{
	namespace Core
	{
		namespace AutoTest
		{
			/// \brief Interface for storing a test class internally
			class ITest
			{
			public:
				virtual bool init() = 0;

				virtual int run(int argc, char* argv[]) = 0;

				virtual bool cleanUp() = 0;

				virtual DString objectName() = 0;

				virtual void setObjectName(DString objectName) = 0;
			};

			typedef std::set<ITest*> TestList;

			inline TestList& testList()
			{
				static TestList list;
				return list;
			}

			inline bool findObject(ITest* object)
			{
				TestList& list = testList();
				if (list.count(object) > 0)
				{
					return true;
				}
				for (auto test : list)
				{
					if (test->objectName() == object->objectName())
					{
						return true;
					}
				}
				return false;
			}

			inline void addTest(ITest* object)
			{
				TestList& list = testList();
				if (!findObject(object))
				{
					list.insert(object);
				}
			}

			//			struct QtCoverageScanner
			//			{
			//				QtCoverageScanner(const char* name)
			//				{
			//					//__coveragescanner_clear();
			//					//__coveragescanner_testname(name);
			//				}
			//				~QtCoverageScanner()
			//				{
			//					//__coveragescanner_save();
			//					//__coveragescanner_testname("");
			//				}
			//			};

			/*
			 * QtCoverageScanner _qtCoverageScanner("EngineTests");
			::QTest::Internal::callInitMain<EngineTests>();
			QApplication app(argc, argv);
			app.setAttribute(Qt::AA_Use96Dpi, true);
			QTEST_DISABLE_KEYPAD_NAVIGATION
				EngineTests tc;
			QTEST_SET_MAIN_SOURCE_PATH
				return QTest::qExec(&tc, argc, argv);*/

			/// \brief Call this event before run tests
			extern DGE_CoreExtern std::shared_ptr<Core::System::Event<bool>> g_beforeRunTestsEvent;
			/// \brief Call this event after terminating tests for releasing the resources
			extern DGE_CoreExtern Core::System::Event<bool> g_startReleasingResourcesEvent;

			inline int run(int argc, char* argv[])
			{
				int ret = 0;
				if (g_beforeRunTestsEvent)
					g_beforeRunTestsEvent->operator()();
#ifdef DGE_UseCRTDebugMemory
				//_crtBreakAlloc = 1728;
				_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#ifdef DGE_USE_QT
				QGuiApplication app(argc, argv);
#endif
#ifdef DGE_USE_GOOGLE_TEST
				::testing::InitGoogleTest(&argc, argv);
#endif
				for (auto test : testList())
				{
					test->init();
					ret += test->run(argc, argv);
					test->cleanUp();
				}

#ifdef DGE_USE_GOOGLE_TEST
				ret = RUN_ALL_TESTS();
#endif
				Core::Utility::g_isExitingApp = true;
				g_startReleasingResourcesEvent();
				Core::Utility::deleteSingletons();
#if DGE_Platform == DGE_Windows_Platform
				_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif
				return ret;
			}

			enum class TestType
			{
				Qt,
				QtQuick,
				Google,
				Boost,
				Catch,
				CTest
			};

			template<class TestType>
			class Test : public ITest
			{
				std::shared_ptr<TestType>			 m_child;
				DString								 m_name;
				DGE::Core::AutoTest::TestType m_type;

			public:
				Test(const DString& name, DGE::Core::AutoTest::TestType type
										 = DGE::Core::AutoTest::TestType::Qt)
					: m_name(name)
					, m_type(type)
				{
					this->setObjectName(m_name);
					DGE::Core::AutoTest::addTest(this);
				}

				// ITest interface
				virtual bool init() override
				{
					m_child.reset(new TestType());
					return true;
				}
				virtual int run(int argc, char* argv[]) override
				{
					if (m_child)
#ifdef DGE_USE_QT
						return QTest::qExec(m_child.get(), argc, argv);
#endif
					return -1;
				}
				virtual bool cleanUp() override
				{
					m_child.reset();
					return true;
				}

				virtual DString objectName() override { return m_name; }

				virtual void setObjectName(DString objectName) override { m_name = objectName; }
			};

#define DECLARE_QTEST(className) \
	static DGE::Core::AutoTest::Test<className> internal##className(#className);

#define DECLARE_GTEST(className)                       \
	static DGE::Core::AutoTest::Test<className> \
		internal##className(#className, DGE::Core::AutoTest::TestType::Google);
			/*  Q_GLOBAL_STATIC_WITH_ARGS(DGE::Core::AutoTest::Test<className>, \
										  internal##className, \
										  #className)*/

#define __TEST_MAIN                                         \
	int main(int argc, char* argv[])                        \
	{                                                       \
		return DGE::Core::AutoTest::run(argc, argv); \
	}

#if DGE_Platform == DGE_Android_Platform
#define TEST_MAIN                                \
	__TEST_MAIN                                  \
	void android_main(struct android_app* state) \
	{                                            \
		main(0, nullptr);                        \
	}
#else
#define TEST_MAIN __TEST_MAIN
#endif

		} // namespace AutoTest
	}	  // namespace Core
} // namespace DGE
