#include <gui/imageeditor.h>

// Qt includes
#include <QIcon>
#include <QLabel>
#include <QToolBar>
#include <QBoxLayout>
#include <QMessageBox>

#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QStandardPaths>

namespace dcis::gui {

ImageEditor::ImageEditor(QWidget* parent)
    : QGraphicsView(parent)
{
    setCacheMode(CacheBackground);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    // setRenderHint(QPainter::HighQualityAntialiasing);

    QGraphicsScene* graphicsScene = new QGraphicsScene();
    graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    graphicsScene->setBackgroundBrush(QColor(Qt::gray));
    setScene(graphicsScene);
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

void ImageEditor::setImage(const QImage& img)
{
    if (!scene()->sceneRect().isEmpty())
        scene()->clear();

    scene()->setSceneRect(img.rect());
    scene()->addPixmap(QPixmap::fromImage(img));

    viewFit();
}

void ImageEditor::onUploadImage()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Image"),
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation),
        tr("Image Files (*.png *.jpg *.bmp)"));

    if (filePath.isEmpty())
        return;

    QImageReader imgReader(filePath);
    if (!imgReader.canRead())
    {
        QMessageBox msgBox;
        msgBox.setText("Cannot read file");
        msgBox.exec();
        return;
    }

    if (!scene()->sceneRect().isEmpty())
        scene()->clear();

    QImage img = imgReader.read();
    setImage(img);
}

void ImageEditor::wheelEvent(QWheelEvent* event)
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
    if(sceneRect().isEmpty())
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

void ImageEditor::resizeEvent(QResizeEvent* event)
{
    isResized_ = true;
    QGraphicsView::resizeEvent(event);
}

} // end namespace dcis::gui
