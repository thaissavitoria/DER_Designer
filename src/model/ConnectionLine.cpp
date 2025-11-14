#include "ConnectionLine.h"
#include "ConnectionPoint.h"
#include "BasicElement.h"
#include "controller/DiagramScene.h"

//----------------------------------------------------------------------------------------------

ConnectionLine::ConnectionLine(
  BasicElement* startElement,
  BasicElement* endElement,
  QObject* parent
)
  : QObject(parent)
  , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
  , m_startElement(startElement)
  , m_endElement(endElement)
  , m_lineType(ConnectionLineType::Orthogonal)
  , m_lineColor(Qt::black)
  , m_lineWidth(2.0)
{
  connectToElements();
}

//----------------------------------------------------------------------------------------------

ConnectionLine::~ConnectionLine()
{
  disconnectFromElements();
  emit connectionBeingDestroyed();
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

void ConnectionLine::setLineColor(
  const QColor& color
)
{
  if (m_lineColor != color) {
    m_lineColor = color;
    emit lineColorChanged(m_lineColor);
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

void ConnectionLine::setStartElement(
  BasicElement* element
)
{
  if (m_startElement != element) {
    if (m_startElement) {
      disconnect(m_startElement, &BasicElement::positionChanged,
        this, &ConnectionLine::onElementPositionChanged);
      disconnect(m_startElement, &BasicElement::elementBeingDestroyed,
        this, &ConnectionLine::onElementDestroyed);
    }

    m_startElement = element;

    if (m_startElement) {
      connect(m_startElement, &BasicElement::positionChanged,
        this, &ConnectionLine::onElementPositionChanged);
      connect(m_startElement, &BasicElement::elementBeingDestroyed,
        this, &ConnectionLine::onElementDestroyed);
    }

    emit startElementChanged(m_startElement);
    emit connectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::setEndElement(
  BasicElement* element
)
{
  if (m_endElement != element) {
    if (m_endElement) {
      disconnect(m_endElement, &BasicElement::positionChanged,
        this, &ConnectionLine::onElementPositionChanged);
      disconnect(m_endElement, &BasicElement::elementBeingDestroyed,
        this, &ConnectionLine::onElementDestroyed);
    }

    m_endElement = element;

    if (m_endElement) {
      connect(m_endElement, &BasicElement::positionChanged,
        this, &ConnectionLine::onElementPositionChanged);
      connect(m_endElement, &BasicElement::elementBeingDestroyed,
        this, &ConnectionLine::onElementDestroyed);
    }

    emit endElementChanged(m_endElement);
    emit connectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

bool ConnectionLine::isValid() const
{
  return m_startElement && m_endElement && m_startElement != m_endElement;
}

//----------------------------------------------------------------------------------------------

QPointF ConnectionLine::getStartPosition() const
{
  if (!m_startElement || !m_endElement) {
    return QPointF();
  }

  auto connectionPoint = findBestConnectionPoint(m_startElement, m_endElement);
  if (!connectionPoint) {
    return m_startElement->position() + QPointF(m_startElement->size().width() / 2,
      m_startElement->size().height() / 2);
  }

  return connectionPoint->absolutePosition(m_startElement->position(), m_startElement->size());
}

//----------------------------------------------------------------------------------------------

QPointF ConnectionLine::getEndPosition() const
{
  if (!m_startElement || !m_endElement) {
    return QPointF();
  }

  auto connectionPoint = findBestConnectionPoint(m_endElement, m_startElement);
  if (!connectionPoint) {
    return m_endElement->position() + QPointF(m_endElement->size().width() / 2,
      m_endElement->size().height() / 2);
  }

  return connectionPoint->absolutePosition(m_endElement->position(), m_endElement->size());
}

//----------------------------------------------------------------------------------------------

ConnectionPoint* ConnectionLine::getStartConnectionPoint() const
{
  if (!m_startElement || !m_endElement) {
    return nullptr;
  }

  return findBestConnectionPoint(m_startElement, m_endElement);
}

//----------------------------------------------------------------------------------------------

ConnectionPoint* ConnectionLine::getEndConnectionPoint() const
{
  if (!m_startElement || !m_endElement) {
    return nullptr;
  }

  return findBestConnectionPoint(m_endElement, m_startElement);
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

void ConnectionLine::connectToElements()
{
  if (m_startElement) {
    connect(m_startElement, &BasicElement::positionChanged,
      this, &ConnectionLine::onElementPositionChanged);
    connect(m_startElement, &BasicElement::elementBeingDestroyed,
      this, &ConnectionLine::onElementDestroyed);
  }

  if (m_endElement) {
    connect(m_endElement, &BasicElement::positionChanged,
      this, &ConnectionLine::onElementPositionChanged);
    connect(m_endElement, &BasicElement::elementBeingDestroyed,
      this, &ConnectionLine::onElementDestroyed);
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::disconnectFromElements()
{
  if (m_startElement) {
    disconnect(m_startElement, &BasicElement::positionChanged,
      this, &ConnectionLine::onElementPositionChanged);
    disconnect(m_startElement, &BasicElement::elementBeingDestroyed,
      this, &ConnectionLine::onElementDestroyed);
  }

  if (m_endElement) {
    disconnect(m_endElement, &BasicElement::positionChanged,
      this, &ConnectionLine::onElementPositionChanged);
    disconnect(m_endElement, &BasicElement::elementBeingDestroyed,
      this, &ConnectionLine::onElementDestroyed);
  }
}

//----------------------------------------------------------------------------------------------

ConnectionPoint* ConnectionLine::findBestConnectionPoint(
  BasicElement* fromElement,
  BasicElement* toElement
) const
{
  if (!fromElement || !toElement) {
    return nullptr;
  }

  auto connectionPoints = fromElement->connectionPoints();
  if (connectionPoints.isEmpty()) {
    return nullptr;
  }

   //Calcular o centro do elemento de destino
  QPointF targetCenter = toElement->position() +
    QPointF(toElement->size().width() / 2, toElement->size().height() / 2);

  ConnectionPoint* bestPoint = nullptr;
  qreal minDistance = std::numeric_limits<qreal>::max();

  for (auto* connectionPoint : connectionPoints) {
    QPointF pointPosition = connectionPoint->absolutePosition(fromElement->position(),
      fromElement->size());
    qreal distance = QLineF(pointPosition, targetCenter).length();

    if (distance < minDistance) {
      minDistance = distance;
      bestPoint = connectionPoint;
    }
  }

  return bestPoint;
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::onElementPositionChanged()
{
  emit connectionChanged();
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::onElementDestroyed(
  BasicElement* basicElementSender
)
{
  disconnectFromElements();

  if (basicElementSender == m_startElement) {
    m_startElement = nullptr;
  }
  else if (basicElementSender == m_endElement) {
    m_endElement = nullptr;
  }
  
  deleteLater();
}

//----------------------------------------------------------------------------------------------

