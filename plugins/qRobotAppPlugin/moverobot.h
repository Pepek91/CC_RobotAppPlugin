#ifndef MOVEROBOT_H
#define MOVEROBOT_H

#include <QDialog>

#include "Connection.h"

namespace Ui {
class MoveRobot;
}

class MoveRobot : public QDialog
{
    Q_OBJECT

public:
    explicit MoveRobot(Connection* con, QWidget *parent = 0);
    ~MoveRobot();

private:
    Ui::MoveRobot *ui;
	Connection* m_con;

	void moveRobot(int joint, bool positive);

private slots:
	void buttonPressed();
	void updateShiftValue();
};

#endif // MOVEROBOT_H
