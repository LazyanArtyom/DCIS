#ifndef DCIS_GUI_GRAPHINFO_H_
#define DCIS_GUI_GRAPHINFO_H_

// QT includes
#include <QLabel>
#include <QLineEdit>

namespace dcis::gui
{

class GraphInfo : public QWidget
{
  public:
    GraphInfo(QWidget *parent = nullptr);

    void setNodeCount(int count);
    void setEdgeCount(int count);
    void setIsDirected(bool isDirected);

    void setNodePos(const QPointF pos);
    void setNodeName(const QString nodeName);

  private:
    QLineEdit *txtDirected_;
    QLineEdit *txtEdgeCount_;

    QLineEdit *txtNodePos_;
    QLineEdit *txtNodeName_;
    QLineEdit *txtNodeCount_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_DCIS_GUI_GRAPHINFO_H_
