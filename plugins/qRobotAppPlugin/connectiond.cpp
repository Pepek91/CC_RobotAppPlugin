#include "connectiond.h"
#include "ui_connectiond.h"
#include <qdatetime.h>
#include <qRobotAppSettings.h>

ConnectionD::ConnectionD(ConnectionData* conData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionD),
    m_connData(conData)
{
	m_connData->message = QString();
    ui->setupUi(this);
	QString ipAndPort = RobotAppSettings::getInstance().getSetting("SERVER", "IP_AND_PORT", "127.000.000.001:1234").toString();
	ui->IPandPortInput->setInputMask("000.000.000.000:0000");
	ui->IPandPortInput->setText(ipAndPort);
	QDateTime current = QDateTime(QDateTime::currentDateTime());
	ui->lineEditSessionName->setText(QString("Scanning-session-") + current.toString(QString("yyyyMMddHHmmss")));
}

ConnectionD::~ConnectionD()
{
    delete ui;
}

void  ConnectionD::accept()
{
	QString qsConnectionData = ui->IPandPortInput->text();
	QStringList qslIPandPort = qsConnectionData.split(":");

	try
	{
		bool bConv;
		quint16 iPort = qslIPandPort[1].toUShort(&bConv);
		if (!bConv || iPort < 0 || 65535 < iPort)
		{
			throw "Proper port range is 0-65535, you have: " + std::to_string(iPort);
		}
		m_connData->Port = iPort;
		QStringList qslIPParts =  qslIPandPort[0].split(".");
		foreach(QString qsIpPart, qslIPParts){
			int iIPPart = qsIpPart.toInt(&bConv);
			if (!bConv || iIPPart < 0 || 255 < iIPPart)
			{
				throw "Proper ip range is 0-255, you have: " + std::to_string(iIPPart);
			}
		}
		m_connData->IP = qslIPandPort[0];
		m_connData->set = true;
		m_connData->message = QString("Connection parameters: " + qsConnectionData);

		m_connData->sessionName = ui->lineEditSessionName->text();

		QString ipAndPort = m_connData->IP + ":" + QString::number(m_connData->Port);

		if (RobotAppSettings::getInstance().getSetting("SERVER", "IP_AND_PORT", "127.000.000.001:1234").compare(ipAndPort) != 0)
		{
			RobotAppSettings::getInstance().setSetting("SERVER", "IP_AND_PORT", QVariant(ipAndPort));
		}
	}
	catch (std::string obj)
	{
		m_connData->message = QString::fromStdString(obj);
		m_connData->set = false;
	}

	QDialog::accept();
}