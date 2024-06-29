#ifndef COORDMAPPER_H
#define COORDMAPPER_H

#include <QGeoCoordinate>
#include <QPointF>
#include <QString>

class CCoordMapper
{
  public:
    CCoordMapper(QString sLeftTop, QString sRightBottom, size_t imgW, size_t imgH)
    {
        QStringList parts = sLeftTop.split(' ');
        m_oLeftTop = QGeoCoordinate(parts[0].trimmed().toDouble(), parts[1].trimmed().toDouble());
        parts = sRightBottom.split(' ');
        m_oRightBottom = QGeoCoordinate(parts[0].trimmed().toDouble(), parts[1].trimmed().toDouble());
        m_iImgW = imgW;
        m_iImgH = imgH;
    }

    void pixelToWgs(const QPointF &pixel, double &lat, double &lon)
    {
        lat = m_oLeftTop.latitude() - (pixel.y() / m_iImgH) * (m_oLeftTop.latitude() - m_oRightBottom.latitude());
        lon = m_oLeftTop.longitude() + (pixel.x() / m_iImgH) * (m_oRightBottom.longitude() - m_oLeftTop.longitude());
    }

  private:
    size_t m_iImgW = 0;
    size_t m_iImgH = 0;
    QGeoCoordinate m_oLeftTop;
    QGeoCoordinate m_oRightBottom;
};

#endif // COORDMAPPER_H
