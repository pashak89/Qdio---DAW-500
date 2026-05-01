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

#include "AppManager.h"
#include <Core/Utility/Singleton/Singleton.h>
#include <QuickFlux>

/// \brief It's App Manager class that support flux architecture and flux library.
class QD_API FluxAppManager
	: public AppManager
	, public DGE::Core::Utility::Singleton<FluxAppManager, std::shared_ptr<FluxAppManager>>
{
	Q_OBJECT

	D_SINGLETON(FluxAppManager, std::shared_ptr<FluxAppManager>);

	FluxAppManager();
	FluxAppManager(int argc, char **argv);
	FluxAppManager(const FluxAppManager &)			  = delete;
	FluxAppManager(FluxAppManager &&)				  = delete;
	FluxAppManager &operator=(const FluxAppManager &) = delete;
	FluxAppManager &operator=(FluxAppManager &&)	  = delete;

public:
	D_Destructor(FluxAppManager);
	// AppManager interface

	using DGE::Core::Utility::Singleton<FluxAppManager,
											   std::shared_ptr<FluxAppManager>>::getInstance;

	using DGE::Core::Utility::Singleton<FluxAppManager,
											   std::shared_ptr<FluxAppManager>>::releaseInstance;
	// AppManager interface
protected:
	virtual bool registerQMLType() override;

public slots:
	virtual bool init() override;

protected:
	/// \brief It's Flux app dispatcher that is required for initialize Flux library.
	std::shared_ptr<QFAppDispatcher> m_fluxAppDispatcher;
};
