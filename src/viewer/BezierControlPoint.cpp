#include "BezierControlPoint.h"
#include "ConnectionGraphicsItem.h"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsScene>

//----------------------------------------------------------------------------------------------

BezierControlPoint::BezierControlPoint(
  ConnectionGraphicsItem* parent,
  ControlPointType type
)
  : QGraphicsEllipseItem(parent)
  , m_connectionItem(parent)
  , m_type(type)
  , m_isDragging(false)
{
  const qreal size = 8.0;
  setRect(-size / 2, -size / 2, size, size);

  setPen(QPen(QColor(0, 120, 215), 2));
  setBrush(QBrush(Qt::white));

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  setFlag(QGraphicsItem::ItemStacksBehindParent, false);
  setAcceptHoverEvents(true);
  setZValue(1000);

  setVisible(false);
  hide();
}

//----------------------------------------------------------------------------------------------

void BezierControlPoint::updatePosition(
  const QPointF& position
)
{
  setPos(position);
}

//----------------------------------------------------------------------------------------------

void BezierControlPoint::mousePressEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (event->button() == Qt::LeftButton) {
    m_isDragging = true;
    event->accept();
  }
  QGraphicsEllipseItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------------------------

void BezierControlPoint::mouseMoveEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (m_isDragging && m_connectionItem) {
    QGraphicsEllipseItem::mouseMoveEvent(event);
    m_connectionItem->updateControlPointPosition(m_type, pos());
  }
}

//----------------------------------------------------------------------------------------------

void BezierControlPoint::mouseReleaseEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (event->button() == Qt::LeftButton) {
    m_isDragging = false;
    event->accept();
  }
  QGraphicsEllipseItem::mouseReleaseEvent(event);
}

//----------------------------------------------------------------------------------------------

void BezierControlPoint::hoverEnterEvent(
  QGraphicsSceneHoverEvent* event
)
{
  setBrush(QBrush(QColor(0, 120, 215)));
  QGraphicsEllipseItem::hoverEnterEvent(event);
}

//----------------------------------------------------------------------------------------------

void BezierControlPoint::hoverLeaveEvent(
  QGraphicsSceneHoverEvent* event
)
{
  setBrush(QBrush(Qt::white));
  QGraphicsEllipseItem::hoverLeaveEvent(event);
}

//----------------------------------------------------------------------------------------------