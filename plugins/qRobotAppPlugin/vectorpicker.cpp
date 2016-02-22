#include "vectorpicker.h"

VectorPicker::VectorPicker(CCVector3 *defaultVector, bool *cancel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VectorPicker),
	m_vector(defaultVector),
	m_bCancel(cancel)
{
    ui->setupUi(this);

	ui->lineEditX->setText(QString::number(m_vector->x));
	ui->lineEditY->setText(QString::number(m_vector->y));
	ui->lineEditZ->setText(QString::number(m_vector->z));
}

void VectorPicker::accept()
{
	m_vector->x = ui->lineEditX->text().toDouble();
	m_vector->y = ui->lineEditY->text().toDouble();
	m_vector->z = ui->lineEditZ->text().toDouble();
	(*m_bCancel) = false;

	QDialog::accept();
}
void VectorPicker::reject()
{
	(*m_bCancel) = true;

	QDialog::reject();
}

VectorPicker::~VectorPicker()
{
    delete ui;
}
