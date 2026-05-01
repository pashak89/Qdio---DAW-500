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

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
#define D_QMLNodeType()                   \
public:                                   \
	/* QQmlParserStatus interface*/       \
	virtual void classBegin() override {} \
	virtual void componentComplete() override {}
#define __D_MapNodeSigniture() virtual void mapNode(QObject* parentNode)
#define __D_MapNodeImpl(QNodeType, NodeType)           \
	{                                                  \
		registerNode<QNodeType, NodeType>(parentNode); \
	}

#define D_MapNode(QNodeType, NodeType) \
protected:                             \
	__D_MapNodeSigniture()

#define D_RegisterNode(QNodeType, EngineNodeType) \
protected:                                        \
	__D_MapNodeSigniture() override               \
                                                  \
		__D_MapNodeImpl(QNodeType, EngineNodeType)

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
