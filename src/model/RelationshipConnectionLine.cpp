#include "RelationshipConnectionLine.h"
#include "ConnectionPoint.h"
#include "BasicElement.h"

#include <QtCore/qmath.h>

//----------------------------------------------------------------------------------------------

RelationshipConnectionLine::RelationshipConnectionLine(
  ConnectionPoint* startPoint,
  ConnectionPoint* endPoint,
  RelationshipEnd* relationshipEnd,
  QObject* parent
)
  : ConnectionLine(startPoint, endPoint, parent)
  , m_relationshipEnd(relationshipEnd)
  , m_cardinalityOffset(20.0, 0.0)
{
  connectToRelationshipEnd();
}

//----------------------------------------------------------------------------------------------

RelationshipConnectionLine::~RelationshipConnectionLine()
{
  disconnectFromRelationshipEnd();
  m_relationshipEnd = nullptr;
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLine::setRelationshipEnd(
  RelationshipEnd* relationshipEnd
)
{
  if (m_relationshipEnd != relationshipEnd) {
    disconnectFromRelationshipEnd();
    m_relationshipEnd = relationshipEnd;
    connectToRelationshipEnd();
    emit relationshipEndChanged(m_relationshipEnd);
    emit connectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLine::setCardinalityOffset(
  const QPointF& offset
)
{
  if (m_cardinalityOffset != offset) {
    if (isCardinalityOffsetValid(offset)) {
      m_cardinalityOffset = offset;
      emit cardinalityOffsetChanged(m_cardinalityOffset);
      emit connectionChanged();
    }
  }
}

//----------------------------------------------------------------------------------------------

bool RelationshipConnectionLine::isRelationshipAtStart() const
{
  if (!startPoint()) {
    return false;
  }

  auto startElement = qobject_cast<BasicElement*>(startPoint()->parent());
  if (!startElement) {
    return false;
  }

  return startElement->type() == ElementType::Relationship ||
    startElement->type() == ElementType::IdentifyingRelationship;
}

//----------------------------------------------------------------------------------------------

QPointF RelationshipConnectionLine::getCardinalityPosition() const
{
  QPointF startPos = getStartPosition();
  QPointF endPos = getEndPosition();

  QPointF relationshipPos = isRelationshipAtStart() ? startPos : endPos;
  QPointF entityPos = isRelationshipAtStart() ? endPos : startPos;

  if (relationshipPos == entityPos) {
    return relationshipPos;
  }

  QPointF direction = entityPos - relationshipPos;
  qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());

  if (length < 0.001) {
    return relationshipPos;
  }

  QPointF normalized = direction / length;
  QPointF perpendicular(-normalized.y(), normalized.x());

  qreal distanceFromRelationship = m_cardinalityOffset.x();
  qreal perpendicularOffset = m_cardinalityOffset.y();

  QPointF position = relationshipPos + (normalized * distanceFromRelationship) + (perpendicular * perpendicularOffset);

  return position;
}

//----------------------------------------------------------------------------------------------

QRectF RelationshipConnectionLine::getCardinalityDragBounds() const
{
  QPointF startPos = getStartPosition();
  QPointF endPos = getEndPosition();

  QPointF relationshipPos = isRelationshipAtStart() ? startPos : endPos;
  QPointF entityPos = isRelationshipAtStart() ? endPos : startPos;

  if (relationshipPos == entityPos) {
    return QRectF(relationshipPos, QSizeF(0, 0));
  }

  QPointF direction = entityPos - relationshipPos;
  qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());

  if (length < 0.001) {
    return QRectF(relationshipPos, QSizeF(0, 0));
  }

  QPointF normalized = direction / length;
  QPointF perpendicular(-normalized.y(), normalized.x());

  constexpr qreal perpendicularLimit = 10.0;

  QPointF topLeft = relationshipPos - (perpendicular * perpendicularLimit);
  QPointF topRight = entityPos - (perpendicular * perpendicularLimit);
  QPointF bottomLeft = relationshipPos + (perpendicular * perpendicularLimit);
  QPointF bottomRight = entityPos + (perpendicular * perpendicularLimit);

  qreal left = qMin(qMin(topLeft.x(), topRight.x()), qMin(bottomLeft.x(), bottomRight.x()));
  qreal top = qMin(qMin(topLeft.y(), topRight.y()), qMin(bottomLeft.y(), bottomRight.y()));
  qreal right = qMax(qMax(topLeft.x(), topRight.x()), qMax(bottomLeft.x(), bottomRight.x()));
  qreal bottom = qMax(qMax(topLeft.y(), topRight.y()), qMax(bottomLeft.y(), bottomRight.y()));

  return QRectF(QPointF(left, top), QPointF(right, bottom));
}

//----------------------------------------------------------------------------------------------

bool RelationshipConnectionLine::isCardinalityOffsetValid(
  const QPointF& offset
) const
{
  QPointF startPos = getStartPosition();
  QPointF endPos = getEndPosition();

  QPointF relationshipPos = isRelationshipAtStart() ? startPos : endPos;
  QPointF entityPos = isRelationshipAtStart() ? endPos : startPos;

  if (relationshipPos == entityPos) {
    return false;
  }

  QPointF direction = entityPos - relationshipPos;
  qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());

  if (length < 0.001) {
    return false;
  }

  qreal distanceFromRelationship = offset.x();
  qreal perpendicularOffset = qAbs(offset.y());

  constexpr qreal perpendicularLimit = 10.0;

  if (distanceFromRelationship < 0.0 || distanceFromRelationship > length) {
    return false;
  }

  if (perpendicularOffset > perpendicularLimit) {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------

QString RelationshipConnectionLine::getCardinalityText() const
{
  if (!m_relationshipEnd) {
    return "1";
  }

  return m_relationshipEnd->getDisplayCardinalityText();
}

//----------------------------------------------------------------------------------------------

bool RelationshipConnectionLine::isTotalParticipation() const
{
  if (!m_relationshipEnd) {
    return false;
  }

  return m_relationshipEnd->isTotalParticipation();
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLine::connectToRelationshipEnd()
{
  if (m_relationshipEnd) {
    connect(m_relationshipEnd, &RelationshipEnd::cardinalityChanged,
      this, &RelationshipConnectionLine::onRelationshipEndChanged);
    connect(m_relationshipEnd, &RelationshipEnd::totalParticipationChanged,
      this, &RelationshipConnectionLine::onRelationshipEndChanged);
    connect(m_relationshipEnd, &RelationshipEnd::customCardinalityTextChanged,
      this, &RelationshipConnectionLine::onRelationshipEndChanged);
  }
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLine::disconnectFromRelationshipEnd()
{
  if (m_relationshipEnd) {
    disconnect(m_relationshipEnd, &RelationshipEnd::cardinalityChanged,
      this, &RelationshipConnectionLine::onRelationshipEndChanged);
    disconnect(m_relationshipEnd, &RelationshipEnd::totalParticipationChanged,
      this, &RelationshipConnectionLine::onRelationshipEndChanged);
    disconnect(m_relationshipEnd, &RelationshipEnd::customCardinalityTextChanged,
      this, &RelationshipConnectionLine::onRelationshipEndChanged);
  }
}

//----------------------------------------------------------------------------------------------

void RelationshipConnectionLine::onRelationshipEndChanged()
{
  emit connectionChanged();
}

//----------------------------------------------------------------------------------------------

QVariantMap RelationshipConnectionLine::serialize() const
{
  QVariantMap data = ConnectionLine::serialize();

  data["cardinalityOffsetX"] = m_cardinalityOffset.x();
  data["cardinalityOffsetY"] = m_cardinalityOffset.y();

  if (m_relationshipEnd) {
    data["relationshipEndEntityId"] = m_relationshipEnd->entityId();
    data["relationshipEndCardinality"] = static_cast<int>(m_relationshipEnd->cardinality());
    data["relationshipEndTotalParticipation"] = m_relationshipEnd->isTotalParticipation();
    data["relationshipEndCustomText"] = m_relationshipEnd->customCardinalityText();
  }

  return data;
}

//----------------------------------------------------------------------------------------------

bool RelationshipConnectionLine::deserialize(
  const QVariantMap& data
)
{
  if (!ConnectionLine::deserialize(data)) {
    return false;
  }
  
  if (data.contains("cardinalityOffsetX") && data.contains("cardinalityOffsetY")) {
    qreal offsetX = data["cardinalityOffsetX"].toDouble();
    qreal offsetY = data["cardinalityOffsetY"].toDouble();
    QPointF offset(offsetX, offsetY);

    if (isCardinalityOffsetValid(offset)) {
      m_cardinalityOffset = offset;
    }
  }

  if (m_relationshipEnd) {
    if (data.contains("relationshipEndEntityId")) {
      m_relationshipEnd->setEntityId(data["relationshipEndEntityId"].toString());
    }

    if (data.contains("relationshipEndCardinality")) {
      auto cardinality = static_cast<Cardinality>(data["relationshipEndCardinality"].toInt());
      m_relationshipEnd->setCardinality(cardinality);
    }

    if (data.contains("relationshipEndTotalParticipation")) {
      m_relationshipEnd->setIsTotalParticipation(data["relationshipEndTotalParticipation"].toBool());
    }

    if (data.contains("relationshipEndCustomText")) {
      m_relationshipEnd->setCustomCardinalityText(data["relationshipEndCustomText"].toString());
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------