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
#if defined(DGE_ScriptUseDLL)
#define DGE_ScriptExtern DGE_Import
#elif defined(DGE_ScriptDLLExport)
#define DGE_ScriptExtern DGE_Export
#else
#define DGE_ScriptExtern
#endif

#define DGE_ScriptAPI DGE_ScriptExtern
#define DGE_ScriptTemplate_Export DGE_ScriptExtern
#define NEDMALLOC_ScriptDLL_EXPORTS DGE_ScriptExtern

#ifndef DGE_ScriptModule
#define DGE_ScriptModule
#endif
