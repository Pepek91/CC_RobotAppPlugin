//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qRobotAppPlugin             #
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
//#                     COPYRIGHT: Michał Stępień				           #
//#                                                                        #
//##########################################################################

#ifndef Q_MY_PLUGIN_PLUGIN_HEADER
#define Q_MY_PLUGIN_PLUGIN_HEADER

//Qt
#include <QObject>

//qCC_db
#include <ccPolyline.h>
#include <PointProjectionTools.h>

#include "Connection.h"

class qRobotAppPlugin : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccStdPluginInterface)
#ifdef CC_QT5
	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qRobotAppPlugin")
#endif

public:

	/// <summary>
	/// Default constructor: should mainly be used to initialize
	/// actions (pointers) and other members
	/// </summary>
	/// <param name="parent">Parent object pointer</param>
	qRobotAppPlugin(QObject* parent=0);

	/// <summary>
	/// Default destructor
	/// </summary>
	~qRobotAppPlugin();


	//inherited from ccPluginInterface
	QString getNameTCP() const { return "Connect/Disconnect"; }
	QString getNameFast() const { return "Fast scan"; }
	QString getNameAccurateVec() const { return "Set vector for accurate scan"; }
	QString getNameAccurate() const { return "Accurate scan"; }
	QString getNameCalibration() const { return "Calibration"; }
	QString getNameMoveRobot() const { return "Move robot"; }
	QString getNameSendNBV() const { return "Send NBV"; }

	QString getDescriptionTCP() const { return "Connect/Disconnects from robot measurement server."; }
	QString getDescriptionFast() const { return "Sends request to server to do fast scan from set points."; }
	QString getDescriptionAccurateVec() const { return "Sets point and normal vector that will be send to server as point for accurate scan."; }
	QString getDescriptionAccurate() const { return "Sends request to server to do accurate scan in set point."; }
	QString getDescriptionCalibration() const { return "Runs calibration on server"; }
	QString getDescriptionMoveRobot() const { return "Shows dialog responsible for moving robot"; }
	QString getDescriptionSendNBV() const { return "Pick next best view file from hard drive to send it to robot"; }
	
	QIcon getIconTCP() const;
	QIcon getIconFast() const;
	QIcon getIconAccurateVec() const;
	QIcon getIconAccurate() const;
	QIcon getIconCalibration() const;
	QIcon getIconMoveRobot() const;
	QIcon getIconSendNBV() const;
	QIcon getIconStop() const;
	QIcon getIconScanNow() const;
	QIcon getIconPrescanFromPoints() const;
	QIcon getIconLoadSinglePosition() const;
	QIcon getIconLoadPrescan() const;

	/// <summary>
	/// Returns plugin name.
	/// </summary>
	/// <returns>Plugin name.</returns>
	virtual QString getName() const { return "OGXRobotApp Plugin"; }
	/// <summary>
	/// Returns plugin description.
	/// </summary>
	/// <returns>Plugin description.</returns>
	virtual QString getDescription() const { return "Allows to connect and sends scan request to OGX measurement server."; }
	
	/// <summary>
	/// This method returns the plugin main icon.
	/// </summary>
	virtual QIcon getIcon() const;

	
	/// <summary>
	/// This method should enable or disable each plugin action
	/// depending on the currently selected entities ('selectedEntities').
	/// For example: if none of the selected entities is a cloud, and your
	/// plugin deals only with clouds, call 'm_action->setEnabled(false)'
	/// inherited from ccStdPluginInterface
	/// </summary>
	/// <param name="selectedEntities">Selected entities container</param>
	void onNewSelection(const ccHObject::Container& selectedEntities);
	/// <summary>
	/// This method returns all 'actions' of your plugin.
	/// It will be called only once, when plugin is loaded.
	/// </summary>
	/// <param name="group">Group object which will store all the actions</param>
	virtual void getActions(QActionGroup& group);

protected slots:

	//**** ADD YOUR CUSTOM ACTIONS' SLOTS HERE ****
	/// <summary>
	/// TCP connection action slot. It manages TCP connection between plugin (client) and robot/measurement head server.
	/// </summary>
	void doActionTCP();
	/// <summary>
	/// Fast scan action slot. Request fast, predefined scan from robot/measurement server.
	/// </summary>
	void doActionFastScan();
	/// <summary>
	/// Accurate scan action slot. Request accurate, precisely defined scan from robot/measurement server.
	/// </summary>
	void doActionAccurateScan();
	/// <summary>
	/// Set accurate scan vector action slot. Sets accurate scan vector set in scan point.
	/// Requires point 2d label to be selected.
	/// </summary>
	void doActionSetAccurateScanVec();
	/// <summary>
	/// Runs calibration on server
	/// </summary>
	void doActionCalibration();
	/// <summary>
	/// Action responsible for moving robot
	/// </summary>
	void doActionMoveRobot();
	/// <summary>
	/// Action responsible for sending NBV
	/// </summary>
	void doActionSendNBV();
	/// <summary>
	/// Action responsible for sending stop signal to server
	/// </summary>
	void doActionStop();
	/// <summary>
	/// Action responsible for sending signal to perform scan from current position
	/// </summary>
	void doActionScanNow();
	/// <summary>
	/// Action generates NBV from selected points
	/// </summary>
	void doActionPrescanFromPoints();
	/// <summary>
	/// Load single robot position
	/// </summary>
	void doActionLoadSinglePosition();
	/// <summary>
	/// Load prescan
	/// </summary>
	void doActionLoadPrescan();

protected:
	// ******************* Actions
	QAction* m_fastScann;
	QAction* m_tcpConnection;
	QAction* m_setAccurateScannVector;
	QAction* m_accurateScann;
	QAction* m_calibration;
	QAction* m_moveRobot;
	QAction* m_sendNBV;
	QAction* m_stopScan;
	QAction* m_scanNow;
	QAction* m_prescanFromPoints;
	QAction* m_loadSinglePosition;
	QAction* m_loadPrescan;

	std::vector<CCVector3> m_vAccurateXYZ;
	std::vector<CCVector3> m_vAccurateNormalsXYZ;
	bool m_bAccurateScannPointSet;

    Connection* m_connectionManager;

	CCVector3 CalcRotationformation3DToCoverTwoVectors(CCVector3 p_psStableP3D, CCVector3 p_psToTransP3D);

	CCVector3 TransformVector(CCVector3 p_psVector, CCVector3 p_psTransformation);
};

#endif
