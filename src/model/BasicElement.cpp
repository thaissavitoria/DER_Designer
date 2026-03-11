#include "BasicElement.h"

#include <QtCore/QDebug>
#include <QtCore/QUuid>
#include <QtCore/QVariantMap>

// -----------------------------------------------------------------------------------------------------

BasicElement::BasicElement(
  ElementType type,
  QObject* parent
)
  : QObject(parent)
  , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
  , m_type(type)
  , m_position(0, 0)
  , m_size(100, 50)
  , m_name(stringElementType())
{
  createAndAddDefaultConnectionPoints();
}

//----------------------------------------------------------------------------------------------

BasicElement::BasicElement(
  const BasicElement& otherElement
)
  : QObject(nullptr)
  , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
  , m_type(otherElement.m_type)
  , m_position(otherElement.m_position)
  , m_size(otherElement.m_size)
  , m_name(otherElement.m_name)
{
  createAndAddDefaultConnectionPoints();
}

//----------------------------------------------------------------------------------------------

BasicElement::~BasicElement()
{
  m_observers.clear();
  m_connectionPoints.clear();
}

//----------------------------------------------------------------------------------------------

void BasicElement::setName(
  const QString& name
)
{
  if (m_name != name) {
    QString oldName = m_name;
    m_name = name;
    notifyObservers();
  }
}

//----------------------------------------------------------------------------------------------

void BasicElement::setPosition(
  const QPointF& position
)
{
  if (m_position != position) {
    QPointF oldPosition = m_position;
    m_position = position;
    emit positionChanged(position);
    notifyPositionChanged(oldPosition, position);
  }
}

//----------------------------------------------------------------------------------------------

void BasicElement::setSize(
  const QSizeF& size
)
{
  QSizeF newSize = size;
  QSizeF minSize = minimumSize();

  if (newSize.width() < minSize.width()) {
    newSize.setWidth(minSize.width());
  }
  if (newSize.height() < minSize.height()) {
    newSize.setHeight(minSize.height());
  }

  if (m_size != newSize) {
    QSizeF oldSize = m_size;
    m_size = newSize;
    notifyObservers();
  }
}

//----------------------------------------------------------------------------------------------

void BasicElement::move(
  const QPointF& delta
)
{
  setPosition(m_position + delta);
}

//----------------------------------------------------------------------------------------------

void BasicElement::addObserver(
  IElementObserver* observer
)
{
  if (observer && !m_observers.contains(observer)) {
    m_observers.append(observer);
  }
}

//----------------------------------------------------------------------------------------------

void BasicElement::removeObserver(
  IElementObserver* observer
)
{
  m_observers.removeOne(observer);
}

//----------------------------------------------------------------------------------------------

bool BasicElement::isValid() const
{
  return !m_name.isEmpty() &&
    m_size.width() > 0 &&
    m_size.height() > 0 &&
    m_type != ElementType::Unknown;
}

//----------------------------------------------------------------------------------------------

QVariantMap BasicElement::serialize() const
{
  QVariantMap data;

  data["id"] = m_id;
  data["name"] = m_name;
  data["type"] = static_cast<int>(m_type);

  data["positionX"] = m_position.x();
  data["positionY"] = m_position.y();

  data["width"] = m_size.width();
  data["height"] = m_size.height();

  if (!m_attributeIds.isEmpty()) {
    QVariantList attributesList;
    for (const QString& attrId : m_attributeIds) {
      attributesList.append(attrId);
    }
    data["attributeIds"] = attributesList;
  }

  return data;
}

//----------------------------------------------------------------------------------------------

bool BasicElement::deserialize(
  const QVariantMap& data
)
{
  try {
    if (data.contains("id")) {
      m_id = data["id"].toString();
    }

    if (data.contains("name")) {
      m_name = data["name"].toString();
    }

    if (data.contains("type")) {
      m_type = static_cast<ElementType>(data["type"].toInt());
    }

    if (data.contains("positionX") && data.contains("positionY")) {
      qreal x = data["positionX"].toDouble();
      qreal y = data["positionY"].toDouble();
      m_position = QPointF(x, y);
    }

    if (data.contains("width") && data.contains("height")) {
      qreal width = data["width"].toDouble();
      qreal height = data["height"].toDouble();
      m_size = QSizeF(width, height);
    }

    m_attributeIds.clear();

    if (data.contains("attributeIds")) {
      QVariantList attributesList = data["attributeIds"].toList();
      for (const QVariant& attrVariant : attributesList) {
        QString attrId = attrVariant.toString();
        if (!attrId.isEmpty()) {
          m_attributeIds.append(attrId);
        }
      }
    }

    return true;
  }
  catch (...) {
    qWarning() << "Erro ao deserializar elemento" << m_id;
    return false;
  }
}

//----------------------------------------------------------------------------------------------

void BasicElement::notifyObservers()
{
  QList<IElementObserver*> observers = m_observers;
  for (IElementObserver* observer : observers) {
    if (m_observers.contains(observer)) {
      observer->onElementChanged(this);
    }
  }
}

//----------------------------------------------------------------------------------------------

void BasicElement::notifyPositionChanged(
  const QPointF& oldPosition,
  const QPointF& newPosition
)
{
  QList<IElementObserver*> observers = m_observers;
  for (IElementObserver* observer : observers) {
    if (m_observers.contains(observer)) {
      observer->onElementPositionChanged(this, oldPosition, newPosition);
    }
  }
}

//----------------------------------------------------------------------------------------------

void BasicElement::addConnectionPoint(
  ConnectionPoint* connectionPoint
)
{
  if (!connectionPoint || m_connectionPoints.contains(connectionPoint)) {
    return;
  }

  connectionPoint->setParent(this);
  m_connectionPoints.append(connectionPoint);
}

//----------------------------------------------------------------------------------------------

ConnectionPoint* BasicElement::getConnectionPointByDirection(
  ConnectionDirection connectionDirection
)
{
  for (ConnectionPoint* point : m_connectionPoints) {
    if (point->direction() == connectionDirection) {
      return point;
    }
  }

  return nullptr;
}

//----------------------------------------------------------------------------------------------

void BasicElement::createAndAddDefaultConnectionPoints()
{
  for (auto connectionPoint : m_connectionPoints) {
    connectionPoint->deleteLater();
  }
  m_connectionPoints.clear();

  auto defaultPoints = ConnectionPoint::createDefaultConnectionPoints(this);
  for (auto point : defaultPoints) {
    addConnectionPoint(point);
  }
}

//----------------------------------------------------------------------------------------------

void BasicElement::addAttributeId(
  const QString& attributeId
)
{
  if (!attributeId.isEmpty() && !m_attributeIds.contains(attributeId)) {
    m_attributeIds.append(attributeId);
  }
}

//----------------------------------------------------------------------------------------------

bool BasicElement::removeAttributeId(
  const QString& attributeId
)
{
  return m_attributeIds.removeOne(attributeId);
}

//----------------------------------------------------------------------------------------------

QList<QString> BasicElement::getAttributeIds() const
{
  return m_attributeIds;
}

//----------------------------------------------------------------------------------------------

void BasicElement::clearAttributesIds()
{
  m_attributeIds.clear();
}

//----------------------------------------------------------------------------------------------

QString BasicElement::stringElementType() const
{
  return "Elemento";
}

//----------------------------------------------------------------------------------------------