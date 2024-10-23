#ifndef COORDINPUTDIALOG_H
#define COORDINPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QDoubleValidator>


class CoordInputDialog : public QDialog
{
  public:
    explicit CoordInputDialog(QWidget *parent = nullptr);
    QString leftTopCoordinate() const;
    QString rightBottomCoordinate() const;

  private:
    QPushButton *okButton_         = nullptr;
    QLineEdit *leftTopLatEdit_     = nullptr;
    QLineEdit *leftTopLonEdit_     = nullptr;
    QLineEdit *rightBottomLatEdit_ = nullptr;
    QLineEdit *rightBottomLonEdit_ = nullptr;

  private slots:
    void accept();
};

class DroneIpInputDialog : public QDialog
{
  public:
    explicit DroneIpInputDialog(QWidget *parent = nullptr);
    QString getIp() const;
    QString getPort() const;

  private:
    QPushButton *okButton_   = nullptr;
    QLineEdit *ipLineEdit_   = nullptr;
    QLineEdit *portLineEdit_ = nullptr;

  private slots:
    void accept();
};

#endif // COORDINPUTDIALOG_H
