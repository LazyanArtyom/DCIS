#ifndef DCIS_GUI_EDGEITEM_H_
#define DCIS_GUI_EDGEITEM_H_

// App includes
#include <graph/edge.h>
#include <gui/nodeitem.h>

// Qt includes
class QRectF;
class QPainterPath;
class QGraphicsScene;
class QGraphicsLineItem;
class GraphGraphicsScene;
class QGraphicsSceneMouseEvent;


namespace dcis::gui {
using namespace common;

class GraphScene;
class EdgeItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    enum
    {
        Type = UserType + 4
    };

    EdgeItem(GraphScene* scene,
                     NodeItem* startItem,
                     NodeItem* endItem,
                     QColor color = getDefaultColor(),
                     QGraphicsItem* parent = nullptr);

    static QColor getDefaultColor();
    static QColor getDefaultSelectedColor();
    static const QList<QColor>& getColorTable();

    int type() const override;
    QPainterPath shape() const override;
    QRectF boundingRect() const override;

    graph::Edge getEdge() const;
    bool isInversionAvailable() const;

    QColor getSelectedColor() const;
    void setSelectedColor(QColor color);

public slots:
    void onUpdatePosition();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QColor color_;
    QPainterPath path_;
    NodeItem* endItem_;
    GraphScene* gscene_;
    NodeItem* startItem_;
    QColor selectedColor_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_EDGEITEM_H_
