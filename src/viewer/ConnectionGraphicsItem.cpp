#include "ConnectionGraphicsItem.h"
#include "BezierControlPoint.h"
#include "model/BasicElement.h"

#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtGui/QPainterPath>
#include <QtCore/QDebug>

#include "controller/DiagramScene.h"

//----------------------------------------------------------------------------------------------

ConnectionGraphicsItem::ConnectionGraphicsItem(
  ConnectionLine* connection,
  QGraphicsItem* parent
)
  : QGraphicsItem(parent)
  , m_connection(connection)
  , m_isHovered(false)
  , m_controlPoint1(nullptr)
  , m_controlPoint2(nullptr)
{
  if (m_connection) {
    connectToConnection();
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    createControlPoints();
    updateFromConnection();
  }
}

//----------------------------------------------------------------------------------------------

ConnectionGraphicsItem::~ConnectionGraphicsItem()
{
  disconnectFromConnection();

  if (m_controlPoint1) {
    delete m_controlPoint1;
    m_controlPoint1 = nullptr;
  }

  if (m_controlPoint2) {
    delete m_controlPoint2;
    m_controlPoint2 = nullptr;
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::setSelected(
  const bool selected
)
{
  QGraphicsItem::setSelected(selected);

  if (m_controlPoint1 && m_connection->lineType() == ConnectionLineType::Bezier) {
    m_controlPoint1->setVisible(selected);
  }
  if (m_controlPoint2 && m_connection->lineType() == ConnectionLineType::Bezier) {
    m_controlPoint2->setVisible(selected);
  }

  update();
}

//----------------------------------------------------------------------------------------------

QRectF ConnectionGraphicsItem::boundingRect() const
{
  if (!m_connection || !m_connection->isValid()) {
    return QRectF();
  }

  QPointF startPos = m_connection->getStartPosition();
  QPointF endPos = m_connection->getEndPosition();

  if (startPos.isNull() || endPos.isNull()) {
    return QRectF();
  }

  qreal minX = qMin(startPos.x(), endPos.x());
  qreal minY = qMin(startPos.y(), endPos.y());
  qreal maxX = qMax(startPos.x(), endPos.x());
  qreal maxY = qMax(startPos.y(), endPos.y());

  if (m_connection->lineType() == ConnectionLineType::Bezier) {
    QPointF control1 = startPos + m_connection->control1Offset();
    QPointF control2 = endPos + m_connection->control2Offset();

    minX = qMin(minX, qMin(control1.x(), control2.x()));
    minY = qMin(minY, qMin(control1.y(), control2.y()));
    maxX = qMax(maxX, qMax(control1.x(), control2.x()));
    maxY = qMax(maxY, qMax(control1.y(), control2.y()));
  }

  qreal margin = qMax(m_connection->lineWidth(), 5.0) + 20;

  return QRectF(minX - margin, minY - margin,
    maxX - minX + 2 * margin, maxY - minY + 2 * margin);
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::paint(
  QPainter* painter,
  const QStyleOptionGraphicsItem* option,
  QWidget* widget
)
{
  Q_UNUSED(option)
    Q_UNUSED(widget)

    if (!m_connection || !m_connection->isValid()) {
      return;
    }

  painter->save();

  QPen pen(Qt::black, m_connection->lineWidth());

  if (m_isHovered) {
    pen.setColor(QColor(0, 120, 215));
    pen.setWidth(pen.width() + 1);
  }

  if (isSelected()) {
    pen.setStyle(Qt::DashLine);
    pen.setColor(QColor(0, 150, 0));
  }

  painter->setPen(pen);
  painter->setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  switch (m_connection->lineType()) {
  case ConnectionLineType::Straight:
    path = createStraightPath();
    break;
  case ConnectionLineType::Orthogonal:
    path = createOrthogonalPath();
    break;
  case ConnectionLineType::Bezier:
    path = createBezierPath();

    if (isSelected()) {
      QPointF startPos = m_connection->getStartPosition();
      QPointF endPos = m_connection->getEndPosition();
      QPointF control1 = startPos + m_connection->control1Offset();
      QPointF control2 = endPos + m_connection->control2Offset();

      painter->drawLine(startPos, control1);
      painter->drawLine(endPos, control2);
    }
    break;
  }

  painter->drawPath(path);

  painter->restore();
}

//----------------------------------------------------------------------------------------------

QPainterPath ConnectionGraphicsItem::shape() const
{
  if (!m_connection || !m_connection->isValid()) {
    return QPainterPath();
  }

  QPainterPath path;
  switch (m_connection->lineType()) {
  case ConnectionLineType::Straight:
    path = createStraightPath();
    break;
  case ConnectionLineType::Orthogonal:
    path = createOrthogonalPath();
    break;
  case ConnectionLineType::Bezier:
    path = createBezierPath();
    break;
  }

  QPainterPathStroker stroker;
  stroker.setWidth(qMax(m_connection->lineWidth() + 4, 8.0));
  stroker.setCapStyle(Qt::RoundCap);
  stroker.setJoinStyle(Qt::RoundJoin);

  return stroker.createStroke(path);
}

//----------------------------------------------------------------------------------------------

QPainterPath ConnectionGraphicsItem::createStraightPath() const
{
  QPainterPath path;
  QPointF startPos = m_connection->getStartPosition();
  QPointF endPos = m_connection->getEndPosition();

  path.moveTo(startPos);
  path.lineTo(endPos);

  return path;
}

//----------------------------------------------------------------------------------------------

QPainterPath ConnectionGraphicsItem::createOrthogonalPath() const
{
  QPainterPath path;
  QPointF startPos = m_connection->getStartPosition();
  QPointF endPos = m_connection->getEndPosition();

  path.moveTo(startPos);

  auto startPoint = m_connection->startPoint();
  auto endPoint = m_connection->endPoint();

  if (startPoint && endPoint) {
    auto startDirection = startPoint->direction();
    auto endDirection = endPoint->direction();

    createOrthogonalPathWithDirections(path, startPos, endPos, startDirection, endDirection);
  }
  else {
    createSimpleOrthogonalPath(path, startPos, endPos);
  }

  return path;
}

//----------------------------------------------------------------------------------------------

QPainterPath ConnectionGraphicsItem::createBezierPath() const
{
  QPainterPath path;
  QPointF startPos = m_connection->getStartPosition();
  QPointF endPos = m_connection->getEndPosition();

  path.moveTo(startPos);

  QPointF control1, control2;

  if (m_connection->hasCustomControlPoints()) {
    control1 = startPos + m_connection->control1Offset();
    control2 = endPos + m_connection->control2Offset();
  }
  else {
    auto startPoint = m_connection->startPoint();
    auto endPoint = m_connection->endPoint();

    if (startPoint && endPoint) {
      control1 = calculateControlPointFromDirection(startPos, startPoint->direction(), 50);
      control2 = calculateControlPointFromDirection(endPos, endPoint->direction(), -50);
    }
    else {
      control1 = QPointF(startPos.x() + (endPos.x() - startPos.x()) * 0.3, startPos.y());
      control2 = QPointF(endPos.x() - (endPos.x() - startPos.x()) * 0.3, endPos.y());
    }
  }

  path.cubicTo(control1, control2, endPos);

  return path;
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::createSimpleOrthogonalPath(
  QPainterPath& path,
  const QPointF& startPos,
  const QPointF& endPos
) const
{
  QPointF midPoint;
  qreal deltaX = endPos.x() - startPos.x();
  qreal deltaY = endPos.y() - startPos.y();

  if (qAbs(deltaX) > qAbs(deltaY)) {
    midPoint = QPointF(startPos.x() + deltaX / 2, startPos.y());
    path.lineTo(midPoint);
    path.lineTo(QPointF(midPoint.x(), endPos.y()));
    path.lineTo(endPos);
  }
  else {
    midPoint = QPointF(startPos.x(), startPos.y() + deltaY / 2);
    path.lineTo(midPoint);
    path.lineTo(QPointF(endPos.x(), midPoint.y()));
    path.lineTo(endPos);
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::createOrthogonalPathWithDirections(
  QPainterPath& path,
  const QPointF& startPos,
  const QPointF& endPos,
  ConnectionDirection startDirection,
  ConnectionDirection endDirection
) const
{
  path.lineTo(startPos);

  if (startDirection == ConnectionDirection::Left || startDirection == ConnectionDirection::Right) {
    if (endDirection == ConnectionDirection::Left || endDirection == ConnectionDirection::Right) {
      qreal midX = (startPos.x() + endPos.x()) / 2;
      path.lineTo(QPointF(midX, startPos.y()));
      path.lineTo(QPointF(midX, endPos.y()));
    }
    else {
      path.lineTo(QPointF(startPos.x(), endPos.y()));
    }
  }
  else {
    if (endDirection == ConnectionDirection::Top || endDirection == ConnectionDirection::Bottom) {
      qreal midY = (startPos.y() + endPos.y()) / 2;
      path.lineTo(QPointF(startPos.x(), midY));
      path.lineTo(QPointF(endPos.x(), midY));
    }
    else {
      path.lineTo(QPointF(endPos.x(), startPos.y()));
    }
  }

  path.lineTo(endPos);
}

//----------------------------------------------------------------------------------------------

QPointF ConnectionGraphicsItem::calculateControlPointFromDirection(
  const QPointF& position,
  ConnectionDirection direction,
  qreal distance
) const
{
  switch (direction) {
  case ConnectionDirection::Top:
    return QPointF(position.x(), position.y() + distance);
  case ConnectionDirection::Bottom:
    return QPointF(position.x(), position.y() - distance);
  case ConnectionDirection::Left:
    return QPointF(position.x() + distance, position.y());
  case ConnectionDirection::Right:
    return QPointF(position.x() - distance, position.y());
  default:
    return position;
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::mousePressEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (event->button() == Qt::LeftButton) {
    const bool newSelectedState = !isSelected();
    setSelected(newSelectedState);

    if (auto diagramScene = dynamic_cast<DiagramScene*>(scene())) {
      if (m_connection) {
        diagramScene->selectConnection(m_connection, newSelectedState);
      }
    }

    event->accept();
    return;
  }

  QGraphicsItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::hoverEnterEvent(
  QGraphicsSceneHoverEvent* event
)
{
  m_isHovered = true;
  update();
  QGraphicsItem::hoverEnterEvent(event);
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::hoverLeaveEvent(
  QGraphicsSceneHoverEvent* event
)
{
  m_isHovered = false;
  update();
  QGraphicsItem::hoverLeaveEvent(event);
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::updateFromConnection()
{
  if (!m_connection) return;

  prepareGeometryChange();
  updateControlPointsPosition();
  update();
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::connectToConnection()
{
  if (!m_connection) return;

  connect(m_connection, &ConnectionLine::connectionChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::lineWidthChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::lineTypeChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::startPointChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::endPointChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::controlPointsChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::disconnectFromConnection()
{
  if (!m_connection) return;

  disconnect(m_connection, &ConnectionLine::connectionChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::lineWidthChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::lineTypeChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::startPointChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::endPointChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::controlPointsChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::createControlPoints()
{
  m_controlPoint1 = new BezierControlPoint(this, ControlPointType::Control1);
  m_controlPoint2 = new BezierControlPoint(this, ControlPointType::Control2);
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::updateControlPointsPosition()
{
  if (!m_controlPoint1 || !m_controlPoint2 || !m_connection) {
    return;
  }

  QPointF startPos = m_connection->getStartPosition();
  QPointF endPos = m_connection->getEndPosition();

  m_controlPoint1->updatePosition(startPos + m_connection->control1Offset());
  m_controlPoint2->updatePosition(endPos + m_connection->control2Offset());
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::updateControlPointPosition(
  ControlPointType type,
  const QPointF& position
)
{
  if (!m_connection) {
    return;
  }

  auto diagramScene = dynamic_cast<DiagramScene*>(scene());
  if (!diagramScene) {
    return;
  }

  QPointF startPos = m_connection->getStartPosition();
  QPointF endPos = m_connection->getEndPosition();

  if (type == ControlPointType::Control1) {
    diagramScene->updateConnectionControlPoint(m_connection, type, position - startPos);
  }
  else {
    diagramScene->updateConnectionControlPoint(m_connection, type, position - endPos);
  }
}

//----------------------------------------------------------------------------------------------