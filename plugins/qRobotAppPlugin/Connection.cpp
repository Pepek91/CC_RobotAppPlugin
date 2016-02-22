#include "Connection.h"
#include <QString>
#include <QXmlStreamWriter>
#include <ccPointCloud.h>
#include <ccGLWindow.h>
#include <ccNormalVectors.h>

Connection::Connection()
	: m_idConnection(false)
	, m_app(0)
	, m_uiBlockSize(0)
	, m_dXYZ(0)
	, m_ucRGB(0)
	, m_dNorm(0)
	, m_iScanID(0)
	, m_sessionName("")
	, m_serverState(ServerStateEnum::Ready)
	, m_timer(new QTimer(this))
{
	connect(&m_tcpSocket, &QTcpSocket::connected, this, &Connection::onConnected);
	connect(&m_tcpSocket, &QTcpSocket::disconnected, this, &Connection::onDisconect);
	connect(&m_tcpSocket, &QTcpSocket::readyRead, this, &Connection::readFromServer);
}

Connection::~Connection()
{
	m_tcpSocket.close();

	if (m_dXYZ)
	{
		delete[] m_dXYZ;
		m_dXYZ = 0;
	}
	if (m_ucRGB)
	{
		delete[] m_ucRGB;
		m_ucRGB = 0;
	}

	delete m_timer;
}

bool Connection::connectToServer(QString ip, quint16 port, QString sessionName)
{
	m_sessionName = sessionName;
	m_tcpSocket.connectToHost(ip, port);
	m_idConnection = m_tcpSocket.waitForConnected(5000);
	return m_idConnection;
}

void Connection::onConnected()
{
	m_idConnection = true;
	m_app->dispToConsole("[qRobotAppPlugin] Connected to server!", ccMainAppInterface::STD_CONSOLE_MESSAGE);
	sendSessionName(m_sessionName);
	m_iScanID = 0;
}
void Connection::onDisconect()
{
	m_idConnection = false;
	m_app->dispToConsole("[qRobotAppPlugin] Server disconnected", ccMainAppInterface::STD_CONSOLE_MESSAGE);
}

void Connection::sendSessionName(QString sessionName)
{
	if (m_tcpSocket.isOpen())
	{
		QString xmlQS;
		QXmlStreamWriter textWriter(&xmlQS);
		//m_xmlWriter.setDevice(&m_tcpSocket);

		textWriter.writeStartDocument();
		textWriter.writeStartElement(SESSION_NAME);
		textWriter.writeCharacters(sessionName);
		// end tag SESSION_NAME
		textWriter.writeEndElement();
		// endm document
		textWriter.writeEndDocument();

		sendXMLData(xmlQS);
	}
	scanState.setWaitingForScan(true);
}

void Connection::setMainAppIfNotSet(ccMainAppInterface* app)
{
	if (m_app == 0)
		m_app = app;
}

void Connection::disconnect(bool close)
{
	if(close)
	{
		if (m_tcpSocket.isOpen())
		{
			QString xmlQS;
			QXmlStreamWriter textWriter(&xmlQS);
			//m_xmlWriter.setDevice(&m_tcpSocket);
 
			textWriter.writeStartDocument();
			textWriter.writeStartElement(CLOSE_QSTR);
			// end tag close
			textWriter.writeEndElement();
			// end document
			textWriter.writeEndDocument();

			sendXMLData(xmlQS);
		}
	}
	else
	{
		m_tcpSocket.close();
	}
}

void Connection::requestFastScan()
{
	if (m_tcpSocket.isOpen())
	{
		QString xmlQS;
		QXmlStreamWriter textWriter(&xmlQS);

		//m_xmlWriter.setDevice(&m_tcpSocket);
 
		textWriter.writeStartDocument();
		textWriter.writeStartElement(SCAN_QSTR);
		textWriter.writeStartElement(FAST_QSTR);
		// close tag fast
		textWriter.writeEndElement();
		// end tag scan
		textWriter.writeEndElement();
		// endm document
		textWriter.writeEndDocument();

		scanState.setWaitingForScan(true);

		sendXMLData(xmlQS);
	}
}

void Connection::requestStopAction()
{
	QString xmlQS;
	QXmlStreamWriter textWriter(&xmlQS);

	if (m_tcpSocket.isOpen())
	{
		//textWriter.setDevice(&m_tcpSocket);

		textWriter.writeStartDocument();
		textWriter.writeStartElement(STOP_SCANNING);
		// end tag STOP_SCANNING
		textWriter.writeEndElement();
		// end document
		textWriter.writeEndDocument();
	}
	scanState.setWaitingForScan(true);

	sendXMLData(xmlQS);
}

void Connection::requestScanNow()
{
	QString xmlQS;
	QXmlStreamWriter textWriter(&xmlQS);

	if (m_tcpSocket.isOpen())
	{
		//textWriter.setDevice(&m_tcpSocket);

		textWriter.writeStartDocument();
		textWriter.writeStartElement(SCAN_QSTR);
		textWriter.writeStartElement(SCAN_NOW);
		// end tag SCAN_NOW
		textWriter.writeEndElement();
		// end tag SCAN_QSTR
		textWriter.writeEndElement();
		// end document
		textWriter.writeEndDocument();
	}
	scanState.setWaitingForScan(true);

	sendXMLData(xmlQS);
}

void Connection::requestAccurateScan(std::vector<CCVector3> xyz, std::vector<CCVector3> norm_xyz)
{
	QString xmlQS;
	QXmlStreamWriter textWriter(&xmlQS);
	int numberOfPoinToInspecta = xyz.size();

	textWriter.writeStartDocument();
	textWriter.writeStartElement(SCAN_QSTR);
	textWriter.writeStartElement(ACCURATE_QSTR);
	for (int i = 0; i < numberOfPoinToInspecta; ++i)
	{
		textWriter.writeStartElement(NEXT_SCAN_QSTR);
		textWriter.writeAttribute("xyz", QString::number(xyz.at(i).x) +
			QString(" ") + QString::number(xyz.at(i).y) +
			QString(" ") + QString::number(xyz.at(i).z));

		textWriter.writeAttribute("nxyz", QString::number(norm_xyz.at(i).x) +
			QString(" ") + QString::number(norm_xyz.at(i).y) +
			QString(" ") + QString::number(norm_xyz.at(i).z));

		// close tag scan_info
		textWriter.writeEndElement();
	}
	// close tag accurate
	textWriter.writeEndElement();
	// end tag scan
	textWriter.writeEndElement();
	// end document 
	textWriter.writeEndDocument();

	sendXMLData(xmlQS);
}

void Connection::requestLoadSignlePosition(QString& singlePositionFileContent)
{
	QString xmlQS;
	QXmlStreamWriter textWriter(&xmlQS);


	textWriter.writeStartDocument();
	textWriter.writeStartElement(SINGLE_POSITION);

	textWriter.writeCharacters(singlePositionFileContent);

	// end tag SINGLE_POSITION
	textWriter.writeEndElement();
	// end document 
	textWriter.writeEndDocument();

	sendXMLData(xmlQS);
}

void Connection::requestLoadingPrescan(QString& prescanFileContent)
{
	QString xmlQS;
	QXmlStreamWriter textWriter(&xmlQS);


	textWriter.writeStartDocument();
	textWriter.writeStartElement(LOAD_PRESCAN);

	textWriter.writeCharacters(prescanFileContent);

	// end tag LOAD_PRESCAN
	textWriter.writeEndElement();
	// end document 
	textWriter.writeEndDocument();

	sendXMLData(xmlQS);
}

void Connection::readFromServer()
{
	QDataStream in(&m_tcpSocket);
	in.setVersion(QDataStream::Qt_5_4);

	quint64 bytesAvailable = m_tcpSocket.bytesAvailable();

	m_app->dispToConsole(QString("[qRobotAppPlugin] Reading data from server, %1!").arg(bytesAvailable), ccMainAppInterface::STD_CONSOLE_MESSAGE);

	if (m_uiBlockSize == 0) {
		if (bytesAvailable < (int)(sizeof(quint64) + sizeof(unsigned char)))
			return;

		in >> m_uiBlockSize;
		in >> m_uiCurrentScanType;

		//m_timer->singleShot(2000, this, SLOT(resetTCPConnection));
	}
	if (bytesAvailable < m_uiBlockSize)
		return;

	//m_timer->stop();

	/*to do*
	if (m_uiCurrentScanType == SERVER_STATE)
	{
		unsigned char serverState;
		QString msg;
		in >> serverState;
		in >> msg;
		m_serverState = (ServerStateEnum) serverState;
		if (msg.length())
		{
			m_app->dispToConsole(msg, ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		}
		m_uiBlockSize = 0;
		return;
	}
	/**/

	unsigned int dataSize;
	if (m_uiCurrentScanType == XZY_DATA)
	{
		dataSize = static_cast<unsigned int>(m_uiBlockSize / sizeof(double));
		scanState.setDataSize(dataSize);
	}
	else
	{
		dataSize = scanState.getDataSize();
	}

	m_uiBlockSize = 0;

	if (m_uiCurrentScanType == XZY_DATA)
	{
		if (m_dXYZ)
			delete m_dXYZ;

		m_dXYZ = new double[dataSize];

		for (int i = 0; i < dataSize; ++i)
		{
			in >> m_dXYZ[i];
		}

		scanState.setXYZReceived(true);
	}
	else if (m_uiCurrentScanType == RGB_DATA)
	{
		if (m_ucRGB)
			delete m_ucRGB;

		m_ucRGB = new unsigned char[dataSize];

		for (int i = 0; i < dataSize; ++i)
		{
			in >> m_ucRGB[i];
		}
		scanState.setRBGReceived(true);
	}
	else if (m_uiCurrentScanType == NORM_DATA)
	{
		if (m_dNorm)
			delete m_dNorm;

		m_dNorm = new double[dataSize];

		for (int i = 0; i < dataSize; ++i)
		{
			in >> m_dNorm[i];
		}
		scanState.setNormReceived(true);
	}
	else
	{
		m_app->dispToConsole("[qRobotAppPlugin] Data corrupted? Don't know what is this.", ccMainAppInterface::STD_CONSOLE_MESSAGE);
		m_tcpSocket.readAll();
		return;
	}

	m_app->dispToConsole("[qRobotAppPlugin] Data received", ccMainAppInterface::STD_CONSOLE_MESSAGE);

	if (scanState.isRBGReceived() && scanState.isXYZReceived() && scanState.isNormReceived())
	{
		if (createCloudOfPointsFromData())
		{
			m_app->dispToConsole("[qRobotAppPlugin] Cloud created.", ccMainAppInterface::STD_CONSOLE_MESSAGE);
		}
		else
		{
			m_app->dispToConsole("[qRobotAppPlugin] Could not create cloud.", ccMainAppInterface::STD_CONSOLE_MESSAGE);
		}
	}
}

void Connection::resetTCPConnection()
{
	// to do somehow
	m_app->dispToConsole("[qRobotAppPlugin] resetTCPConnection", ccMainAppInterface::STD_CONSOLE_MESSAGE);
}

bool Connection::createCloudOfPointsFromData()
{
	ccHObject* dbRoot = m_app->dbRootObject();
	ccHObject::Container objects;
	ccHObject* container = NULL;
	bool newContainer = true;
	if (dbRoot->filterChildren(objects, false, CC_TYPES::OBJECT))
	{
		for (size_t i = 0; i < objects.size(); ++i)
		{
			if (objects[i]->getName().compare(m_sessionName) == 0)
			{
				container = objects[i];
				newContainer = false;
			}
		}
	}
	if (!container)
		container = new ccHObject(m_sessionName);

	ccPointCloud* cc = new ccPointCloud(m_sessionName + QString(" cloud ") + QString::number(m_iScanID));
	unsigned int iDataSize = scanState.getDataSize()/3;

	if (iDataSize <= 0)
	{
		m_app->dispToConsole("[qRobotAppPlugin] Cloud have zero points. Nothing to create!", ccMainAppInterface::WRN_CONSOLE_MESSAGE);
		return false;
	}
	
	if (!cc->reserveThePointsTable(iDataSize) || !cc->reserveTheRGBTable() || !cc->reserveTheNormsTable())
	{
		m_app->dispToConsole("[qRobotAppPlugin] Could not reserve memory for cloud.", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return false;
	}

	cc->showColors(true);
	cc->showNormals(true);

	for (unsigned int i = 0; i < iDataSize; ++i)
	{
		cc->addPoint(CCVector3(m_dXYZ[3 * i + 0], m_dXYZ[3 * i + 1], m_dXYZ[3 * i + 2]));
		cc->addRGBColor(m_ucRGB[3 * i + 0], m_ucRGB[3 * i + 1], m_ucRGB[3 * i + 2]);
		cc->addNorm(CCVector3(m_dNorm[3 * i + 0], m_dNorm[3 * i + 1], m_dNorm[3 * i + 2]));
	}

	cc->invalidateBoundingBox();
	cc->setDisplay(m_app->getActiveGLWindow());
	cc->setCurrentDisplayedScalarField(0);
	cc->showSF(true);

	if (cc->size() < cc->capacity())
		cc->resize(cc->size());

	container->addChild(cc);

	container->resetGLTransformationHistory_recursive();
	if (newContainer)
	{
		m_app->addToDB(container, true, true, false);
	}
	else
	{
		m_app->removeFromDB(container, false);
		m_app->addToDB(container, true, true, false);
	}
	scanState.setXYZReceived(false);
	scanState.setRBGReceived(false);
	scanState.setNormReceived(false);

	++m_iScanID;

	return true;
}

void Connection::requestCalibration(int head, int type)
{

	QString xmlQS;
	QXmlStreamWriter textWriter(&xmlQS);

	//textWriter.setDevice(&m_tcpSocket);

	textWriter.writeStartDocument();
	textWriter.writeStartElement(CALIBRATIN_QSTR);
	textWriter.writeAttribute(CALIB_HEAD_QSTR, QString::number(head));
	textWriter.writeAttribute(CALIB_TYPE_QSTR, QString::number(type));
	// end tag CALIBRATIN_QSTR
	textWriter.writeEndElement();
	// end document
	textWriter.writeEndDocument();

	sendXMLData(xmlQS);
}

void Connection::requestRobotToMove(double p_value, int p_coord, bool p_joints)
{
	QString xmlQS;
	QXmlStreamWriter textWriter(&xmlQS);

	//textWriter.setDevice(&m_tcpSocket);

	textWriter.writeStartDocument();
	QString toSend = QString::number(p_value) + " " + QString::number(p_coord) + " " + QString::number(p_joints);
	textWriter.writeStartElement(MOVE_ROBOT_QSTR);
	textWriter.writeAttribute(MOVE_ROBOT_ATTRIBUTE_QSTR, toSend);
	// end tag MOVE_ROBOT_QSTR
	textWriter.writeEndElement();
	// end document
	textWriter.writeEndDocument();

	sendXMLData(xmlQS);
}

void Connection::sendXMLData(QString &xmlData)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_4);
	// ************************************* DATA ***************************************
	out << (quint64)0;
	out << XML_DATA;
	out << xmlData;
	quint64 size = (quint64)(block.size() - sizeof(quint64) - sizeof(unsigned char));
	out.device()->seek(0);
	out << size;
	m_tcpSocket.write(block);
	m_tcpSocket.flush();
	m_tcpSocket.waitForBytesWritten();
	m_serverState = ServerStateEnum::NotReady;
}