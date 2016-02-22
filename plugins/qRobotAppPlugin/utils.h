#ifndef UTILS_H
#define UTILS_H

#include <QString>

const QString ACCURATE_QSTR = "accurate";
const QString FAST_QSTR = "fast";
const QString CLOSE_QSTR = "close";
const QString SCAN_QSTR = "scan";
const QString NEXT_SCAN_QSTR = "next_scan";
const QString CALIBRATIN_QSTR = "calibration";
const QString CALIB_HEAD_QSTR = "head";
const QString CALIB_TYPE_QSTR = "type";
const QString CONFIG_QSTR = "config";
const QString MOVE_ROBOT_QSTR = "move_robot";
const QString MOVE_ROBOT_ATTRIBUTE_QSTR = "move_robot_atribute";
const QString SESSION_NAME = "session_name";
const QString STOP_SCANNING = "stop_scanning";
const QString SCAN_NOW = "scan_now";
const QString SINGLE_POSITION = "single_position";
const QString LOAD_PRESCAN = "load_prescan";

const unsigned char XZY_DATA = 1;
const unsigned char RGB_DATA = 2;
const unsigned char QSTR_DATA = 3;
const unsigned char NORM_DATA = 4;
const unsigned char CONFIG_DATA = 5;
const unsigned char XML_DATA = 6;
const unsigned char SERVER_STATE = 7;

enum class CalibrationEnum
{ 
	Accurate = 0, Fast,				//head types
	Robot, Stage, Crosshead, Head	//calibration types
};

enum class ServerStateEnum : unsigned char
{
	Ready = 0,		// server is not doing anything
	StopReady,		// server had processed last request and it is in progress
	NotReady		// server is processing CC request
};

struct sConfigData{
	int iNumberOfPrescans;
};

struct sVector3D{
    double x;
    double y;
    double z;
};

#endif // UTILS_H
