#include "RelationshipEnd.h"

//----------------------------------------------------------------------------------------------

RelationshipEnd::RelationshipEnd(
  const QString& entityId,
  Cardinality cardinality,
  bool isTotalParticipation,
  QObject* parent
)
  : QObject(parent)
  , m_entityId(entityId)
  , m_cardinality(cardinality)
  , m_isTotalParticipation(isTotalParticipation)
  , m_customCardinalityText("")
{
}

//----------------------------------------------------------------------------------------------

void RelationshipEnd::setEntityId(
  const QString& entityId
)
{
  if (m_entityId != entityId) {
    m_entityId = entityId;
  }
}

//----------------------------------------------------------------------------------------------

void RelationshipEnd::setCardinality(
  Cardinality cardinality
)
{
  if (m_cardinality != cardinality) {
    m_cardinality = cardinality;

    if (cardinality == Cardinality::One) {
      m_customCardinalityText = "";
    }
    else if (m_customCardinalityText.isEmpty()) {
      m_customCardinalityText = "M";
    }

    emit cardinalityChanged(m_cardinality);
  }
}

//----------------------------------------------------------------------------------------------

void RelationshipEnd::setIsTotalParticipation(
  bool isTotalParticipation
)
{
  if (m_isTotalParticipation != isTotalParticipation) {
    m_isTotalParticipation = isTotalParticipation;
    emit totalParticipationChanged(m_isTotalParticipation);
  }
}

//----------------------------------------------------------------------------------------------

void RelationshipEnd::setCustomCardinalityText(
  const QString& text
)
{
  QString sanitizedText = text.toUpper().trimmed();

  if (sanitizedText.length() > 1) {
    sanitizedText = sanitizedText.left(1);
  }

  bool isValidLetter = sanitizedText.isEmpty() ||
    (sanitizedText.length() == 1 && sanitizedText[0].isLetter());

  if (!isValidLetter) {
    return;
  }

  if (m_customCardinalityText != sanitizedText) {
    m_customCardinalityText = sanitizedText;
    emit customCardinalityTextChanged(m_customCardinalityText);
  }
}

//----------------------------------------------------------------------------------------------

QString RelationshipEnd::getDisplayCardinalityText() const
{
  if (m_cardinality == Cardinality::One) {
    return "1";
  }

  if (!m_customCardinalityText.isEmpty()) {
    return m_customCardinalityText;
  }

  return "M";
}

//----------------------------------------------------------------------------------------------

QString RelationshipEnd::cardinalityToString(
  Cardinality cardinality
)
{
  switch (cardinality) {
  case Cardinality::One:
    return "Um";
  case Cardinality::Many:
  default:
    return "Muitos";
  }
}

//----------------------------------------------------------------------------------------------

Cardinality RelationshipEnd::cardinalityFromString(
  const QString& cardinalityString
)
{
  if (cardinalityString == "Um") {
    return Cardinality::One;
  }

  return Cardinality::Many;
}

//----------------------------------------------------------------------------------------------