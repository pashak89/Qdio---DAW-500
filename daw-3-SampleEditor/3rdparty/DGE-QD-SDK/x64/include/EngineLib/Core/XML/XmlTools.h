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
#include "../Config/EngineConfigurator.h"

namespace DGE
{
	namespace Core
	{
		namespace Xml
		{
#ifdef DGE_UseXmlLibrary
			#if defined(MemTrace)
			#undef new
			#endif
			#include"rapidxml-1.13\rapidxml.hpp"
			#include"rapidxml-1.13\rapidxml_utils.hpp"
			#include"rapidxml-1.13\rapidxml_iterators.hpp"
			#include"rapidxml-1.13\rapidxml_print.hpp"
			using namespace rapidxml;
			#if defined(MemTrace)
			#define new DBG_NEW
			#endif
			#define GetValueNode(node,attributeName) node->first_attribute(attributeName)->value()
			#define NextSelf(node) (node=(node->next_sibling()))
			#define NextSelfByName(node,name) (node=(node->next_sibling(name)))
			#define GoDown(node) node=node->first_node()
			#define GoDownByName(node,name) node=node->first_node(name)
#endif
		}
	}
}
