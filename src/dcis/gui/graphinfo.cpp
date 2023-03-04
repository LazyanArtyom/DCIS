#include <gui/graphinfo.h>

// QT includes
#include <QVBoxLayout>
#include <QHBoxLayout>


namespace dcis::gui {

GraphInfo::GraphInfo(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* vLayoutMain = new QVBoxLayout();
    setLayout(vLayoutMain);

    // is graph directed
    QHBoxLayout* layoutDirected = new QHBoxLayout();
    vLayoutMain->addLayout(layoutDirected);
    QLabel* lblDirected = new QLabel(this);
    lblDirected->setText(tr("IsDirected: "));
    layoutDirected->addWidget(lblDirected);
    txtDirected_ = new QLineEdit(this);
    txtDirected_->setDisabled(true);
    layoutDirected->addWidget(txtDirected_);

    // edge count
    QHBoxLayout* layoutEdgeCount = new QHBoxLayout();
    vLayoutMain->addLayout(layoutEdgeCount);
    QLabel* lblEdgeCount = new QLabel(this);
    lblEdgeCount->setText(tr("EdgeCount: "));
    layoutEdgeCount->addWidget(lblEdgeCount);
    txtEdgeCount_ = new QLineEdit(this);
    txtEdgeCount_->setDisabled(true);
    layoutEdgeCount->addWidget(txtEdgeCount_);

    // node count
    QHBoxLayout* layoutNodeCount = new QHBoxLayout();
    vLayoutMain->addLayout(layoutNodeCount);
    QLabel* lblNodeCount = new QLabel(this);
    lblNodeCount->setText(tr("NodeCount: "));
    layoutNodeCount->addWidget(lblNodeCount);
    txtNodeCount_ = new QLineEdit(this);
    txtNodeCount_->setDisabled(true);
    layoutNodeCount->addWidget(txtNodeCount_);

    // node name
    QHBoxLayout* layoutNodeName = new QHBoxLayout();
    vLayoutMain->addLayout(layoutNodeName);
    QLabel* lblNodeName = new QLabel(this);
    lblNodeName->setText(tr("NodeName: "));
    layoutNodeName->addWidget(lblNodeName);
    txtNodeName_ = new QLineEdit(this);
    txtNodeName_->setDisabled(true);
    layoutNodeName->addWidget(txtNodeName_);

    // node pos
    QHBoxLayout* layoutNodePos = new QHBoxLayout();
    vLayoutMain->addLayout(layoutNodePos);
    QLabel* lblNodePos = new QLabel(this);
    lblNodePos->setText(tr("NodePos: "));
    layoutNodePos->addWidget(lblNodePos);
    txtNodePos_ = new QLineEdit(this);
    txtNodePos_->setDisabled(true);
    layoutNodePos->addWidget(txtNodePos_);
}

void dcis::gui::GraphInfo::setNodeCount(int count)
{
    txtNodeCount_->setText(QString::number(count));
}

void GraphInfo::setEdgeCount(int count)
{
    txtEdgeCount_->setText(QString::number(count));
}

void GraphInfo::setIsDirected(bool isDirected)
{
    if (isDirected)
    {
        txtDirected_->setText(tr("True"));
    }
    else
    {
        txtDirected_->setText(tr("False"));
    }
}

void GraphInfo::setNodePos(const QPointF pos)
{
    txtNodePos_->setText(QString("X = ") + QString::number(pos.x()) + QString(" Y = ") + QString::number(pos.y()));
}

void GraphInfo::setNodeName(const QString nodeName)
{
    txtNodeName_->setText(nodeName);
}

} // end namespace dcis::gui
