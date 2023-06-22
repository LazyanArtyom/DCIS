#include <gui/coordinputdialog.h>

#include <QMessageBox>

CCoordInputDialog::CCoordInputDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("GeoCoordinate Dialog");

    QDoubleValidator* validator = new QDoubleValidator(0.0, 360.0, 8);
    m_pLeftTopLatEdit = new QLineEdit(this);
    m_pLeftTopLatEdit->setValidator(validator);
    m_pLeftTopLonEdit = new QLineEdit(this);
    m_pLeftTopLonEdit->setValidator(validator);
    m_pRightBottomLatEdit = new QLineEdit(this);
    m_pRightBottomLatEdit->setValidator(validator);
    m_pRightBottomLonEdit = new QLineEdit(this);
    m_pRightBottomLonEdit->setValidator(validator);

    QFormLayout *pFormLayout = new QFormLayout();
    pFormLayout->addRow("Left Top Latitude:", m_pLeftTopLatEdit);
    pFormLayout->addRow("Left Top Longitude:", m_pLeftTopLonEdit);
    pFormLayout->addRow("Right Bottom Latitude:", m_pRightBottomLatEdit);
    pFormLayout->addRow("Right Bottom Longitude:", m_pRightBottomLonEdit);

    m_pOkButton = new QPushButton("OK", this);
    m_pOkButton->setDisabled(true);

    auto slot = [&]() {
        m_pOkButton->setEnabled(!m_pLeftTopLatEdit->text().isEmpty() &&
                                !m_pLeftTopLonEdit->text().isEmpty() &&
                                !m_pRightBottomLatEdit->text().isEmpty() &&
                                !m_pRightBottomLonEdit->text().isEmpty());
    };

    connect(m_pLeftTopLatEdit, &QLineEdit::textEdited, this, slot);
    connect(m_pLeftTopLonEdit, &QLineEdit::textEdited, this, slot);
    connect(m_pRightBottomLatEdit, &QLineEdit::textEdited, this, slot);
    connect(m_pRightBottomLonEdit, &QLineEdit::textEdited, this, slot);

    connect(m_pOkButton, &QPushButton::clicked, this, &CCoordInputDialog::accept);

    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
    pMainLayout->addLayout(pFormLayout);
    pMainLayout->addWidget(m_pOkButton);
}

QString CCoordInputDialog::leftTopCoordinate() const
{
    QStringList oLeftTop;
    oLeftTop << m_pLeftTopLatEdit->text();
    oLeftTop << m_pLeftTopLonEdit->text();
    return oLeftTop.join(" ");
}

QString CCoordInputDialog::rightBottomCoordinate() const
{
    QStringList oRightBottom;
    oRightBottom << m_pRightBottomLatEdit->text();
    oRightBottom << m_pRightBottomLonEdit->text();
    return oRightBottom.join(" ");
}

void CCoordInputDialog::accept()
{
    // Perform any additional validation or checks here if needed
    QDialog::accept();
}
