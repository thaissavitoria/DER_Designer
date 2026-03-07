#ifndef CONNECTIONGRAPHICSITEM_H
#define CONNECTIONGRAPHICSITEM_H

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QObject>

#include "model/ConnectionLine.h"
#include "model/ConnectionPoint.h"
#include "view/BezierControlPoint.h"

class ConnectionGraphicsItem : public QObject, public QGraphicsItem
{
  Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
  explicit ConnectionGraphicsItem(
    ConnectionLine* connection,
    QGraphicsItem* parent = nullptr
  );

  virtual ~ConnectionGraphicsItem();

  QRectF boundingRect() const override;

  void paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget = nullptr
  ) override;

  QPainterPath shape() const override;

  void updateControlPointPosition(
    ControlPointType type,
    const QPointF& position
  );

  void setSelected(
    const bool selected
  );

protected:
  void mousePressEvent(
    QGraphicsSceneMouseEvent* event
  ) override;

  void hoverEnterEvent(
    QGraphicsSceneHoverEvent* event
  ) override;

  void hoverLeaveEvent(
    QGraphicsSceneHoverEvent* event
  ) override;

private slots:
  void updateFromConnection();

private:
  QPainterPath createStraightPath() const;
  QPainterPath createOrthogonalPath() const;
  QPainterPath createBezierPath() const;

  void createSimpleOrthogonalPath(
    QPainterPath& path,
    const QPointF& startPos,
    const QPointF& endPos
  ) const;

  void createOrthogonalPathWithDirections(
    QPainterPath& path,
    const QPointF& startPos,
    const QPointF& endPos,
    ConnectionDirection startDirection,
    ConnectionDirection endDirection
  ) const;

  QPointF calculateControlPointFromDirection(
    const QPointF& position,
    ConnectionDirection direction,
    qreal distance
  ) const;

  void connectToConnection();
  void disconnectFromConnection();

  void createControlPoints();
  void updateControlPointsPosition();

  ConnectionLine* m_connection;
  bool m_isHovered;

  BezierControlPoint* m_controlPoint1;
  BezierControlPoint* m_controlPoint2;
};

#endif // CONNECTIONGRAPHICSITEM_H