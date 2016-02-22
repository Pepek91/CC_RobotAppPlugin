#include "calibration.h"
#include "ui_calibration.h"
#include "utils.h"

Calibration::Calibration(int* opt1, int* opt2, bool* chosen, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Calibration)
	, m_head(opt1)
	, m_type(opt2)
	, m_chosen(chosen)
{
    ui->setupUi(this);

	ui->comboBox_Scaner->addItem("Accurate", QVariant((int)CalibrationEnum::Accurate));
	ui->comboBox_Scaner->addItem("Fast", QVariant((int)CalibrationEnum::Fast));

	ui->comboBox_Cal_Type->addItem("Robot", QVariant((int)CalibrationEnum::Robot));
	ui->comboBox_Cal_Type->addItem("Stage", QVariant((int)CalibrationEnum::Stage));
	//head type does not mater, both are used
	ui->comboBox_Cal_Type->addItem("Cross-head", QVariant((int)CalibrationEnum::Crosshead));
	ui->comboBox_Cal_Type->addItem("Head", QVariant((int)CalibrationEnum::Head));

	setWindowTitle("Calibration");
}

void Calibration::accept()
{
	QDialog::accept();
	(*m_head) = ui->comboBox_Scaner->itemData(ui->comboBox_Scaner->currentIndex()).toInt();
	(*m_type) = ui->comboBox_Cal_Type->itemData(ui->comboBox_Cal_Type->currentIndex()).toInt();
	(*m_chosen) = true;
}

Calibration::~Calibration()
{
    delete ui;
}
