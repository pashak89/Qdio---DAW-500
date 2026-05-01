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

#include <Core/System/DCoreObject.h>
#include <Core/Utility/Interfaces/IExecutable.h>
#include <Core/Utility/Interfaces/IRelease.h>
#include <UI/BaseUIItem.h>

#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QtCore/QObject>
#include <QtGui>

#include <Core/MemoryManager/MemoryManager.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <Core/CrashReport/CrashReport.h>
#include <memory>
class QD_API AppManager
	: public BaseUIItem
	, public DGE::Core::System::DCoreObject
	, public DGE::Core::Utility::Interfaces::IExecutable
	, public DGE::Core::Utility::Interfaces::IRelease
	, public DGE::Core::Utility::Singleton<
		  AppManager, std::shared_ptr<AppManager>,
		  DGE::Core::Utility::Deleter<AppManager>::emptyDeleter>
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "AppManager.plugin")

	D_SINGLETON(AppManager, std::shared_ptr<AppManager>,
				DGE::Core::Utility::Deleter<AppManager>::emptyDeleter)

public:
	AppManager();
	AppManager(int argc, char** argv);
	D_Destructor(AppManager);

	const std::shared_ptr<QQuickItem>& rootItem() const;
	const QString& defaultQMLFilePath() const;

	virtual int32 exec() override;

	// IRelease interface
	virtual bool release() override;

	std::shared_ptr<QQmlApplicationEngine> qmlAppEngine() const;
	bool								   addQMLPath(QString path);

	QStringList args() const;

protected:
	virtual bool registerQMLType();
	void		 processArgs();

public slots:
	virtual bool init();
	bool deinitialize();
	void onStartUpdate();

	void setDefaultQMLFilePath(const QString& newDefaultQMLFilePath);

signals:
	void startUpdate();
	void startInitialization();

protected:
	DGE::Core::CrashReport::CrashReporter  m_crashReporter;
	std::shared_ptr<QCoreApplication>	   m_app;
	std::shared_ptr<QQmlContext>		   m_qmlContext;
	std::shared_ptr<QQuickItem>			   m_rootItem;
	std::shared_ptr<QQmlApplicationEngine> m_engine;
	QString								   m_defaultQMLFilePath;
	QStringList							   m_args;
	int									   m_argc;
	char**								   m_argv;

	QQuickWindow* m_currentWindow;
};
