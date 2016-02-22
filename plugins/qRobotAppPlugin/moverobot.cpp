#include "moverobot.h"
#include "ui_moverobot.h"
#include <qmessagebox.h>

MoveRobot::MoveRobot(Connection* con, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveRobot),
	m_con(con)
{
    ui->setupUi(this);

	connect(ui->pushButton_J1_min, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J1_plus, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J2_min, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J2_plus, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J3_min, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J3_plus, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J4_min, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J4_plus, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J5_min, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J5_plus, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J6_min, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J6_plus, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J7_min, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(ui->pushButton_J7_plus, SIGNAL(pressed()), this, SLOT(buttonPressed()));

	connect(ui->verticalSlider, SIGNAL(valueChanged(int)), this, SLOT(updateShiftValue()));
	connect(ui->radioButtonJoints, SIGNAL(toggled(bool)), this, SLOT(updateShiftValue()));
	connect(ui->radioButtonXYZ, SIGNAL(toggled(bool)), this, SLOT(updateShiftValue()));
}

MoveRobot::~MoveRobot()
{
    delete ui;
}

void MoveRobot::buttonPressed()
{
	if(this->sender() == ui->pushButton_J1_min)
	{
		moveRobot(0, false);
	}
	else if(this->sender() == ui->pushButton_J1_plus)
	{
		moveRobot(0, true);
	}
	else if(this->sender() == ui->pushButton_J2_min)
	{
		moveRobot(1, false);
	}
	else if(this->sender() == ui->pushButton_J2_plus)
	{
		moveRobot(1, true);
	}
	else if(this->sender() == ui->pushButton_J3_min)
	{
		moveRobot(2, false);
	}
	else if(this->sender() == ui->pushButton_J3_plus)
	{
		moveRobot(2, true);
	}
	else if(this->sender() == ui->pushButton_J4_min)
	{
		moveRobot(3, false);
	}
	else if(this->sender() == ui->pushButton_J4_plus)
	{
		moveRobot(3, true);
	}
	else if(this->sender() == ui->pushButton_J5_min)
	{
		moveRobot(4, false);
	}
	else if(this->sender() == ui->pushButton_J5_plus)
	{
		moveRobot(4, true);
	}
	else if(this->sender() == ui->pushButton_J6_min)
	{
		moveRobot(5, false);
	}
	else if(this->sender() == ui->pushButton_J6_plus)
	{
		moveRobot(5, true);
	}
	else if(this->sender() == ui->pushButton_J7_min)
	{
		moveRobot(6, false);
	}
	else if(this->sender() == ui->pushButton_J7_plus)
	{
		moveRobot(6, true);
	}
}

void MoveRobot::moveRobot(int joint, bool positive)
{
	//jointy 0.1 - 10
	//xyz 1-100

	bool joints = ui->radioButtonJoints->isChecked();
	double multiplier = (joints ? 0.1 : 1) * (positive ? 1 : -1);
	double value = multiplier * ui->verticalSlider->value();
	m_con->requestRobotToMove(value, joint, joints);
}

void MoveRobot::updateShiftValue()
{
	double multiplier = (ui->radioButtonJoints->isChecked() ? 0.1 : 1);
	double value = multiplier * ui->verticalSlider->value();
	QString shift = "Shift: " + QString::number(value);

	ui->label_shift->setText(shift);
}