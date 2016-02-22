#ifndef Q_CONNECTION_HEADER
#define Q_CONNECTION_HEADER

#include "../ccStdPluginInterface.h"
#include <QtNetwork>
#include "ScanState.h"

class Connection : public QObject
{
	Q_OBJECT
public:
	explicit Connection();
	~Connection();
	bool isConnected() { return m_idConnection; }
	/// <summary>
	/// Connects to robot/measurement server
	/// </summary>
	/// <param name="ip">Server IP address</param>
	/// <param name="port">Server port</param>
	/// <returns>Whether connection was successful</returns>
	bool connectToServer(QString ip, quint16 port, QString sessionName);
	void disconnect(bool close = false);
	void setMainAppIfNotSet(ccMainAppInterface* app);

	void requestFastScan();
	void requestAccurateScan(std::vector<CCVector3> xyz, std::vector<CCVector3> norm_xyz);
	void requestCalibration(int head, int type);
	void requestRobotToMove(double p_value, int p_coord, bool p_joints);
	void requestStopAction();
	void requestScanNow();
	void requestLoadSignlePosition(QString& singlePositionFileContent);
	void requestLoadingPrescan(QString& prescanFileContent);

	ServerStateEnum getServerStatus(){ return m_serverState; }

private:
	/// 
	QTcpSocket m_tcpSocket;
	ccMainAppInterface* m_app;

	QString m_sessionName;

	bool m_idConnection;

	quint64 m_uiBlockSize;

	unsigned char m_uiCurrentScanType;

	ScanState scanState;

	double* m_dXYZ;
	unsigned char* m_ucRGB;
	double* m_dNorm;
	unsigned int m_iScanID;

	ServerStateEnum m_serverState;

	QTimer* m_timer;

	bool createCloudOfPointsFromData();
	void sendSessionName(QString sessionName);

	void sendXMLData(QString &xmlData);
private Q_SLOTS :
	void onConnected();
	void onDisconect();
	void readFromServer();
	void resetTCPConnection();
};

#endif
