#include <gui/imageeditor.h>

// Qt includes
#include <QBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QToolBar>

#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QStandardPaths>

namespace dcis::gui
{

ImageEditor::ImageEditor(QWidget *parent) : QGraphicsView(parent)
{
    setCacheMode(CacheBackground);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    // setViewportUpdateMode(BoundingRectViewportUpdate);
    //  setRenderHint(QPainter::HighQualityAntialiasing);
    // setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    /*
        QGraphScene* graphicsScene = new QGraphScene();
        graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);
        graphicsScene->setBackgroundBrush(QColor(Qt::gray));
        setScene(graphicsScene);

        // connections
        connect(this, &ImageEditor::sigGraphChanged, graphicsScene, &gui::GraphScene::onReload); */
}

void ImageEditor::zoomIn()
{
    scaleView(qreal(1.2));
}

void ImageEditor::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

void ImageEditor::viewFit()
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    isResized_ = true;

    if (sceneRect().width() > sceneRect().height())
        isLandscape_ = true;
    else
        isLandscape_ = false;
}

ImageEditor::SizeInfo ImageEditor::getSizeInfo() const
{
    SizeInfo info;

    // Get the current transformation matrix
    // QTransform transform = transform();

    // Get the size of the image in scene coordinates
    // QRectF imageRect = imageEditor_->sceneRect();
    // info.imageSize = imageRect.size();

    // Apply the transformation matrix to the image rectangle
    // QRectF transformedRect = transform.mapRect(imageRect);

    // Get the size of the transformed rectangle
    // info.imageSizeZoomed = transformedRect.size();

    // info.imageViewportSize = size();
    // info.graphViewportSize = graphView_->size();

    return info;
}

void ImageEditor::setImage(const QImage &img)
{
    if (!scene()->sceneRect().isEmpty())
        scene()->clear();

    // scene()->setSceneRect(img.rect());
    // scene()->addPixmap(QPixmap::fromImage(img));

    backgroundImage_ = QPixmap::fromImage(img);

    viewport()->update();
    viewFit();
}

void ImageEditor::showNewNodeDialog(QPointF pos)
{
    bool ok;
    QRegularExpression re(QRegularExpression::anchoredPattern(QLatin1String("[a-zA-Z0-9]{1,3}")));

    QString newNodeName = QInputDialog::getText(this, "Add new node", "Name: ", QLineEdit::Normal,
                                                QString::fromStdString(graph_->getNextNodeName()), &ok);
    if (ok)
    {
        static QRegularExpressionMatch match = re.match(newNodeName);
        if (!match.hasMatch())
        {
            QMessageBox::critical(this, "Error",
                                  tr("Node's name contains only alphabetical or numeric characters\n") +
                                      tr("Length of the name mustn't be greater than 3 or smaller than 1"));
            return;
        }

        graph::Node newNode(newNodeName.toStdString(), pos);
        bool succeeded = graph_->addNode(newNode);
        if (!succeeded)
        {
            QMessageBox::critical(this, "Error", "This name has been used by another node");
        }
        else
        {
            emit sigGraphChanged();
        }
    }
}

graph::Graph *ImageEditor::getGraph() const
{
    return graph_;
}

void ImageEditor::updateGraph(graph::Graph *graph)
{
    graph_ = graph;
    graphScene_->setGraph(graph);
}

void ImageEditor::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        if (event->angleDelta().y() > 0)
            zoomIn();
        else
            zoomOut();
    }
    else if (event->modifiers() == Qt::ShiftModifier)
    {
        viewFit();
    }
    else if (event->modifiers() == Qt::NoModifier)
    {
        QGraphicsView::wheelEvent(event);
    }
}

void ImageEditor::scaleView(qreal scaleFactor)
{
    if (sceneRect().isEmpty())
        return;

    int imgLength;
    int viewportLength;
    qreal expRectLength;
    QRectF rectExpectedRect = transform().scale(scaleFactor, scaleFactor).mapRect(sceneRect());

    if (isLandscape_)
    {
        expRectLength = rectExpectedRect.width();
        viewportLength = viewport()->rect().width();
        imgLength = sceneRect().width();
    }
    else
    {
        expRectLength = rectExpectedRect.height();
        viewportLength = viewport()->rect().height();
        imgLength = sceneRect().height();
    }

    if (expRectLength < viewportLength / 2) // minimum zoom : half of viewport
    {
        if (!isResized_ || scaleFactor < 1)
            return;
    }
    else if (expRectLength > imgLength * 10) // maximum zoom : x10
    {
        if (!isResized_ || scaleFactor > 1)
            return;
    }
    else
    {
        isResized_ = false;
    }

    scale(scaleFactor, scaleFactor);
}

void ImageEditor::resizeEvent(QResizeEvent *event)
{
    isResized_ = true;
    QGraphicsView::resizeEvent(event);
}

void ImageEditor::drawBackground(QPainter *painter, const QRectF &rect)
{
    // Call the base class implementation to draw the scene
    QGraphicsView::drawBackground(painter, rect);

    // Draw the background image
    if (!backgroundImage_.isNull())
    {
        painter->drawPixmap(rect.toRect(), backgroundImage_);
    }
}

} // end namespace dcis::gui
