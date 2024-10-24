#ifndef DCIS_GUI_NODEITEM_H_
#define DCIS_GUI_NODEITEM_H_

// App includes
#include <graph/node.h>

// Qt includes
#include <QGraphicsItem>
#include <QPainterPath>

namespace dcis::gui
{
using namespace common;
class GraphScene;

class NodeItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
  public:
    NodeItem(GraphScene *gscene, graph::Node *node);

    enum
    {
        Type = UserType + 5
    };

    int getDefaultRadius();
    QColor getDefaultColor();
    QColor getDefaultSelectedColor();

    int type() const override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    graph::Node *getNode() const;
    void setNode(graph::Node *node);

    int getRadius() const;
    QColor getSelectedColor() const;

    QColor getColor() const;
    void setColor(const QColor &newColor);

    void setSelectedColor(const QColor &newColor);

  protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  signals:
    void sigPositionChanged();

  private:
    int radius_;
    bool isMoving_;

    graph::Node *node_ = nullptr;
    GraphScene *gscene_ = nullptr;

    const int fontSize_ = 10;
    const QString font_ = "Source Code Pro";
};

} // end namespace dcis::gui
#endif // DCIS_GUI_NODEITEM_H_
