#include "ConnectionLine.h"
#include "ConnectionPoint.h"
#include "BasicElement.h"
#include "controller/DiagramScene.h"

//----------------------------------------------------------------------------------------------

ConnectionLine::ConnectionLine(
  ConnectionPoint* startPoint,
  ConnectionPoint* endPoint,
  QObject* parent
)
  : QObject(parent)
  , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
  , m_startPoint(startPoint)
  , m_endPoint(endPoint)
  , m_lineType(ConnectionLineType::Straight)
  , m_lineWidth(2.0)
{
  connectToPoints();
}

//----------------------------------------------------------------------------------------------

ConnectionLine::~ConnectionLine()
{
  disconnectFromPoints();
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::setLineType(
  ConnectionLineType type
)
{
  if (m_lineType != type) {
    m_lineType = type;
    emit lineTypeChanged(m_lineType);
    emit connectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::setLineWidth(
  qreal width
)
{
  if (m_lineWidth != width) {
    m_lineWidth = width;
    emit lineWidthChanged(m_lineWidth);
    emit connectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::setStartPoint(
  ConnectionPoint* point
)
{
  if (m_startPoint != point) {
    if (m_startPoint) {
      auto startElement = qobject_cast<BasicElement*>(m_startPoint->parent());
      if (startElement) {
        disconnect(startElement, &BasicElement::positionChanged,
          this, &ConnectionLine::onElementPositionChanged);
      }
      disconnect(m_startPoint, &ConnectionPoint::relativePositionChanged,
        this, &ConnectionLine::onConnectionPointChanged);
    }

    m_startPoint = point;

    if (m_startPoint) {
      auto startElement = qobject_cast<BasicElement*>(m_startPoint->parent());
      if (startElement) {
        connect(startElement, &BasicElement::positionChanged,
          this, &ConnectionLine::onElementPositionChanged);
      }
      connect(m_startPoint, &ConnectionPoint::relativePositionChanged,
        this, &ConnectionLine::onConnectionPointChanged);
    }

    emit startPointChanged(m_startPoint);
    emit connectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::setEndPoint(
  ConnectionPoint* point
)
{
  if (m_endPoint != point) {
    if (m_endPoint) {
      auto endElement = qobject_cast<BasicElement*>(m_endPoint->parent());
      if (endElement) {
        disconnect(endElement, &BasicElement::positionChanged,
          this, &ConnectionLine::onElementPositionChanged);
      }
      disconnect(m_endPoint, &ConnectionPoint::relativePositionChanged,
        this, &ConnectionLine::onConnectionPointChanged);
    }

    m_endPoint = point;

    if (m_endPoint) {
      auto endElement = qobject_cast<BasicElement*>(m_endPoint->parent());
      if (endElement) {
        connect(endElement, &BasicElement::positionChanged,
          this, &ConnectionLine::onElementPositionChanged);
      }
      connect(m_endPoint, &ConnectionPoint::relativePositionChanged,
        this, &ConnectionLine::onConnectionPointChanged);
    }

    emit endPointChanged(m_endPoint);
    emit connectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

bool ConnectionLine::isValid() const
{
  if (!m_startPoint || !m_endPoint) {
    return false;
  }

  auto startElement = qobject_cast<BasicElement*>(m_startPoint->parent());
  auto endElement = qobject_cast<BasicElement*>(m_endPoint->parent());

  return startElement && endElement && startElement != endElement;
}

//----------------------------------------------------------------------------------------------

QPointF ConnectionLine::getStartPosition() const
{
  if (!m_startPoint) {
    return QPointF();
  }

  auto startElement = qobject_cast<BasicElement*>(m_startPoint->parent());
  if (!startElement) {
    return QPointF();
  }

  return m_startPoint->absolutePosition(startElement->position(), startElement->size());
}

//----------------------------------------------------------------------------------------------

QPointF ConnectionLine::getEndPosition() const
{
  if (!m_endPoint) {
    return QPointF();
  }

  auto endElement = qobject_cast<BasicElement*>(m_endPoint->parent());
  if (!endElement) {
    return QPointF();
  }

  return m_endPoint->absolutePosition(endElement->position(), endElement->size());
}

//----------------------------------------------------------------------------------------------

BasicElement* ConnectionLine::getStartElement() const
{
  if (!m_startPoint) {
    return nullptr;
  }

  return qobject_cast<BasicElement*>(m_startPoint->parent());
}

//----------------------------------------------------------------------------------------------

BasicElement* ConnectionLine::getEndElement() const
{
  if (!m_endPoint) {
    return nullptr;
  }

  return qobject_cast<BasicElement*>(m_endPoint->parent());
}

//----------------------------------------------------------------------------------------------

QString ConnectionLine::lineTypeToString(
  ConnectionLineType type
)
{
  switch (type) {
  case ConnectionLineType::Straight: return "Straight";
  case ConnectionLineType::Orthogonal: return "Orthogonal";
  case ConnectionLineType::Bezier: return "Bezier";
  default: return "Unknown";
  }
}

//----------------------------------------------------------------------------------------------

ConnectionLineType ConnectionLine::lineTypeFromString(
  const QString& typeString
)
{
  if (typeString == "Straight") return ConnectionLineType::Straight;
  if (typeString == "Orthogonal") return ConnectionLineType::Orthogonal;
  if (typeString == "Bezier") return ConnectionLineType::Bezier;
  return ConnectionLineType::Orthogonal;
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::connectToPoints()
{
  if (m_startPoint) {
    auto startElement = qobject_cast<BasicElement*>(m_startPoint->parent());
    if (startElement) {
      connect(startElement, &BasicElement::positionChanged,
        this, &ConnectionLine::onElementPositionChanged);
    }
    connect(m_startPoint, &ConnectionPoint::relativePositionChanged,
      this, &ConnectionLine::onConnectionPointChanged);
  }

  if (m_endPoint) {
    auto endElement = qobject_cast<BasicElement*>(m_endPoint->parent());
    if (endElement) {
      connect(endElement, &BasicElement::positionChanged,
        this, &ConnectionLine::onElementPositionChanged);
    }
    connect(m_endPoint, &ConnectionPoint::relativePositionChanged,
      this, &ConnectionLine::onConnectionPointChanged);
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::disconnectFromPoints()
{
  if (m_startPoint) {
    if (auto startElement = qobject_cast<BasicElement*>(m_startPoint->parent())) {
      disconnect(startElement, &BasicElement::positionChanged,
        this, &ConnectionLine::onElementPositionChanged);
    }
    disconnect(m_startPoint, &ConnectionPoint::relativePositionChanged,
      this, &ConnectionLine::onConnectionPointChanged);
  }

  if (m_endPoint) {
    if (auto endElement = qobject_cast<BasicElement*>(m_endPoint->parent())) {
      disconnect(endElement, &BasicElement::positionChanged,
        this, &ConnectionLine::onElementPositionChanged);
    }
    disconnect(m_endPoint, &ConnectionPoint::relativePositionChanged,
      this, &ConnectionLine::onConnectionPointChanged);
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::onElementPositionChanged()
{
  emit connectionChanged();
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::onConnectionPointChanged()
{
  emit connectionChanged();
}

//----------------------------------------------------------------------------------------------