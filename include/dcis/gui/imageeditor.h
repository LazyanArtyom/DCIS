#ifndef DCIS_GUI_IMAGEEDITOR_H_
#define DCIS_GUI_IMAGEEDITOR_H_

// App includes
#include <gui/graphscene.h>

// Qt includes
#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>

namespace dcis::gui
{

class ImageEditor : public QGraphicsView
{
    Q_OBJECT
  public:
    struct SizeInfo
    {
        QSizeF imageSize;
        QSizeF imageSizeZoomed;
        QSizeF imageViewportSize;
        QSizeF graphViewportSize;
    };

    ImageEditor(QWidget *parent = nullptr);
    ~ImageEditor() = default;

    void zoomIn();
    void zoomOut();
    void viewFit();

    SizeInfo getSizeInfo() const;
    void setImage(const QImage &img);
    void showNewNodeDialog(QPointF pos = QPointF(0, 0));

    graph::Graph *getGraph() const;
    void updateGraph(graph::Graph *graph);

  signals:
    void sigNodeMoved();
    void sigGraphChanged();

  public slots:

  protected:
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

  private:
    void scaleView(qreal scaleFactor);
    bool isResized_ = false;
    bool isLandscape_ = false;
    QPixmap backgroundImage_;

    graph::Graph *graph_;
    gui::GraphScene *graphScene_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_IMAGEEDITOR_H_
