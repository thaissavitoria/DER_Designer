#ifndef CONNECTIONLINE_H
#define CONNECTIONLINE_H

#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtGui/QColor>

class ConnectionPoint;
class BasicElement;
class DiagramScene;

// -----------------------------------------------------------------------------------------------------

enum class ConnectionLineType {
  Straight = 0,
  Orthogonal,
  Bezier
};

// -----------------------------------------------------------------------------------------------------

class ConnectionLine : public QObject
{
  Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(ConnectionPoint* startPoint READ startPoint NOTIFY startPointChanged)
    Q_PROPERTY(ConnectionPoint* endPoint READ endPoint NOTIFY endPointChanged)
    Q_PROPERTY(ConnectionLineType lineType READ lineType WRITE setLineType NOTIFY lineTypeChanged)

public:
  explicit ConnectionLine(
    ConnectionPoint* startPoint,
    ConnectionPoint* endPoint,
    QObject* parent = nullptr
  );

  virtual ~ConnectionLine();

  QString id() const { return m_id; }
  ConnectionPoint* startPoint() const { return m_startPoint; }
  ConnectionPoint* endPoint() const { return m_endPoint; }
  ConnectionLineType lineType() const { return m_lineType; }
  QColor lineColor() const { return m_lineColor; }
  qreal lineWidth() const { return m_lineWidth; }

  void setLineType(
    ConnectionLineType type
  );

  void setLineColor(
    const QColor& color
  );

  void setLineWidth(
    qreal width
  );

  void setStartPoint(
    ConnectionPoint* point
  );

  void setEndPoint(
    ConnectionPoint* point
  );

  bool isValid() const;

  QPointF getStartPosition() const;
  QPointF getEndPosition() const;

  BasicElement* getStartElement() const;
  BasicElement* getEndElement() const;

  static QString lineTypeToString(
    ConnectionLineType type
  );

  static ConnectionLineType lineTypeFromString(
    const QString& typeString
  );

signals:
  void startPointChanged(
    ConnectionPoint* newStartPoint
  );

  void endPointChanged(
    ConnectionPoint* newEndPoint
  );

  void lineTypeChanged(
    ConnectionLineType newType
  );

  void lineColorChanged(
    const QColor& newColor
  );

  void lineWidthChanged(
    qreal newWidth
  );

  void connectionChanged();

  void connectionBeingDestroyed();

private slots:
  void onElementPositionChanged();

  void onConnectionPointChanged();

  void onElementDestroyed(
    BasicElement* basicElementSender
  );

private:
  Q_DISABLE_COPY(ConnectionLine)

    QString m_id;
  ConnectionPoint* m_startPoint;
  ConnectionPoint* m_endPoint;
  ConnectionLineType m_lineType;
  QColor m_lineColor;
  qreal m_lineWidth;

  void connectToPoints();
  void disconnectFromPoints();
};

#endif // CONNECTIONLINE_H