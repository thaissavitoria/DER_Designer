#include "Attribute.h"

//----------------------------------------------------------------------------------------------

Attribute::Attribute(
  QObject* parent
)
  : BasicElement(
    ElementType::Attribute,
    parent
  ),
  m_attributeType(AttributeType::Type::Normal),
  m_isPrimaryKey(false)
{
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

Attribute::Attribute(
  const QString& name,
  QObject* parent
)
  : BasicElement(
    ElementType::Attribute,
    parent
  ),
  m_attributeType(AttributeType::Type::Normal),
  m_isPrimaryKey(false)
{
  setName(name);
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

Attribute::Attribute(
  const QString& name,
  AttributeType::Type type,
  QObject* parent
)
  : BasicElement(
    ElementType::Attribute,
    parent
  ),
  m_attributeType(type),
  m_isPrimaryKey(false)
{
  setName(name);
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

QSizeF Attribute::minimumSize() const
{
  return QSizeF(60, 30);
}

//----------------------------------------------------------------------------------------------

QSizeF Attribute::preferredSize() const
{
  return QSizeF(100, 50);
}

//----------------------------------------------------------------------------------------------

std::unique_ptr<BasicElement> Attribute::clone() const
{
  auto cloned = std::make_unique<Attribute>();

  QVariantMap data = serialize();
  cloned->deserialize(data);

  cloned->setAttributeType(m_attributeType);
  cloned->setPrimaryKey(m_isPrimaryKey);

  return std::move(cloned);
}

//----------------------------------------------------------------------------------------------

QString Attribute::typeDisplayName() const
{
  return AttributeType::attributeTypeToString(m_attributeType);
}

//----------------------------------------------------------------------------------------------

AttributeType::Type Attribute::attributeType() const
{
  return m_attributeType;
}

//----------------------------------------------------------------------------------------------

void Attribute::setAttributeType(
  AttributeType::Type type
)
{
  if (m_attributeType != type) {
    if (m_attributeType == AttributeType::Composite && type != AttributeType::Composite) {
      QList<QString> subAttributeIdsToRemove = m_subAttributeIds;
      m_subAttributeIds.clear();

      for (const QString& subAttrId : subAttributeIdsToRemove) {
        emit subAttributeRemoved(subAttrId);
      }

      setPrimaryKey(false);
    }

    if (m_attributeType == AttributeType::Normal && type != AttributeType::Normal) {
      setPrimaryKey(false);
    }

    m_attributeType = type;
    emit attributeTypeChanged(type);
    emit elementChanged();
  }
}

//----------------------------------------------------------------------------------------------

QString Attribute::attributeTypeString() const
{
  return AttributeType::attributeTypeToString(m_attributeType);
}

//----------------------------------------------------------------------------------------------

bool Attribute::isPrimaryKey() const
{
  return m_isPrimaryKey;
}

//----------------------------------------------------------------------------------------------

void Attribute::setPrimaryKey(
  bool isPrimary
)
{
  if (m_isPrimaryKey != isPrimary) {
    m_isPrimaryKey = isPrimary;
    emit primaryKeyChanged(isPrimary);
    emit elementChanged();
  }
}

//----------------------------------------------------------------------------------------------

bool Attribute::isNormalAttribute() const
{
  return m_attributeType == AttributeType::Normal;
}

//----------------------------------------------------------------------------------------------

bool Attribute::isDerivedAttribute() const
{
  return m_attributeType == AttributeType::Derived;
}

//----------------------------------------------------------------------------------------------

bool Attribute::isMultivaluedAttribute() const
{
  return m_attributeType == AttributeType::Multivalued;
}

//----------------------------------------------------------------------------------------------

bool Attribute::isCompositeAttribute() const
{
  return m_attributeType == AttributeType::Composite;
}

//----------------------------------------------------------------------------------------------

void Attribute::addSubAttributeId(
  const QString& subAttributeId
)
{
  if (!subAttributeId.isEmpty() && !m_subAttributeIds.contains(subAttributeId)) {
    m_subAttributeIds.append(subAttributeId);
  }
}

//----------------------------------------------------------------------------------------------

bool Attribute::removeSubAttributeId(
  const QString& subAttributeId
)
{
  return m_subAttributeIds.removeOne(subAttributeId);
}

//----------------------------------------------------------------------------------------------

QList<QString> Attribute::getSubAttributeIds() const
{
  return m_subAttributeIds;
}

//----------------------------------------------------------------------------------------------

QVariantMap Attribute::serialize() const
{
  QVariantMap data = BasicElement::serialize();

  data["attributeType"] = static_cast<int>(m_attributeType);
  data["isPrimaryKey"] = m_isPrimaryKey;

  if (!m_subAttributeIds.isEmpty()) {
    QVariantList subAttributesList;
    for (const QString& subAttrId : m_subAttributeIds) {
      subAttributesList.append(subAttrId);
    }
    data["subAttributeIds"] = subAttributesList;
  }

  return data;
}

//----------------------------------------------------------------------------------------------

bool Attribute::deserialize(
  const QVariantMap& data
)
{
  if (!BasicElement::deserialize(data)) {
    return false;
  }

  try {
    if (data.contains("attributeType")) {
      m_attributeType = static_cast<AttributeType::Type>(data["attributeType"].toInt());
    }

    if (data.contains("isPrimaryKey")) {
      m_isPrimaryKey = data["isPrimaryKey"].toBool();
    }

    m_subAttributeIds.clear();

    if (data.contains("subAttributeIds")) {
      QVariantList subAttributesList = data["subAttributeIds"].toList();
      for (const QVariant& subAttrVariant : subAttributesList) {
        QString subAttrId = subAttrVariant.toString();
        if (!subAttrId.isEmpty()) {
          m_subAttributeIds.append(subAttrId);
        }
      }
    }

    return true;
  }
  catch (...) {
    qWarning() << "Erro ao deserializar atributo" << id();
    return false;
  }
}

//----------------------------------------------------------------------------------------------