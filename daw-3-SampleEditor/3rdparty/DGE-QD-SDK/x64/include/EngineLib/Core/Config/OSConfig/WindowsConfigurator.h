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

//!? Do not change this file!!
#pragma region Windows Config
//! -----------------------------------------------------------------
//! Include For Windows API & Other Special Feature that may be needed.
//! -----------------------------------------------------------------
#include<sdkddkver.h>
#include<Windows.h>
#include<commctrl.h>
#include<windowsx.h>
#include<sys\utime.h>
#include<io.h>
#include<conio.h>
#include<excpt.h>
#include<tChar.h>
//#include<strsafe.h>
#include <Dwmapi.h>
#include <Richedit.h>
#include <assert.h>
#include <dde.h>

//! -----------------------------------------------------------------
//! Some Macro.
//! -----------------------------------------------------------------
#pragma region Some Windows Message Macro

#pragma endregion

//! -----------------------------------------------------------------
//! Add necessary *.lib for windows to project.
//! -----------------------------------------------------------------
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"Strmiids.lib")
#pragma comment(lib,"Dwmapi.lib")

#pragma endregion
