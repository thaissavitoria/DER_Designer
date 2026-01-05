#include "Relationship.h"

//----------------------------------------------------------------------------------------------

Relationship::Relationship(
  QObject* parent
)
  : BasicElement(
    ElementType::Relationship,
    parent
  )
{
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

Relationship::Relationship(
  const QString& name,
  const bool isIdentifying,
  QObject* parent
)
  : BasicElement(
    ElementType::Relationship,
    parent
  )
{
  m_isIdentifying = isIdentifying;
  setName(name);
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

Relationship::Relationship(
  const Relationship& otherAtrribute
)
  : BasicElement(otherAtrribute),
  m_isIdentifying(otherAtrribute.m_isIdentifying)
{
}

//----------------------------------------------------------------------------------------------

QSizeF Relationship::minimumSize() const
{
  return QSizeF(100, 60);
}

//----------------------------------------------------------------------------------------------

QSizeF Relationship::preferredSize() const
{
  return QSizeF(140, 80);
}

//----------------------------------------------------------------------------------------------

std::unique_ptr<BasicElement> Relationship::clone() const
{
  auto cloned = std::make_unique<Relationship>(*this);

  return std::move(cloned);
}

//----------------------------------------------------------------------------------------------

QVariantMap Relationship::serialize() const
{
  QVariantMap data = BasicElement::serialize();

  data["isIdentifying"] = m_isIdentifying;

  QVariantList endsData;
  for (RelationshipEnd* end : m_ends) {
    QVariantMap endData;
    endData["entityId"] = end->entityId();
    endData["cardinality"] = RelationshipEnd::cardinalityToString(end->cardinality());
    endData["isTotalParticipation"] = end->isTotalParticipation();
    endData["customCardinalityText"] = end->customCardinalityText();
    endsData.append(endData);
  }
  data["ends"] = endsData;

  return data;
}

//----------------------------------------------------------------------------------------------

bool Relationship::deserialize(
  const QVariantMap& data
)
{
  if (!BasicElement::deserialize(data)) {
    return false;
  }

  m_isIdentifying = data.value("isIdentifying", false).toBool();

  clearEnds();

  QVariantList endsData = data.value("ends").toList();
  for (const QVariant& endVariant : endsData) {
    QVariantMap endData = endVariant.toMap();

    QString entityId = endData.value("entityId").toString();
    Cardinality cardinality = RelationshipEnd::cardinalityFromString(
      endData.value("cardinality").toString()
    );
    bool isTotalParticipation = endData.value("isTotalParticipation").toBool();
    QString customCardinalityText = endData.value("customCardinalityText").toString();

    auto end = new RelationshipEnd(entityId, cardinality, isTotalParticipation, this);
    end->setCustomCardinalityText(customCardinalityText);
    addEnd(end);
  }

  return true;
}

//----------------------------------------------------------------------------------------------

QString Relationship::typeDisplayName() const
{
  if (m_isIdentifying) {
    return "Relacionamento Identificador";
  }

  return "Relacionamento";
}

//----------------------------------------------------------------------------------------------

void Relationship::addEnd(
  RelationshipEnd* end
)
{
  m_ends.append(end);
}

//----------------------------------------------------------------------------------------------

void Relationship::removeEnd(
  RelationshipEnd* end
)
{
  m_ends.removeAll(end);
}

//----------------------------------------------------------------------------------------------

void Relationship::clearEnds()
{
  m_ends.clear();
}

//----------------------------------------------------------------------------------------------

RelationshipEnd* Relationship::findEndByEntityId(
  const QString& entityId
) const
{
  for (RelationshipEnd* end : m_ends) {
    if (end->entityId() == entityId) {
      return end;
    }
  }

  return nullptr;
}
//----------------------------------------------------------------------------------------------

void Relationship::setIsIdentifying(
  const bool isIdentifying
)
{
  if (m_isIdentifying != isIdentifying) {
    m_isIdentifying = isIdentifying;
    notifyObservers();
  }
}

//----------------------------------------------------------------------------------------------