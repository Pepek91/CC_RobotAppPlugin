//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qDummy                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#							COPYRIGHT: Michał Stępień					   #
//#                                                                        #
//##########################################################################

#include "qRobotAppPlugin.h"

//Qt
#include <QtGui>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>

//CC
#include <ccObject.h>
#include <ccHObject.h>

#include <cc2DViewportLabel.h>
#include <cc2DLabel.h>
#include <ccGenericPointCloud.h>
#include <ccPointCloud.h>
#include <ccHObjectCaster.h>

#include "connectiond.h"
#include "vectorpicker.h"
#include "calibration.h"
#include "moverobot.h"

qRobotAppPlugin::qRobotAppPlugin(QObject* parent/*=0*/)
	: QObject(parent)
	, m_fastScann(0), m_tcpConnection(0), m_accurateScann(0), m_calibration(0), m_setAccurateScannVector(0), m_moveRobot(0)
	, m_sendNBV(0), m_stopScan(0), m_scanNow(0), m_prescanFromPoints(0), m_loadSinglePosition(0), m_loadPrescan(0)
	, m_bAccurateScannPointSet(false)
	, m_connectionManager(new Connection())
{
}

qRobotAppPlugin::~qRobotAppPlugin()
{
    delete m_connectionManager;
}

void qRobotAppPlugin::onNewSelection(const ccHObject::Container& selectedEntities)
{
	//if (m_action)
	//	m_action->setEnabled(!selectedEntities.empty());
}

void qRobotAppPlugin::getActions(QActionGroup& group)
{
	//default action (if it has not been already created, it's the moment to do it)
	if (!m_tcpConnection)
	{
		m_tcpConnection = new QAction(getNameTCP(), this);
		m_tcpConnection->setToolTip(getDescriptionTCP());
		m_tcpConnection->setIcon(getIconTCP());

		connect(m_tcpConnection, SIGNAL(triggered()), this, SLOT(doActionTCP()));
	}

	if (!m_fastScann)
	{
		m_fastScann = new QAction(getNameFast(), this);
		m_fastScann->setToolTip(getDescriptionFast());
		m_fastScann->setIcon(getIconFast());
		//connect appropriate signal
		connect(m_fastScann, SIGNAL(triggered()), this, SLOT(doActionFastScan()));
	}

	if (!m_setAccurateScannVector)
	{
		m_setAccurateScannVector = new QAction(getNameAccurateVec(), this);
		m_setAccurateScannVector->setToolTip(getDescriptionAccurateVec());
		m_setAccurateScannVector->setIcon(getIconAccurateVec());

		connect(m_setAccurateScannVector, SIGNAL(triggered()), this, SLOT(doActionSetAccurateScanVec()));
	}

	if (!m_accurateScann)
	{
		m_accurateScann = new QAction(getNameAccurate(), this);
		m_accurateScann->setToolTip(getDescriptionAccurate());
		m_accurateScann->setIcon(getIconAccurate());

		connect(m_accurateScann, SIGNAL(triggered()), this, SLOT(doActionAccurateScan()));
	}

	if(!m_calibration)
	{
		m_calibration = new QAction(getNameCalibration(), this);
		m_calibration->setToolTip(getDescriptionCalibration());
		m_calibration->setIcon(getIconCalibration());

		connect(m_calibration, SIGNAL(triggered()), this, SLOT(doActionCalibration()));
	}

	if(!m_moveRobot)
	{
		m_moveRobot = new QAction(getNameMoveRobot(), this);
		m_moveRobot->setToolTip(getDescriptionMoveRobot());
		m_moveRobot->setIcon(getIconMoveRobot());

		connect(m_moveRobot, SIGNAL(triggered()), this, SLOT(doActionMoveRobot()));
	}

	if (!m_sendNBV)
	{
		m_sendNBV = new QAction(getNameSendNBV(), this);
		m_sendNBV->setToolTip(getDescriptionSendNBV());
		m_sendNBV->setIcon(getIconSendNBV());

		connect(m_sendNBV, SIGNAL(triggered()), this, SLOT(doActionSendNBV()));
	}

	if (!m_stopScan)
	{
		m_stopScan = new QAction("Stop scans", this);
		m_stopScan->setToolTip("Stop scan tasks that are currently running on server");
		m_stopScan->setIcon(getIconStop());

		connect(m_stopScan, SIGNAL(triggered()), this, SLOT(doActionStop()));
		
	}

	if (!m_scanNow)
	{
		m_scanNow = new QAction("Scan now", this);
		m_scanNow->setToolTip("Perform scan from current position");
		m_scanNow->setIcon(getIconScanNow());

		connect(m_scanNow, SIGNAL(triggered()), this, SLOT(doActionScanNow()));
	}

	if (!m_prescanFromPoints)
	{
		m_prescanFromPoints = new QAction("NBV from points", this);
		m_prescanFromPoints->setToolTip("Generate NBV from selected files");
		m_prescanFromPoints->setIcon(getIconPrescanFromPoints());

		connect(m_prescanFromPoints, SIGNAL(triggered()), this, SLOT(doActionPrescanFromPoints()));
	}

	if (!m_loadSinglePosition)
	{
		m_loadSinglePosition = new QAction("Load single position", this);
		m_loadSinglePosition->setToolTip("Load single robot position");
		m_loadSinglePosition->setIcon(getIconLoadSinglePosition());

		connect(m_loadSinglePosition, SIGNAL(triggered()), this, SLOT(doActionLoadSinglePosition()));
	}

	if (!m_loadPrescan)
	{
		m_loadPrescan = new QAction("Load prescan", this);
		m_loadPrescan->setToolTip("Load prescan");
		m_loadPrescan->setIcon(getIconLoadPrescan());

		connect(m_loadPrescan, SIGNAL(triggered()), this, SLOT(doActionLoadPrescan()));
	}

	//calibrate and so on
	group.addAction(m_tcpConnection);
	group.addAction(m_scanNow);
	group.addAction(m_fastScann);
	group.addAction(m_setAccurateScannVector);
	group.addAction(m_accurateScann);
	group.addAction(m_calibration);
	group.addAction(m_moveRobot);
	group.addAction(m_sendNBV);
	group.addAction(m_stopScan);
	group.addAction(m_prescanFromPoints);
	group.addAction(m_loadSinglePosition);
	group.addAction(m_loadPrescan);
}

void qRobotAppPlugin::doActionTCP()
{
	assert(m_app);
	if (!m_app)
		return;

	// *** HERE STARTS THE ACTION ***
	m_app->dispToConsole("[qRobotAppPlugin] Running action designed to establish TCP connection.", ccMainAppInterface::STD_CONSOLE_MESSAGE);

	m_connectionManager->setMainAppIfNotSet(m_app);

	if (!m_connectionManager->isConnected())
	{
        ConnectionD::ConnectionData conData;
        conData.set = false;
        ConnectionD connectionDialog(&conData);
        connectionDialog.exec();

        if (!conData.set)
		{
			if (conData.message != QString())
				m_app->dispToConsole(conData.message, ccMainAppInterface::ERR_CONSOLE_MESSAGE);
			return;
		}

		m_app->dispToConsole(conData.message, ccMainAppInterface::STD_CONSOLE_MESSAGE);

		if (!m_connectionManager->connectToServer(conData.IP, conData.Port, conData.sessionName))
		{
			m_app->dispToConsole("[qRobotAppPlugin] Failed to connect to server.", ccMainAppInterface::STD_CONSOLE_MESSAGE);
		}
	}
	else
	{

		QMessageBox msgBox(QMessageBox::Question,
            tr("Disconnect?"),
            tr("Do you want to disconnect?"));
		QAbstractButton *yesButton = msgBox.addButton(tr("Disconnect"), QMessageBox::YesRole);
		QAbstractButton *closeButton = msgBox.addButton(tr("Disconnect and close server"), QMessageBox::ActionRole);
		QAbstractButton *noButton = msgBox.addButton(QMessageBox::Cancel);

		msgBox.exec();

		if (msgBox.clickedButton() == yesButton) {
			m_connectionManager->disconnect(false);
		} else if (msgBox.clickedButton() == closeButton) {
			m_connectionManager->disconnect(true);
		}
	}
	// *** HERE ENDS THE ACTION ***
}

void qRobotAppPlugin::doActionFastScan()
{
	assert(m_app);
	if (!m_app)
		return;

	//*** HERE STARTS THE ACTION ***
	m_app->dispToConsole("[qRobotAppPlugin] Requesting fast and inaccurate scan!",ccMainAppInterface::STD_CONSOLE_MESSAGE);
	if (m_connectionManager->isConnected())
	{
		QMessageBox::StandardButton reply = QMessageBox::question(NULL, tr("Are you sure?"), "Do you want to run prescan?", QMessageBox::Yes | QMessageBox::Cancel);
		if (reply == QMessageBox::Yes)
			m_connectionManager->requestFastScan();
	}
	else
	{
		m_app->dispToConsole("[qRobotAppPlugin] Please connect to robot server first!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
	}
	//*** HERE ENDS THE ACTION ***
}

void qRobotAppPlugin::doActionSetAccurateScanVec()
{
	assert(m_app);
	if (!m_app)
		return;

	QString errorMSG = "Select at least one point with point picking tool, save label and then select it!";

	//*** HERE STARTS THE ACTION ***
	m_app->dispToConsole("[qRobotAppPlugin] Requesting to set accurate scan vector", ccMainAppInterface::STD_CONSOLE_MESSAGE);

	bool bAddewVecs = false;

	if (m_bAccurateScannPointSet)
	{
		QMessageBox msgBox(QMessageBox::Question,
            tr("How to change current set of vectors?"),
            tr("What do you want to do with new vectors?"));

		QAbstractButton *addNew = msgBox.addButton(tr("Append new vectors to set"), QMessageBox::YesRole);
		QAbstractButton *clearAll = msgBox.addButton(tr("Clear old set and add selected vector to empty set"), QMessageBox::ActionRole);
		QAbstractButton *noButton = msgBox.addButton(QMessageBox::Cancel);

		msgBox.exec();

		if (msgBox.clickedButton() == addNew) {
			bAddewVecs = true;
		} else if (msgBox.clickedButton() == clearAll) {
			m_bAccurateScannPointSet = false;
			m_vAccurateNormalsXYZ.clear();
			m_vAccurateXYZ.clear();
		}
		else
		{
			return;
		}
	}

	if (!m_bAccurateScannPointSet || bAddewVecs)
	{
		const ccHObject::Container& selectedEntities = m_app->getSelectedEntities();

		if (selectedEntities.size() == 0)
		{
			m_app->dispToConsole(errorMSG, ccMainAppInterface::ERR_CONSOLE_MESSAGE);
			return;
		}
		for(int i = 0; i < selectedEntities.size(); ++i)
		{
			if (!selectedEntities.at(i)->isA(CC_TYPES::LABEL_2D))
			{
				m_app->dispToConsole("Selected entity is not a point! Skipping.", ccMainAppInterface::WRN_CONSOLE_MESSAGE);
				continue;
			}
			cc2DLabel* label2D = static_cast<cc2DLabel*>(selectedEntities.at(i));
			unsigned int ui_pointID = label2D->getPoint(0).index;

			ccPointCloud* pc = ccHObjectCaster::ToPointCloud(label2D->getPoint(0).cloud);
			CCVector3 accurateXYZ = (*pc->getPoint(ui_pointID));

			CCVector3 accurateNormalsXYZ;

			if (pc->hasNormals())
			{
				accurateNormalsXYZ = pc->getPointNormal(ui_pointID);
			}
			else
			{
				accurateNormalsXYZ = CCVector3(1, 0, 0);
			}

			bool cancel = true;
			VectorPicker vectorPickerDlg(&accurateNormalsXYZ, &cancel);
			vectorPickerDlg.exec();

			if (!cancel)
			{
				accurateNormalsXYZ.normalize();
				m_bAccurateScannPointSet = true;

				m_vAccurateNormalsXYZ.push_back(accurateNormalsXYZ);
				m_vAccurateXYZ.push_back(accurateXYZ);

				m_app->dispToConsole(QString("[qRobotAppPlugin] Accurate scan vector set: {X,Y,Z} - {%1 %2 %3}, {NX, NY, NZ} - {%4 %5 %6}").arg(
					QString::number(accurateXYZ.x), QString::number(accurateXYZ.y), QString::number(accurateXYZ.z),
					QString::number(accurateNormalsXYZ.x), QString::number(accurateNormalsXYZ.y), QString::number(accurateNormalsXYZ.z)),
					ccMainAppInterface::STD_CONSOLE_MESSAGE);

			}
		}
	}
	//*** HERE ENDS THE ACTION ***
}

void qRobotAppPlugin::doActionAccurateScan()
{
	assert(m_app);
	if (!m_app)
		return;

	//*** HERE STARTS THE ACTION ***
	if (m_bAccurateScannPointSet && m_connectionManager->isConnected())
	{
		if (m_vAccurateXYZ.size() != m_vAccurateNormalsXYZ.size())
		{
			m_app->dispToConsole("[qRobotAppPlugin] Error while sending data. Size of normals and points vector mismatch!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
			return;
		}
		QMessageBox::StandardButton reply = QMessageBox::question(NULL, tr("Are you sure?"), "Do you want to run inspection?", QMessageBox::Yes | QMessageBox::Cancel);
		if (reply == QMessageBox::Yes)
		{
			m_connectionManager->requestAccurateScan(m_vAccurateXYZ, m_vAccurateNormalsXYZ);
			m_app->dispToConsole(QString("[qRobotAppPlugin] Request to do accurate scan send. Number of measurement vectors: %1.").arg(m_vAccurateXYZ.size()), ccMainAppInterface::STD_CONSOLE_MESSAGE);
		}
	}
	else
	{
		m_app->dispToConsole("[qRobotAppPlugin] Please connect to robot and set accurate scan point!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
	}
	//*** HERE ENDS THE ACTION ***

}

void qRobotAppPlugin::doActionCalibration()
{
	assert(m_app);
	if (!m_app)
		return;

	//*** HERE STARTS THE ACTION ***
	if (m_connectionManager->isConnected())
	{
		int head, type;
		bool chosen = false;
		Calibration cal(&head, &type, &chosen);
		cal.exec();
		if(chosen)
			m_connectionManager->requestCalibration(head, type);
	}
	else
	{
		m_app->dispToConsole("[qRobotAppPlugin] Please connect to robot server first!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
	}
	
	//*** HERE ENDS THE ACTION ***

}

void qRobotAppPlugin::doActionMoveRobot()
{
	assert(m_app);
	if (!m_app)
		return;

	// *** HERE STARTS THE ACTION ***
	m_app->dispToConsole("[qRobotAppPlugin] Running action designed to move robot.", ccMainAppInterface::STD_CONSOLE_MESSAGE);

	if(!m_connectionManager->isConnected())
	{
		m_app->dispToConsole("[qRobotAppPlugin] Please connect to robot server first!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	MoveRobot moveRobotDialog(m_connectionManager);
	moveRobotDialog.exec();
	//*** HERE ENDS THE ACTION ***
}

void qRobotAppPlugin::doActionSendNBV()
{
	assert(m_app);
	if (!m_app)
		return;

	// *** HERE STARTS THE ACTION ***
	m_app->dispToConsole("[qRobotAppPlugin] Running action designed to send NBV.", ccMainAppInterface::STD_CONSOLE_MESSAGE);

	if (!m_connectionManager->isConnected())
	{
		m_app->dispToConsole("[qRobotAppPlugin] Please connect to robot server first!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	QString fileName = QFileDialog::getOpenFileName(0, tr("Find next best view file"), QString(), tr("Test files (*.txt *.nbv)"));

	QFile nbv(fileName);
	if (!nbv.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	std::vector<CCVector3> NBV_XYZ;
	std::vector<CCVector3> NBV_NormalsXYZ;

	while (!nbv.atEnd()) {
		QString line(nbv.readLine());
		QStringList splited = line.split(" ");
		if (splited.size() < 6)
		{
			m_app->dispToConsole(QString("[qRobotAppPlugin] NBV - line corrupted: ") + line, ccMainAppInterface::STD_CONSOLE_MESSAGE);
			continue;
		}
		
		NBV_XYZ.push_back(CCVector3(splited[0].toDouble(), splited[1].toDouble(), splited[2].toDouble()));
		NBV_NormalsXYZ.push_back(CCVector3(splited[3].toDouble(), splited[4].toDouble(), splited[5].toDouble()));
	}
	nbv.close();

	QString message = QString("Do you want to run NBV. Point count is ") + QString::number(NBV_XYZ.size());
	QMessageBox::StandardButton reply = QMessageBox::question(NULL, tr("Are you sure?"), message, QMessageBox::Yes | QMessageBox::Cancel);
	if (reply == QMessageBox::Yes)
		m_connectionManager->requestAccurateScan(NBV_XYZ, NBV_NormalsXYZ);
	//*** HERE ENDS THE ACTION ***
}

void qRobotAppPlugin::doActionStop()
{
	if (!m_connectionManager->isConnected())
	{
		m_app->dispToConsole("[qRobotAppPlugin] Please connect to robot server first!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	QMessageBox::StandardButton reply = QMessageBox::question(NULL, tr("Are you sure?"), "Do you want to send request to server to stop scanning?", QMessageBox::Yes | QMessageBox::Cancel);
	if (reply == QMessageBox::Yes)
		m_connectionManager->requestStopAction();
}

void qRobotAppPlugin::doActionScanNow()
{
	assert(m_app);
	if (!m_app)
		return;

	//*** HERE STARTS THE ACTION ***
	m_app->dispToConsole("[qRobotAppPlugin] Requesting scan now!", ccMainAppInterface::STD_CONSOLE_MESSAGE);
	if (m_connectionManager->isConnected())
	{
		QMessageBox::StandardButton reply = QMessageBox::question(NULL, tr("Are you sure?"), "Do you want to scan now?", QMessageBox::Yes | QMessageBox::Cancel);
		if (reply == QMessageBox::Yes)
			m_connectionManager->requestScanNow();
	}
	else
	{
		m_app->dispToConsole("[qRobotAppPlugin] Please connect to robot server first!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
	}
	//*** HERE ENDS THE ACTION ***
}

void qRobotAppPlugin::doActionPrescanFromPoints()
{
	assert(m_app);
	if (!m_app)
		return;

	//*** HERE STARTS THE ACTION ***
	m_app->dispToConsole("[qRobotAppPlugin] Requesting to set accurate scan vector", ccMainAppInterface::STD_CONSOLE_MESSAGE);

	const ccHObject::Container& selectedEntities = m_app->getSelectedEntities();

	if (selectedEntities.size() == 0)
	{
		m_app->dispToConsole("Select at least one point with point picking tool, save label and then select it!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	QString filePath = QFileDialog::getSaveFileName(NULL, QString("Save NBV file"), QString(), tr("Text file (*.txt);;NBV file (*.nbv)"));
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		m_app->dispToConsole(QString("Could not open file: %1!").arg(filePath), ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}
	QTextStream out(&file);

	for (int i = 0; i < selectedEntities.size(); ++i)
	{

		cc2DLabel* label2D = static_cast<cc2DLabel*>(selectedEntities.at(i));

		if (!label2D)
			continue;

		unsigned int ui_pointID = label2D->getPoint(0).index;

		ccPointCloud* pc = ccHObjectCaster::ToPointCloud(label2D->getPoint(0).cloud);
		CCVector3 preXYZ = (*pc->getPoint(ui_pointID));

		CCVector3 preNormals;

		if (pc->hasNormals())
		{
			preNormals = pc->getPointNormal(ui_pointID);
		}
		else
		{
			m_app->dispToConsole("Could have no normals, please calculate model normals!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
			continue;
		}

		CCVector3 v_dir_1{ 0.0, 0.0, 1.0 };
		CCVector3 v_dir_2{ 1.0, 0.0, 0.0 };

		CCVector3 v_rotation = CalcRotationformation3DToCoverTwoVectors(preNormals, v_dir_1);

		v_dir_2 = TransformVector(v_dir_2, v_rotation);

		preNormals.normalize();
		v_dir_2.normalize();

		m_app->dispToConsole(QString("[qRobotAppPlugin] Prescan point set: {X,Y,Z} - {%1 %2 %3}, {NX, NY, NZ} - {%4 %5 %6}, {upX, upY, upZ} - {%7, %8, %9}").arg(
			QString::number(preXYZ.x), QString::number(preXYZ.y), QString::number(preXYZ.z),
			QString::number(preNormals.x), QString::number(preNormals.y), QString::number(preNormals.z),
			QString::number(v_dir_2.x), QString::number(v_dir_2.y), QString::number(v_dir_2.z)),
			ccMainAppInterface::STD_CONSOLE_MESSAGE);

		out << QString("%1 %2 %3 %4 %5 %6 %7 %8 %9\n").arg(QString::number(preXYZ.x), QString::number(preXYZ.y), QString::number(preXYZ.z),
			QString::number(preNormals.x), QString::number(preNormals.y), QString::number(preNormals.z),
			QString::number(v_dir_2.x), QString::number(v_dir_2.y), QString::number(v_dir_2.z));
	}
	file.close();
	//*** HERE ENDS THE ACTION ***
}

void qRobotAppPlugin::doActionLoadSinglePosition()
{
	assert(m_app);
	if (!m_app)
		return;

	//*** HERE STARTS THE ACTION ***
	m_app->dispToConsole("[qRobotAppPlugin] Load single position", ccMainAppInterface::STD_CONSOLE_MESSAGE);
	if (m_connectionManager->isConnected())
	{
		QString loadText;
		{
			QString filePath = QFileDialog::getOpenFileName(NULL, tr("Load single position"), QString(), tr("Text file (*.txt);;XML file (*.chpxml)"));
			QFile file(filePath);
			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				m_app->dispToConsole(QString("Could not open file: %1!").arg(filePath), ccMainAppInterface::ERR_CONSOLE_MESSAGE);
				return;
			}

			QTextStream in(&file);

			loadText = in.readAll();
		}

		m_connectionManager->requestLoadSignlePosition(loadText);
	}
	else
	{
		m_app->dispToConsole("[qRobotAppPlugin] Please connect to robot server first!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
	}
	//*** HERE ENDS THE ACTION ***
}

void qRobotAppPlugin::doActionLoadPrescan()
{
	assert(m_app);
	if (!m_app)
		return;

	//*** HERE STARTS THE ACTION ***
	m_app->dispToConsole("[qRobotAppPlugin] Load prescan", ccMainAppInterface::STD_CONSOLE_MESSAGE);
	if (m_connectionManager->isConnected())
	{
		QString loadText;

		{
			QString filePath = QFileDialog::getOpenFileName(NULL, tr("Load prescan"), QString(), tr("XML file (*.chpxml);;Text file (*.txt)"));
			QFile file(filePath);
			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				m_app->dispToConsole(QString("Could not open file: %1!").arg(filePath), ccMainAppInterface::ERR_CONSOLE_MESSAGE);
				return;
			}

			QTextStream in(&file);

			loadText = in.readAll();
		}

		m_connectionManager->requestLoadingPrescan(loadText);
	}
	else
	{
		m_app->dispToConsole("[qRobotAppPlugin] Please connect to robot server first!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
	}
	//*** HERE ENDS THE ACTION ***
}

QIcon qRobotAppPlugin::getIcon() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/general.png");
}

QIcon qRobotAppPlugin::getIconTCP() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/connection.png");
}

QIcon qRobotAppPlugin::getIconFast() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/fast.png");
}

QIcon qRobotAppPlugin::getIconAccurateVec() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/accuratevec.png");
}

QIcon qRobotAppPlugin::getIconAccurate() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/accurate.png");
}

QIcon qRobotAppPlugin::getIconCalibration() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/calibration.png");
}

QIcon qRobotAppPlugin::getIconMoveRobot() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/moverobot.png");
}

QIcon qRobotAppPlugin::getIconSendNBV() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/sendnbv.png");
}

QIcon qRobotAppPlugin::getIconStop() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/stop.png");
}

QIcon qRobotAppPlugin::getIconScanNow() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/scannow.png");
}

QIcon qRobotAppPlugin::getIconPrescanFromPoints() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/prefrompts.png");
}

QIcon qRobotAppPlugin::getIconLoadSinglePosition() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/loadsingleposition.png");
}

QIcon qRobotAppPlugin::getIconLoadPrescan() const
{
	return QIcon(":/CC/plugin/qDummyPlugin/MyPlugUtils/icons/loadprescan.png");
}

CCVector3 qRobotAppPlugin::CalcRotationformation3DToCoverTwoVectors(CCVector3 p_psStableP3D, CCVector3 p_psToTransP3D)
{

	CCVector3 v_rotation;
	//
	// calc rotX
	//
	double v_rPsY = p_psStableP3D.y;
	double v_rPsZ = p_psStableP3D.z;
	double v_rPfY = p_psToTransP3D.y;
	double v_rPfZ = p_psToTransP3D.z;

	// normalize vectors
	double v_rTemp = sqrt(v_rPsY*v_rPsY + v_rPsZ*v_rPsZ);
	if (v_rTemp > 0.0)
	{
		v_rPsY = v_rPsY / v_rTemp;
		v_rPsZ = v_rPsZ / v_rTemp;

		v_rTemp = sqrt(v_rPfY*v_rPfY + v_rPfZ*v_rPfZ);
		if (v_rTemp > 0.0)
		{
			v_rPfY = v_rPfY / v_rTemp;
			v_rPfZ = v_rPfZ / v_rTemp;

			v_rTemp = sqrt((v_rPsY - v_rPfY)*(v_rPsY - v_rPfY) +
				(v_rPsZ - v_rPfZ)*(v_rPsZ - v_rPfZ));

			v_rotation.x = 2.0*asin(v_rTemp*0.5);

			double v_rPfYp = v_rPfY*cos(-v_rotation.x) + v_rPfZ*sin(-v_rotation.x);
			double v_rPfZp = v_rPfZ*cos(-v_rotation.x) - v_rPfY*sin(-v_rotation.x);
			double v_rTp = sqrt((v_rPsY - v_rPfYp)*(v_rPsY - v_rPfYp) +
				(v_rPsZ - v_rPfZp)*(v_rPsZ - v_rPfZp));

			double v_rPfYm = v_rPfY*cos(v_rotation.x) + v_rPfZ*sin(v_rotation.x);
			double v_rPfZm = v_rPfZ*cos(v_rotation.x) - v_rPfY*sin(v_rotation.x);
			double v_rTm = sqrt((v_rPsY - v_rPfYm)*(v_rPsY - v_rPfYm) +
				(v_rPsZ - v_rPfZm)*(v_rPsZ - v_rPfZm));

			if (v_rTm < v_rTp)
			{
				v_rotation.x = -v_rotation.x;
			};

			// rotateX p_psToTransP3D 
			double v_rPfYx = p_psToTransP3D.y*cos(-v_rotation.x) + p_psToTransP3D.z*sin(-v_rotation.x);
			double v_rPfZx = p_psToTransP3D.z*cos(-v_rotation.x) - p_psToTransP3D.y*sin(-v_rotation.x);

			//
			// calc rotY
			//
			double v_rPsX = p_psStableP3D.x;
			v_rPsZ = p_psStableP3D.z;
			double v_rPfX = p_psToTransP3D.x;
			v_rPfZ = v_rPfZx;

			v_rTemp = sqrt((v_rPsX - v_rPfX)*(v_rPsX - v_rPfX) +
				(v_rPsZ - v_rPfZ)*(v_rPsZ - v_rPfZ));

			v_rotation.y = 2.0*asin(v_rTemp*0.5);

			double v_rPfXp = v_rPfX*cos(-v_rotation.y) - v_rPfZ*sin(-v_rotation.y);
			v_rPfZp = v_rPfX*sin(-v_rotation.y) + v_rPfZ*cos(-v_rotation.y);
			v_rTp = sqrt((v_rPsX - v_rPfXp)*(v_rPsX - v_rPfXp) +
				(v_rPsZ - v_rPfZp)*(v_rPsZ - v_rPfZp));

			double v_rPfXm = v_rPfX*cos(v_rotation.y) - v_rPfZ*sin(v_rotation.y);
			v_rPfZm = v_rPfX*sin(v_rotation.y) + v_rPfZ*cos(v_rotation.y);
			v_rTm = sqrt((v_rPsX - v_rPfXm)*(v_rPsX - v_rPfXm) +
				(v_rPsZ - v_rPfZm)*(v_rPsZ - v_rPfZm));

			if (v_rTm < v_rTp)
			{
				v_rotation.y = -v_rotation.y;
			};
		};
	};

	return v_rotation;
}

CCVector3 qRobotAppPlugin::TransformVector(CCVector3 p_psVector, CCVector3 p_psTransformation)
{
	CCVector3 v_sP1, v_sP2;

	// rotationX
	v_sP1.x = p_psVector.x;
	v_sP1.y = p_psVector.y*cos(-p_psTransformation.x) + p_psVector.z*sin(-p_psTransformation.x);
	v_sP1.z = p_psVector.z*cos(-p_psTransformation.x) - p_psVector.y*sin(-p_psTransformation.x);

	// rotationY
	v_sP2.x = v_sP1.x*cos(-p_psTransformation.y) - v_sP1.z*sin(-p_psTransformation.y);
	v_sP2.y = v_sP1.y;
	v_sP2.z = v_sP1.x*sin(-p_psTransformation.y) + v_sP1.z*cos(-p_psTransformation.y);

	// rotationZ
	v_sP1.x = v_sP2.x*cos(p_psTransformation.z) - v_sP2.y*sin(p_psTransformation.z);
	v_sP1.y = v_sP2.x*sin(p_psTransformation.z) + v_sP2.y*cos(p_psTransformation.z);
	v_sP1.z = v_sP2.z;

	return v_sP1;
}

#ifndef CC_QT5
Q_EXPORT_PLUGIN2(qRobotAppPlugin,qRobotAppPlugin);
#endif
