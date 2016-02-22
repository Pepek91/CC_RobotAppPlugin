#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QDialog>

namespace Ui {
class Calibration;
}

class Calibration : public QDialog
{
    Q_OBJECT

public:
    explicit Calibration(int* opt1, int* opt2, bool* chosen, QWidget *parent = 0);
    ~Calibration();

private slots:
		void accept();

private:
    Ui::Calibration *ui;

	int* m_head;
	int* m_type;
	bool* m_chosen;
};

#endif // CALIBRATION_H
