#include "ConnectionGraphicsItem.h"
#include "model/BasicElement.h"

#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtGui/QPainterPath>
#include <QtCore/QDebug>

//----------------------------------------------------------------------------------------------

ConnectionGraphicsItem::ConnectionGraphicsItem(
  ConnectionLine* connection,
  QGraphicsItem* parent
)
  : QGraphicsItem(parent)
  , m_connection(connection)
  , m_isHovered(false)
{
  if (m_connection) {
    connectToConnection();
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    updateFromConnection();
  }
}

//----------------------------------------------------------------------------------------------

ConnectionGraphicsItem::~ConnectionGraphicsItem()
{
  disconnectFromConnection();
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

  qreal margin = qMax(m_connection->lineWidth(), 5.0) + 2;

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

  QPen pen(m_connection->lineColor(), m_connection->lineWidth());

  if (m_isHovered) {
    pen.setColor(QColor(0, 120, 215));
    pen.setWidth(pen.width() + 1);
  }

  if (isSelected()) {
    pen.setStyle(Qt::DashLine);
    pen.setColor(QColor(255, 140, 0));
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

  // Melhorar o algoritmo ortogonal baseado na direção dos connection points
  auto startElement = m_connection->startElement();
  auto endElement = m_connection->endElement();

  if (startElement && endElement) {
    auto startConnectionPoint = m_connection->getStartConnectionPoint();
    auto endConnectionPoint = m_connection->getEndConnectionPoint();

    if (startConnectionPoint && endConnectionPoint) {
      // Usar as direções dos connection points para criar um path mais inteligente
      auto startDirection = startConnectionPoint->direction();
      auto endDirection = endConnectionPoint->direction();

      createOrthogonalPathWithDirections(path, startPos, endPos, startDirection, endDirection);
    }
    else {
      // Fallback para o algoritmo original
      createSimpleOrthogonalPath(path, startPos, endPos);
    }
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

  // Melhorar o algoritmo bezier baseado na direção dos connection points
  auto startElement = m_connection->startElement();
  auto endElement = m_connection->endElement();

  QPointF control1, control2;

  if (startElement && endElement) {
    auto startConnectionPoint = m_connection->getStartConnectionPoint();
    auto endConnectionPoint = m_connection->getEndConnectionPoint();

    if (startConnectionPoint && endConnectionPoint) {
      // Calcular pontos de controle baseados nas direções dos connection points
      control1 = calculateControlPointFromDirection(startPos, startConnectionPoint->direction(), 100);
      control2 = calculateControlPointFromDirection(endPos, endConnectionPoint->direction(), -100);
    }
    else {
      // Fallback para pontos de controle padrão
      control1 = QPointF(startPos.x() + (endPos.x() - startPos.x()) * 0.3, startPos.y());
      control2 = QPointF(endPos.x() - (endPos.x() - startPos.x()) * 0.3, endPos.y());
    }
  }
  else {
    control1 = QPointF(startPos.x() + (endPos.x() - startPos.x()) * 0.3, startPos.y());
    control2 = QPointF(endPos.x() - (endPos.x() - startPos.x()) * 0.3, endPos.y());
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
  const qreal margin = 20.0; // Distância mínima para sair do elemento

  // Calcular pontos de saída baseados na direção
  QPointF startExitPoint = calculateExitPoint(startPos, startDirection, margin);
  QPointF endExitPoint = calculateExitPoint(endPos, endDirection, -margin);

  path.lineTo(startExitPoint);

  // Criar segmentos intermediários
  if (startDirection == ConnectionDirection::Left || startDirection == ConnectionDirection::Right) {
    if (endDirection == ConnectionDirection::Left || endDirection == ConnectionDirection::Right) {
      // Horizontal para horizontal
      qreal midX = (startExitPoint.x() + endExitPoint.x()) / 2;
      path.lineTo(QPointF(midX, startExitPoint.y()));
      path.lineTo(QPointF(midX, endExitPoint.y()));
    }
    else {
      // Horizontal para vertical
      path.lineTo(QPointF(startExitPoint.x(), endExitPoint.y()));
    }
  }
  else {
    if (endDirection == ConnectionDirection::Top || endDirection == ConnectionDirection::Bottom) {
      // Vertical para vertical
      qreal midY = (startExitPoint.y() + endExitPoint.y()) / 2;
      path.lineTo(QPointF(startExitPoint.x(), midY));
      path.lineTo(QPointF(endExitPoint.x(), midY));
    }
    else {
      // Vertical para horizontal
      path.lineTo(QPointF(endExitPoint.x(), startExitPoint.y()));
    }
  }

  path.lineTo(endExitPoint);
  path.lineTo(endPos);
}

//----------------------------------------------------------------------------------------------

QPointF ConnectionGraphicsItem::calculateExitPoint(
  const QPointF& position,
  ConnectionDirection direction,
  qreal distance
) const
{
  switch (direction) {
  case ConnectionDirection::Top:
    return QPointF(position.x(), position.y() - distance);
  case ConnectionDirection::Bottom:
    return QPointF(position.x(), position.y() + distance);
  case ConnectionDirection::Left:
    return QPointF(position.x() - distance, position.y());
  case ConnectionDirection::Right:
    return QPointF(position.x() + distance, position.y());
  default:
    return position;
  }
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
    setSelected(!isSelected());
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
  update();
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::connectToConnection()
{
  if (!m_connection) return;

  connect(m_connection, &ConnectionLine::connectionChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::lineColorChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::lineWidthChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::lineTypeChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::startElementChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  connect(m_connection, &ConnectionLine::endElementChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
}

//----------------------------------------------------------------------------------------------

void ConnectionGraphicsItem::disconnectFromConnection()
{
  if (!m_connection) return;

  disconnect(m_connection, &ConnectionLine::connectionChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::lineColorChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::lineWidthChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::lineTypeChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::startElementChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
  disconnect(m_connection, &ConnectionLine::endElementChanged,
    this, &ConnectionGraphicsItem::updateFromConnection);
}

//----------------------------------------------------------------------------------------------