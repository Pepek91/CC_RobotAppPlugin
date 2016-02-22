#pragma once
#include <qsettings.h>

/// Application settings class. Something else then robot/measurement head settings.
class RobotAppSettings
{
public:
	static RobotAppSettings& getInstance()
	{
		static RobotAppSettings instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	
	QVariant getSetting(QString group, QString key, const QVariant &defaultVal = QVariant())
	{
		QVariant ret = settings.value(group + QString("/") + key, defaultVal);
		return ret;
	};

	void setSetting(QString group, QString key, QVariant &value){ settings.setValue(group + QString("/") + key, value); };
private:
	QSettings settings;


	RobotAppSettings()
		: settings("QRobotAppPlugin/settings.ini", QSettings::IniFormat)
	{};

	RobotAppSettings(RobotAppSettings const&) = delete;
	void operator=(RobotAppSettings const&) = delete;
};

