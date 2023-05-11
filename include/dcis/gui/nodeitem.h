#ifndef DCIS_GUI_NODEITEM_H_
#define DCIS_GUI_NODEITEM_H_

// App includes
#include <graph/node.h>

// Qt includes
#include <QGraphicsItem>
#include <QPainterPath>


namespace dcis::gui {
using namespace common;
class GraphScene;

class NodeItem : public QObject, public QGraphicsItem {
    Q_OBJECT
public:
    NodeItem(gui::GraphScene* gscene, graph::Node* node, QColor color = getColorTable()[0]);

    enum
    {
        Type = UserType + 5
    };

    static QColor getDefaultColor();
    static QColor getDefaultSelectedColor();
    static const QList<QColor>& getColorTable();

    int type() const override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    graph::Node* getNode() const;
    void setNode(graph::Node* node);

    int getRadius() const;
    QColor getSelectedColor() const;

    QColor getColor() const;
    void setColor(const QColor& newColor);

    void setSelectedColor(const QColor& newColor);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

signals:
    void sigPositionChanged();

private:
    int radius_;
    bool isMoving_;
    QColor color_;
    QColor selectedColor_;

    graph::Node* node_;
    gui::GraphScene* gscene_;

    const int fontSize_ = 10;
    const QString font_ = "Source Code Pro";
};

} // end namespace dcis::gui
#endif // DCIS_GUI_NODEITEM_H_
