#include "RelationshipConnectionLineGraphicsItem.h"
#include "model/RelationshipConnectionLine.h"

#include <QCursor>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsSceneHoverEvent>

#include <QtCore/qmath.h>

//----------------------------------------------------------------------------------------------

RelationshipConnectionLineGraphicsItem::RelationshipConnectionLineGraphicsItem(
  RelationshipConnectionLine* connection,
  QGraphicsItem* parent
)
  : ConnectionGraphicsItem(connection, parent)
  , m_relationshipConnection(connection)
  , m_isDraggingCardinality(false)
{
  setAcceptHoverEvents(true);
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLineGraphicsItem::paint(
  QPainter* painter,
  const QStyleOptionGraphicsItem* option,
  QWidget* widget
)
{
  if (!m_relationshipConnection) {
    return;
  }

  painter->setRenderHint(QPainter::Antialiasing);

  QPointF start = m_relationshipConnection->getStartPosition();
  QPointF end = m_relationshipConnection->getEndPosition();

  if (m_relationshipConnection->isTotalParticipation()) {
    paintDoubleLine(painter, start, end);
  }
  else {
    ConnectionGraphicsItem::paint(painter, option, widget);
  }

  paintCardinality(painter);
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLineGraphicsItem::paintDoubleLine(
  QPainter* painter,
  const QPointF& start,
  const QPointF& end
)
{
  QPointF direction = end - start;
  qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());

  if (length < 0.001) {
    return;
  }

  QPointF normalized = direction / length;
  QPointF perpendicular(-normalized.y(), normalized.x());
  QPointF offset = perpendicular * 3.0 /*espaço entre as linhas*/;

  QPen pen(Qt::black, m_relationshipConnection->lineWidth());
  if (isSelected()) {
    pen.setColor(QColor(0, 120, 215));
    pen.setWidth(pen.width() + 1);
  }

  painter->setPen(pen);

  painter->drawLine(start + offset, end + offset);
  painter->drawLine(start - offset, end - offset);
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLineGraphicsItem::paintCardinality(
  QPainter* painter
)
{
  if (!m_relationshipConnection) {
    return;
  }

  QString cardinalityText = m_relationshipConnection->getCardinalityText();
  QPointF cardinalityPos = m_relationshipConnection->getCardinalityPosition();

  QFont font;
  font.setPointSizeF(CARDINALITY_FONT_SIZE);
  font.setBold(true);
  painter->setFont(font);

  QFontMetrics metrics(font);
  QRectF textRect = metrics.boundingRect(cardinalityText);
  textRect.moveCenter(cardinalityPos);

  painter->fillRect(textRect.adjusted(-2, -2, 2, 2), QColor(255, 255, 255, 200));

  painter->setPen(QPen(Qt::black, 1));
  painter->drawText(textRect, Qt::AlignCenter, cardinalityText);

  if (m_isDraggingCardinality) {
    QRectF dragBounds = m_relationshipConnection->getCardinalityDragBounds();
    QPen boundsPen(QColor(0, 120, 215, 100), 1, Qt::DashLine);
    painter->setPen(boundsPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(dragBounds);
  }
}

//----------------------------------------------------------------------------------------------

QRectF RelationshipConnectionLineGraphicsItem::boundingRect() const
{
  QRectF baseRect = ConnectionGraphicsItem::boundingRect();
  QRectF cardinalityRect = getCardinalityBoundingRect();

  return baseRect.united(cardinalityRect);
}

//----------------------------------------------------------------------------------------------

QPainterPath RelationshipConnectionLineGraphicsItem::shape() const
{
  QPainterPath path = ConnectionGraphicsItem::shape();

  QRectF cardinalityRect = getCardinalityBoundingRect();
  path.addRect(cardinalityRect);

  return path;
}

//----------------------------------------------------------------------------------------------

QRectF RelationshipConnectionLineGraphicsItem::getCardinalityBoundingRect() const
{
  if (!m_relationshipConnection) {
    return QRectF();
  }

  QString cardinalityText = m_relationshipConnection->getCardinalityText();
  QPointF cardinalityPos = m_relationshipConnection->getCardinalityPosition();

  QFont font;
  font.setPointSizeF(CARDINALITY_FONT_SIZE);
  font.setBold(true);

  QFontMetrics metrics(font);
  QRectF textRect = metrics.boundingRect(cardinalityText);
  textRect.moveCenter(cardinalityPos);

  return textRect.adjusted(-5, -5, 5, 5);
}

//----------------------------------------------------------------------------------------------

bool RelationshipConnectionLineGraphicsItem::isPointNearCardinality(
  const QPointF& point
) const
{
  QRectF cardinalityRect = getCardinalityBoundingRect();
  return cardinalityRect.contains(point);
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLineGraphicsItem::mousePressEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (event->button() == Qt::LeftButton && isPointNearCardinality(event->pos())) {
    m_isDraggingCardinality = true;
    m_dragStartOffset = m_relationshipConnection->cardinalityOffset();
    event->accept();
    update();
    return;
  }

  ConnectionGraphicsItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLineGraphicsItem::mouseMoveEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (m_isDraggingCardinality && m_relationshipConnection) {
    QPointF startPos = m_relationshipConnection->getStartPosition();
    QPointF endPos = m_relationshipConnection->getEndPosition();
    QPointF currentPos = event->pos();

    bool relationshipAtStart = m_relationshipConnection->isRelationshipAtStart();
    QPointF relationshipPos = relationshipAtStart ? startPos : endPos;
    QPointF entityPos = relationshipAtStart ? endPos : startPos;

    QPointF direction = entityPos - relationshipPos;
    qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());

    if (length > 0.001) {
      QPointF normalized = direction / length;
      QPointF perpendicular(-normalized.y(), normalized.x());

      QPointF toMouse = currentPos - relationshipPos;
      qreal distanceAlongLine = QPointF::dotProduct(toMouse, normalized);
      qreal distancePerpendicular = QPointF::dotProduct(toMouse, perpendicular);

      constexpr qreal perpendicularLimit = 10.0;
      distanceAlongLine = qBound(0.0, distanceAlongLine, length);
      distancePerpendicular = qBound(-perpendicularLimit, distancePerpendicular, perpendicularLimit);

      QPointF newOffset(distanceAlongLine, distancePerpendicular);

      m_relationshipConnection->setCardinalityOffset(newOffset);
      update();
    }

    event->accept();
    return;
  }

  ConnectionGraphicsItem::mouseMoveEvent(event);
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLineGraphicsItem::mouseReleaseEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (m_isDraggingCardinality) {
    m_isDraggingCardinality = false;
    update();
    event->accept();
    return;
  }

  ConnectionGraphicsItem::mouseReleaseEvent(event);
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLineGraphicsItem::hoverMoveEvent(
  QGraphicsSceneHoverEvent* event
)
{
  if (isPointNearCardinality(event->pos())) {
    setCursor(Qt::OpenHandCursor);
  }
  else {
    setCursor(Qt::ArrowCursor);
  }

  ConnectionGraphicsItem::hoverMoveEvent(event);
}

//----------------------------------------------------------------------------------------------