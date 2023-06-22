#ifndef COORDINPUTDIALOG_H
#define COORDINPUTDIALOG_H


#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QDoubleValidator>


class CCoordInputDialog : public QDialog
{
public:
    explicit CCoordInputDialog(QWidget *parent = nullptr);
    QString leftTopCoordinate() const;
    QString rightBottomCoordinate() const;

private:
    QLineEdit*   m_pLeftTopLatEdit = nullptr;
    QLineEdit*   m_pLeftTopLonEdit = nullptr;
    QLineEdit*   m_pRightBottomLatEdit = nullptr;
    QLineEdit*   m_pRightBottomLonEdit = nullptr;
    QPushButton* m_pOkButton = nullptr;

private slots:
    void accept();
};

#endif // COORDINPUTDIALOG_H