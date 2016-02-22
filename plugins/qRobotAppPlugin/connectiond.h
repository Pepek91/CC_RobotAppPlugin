#ifndef CONNECTIOND_H
#define CONNECTIOND_H

#include <QDialog>

namespace Ui {
class ConnectionD;
}

class ConnectionD : public QDialog
{
    Q_OBJECT

public:
    struct ConnectionData{
        QString IP;
        quint16 Port;
		QString sessionName;
        bool set;
		QString message;
    };
    explicit ConnectionD(ConnectionData* conData, QWidget *parent = 0);
    ~ConnectionD();

public slots:
	void accept();

private:
    Ui::ConnectionD *ui;
    ConnectionData* m_connData;
	
};

#endif // CONNECTIOND_H
