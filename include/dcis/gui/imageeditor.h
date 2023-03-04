#ifndef DCIS_GUI_IMAGEEDITOR_H_
#define DCIS_GUI_IMAGEEDITOR_H_

// Qt includes
#include <QKeyEvent>
#include <QWheelEvent>
#include <QGraphicsView>


namespace dcis::gui {

class ImageEditor : public QGraphicsView
{
    Q_OBJECT
public:
    ImageEditor(QWidget* parent = nullptr);
    ~ImageEditor() = default;

    void zoomIn();
    void zoomOut();
    void viewFit();

    void setImage(const QImage& img);

public slots:
    void onUploadImage();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void scaleView(qreal scaleFactor);
    bool isResized_ = false;
    bool isLandscape_ = false;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_IMAGEEDITOR_H_
