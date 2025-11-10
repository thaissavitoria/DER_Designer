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
    , m_name(elementTypeToString(type))
{
    createDefaultConnectionPoints();
}

//----------------------------------------------------------------------------------------------

BasicElement::~BasicElement()
{
    m_observers.clear();

    qDeleteAll(m_connectionPoints);
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
        onNameChanged(oldName, name);
        emit nameChanged(name);
        emit elementChanged();
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
        onPositionChanged(oldPosition, position);
        emit positionChanged(position);
        emit elementChanged();
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
        onSizeChanged(oldSize, newSize);
        emit sizeChanged(newSize);
        emit elementChanged();
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

void BasicElement::setProperty(
    const QString& key,
    const QVariant& value
)
{
    if (key.isEmpty()) return;

    QVariant oldValue = m_customProperties.value(key);
    if (oldValue != value) {
        m_customProperties[key] = value;
        emit propertyChanged(key, value);
        emit elementChanged();
        notifyObservers();
    }
}

//----------------------------------------------------------------------------------------------

QVariant BasicElement::getProperty(
    const QString& key,
    const QVariant& defaultValue
) const
{
    return m_customProperties.value(key, defaultValue);
}

//----------------------------------------------------------------------------------------------

bool BasicElement::hasProperty(
    const QString& key
) const
{
    return m_customProperties.contains(key);
}

//----------------------------------------------------------------------------------------------

void BasicElement::removeProperty(
    const QString& key
)
{
    if (m_customProperties.remove(key)) {
        emit propertyChanged(key, QVariant());
        emit elementChanged();
        notifyObservers();
    }
}

//----------------------------------------------------------------------------------------------

QStringList BasicElement::propertyKeys() const
{
    return m_customProperties.keys();
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

QStringList BasicElement::validationErrors() const
{
    QStringList errors;

    if (m_name.isEmpty()) {
        errors << "Nome do elemento não pode estar vazio";
    }

    if (m_size.width() <= 0) {
        errors << "Largura deve ser maior que zero";
    }

    if (m_size.height() <= 0) {
        errors << "Altura deve ser maior que zero";
    }

    if (m_type == ElementType::Unknown) {
        errors << "Tipo do elemento não pode ser desconhecido";
    }

    QSizeF minSize = minimumSize();
    if (m_size.width() < minSize.width()) {
        errors << QString("Largura deve ser pelo menos %1").arg(minSize.width());
    }

    if (m_size.height() < minSize.height()) {
        errors << QString("Altura deve ser pelo menos %1").arg(minSize.height());
    }

    return errors;
}

//----------------------------------------------------------------------------------------------

QVariantMap BasicElement::serialize() const
{
    QVariantMap data;

    data["id"] = m_id;
    data["name"] = m_name;
    data["type"] = static_cast<int>(m_type);
    data["position"] = m_position;
    data["size"] = m_size;

    if (!m_customProperties.isEmpty()) {
        QVariantMap customProps;
        for (auto it = m_customProperties.begin(); it != m_customProperties.end(); ++it) {
            customProps[it.key()] = it.value();
        }
        data["customProperties"] = customProps;
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

        if (data.contains("position")) {
            m_position = data["position"].toPointF();
        }

        if (data.contains("size")) {
            m_size = data["size"].toSizeF();
        }

        if (data.contains("customProperties")) {
            QVariantMap customProps = data["customProperties"].toMap();
            m_customProperties.clear();
            for (auto it = customProps.begin(); it != customProps.end(); ++it) {
                m_customProperties[it.key()] = it.value();
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
    // Create a copy of the observers list to avoid issues with concurrent modifications
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

void BasicElement::onNameChanged(
    const QString& oldName,
    const QString& newName
)
{
    Q_UNUSED(oldName)
    Q_UNUSED(newName)
}

//----------------------------------------------------------------------------------------------

void BasicElement::onPositionChanged(
    const QPointF& oldPosition,
    const QPointF& newPosition
)
{
    Q_UNUSED(oldPosition)
    Q_UNUSED(newPosition)
}

//----------------------------------------------------------------------------------------------

void BasicElement::onSizeChanged(
    const QSizeF& oldSize,
    const QSizeF& newSize
)
{
    Q_UNUSED(oldSize)
    Q_UNUSED(newSize)
}

//----------------------------------------------------------------------------------------------
QString BasicElement::elementTypeToString(
  ElementType type
)
{
  switch (type) {
    case ElementType::Entity:
      return "Entity";
    case ElementType::Relationship:
      return "Relationship";
    default:
      return "Unknown";
  }
}

//----------------------------------------------------------------------------------------------

ElementType BasicElement::elementTypeFromString(
    const QString& typeString
)
{
    if (typeString == "Entity")
      return ElementType::Entity;
    if(typeString == "Attribute")
      return ElementType::Attribute;
    if (typeString == "Relationship")
      return ElementType::Relationship;

    return ElementType::Unknown;
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
    emit connectionPointAdded(connectionPoint);
}

//----------------------------------------------------------------------------------------------

void BasicElement::removeConnectionPoint(
    ConnectionPoint* connectionPoint
)
{
    if (!connectionPoint) {
        return;
    }
    
    if (m_connectionPoints.removeOne(connectionPoint)) {
        emit connectionPointRemoved(connectionPoint);
        connectionPoint->deleteLater();
    }
}

//----------------------------------------------------------------------------------------------

void BasicElement::removeConnectionPoint(
    const QString& connectionPointId
)
{
    auto connectionPoint = findConnectionPoint(connectionPointId);
    if (connectionPoint) {
        removeConnectionPoint(connectionPoint);
    }
}

//----------------------------------------------------------------------------------------------

ConnectionPoint* BasicElement::findConnectionPoint(
    const QString& connectionPointId
) const
{
    for (auto connectionPoint : m_connectionPoints) {
        if (connectionPoint->id() == connectionPointId) {
            return connectionPoint;
        }
    }
    return nullptr;
}

//----------------------------------------------------------------------------------------------

ConnectionPoint* BasicElement::findNearestConnectionPoint(
    const QPointF& worldPosition
) const
{
    if (m_connectionPoints.isEmpty()) {
        return nullptr;
    }
    
    ConnectionPoint* nearest = nullptr;
    qreal minDistance = std::numeric_limits<qreal>::max();
    
    for (auto connectionPoint : m_connectionPoints) {
        QPointF absolutePos = connectionPoint->absolutePosition(m_position, m_size);
        QPointF delta = worldPosition - absolutePos;
        qreal distance = delta.x() * delta.x() + delta.y() * delta.y();
        
        if (distance < minDistance) {
            minDistance = distance;
            nearest = connectionPoint;
        }
    }
    
    return nearest;
}

//----------------------------------------------------------------------------------------------

void BasicElement::createDefaultConnectionPoints()
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