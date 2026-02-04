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
  , m_control1Offset(50, 0)
  , m_control2Offset(-50, 0)
  , m_hasCustomControlPoints(false)
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

    if (type != ConnectionLineType::Bezier) {
      resetControlPoints();
    }

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
      if (auto startElement = qobject_cast<BasicElement*>(m_startPoint->parent())) {
        disconnect(startElement, &BasicElement::positionChanged,
          this, &ConnectionLine::onElementPositionChanged);
      }
      disconnect(m_startPoint, &ConnectionPoint::relativePositionChanged,
        this, &ConnectionLine::onConnectionPointChanged);
    }

    m_startPoint = point;

    if (m_startPoint) {
      if (auto startElement = qobject_cast<BasicElement*>(m_startPoint->parent())) {
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

void ConnectionLine::setControl1Offset(
  const QPointF& offset
)
{
  if (m_control1Offset != offset) {
    m_control1Offset = offset;
    m_hasCustomControlPoints = true;
    emit controlPointsChanged();
    emit connectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::setControl2Offset(
  const QPointF& offset
)
{
  if (m_control2Offset != offset) {
    m_control2Offset = offset;
    m_hasCustomControlPoints = true;
    emit controlPointsChanged();
    emit connectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::setHasCustomControlPoints(
  bool hasCustom
)
{
  if (m_hasCustomControlPoints != hasCustom) {
    m_hasCustomControlPoints = hasCustom;
    emit controlPointsChanged();
  }
}

//----------------------------------------------------------------------------------------------

void ConnectionLine::resetControlPoints()
{
  m_control1Offset = QPointF(50, 0);
  m_control2Offset = QPointF(-50, 0);
  m_hasCustomControlPoints = false;
  emit controlPointsChanged();
  emit connectionChanged();
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
    if (auto startElement = qobject_cast<BasicElement*>(m_startPoint->parent())){
      connect(startElement, &BasicElement::positionChanged,
        this, &ConnectionLine::onElementPositionChanged);
    }
    connect(m_startPoint, &ConnectionPoint::relativePositionChanged,
      this, &ConnectionLine::onConnectionPointChanged);
  }

  if (m_endPoint) {
    if (auto endElement = qobject_cast<BasicElement*>(m_endPoint->parent())) {
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

QVariantMap ConnectionLine::serialize() const
{
  QVariantMap data;

  data["id"] = m_id;

  BasicElement* startElement = getStartElement();
  BasicElement* endElement = getEndElement();

  if (startElement && endElement) {
    data["startConnectionPointDirection"] = static_cast<int>(m_startPoint->direction());
    data["endConnectionPointDirection"] = static_cast<int>(m_endPoint->direction());
    data["startElementId"] = startElement->id();
    data["endElementId"] = endElement->id();
    data["lineType"] = static_cast<int>(lineType());
    data["lineWidth"] = lineWidth();

    if (m_hasCustomControlPoints && m_lineType == ConnectionLineType::Bezier) {
      QVariantMap control1Data;
      control1Data["x"] = m_control1Offset.x();
      control1Data["y"] = m_control1Offset.y();

      QVariantMap control2Data;
      control2Data["x"] = m_control2Offset.x();
      control2Data["y"] = m_control2Offset.y();

      data["control1Offset"] = control1Data;
      data["control2Offset"] = control2Data;
      data["hasCustomControlPoints"] = m_hasCustomControlPoints;
    }
  }

  return data;
}

//----------------------------------------------------------------------------------------------

bool ConnectionLine::deserialize(
  const QVariantMap& data
)
{
  if (data.isEmpty()) {
    return false;
  }

  if (data.contains("lineType")) {
    auto lineType = static_cast<ConnectionLineType>(data["lineType"].toInt());
    setLineType(lineType);
  }

  if (data.contains("lineWidth")) {
    setLineWidth(data["lineWidth"].toDouble());
  }

  if (data.contains("hasCustomControlPoints")) {
    m_hasCustomControlPoints = data["hasCustomControlPoints"].toBool();
  }

  if (data.contains("control1Offset")) {
    QVariantMap control1Data = data["control1Offset"].toMap();
    if (control1Data.contains("x") && control1Data.contains("y")) {
      m_control1Offset = QPointF(
        control1Data["x"].toDouble(),
        control1Data["y"].toDouble()
      );
    }
  }

  if (data.contains("control2Offset")) {
    QVariantMap control2Data = data["control2Offset"].toMap();
    if (control2Data.contains("x") && control2Data.contains("y")) {
      m_control2Offset = QPointF(
        control2Data["x"].toDouble(),
        control2Data["y"].toDouble()
      );
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------