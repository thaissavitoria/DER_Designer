#include "ConnectionPoint.h"

#include <QtCore/QUuid>
#include <QSizeF>

// -----------------------------------------------------------------------------------------------------

ConnectionPoint::ConnectionPoint(
  QObject* parent
)
  : QObject(parent)
  , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
  , m_relativePosition(0.5, 0.0)
  , m_direction(ConnectionDirection::Top)
{
}

// -----------------------------------------------------------------------------------------------------

ConnectionPoint::ConnectionPoint(
  const QPointF& relativePosition,
  ConnectionDirection direction,
  QObject* parent
)
  : QObject(parent)
  , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
  , m_relativePosition(relativePosition)
  , m_direction(direction)
{
}

// -----------------------------------------------------------------------------------------------------

ConnectionPoint::~ConnectionPoint()
{
  emit destroyed();
}

// -----------------------------------------------------------------------------------------------------

void ConnectionPoint::setRelativePosition(
  const QPointF& position
)
{
  if (m_relativePosition != position) {
    m_relativePosition = position;
    emit relativePositionChanged(m_relativePosition);
  }
}

// -----------------------------------------------------------------------------------------------------

QPointF ConnectionPoint::absolutePosition(
  const QPointF& elementPosition,
  const QSizeF& elementSize
) const
{
  QPointF absolute;
  absolute.setX(elementPosition.x() + (m_relativePosition.x() * elementSize.width()));
  absolute.setY(elementPosition.y() + (m_relativePosition.y() * elementSize.height()));
  return absolute;
}

// -----------------------------------------------------------------------------------------------------

QList<ConnectionPoint*> ConnectionPoint::createDefaultConnectionPoints(
  QObject* parent
)
{
  QList<ConnectionPoint*> points;

  addConectionPoint(points, QPointF(0.5, 0.0), ConnectionDirection::Top, parent);
  addConectionPoint(points, QPointF(0.5, 1.0), ConnectionDirection::Bottom, parent);
  addConectionPoint(points, QPointF(1.0, 0.5), ConnectionDirection::Left, parent);
  addConectionPoint(points, QPointF(0.0, 0.5), ConnectionDirection::Right, parent);

  return points;
}

// -----------------------------------------------------------------------------------------------------

void ConnectionPoint::addConectionPoint(
  QList<ConnectionPoint*>& list,
  const QPointF& relativePosition,
  ConnectionDirection direction,
  QObject* parent
)
{
  auto point = new ConnectionPoint(relativePosition, direction, parent);
  list.append(point);
}

// -----------------------------------------------------------------------------------------------------
