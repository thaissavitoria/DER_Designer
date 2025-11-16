#ifndef BEZIERCONTROLPOINT_H
#define BEZIERCONTROLPOINT_H

#include <QtWidgets/QGraphicsEllipseItem>
#include <QtGui/QPen>
#include <QtGui/QBrush>

class ConnectionGraphicsItem;

enum class ControlPointType {
  Control1,
  Control2
};

class BezierControlPoint : public QGraphicsEllipseItem
{
public:
  explicit BezierControlPoint(
    ConnectionGraphicsItem* parent,
    ControlPointType type
  );

  ControlPointType controlPointType() const;

  void updatePosition(
    const QPointF& position
  );

protected:
  void mousePressEvent(
    QGraphicsSceneMouseEvent* event
  ) override;

  void mouseMoveEvent(
    QGraphicsSceneMouseEvent* event
  ) override;

  void mouseReleaseEvent(
    QGraphicsSceneMouseEvent* event
  ) override;

  void hoverEnterEvent(
    QGraphicsSceneHoverEvent* event
  ) override;

  void hoverLeaveEvent(
    QGraphicsSceneHoverEvent* event
  ) override;

private:
  ConnectionGraphicsItem* m_connectionItem;
  ControlPointType m_type;
  bool m_isDragging;
};

#endif // BEZIERCONTROLPOINT_H