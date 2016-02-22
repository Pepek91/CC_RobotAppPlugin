#ifndef VECTORPICKER_H
#define VECTORPICKER_H

#include <QDialog>
#include <CCGeom.h>
#include "ui_vectorpicker.h"

namespace Ui {
class VectorPicker;
}

class VectorPicker : public QDialog
{
    Q_OBJECT

public:
	explicit VectorPicker(CCVector3 *defaultVector, bool *cancel, QWidget *parent = 0);
    ~VectorPicker();

private:
    Ui::VectorPicker *ui;

	CCVector3* m_vector;
	bool* m_bCancel;

private slots:
	void accept();
	void reject();
};

#endif // VECTORPICKER_H
