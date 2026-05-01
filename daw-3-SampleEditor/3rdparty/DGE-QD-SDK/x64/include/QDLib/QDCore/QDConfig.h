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
 *
 */
#pragma once
#if defined(_MSC_VER)
#define QD_Import __declspec(dllimport)
#ifdef QD_DLLExport
#define QD_Extern __declspec(dllexport)
#elif defined(QD_UseDLL)
#define QD_Extern QD_Import
#else
#define QD_Extern
#endif
#define QD_API QD_Extern
#define QD_Template_Export QD_Extern

#define QD_NEDMALLOC_DLL_EXPORTS QD_Extern
#elif defined(__GNUC__)
#define QD_Import __attribute__((dllimport))
#ifdef QD_DLLExport
#define QD_Extern __attribute__((dllexport))
#elif defined(QD_UseDLL)
#define QD_Extern QD_Import
#else
#define QD_Extern
#endif
#define QD_API QD_Extern
#define QD_Template_Export QD_Extern

#define QD_NEDMALLOC_DLL_EXPORTS QD_Extern

#endif
