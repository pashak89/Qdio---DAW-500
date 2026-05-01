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

#if defined(_LIB)&&!defined(EntryPoint)

using namespace DGE;
using namespace DGE::Core::GlobalObjects;
#define EntryPoint

//! -----------------------------------------------------------------
//! Call When Engine Began to register state
//! -----------------------------------------------------------------
extern void OnRegisterState();

//! -----------------------------------------------------------------
//! Call When Engine Began to unregister state
//! -----------------------------------------------------------------
extern void OnUnregisterState();

//! -----------------------------------------------------------------
//! Call When Engine Began to loading
//! -----------------------------------------------------------------
extern EngineSetup* OnLoad();

//!-----------------------------------------------------------------
//! Entry of Engine,After Loaded Engine!
//!-----------------------------------------------------------------
extern void OnEntry();

//! -----------------------------------------------------------------
//! Call When Engine Began to unloading
//! -----------------------------------------------------------------
extern bool OnUnload();

//! -----------------------------------------------------------------
//! Call When Engine Unloaded.
//! -----------------------------------------------------------------
extern bool OnUnloaded();

//! -----------------------------------------------------------------
//!? Call in unexpected error
//! -----------------------------------------------------------------
extern bool OnError();

#if DGE_Platform==DGE_Windows_Platform

#pragma comment(lib,"GameEngine.lib")
__declspec(selectany) HINSTANCE IntanceApp;
__declspec(selectany) EngineSetup* egs=nullptr;
__declspec(selectany) Engine *_engine=nullptr;

inline bool CreateDGE(const EngineSetup* setupData)
{
	egs = new EngineSetup(setupData->name, Utility::ptr_fun(&OnRegisterState),Utility::ptr_fun(&OnUnregisterState),
						  setupData->Width, setupData->Height, 
						  setupData->FullScreen,setupData->instance!=nullptr?setupData->instance:IntanceApp);
	egs->m_WindowHandle=setupData->m_WindowHandle;
	egs->m_DrawWindowHandle=setupData->m_DrawWindowHandle;
	_engine = (Engine*) Engine::CreateEngine();

	_engine->Intialize(egs);
	SafeDelete(setupData);
	if (!_engine->IsLoaded())
		return false;
	return true;
}

inline void _RunEngine(HINSTANCE hInstance)
{
	IntanceApp = hInstance;
#ifdef DGE_UseWindowConsoleDebug
	AddDebugConsole();
#endif
	
	if (CreateDGE(OnLoad()))
	{
		if (_engine!=nullptr&&_engine->IsLoaded())
		{
			OnEntry();
			_engine->RunAutoManage();
		}
		else
			cout << "Engine Not Run!!" << endl;
	}
}

//! -----------------------------------------------------------------
//! Call When App Began to run
//! -----------------------------------------------------------------
#ifndef _M_CEE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#else
[System::STAThreadAttribute]
int Main(cli::array<System::String ^> ^args)
{
	HINSTANCE hInstance=GetModuleHandle(NULL);
#endif
	__try
	{
		/*
		in this part,try to find how many of this application is now running.
		if more than one copy of this application is run,then exit of engine!
		*/
		void* m_MutexHandle = HasRunOnce();
		if (m_MutexHandle == nullptr)
		{
			ReleaseMutex(m_MutexHandle);
			CloseHandle(m_MutexHandle);
			// Program already running somewhere
			exit(0);
			return 1; // Exit program
		}
#if defined(DGE_UseWindowConsoleDebug)&&defined(DGE_UseCRTDebugMemory)
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR );
		/*
		* Set the debug-heap flag to keep freed blocks in the
		* heap's linked list - This will allow us to catch any
		* inadvertent use of freed memory
		*/
		int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
		tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
		tmpDbgFlag = (tmpDbgFlag & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_16_DF;
		_CrtSetDbgFlag(tmpDbgFlag);
#endif
		__try
		{
			__try
			{
				_RunEngine(hInstance);
			}
			__finally
			{
				OnUnload();
			}
		}
		__except (GenerateDump(GetExceptionInformation()))
		{
			OnError();
		}
		if(!Engine::ReleaseEngine()) 
			return 1;
		SafeDelete(egs);
		//CHKHEAP();
		OnUnloaded();
#if defined(DGE_UseWindowConsoleDebug)&&defined(DGE_UseCRTDebugMemory)
		_CrtDumpMemoryLeaks();
#endif
		ReleaseMutex(m_MutexHandle);
		CloseHandle(m_MutexHandle);
		/*cout<<"end";
		cin.get();*/
	}
	__except (GenerateDump(GetExceptionInformation()))
	{
		cout<<"Fatal Error Occurred!!  :(((";
	}
	

	return 0;
}

#else
#endif


#endif