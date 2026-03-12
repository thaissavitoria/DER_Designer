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

Attribute::Attribute(
  const Attribute& otherAtrribute
)
  : BasicElement(otherAtrribute),
  m_attributeType(otherAtrribute.m_attributeType),
  m_isPrimaryKey(otherAtrribute.m_isPrimaryKey)
{
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
  auto cloned = std::make_unique<Attribute>(*this);

  return cloned;
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
      clearAttributesIds();
      setPrimaryKey(false);
    }

    if (m_attributeType == AttributeType::Normal && type != AttributeType::Normal) {
      setPrimaryKey(false);
    }

    m_attributeType = type;
    notifyObservers();
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
    notifyObservers();
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

QVariantMap Attribute::serialize() const
{
  QVariantMap data = BasicElement::serialize();

  data["attributeType"] = static_cast<int>(m_attributeType);
  data["isPrimaryKey"] = m_isPrimaryKey;

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

    return true;
  }
  catch (...) {
    qWarning() << "Erro ao deserializar atributo" << id();
    return false;
  }
}

//----------------------------------------------------------------------------------------------

QString Attribute::stringElementType() const
{
  return "Atributo";
}

//----------------------------------------------------------------------------------------------
