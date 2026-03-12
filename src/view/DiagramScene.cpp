#include "DiagramScene.h"
#include "model/Entity.h"
#include "model/Relationship.h"
#include "model/RelationshipConnectionLine.h"
#include "view/ConnectionGraphicsItem.h"
#include "view/RelationshipConnectionLineGraphicsItem.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPageLayout>

#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QGraphicsSceneMouseEvent>

// -----------------------------------------------------------------------------------------------------

DiagramScene::DiagramScene(QObject* parent)
  : QGraphicsScene(parent)
  , m_selectionRectActive(false)
  , m_selectionRect(nullptr)
  , m_isCreatingConnection(false)
  , m_connectionStartPoint(nullptr)
  , m_temporaryConnectionLine(nullptr)
{
  setItemIndexMethod(QGraphicsScene::NoIndex);
  setSceneRect(-5000, -5000, 10000, 10000);
}

// -----------------------------------------------------------------------------------------------------

DiagramScene::~DiagramScene()
{
  clearDiagram();
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::addElement(
  BasicElement* element
)
{
  if (!element || m_elements.contains(element->id())) {
    return;
  }

  m_elements[element->id()] = element;

  auto item = new ElementGraphicsItem(element);
  m_elementToItem[element] = item;

  addItem(item);

  emit elementAdded(element);
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::clearDiagram()
{
  cancelConnection();

  clearSelection(false/*emitSignals*/);

  m_clipboardElements.clear();
  m_clipboardConnections.clear();
  m_connectionToItem.clear();
  m_elementToItem.clear();
  m_connections.clear();
  m_elements.clear();

  destroySelectionRect();

  QGraphicsScene::clear();
}

//----------------------------------------------------------------------------------------------

void DiagramScene::removeElement(
  BasicElement* element
)
{
  if (!element || !m_elements.contains(element->id())) {
    return;
  }

  if (auto attribute = qobject_cast<Attribute*>(element)) {
    removeAttributeFromParents(attribute);
  }

  removeElementConnections(element);

  if (ElementGraphicsItem* item = m_elementToItem.value(element, nullptr)) {
    removeItem(item);
    m_elementToItem.remove(element);
    delete item;
  }

  m_elements.remove(element->id());

  delete element;
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::removeAttributeFromParents(
  Attribute* attribute
)
{
  if (auto parent = qobject_cast<BasicElement*>(attribute->parent())) {
    parent->removeAttributeId(attribute->id());
  }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::removeElementConnections(
  BasicElement* element
)
{
  QSet<ConnectionLine*> connectionsToRemove;
  for (ConnectionLine* connection : getAllConnections()) {
    if (connection->getStartElement() == element || connection->getEndElement() == element) {
      connectionsToRemove.insert(connection);
    }
  }

  for (ConnectionLine* connection : connectionsToRemove) {
    removeConnection(connection);
  }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::removeElement(
  const QString& elementId
)
{
  if (BasicElement* element = findElement(elementId)) {
    removeElement(element);
  }
}

// -----------------------------------------------------------------------------------------------------

BasicElement* DiagramScene::findElement(
  const QString& id
) const
{
  return m_elements.value(id, nullptr);
}

// -----------------------------------------------------------------------------------------------------

ElementGraphicsItem* DiagramScene::findGraphicsItem(
  const QString& elementId
) const
{
  BasicElement* element = findElement(elementId);
  return element ? findGraphicsItem(element) : nullptr;
}

// -----------------------------------------------------------------------------------------------------

ElementGraphicsItem* DiagramScene::findGraphicsItem(
  BasicElement* element
) const
{
  return m_elementToItem.value(element, nullptr);
}

// -----------------------------------------------------------------------------------------------------

QList<BasicElement*> DiagramScene::getAllElements() const
{
  return m_elements.values();
}

// -----------------------------------------------------------------------------------------------------

QList<BasicElement*> DiagramScene::getSelectedElements() const
{
  QList<BasicElement*> selected;

  for (auto it = m_elementToItem.begin(); it != m_elementToItem.end(); ++it) {
    if (it.value()->isSelected()) {
      selected.append(it.key());
    }
  }

  return selected;
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::selectElement(
  BasicElement* element,
  const bool selected,
  const bool emitSignals
)
{
  if (ElementGraphicsItem* item = findGraphicsItem(element)) {
    item->setSelected(selected);

    if (emitSignals) {
      if (selected) {
        emit elementSelected(element);
      }
      else {
        emit elementDeselected(element);
      }

      emit selectionChanged();
    }
  }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::selectConnection(
  ConnectionLine* connection,
  const bool selected,
  const bool emitSignals
)
{
  auto connectionItem = m_connectionToItem.value(connection, nullptr);
  if (connectionItem) {
    connectionItem->setSelected(selected);

    if (emitSignals) {
      if (selected) {
        emit connectionSelected(connection);
      }
      else {
        emit connectionDeselected(connection);
      }

      emit selectionChanged();
    }
  }
}

//----------------------------------------------------------------------------------------------

void DiagramScene::clearSelection(
  const bool emitSignals
)
{
  QList<BasicElement*> selectedElements = getSelectedElements();
  QSet<ConnectionLine*> selectedConnections = getSelectedConnections();

  for (BasicElement* element : selectedElements) {
    selectElement(element, false /*selected*/, emitSignals);
  }

  for (ConnectionLine* connection : selectedConnections) {
    selectConnection(connection, false /*selected*/, emitSignals);
  }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::selectAll()
{
  for (auto it = m_elementToItem.begin(); it != m_elementToItem.end(); ++it) {
    it.value()->setSelected(true);
    emit elementSelected(it.key());
  }

  for (auto it = m_connectionToItem.begin(); it != m_connectionToItem.end(); ++it) {
    it.value()->setSelected(true);
    emit connectionSelected(it.key());
  }

  if (!m_elementToItem.isEmpty() || !m_connectionToItem.isEmpty()) {
    emit selectionChanged();
  }
}

// -----------------------------------------------------------------------------------------------------

QSet<ConnectionLine*> DiagramScene::getSelectedConnections() const
{
  QSet<ConnectionLine*> selected;

  for (auto it = m_connectionToItem.begin(); it != m_connectionToItem.end(); ++it) {
    if (it.value()->isSelected()) {
      selected.insert(it.key());
    }
  }

  return selected;
}

//----------------------------------------------------------------------------------------------

void DiagramScene::deleteSelected()
{
  QList<BasicElement*> selectedElements = getSelectedElements();

  for (BasicElement* element : selectedElements) {
    removeElement(element);
  }

  QSet<ConnectionLine*> selectedConnections = getSelectedConnections();

  for (ConnectionLine* connection : selectedConnections) {
    removeConnection(connection);
  }

  if (!selectedElements.isEmpty() || !selectedConnections.isEmpty()) {
    emit selectionChanged();
  }
}

//----------------------------------------------------------------------------------------------

void DiagramScene::removeConnection(
  ConnectionLine* connection
)
{
  if (!connection) return;

  if (auto relationshipConnection = qobject_cast<RelationshipConnectionLine*>(connection)) {
    handleRelationshipDisconnection(relationshipConnection);
  }
  else {
    handleAttributeDisconnection(connection);
  }

  if (ConnectionGraphicsItem* item = m_connectionToItem.value(connection, nullptr)) {
    removeItem(item);
    m_connectionToItem.remove(connection);
    delete item;
  }

  m_connections.remove(connection->id());
  delete connection;

  update();
}

//----------------------------------------------------------------------------------------------

void DiagramScene::duplicateSelected()
{
  QList<BasicElement*> selected = getSelectedElements();

  for (BasicElement* element : selected) {
    if (auto cloned = element->clone()) {
      QPointF offset(20, 20);
      cloned->setPosition(element->position() + offset);

      addElement(cloned.release());
    }
  }
}

//----------------------------------------------------------------------------------------------

void DiagramScene::copySelected()
{
  m_clipboardElements.clear();
  m_clipboardConnections.clear();

  QList<BasicElement*> selectedElements = getSelectedElements();
  QSet<ConnectionLine*> selectedConnections = getSelectedConnections();

  QHash<QString, QString> originalToClipboardId;

  for (BasicElement* element : selectedElements) {
    if (auto cloned = element->clone()) {
      originalToClipboardId[element->id()] = cloned->id();
      m_clipboardElements.append(cloned.release());
    }
  }

  for (ConnectionLine* connection : selectedConnections) {
    auto startElement = connection->getStartElement();
    auto endElement = connection->getEndElement();

    if (selectedElements.contains(startElement) && selectedElements.contains(endElement)) {
      m_clipboardConnections.insert(connection);
    }
  }

  m_clipboardIdMap = originalToClipboardId;
}

//----------------------------------------------------------------------------------------------

void DiagramScene::pasteFromClipboard()
{
  if (m_clipboardElements.isEmpty()) {
    return;
  }

  clearSelection();

  QHash<QString, BasicElement*> clipboardIdToNewElement;
  QList<BasicElement*> newElements;
  QList<ConnectionLine*> newConnections;

  static int pasteCount = 0;
  QPointF offset(20 * pasteCount, 20 * pasteCount);
  pasteCount = (pasteCount + 1) % 10;

  for (BasicElement* clipboardElement : m_clipboardElements) {
    if (auto cloned = clipboardElement->clone()) {
      cloned->setPosition(clipboardElement->position() + offset);

      clipboardIdToNewElement[clipboardElement->id()] = cloned.get();
      newElements.append(cloned.get());

      addElement(cloned.release());
    }
  }

  for (ConnectionLine* originalConnection : m_clipboardConnections) {
    auto startElement = originalConnection->getStartElement();
    auto endElement = originalConnection->getEndElement();

    if (!startElement || !endElement) {
      continue;
    }

    QString clipboardStartId = m_clipboardIdMap.value(startElement->id(), QString());
    QString clipboardEndId = m_clipboardIdMap.value(endElement->id(), QString());

    if (clipboardStartId.isEmpty() || clipboardEndId.isEmpty()) {
      continue;
    }

    auto newStartElement = clipboardIdToNewElement.value(clipboardStartId, nullptr);
    auto newEndElement = clipboardIdToNewElement.value(clipboardEndId, nullptr);

    if (!newStartElement || !newEndElement) {
      continue;
    }

    auto originalStartPoint = originalConnection->startPoint();
    auto originalEndPoint = originalConnection->endPoint();

    if (!originalStartPoint || !originalEndPoint) {
      continue;
    }

    auto newStartPoint = findConnectionPointInClonedElement(newStartElement, originalStartPoint);
    auto newEndPoint = findConnectionPointInClonedElement(newEndElement, originalEndPoint);

    if (!newStartPoint || !newEndPoint) {
      continue;
    }

    if (auto relationshipConnection = qobject_cast<RelationshipConnectionLine*>(originalConnection)) {
      auto relationship = qobject_cast<Relationship*>(newStartElement);
      if (!relationship) {
        relationship = qobject_cast<Relationship*>(newEndElement);
      }

      auto entity = qobject_cast<Entity*>(newStartElement);
      if (!entity) {
        entity = qobject_cast<Entity*>(newEndElement);
      }

      if (relationship && entity) {
        auto relationshipEnd = new RelationshipEnd(
          entity->id(),
          relationshipConnection->relationshipEnd()->cardinality(),
          relationshipConnection->relationshipEnd()->isTotalParticipation(),
          relationship
        );
        relationshipEnd->setCustomCardinalityText(relationshipConnection->relationshipEnd()->customCardinalityText());

        relationship->addEnd(relationshipEnd);

        auto newConnection = new RelationshipConnectionLine(
          newStartPoint,
          newEndPoint,
          relationshipEnd,
          this
        );

        newConnection->setControl1Offset(originalConnection->control1Offset());
        newConnection->setControl2Offset(originalConnection->control2Offset());
        newConnection->setHasCustomControlPoints(originalConnection->hasCustomControlPoints());
        newConnection->setCardinalityOffset(relationshipConnection->cardinalityOffset());
        newConnection->setLineType(originalConnection->lineType());

        addRelationshipConnection(newConnection);
        newConnections.append(newConnection);
      }
    }
    else {
      auto newConnection = new ConnectionLine(
        newStartPoint,
        newEndPoint,
        this
      );

      newConnection->setControl1Offset(originalConnection->control1Offset());
      newConnection->setControl2Offset(originalConnection->control2Offset());
      newConnection->setHasCustomControlPoints(originalConnection->hasCustomControlPoints());
      newConnection->setLineType(originalConnection->lineType());

      addConnection(newConnection);
      newConnections.append(newConnection);
    }
  }

  for (BasicElement* element : newElements) {
    selectElement(element, true);
  }
}

//----------------------------------------------------------------------------------------------

ConnectionPoint* DiagramScene::findConnectionPointInClonedElement(
  BasicElement* clonedElement,
  ConnectionPoint* originalPoint
) const
{
  if (!clonedElement || !originalPoint) {
    return nullptr;
  }

  QString originalPointId = originalPoint->id();

  QList<ConnectionPoint*> clonedPoints = clonedElement->connectionPoints();

  for (int i = 0; i < clonedPoints.size(); ++i) {
    ConnectionPoint* clonedPoint = clonedPoints[i];
    if (clonedPoint && clonedPoint->direction() == originalPoint->direction()) {
      return clonedPoint;
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::mousePressEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (event->button() == Qt::LeftButton) {
    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

    if (!item) {
      m_selectionRectActive = true;
      m_selectionStartPoint = event->scenePos();
      createSelectionRect();

      if (!(event->modifiers() & Qt::ControlModifier)) {
        clearSelection();
      }
    }
  }

  QGraphicsScene::mousePressEvent(event);
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::mouseMoveEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (m_selectionRectActive) {
    updateSelectionRect(event->scenePos());
  }
  else if (m_isCreatingConnection) {
    updateTemporaryConnection(event->scenePos());
  }

  QGraphicsScene::mouseMoveEvent(event);
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::mouseReleaseEvent(
  QGraphicsSceneMouseEvent* event
)
{
  if (event->button() == Qt::LeftButton && m_selectionRectActive) {
    finishSelectionRect();
  }

  QGraphicsScene::mouseReleaseEvent(event);
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::keyPressEvent(
  QKeyEvent* event
)
{
  switch (event->key()) {
  case Qt::Key_Delete:
    deleteSelected();
    break;

  case Qt::Key_A:
    if (event->modifiers() & Qt::ControlModifier) {
      selectAll();
    }
    break;

  case Qt::Key_C:
    if (event->modifiers() & Qt::ControlModifier) {
      copySelected();
    }
    break;

  case Qt::Key_V:
    if (event->modifiers() & Qt::ControlModifier) {
      pasteFromClipboard();
    }
    break;

  case Qt::Key_D:
    if (event->modifiers() & Qt::ControlModifier) {
      duplicateSelected();
    }
    break;

  case Qt::Key_Escape:
    if (m_isCreatingConnection) {
      cancelConnection();
    }
    else {
      clearSelection();
      m_clipboardConnections.clear();
      m_clipboardElements.clear();
    }
    break;

  default:
    QGraphicsScene::keyPressEvent(event);
    break;
  }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::createSelectionRect()
{
  if (!m_selectionRect) {
    m_selectionRect = new QGraphicsRectItem();
    m_selectionRect->setPen(QPen(QColor(0, 120, 215), 1, Qt::DashLine));
    m_selectionRect->setBrush(QBrush(QColor(0, 120, 215, 30)));
    addItem(m_selectionRect);
  }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::destroySelectionRect()
{
  if (m_selectionRect) {
    removeItem(m_selectionRect);
    delete m_selectionRect;
    m_selectionRect = nullptr;
  }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::updateSelectionRect(
  const QPointF& currentPoint
)
{
  if (m_selectionRect) {
    QRectF rect(m_selectionStartPoint, currentPoint);
    m_selectionRect->setRect(rect.normalized());
  }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::finishSelectionRect()
{
  if (m_selectionRect) {
    QRectF selectionArea = m_selectionRect->rect();

    for (auto it = m_elementToItem.begin(); it != m_elementToItem.end(); ++it) {
      ElementGraphicsItem* item = it.value();
      BasicElement* element = it.key();

      if (selectionArea.intersects(item->boundingRect().translated(item->pos()))) {
        selectElement(element, true);
      }
    }

    for (auto it = m_connectionToItem.begin(); it != m_connectionToItem.end(); ++it) {
      ConnectionGraphicsItem* item = it.value();
      ConnectionLine* connection = it.key();

      if (item && connection && connection->isValid()) {
        if (selectionArea.intersects(item->boundingRect())) {
          selectConnection(connection, true);
        }
      }
    }

    destroySelectionRect();
  }

  m_selectionRectActive = false;
}
// -----------------------------------------------------------------------------------------------------

void DiagramScene::addConnection(
  ConnectionLine* connection
)
{
  if (!connection || m_connections.contains(connection->id())) {
    return;
  }

  m_connections[connection->id()] = connection;

  auto connectionItem = new ConnectionGraphicsItem(connection);
  m_connectionToItem[connection] = connectionItem;
  addItem(connectionItem);

  handleAttributeConnection(connection);
}

//----------------------------------------------------------------------------------------------

void DiagramScene::handleAttributeConnection(
  ConnectionLine* connection
)
{
  if (!connection || !connection->isValid()) {
    return;
  }

  auto startElement = connection->getStartElement();
  auto endElement = connection->getEndElement();

  if (!startElement || !endElement) {
    return;
  }

  auto startEntity = qobject_cast<Entity*>(startElement);
  auto endAttribute = qobject_cast<Attribute*>(endElement);

  if (startEntity && endAttribute) {
    if (endAttribute->parent() != startEntity) {
      startEntity->addAttributeId(endAttribute->id());
      endAttribute->setParent(startEntity);
    }
    return;
  }

  auto endEntity = qobject_cast<Entity*>(endElement);
  auto startAttribute = qobject_cast<Attribute*>(startElement);

  if (endEntity && startAttribute) {
    if (startAttribute->parent() != endEntity) {
      endEntity->addAttributeId(startAttribute->id());
      startAttribute->setParent(endEntity);
    }
    return;
  }

  auto parentAttribute = qobject_cast<Attribute*>(startElement);
  auto subAttribute = qobject_cast<Attribute*>(endElement);

  if (parentAttribute && subAttribute && parentAttribute->isCompositeAttribute()) {
    if (subAttribute->parent() != parentAttribute) {
      parentAttribute->addAttributeId(subAttribute->id());
      subAttribute->setParent(parentAttribute);
    }
    return;
  }

  parentAttribute = qobject_cast<Attribute*>(endElement);
  subAttribute = qobject_cast<Attribute*>(startElement);

  if (parentAttribute && subAttribute && parentAttribute->isCompositeAttribute()) {
    if (subAttribute->parent() != parentAttribute) {
      parentAttribute->addAttributeId(subAttribute->id());
      subAttribute->setParent(parentAttribute);
    }
  }
}

//----------------------------------------------------------------------------------------------

void DiagramScene::addRelationshipConnection(
  RelationshipConnectionLine* connection
)
{
  if (!connection || m_connections.contains(connection->id())) {
    return;
  }

  m_connections[connection->id()] = connection;

  auto connectionItem = new RelationshipConnectionLineGraphicsItem(connection);
  m_connectionToItem[connection] = connectionItem;
  addItem(connectionItem);
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::removeConnection(
  const QString& connectionId
)
{
  if (auto connection = findConnection(connectionId)) {
    removeConnection(connection);
  }
}

// -----------------------------------------------------------------------------------------------------

ConnectionLine* DiagramScene::findConnection(
  const QString& id
) const
{
  return m_connections.value(id, nullptr);
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::startConnection(
  ConnectionPoint* startPoint
)
{
  if (!startPoint || m_isCreatingConnection) return;

  m_isCreatingConnection = true;
  m_connectionStartPoint = startPoint;

  m_temporaryConnectionLine = new QGraphicsLineItem();
  m_temporaryConnectionLine->setPen(QPen(QColor(0, 120, 215), 2, Qt::DashLine));
  addItem(m_temporaryConnectionLine);

  emit connectionStarted(startPoint);
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::finishConnection(
  ConnectionPoint* endPoint
)
{
  if (!m_isCreatingConnection || !m_connectionStartPoint || !endPoint || m_connectionStartPoint == endPoint) {
    cancelConnection();
    return;
  }

  auto startElement = qobject_cast<BasicElement*>(m_connectionStartPoint->parent());
  auto endElement = qobject_cast<BasicElement*>(endPoint->parent());

  if (!startElement || !endElement || startElement == endElement) {
    cancelConnection();
    return;
  }

  Relationship* relationship = nullptr;
  Entity* entity = nullptr;

  if (isRelationshipToEntityConnection(startElement, endElement, &relationship, &entity)) {
    auto relationshipEnd = new RelationshipEnd(
      entity->id(),
      Cardinality::One,
      false,
      relationship
    );

    relationship->addEnd(relationshipEnd);

    auto relationshipConnection = new RelationshipConnectionLine(
      m_connectionStartPoint,
      endPoint,
      relationshipEnd,
      this
    );

    addRelationshipConnection(relationshipConnection);
  }
  else {
    auto connection = new ConnectionLine(
      m_connectionStartPoint,
      endPoint,
      this
    );

    addConnection(connection);
  }

  m_isCreatingConnection = false;
  m_connectionStartPoint = nullptr;

  if (m_temporaryConnectionLine) {
    removeItem(m_temporaryConnectionLine);
    delete m_temporaryConnectionLine;
    m_temporaryConnectionLine = nullptr;
  }
}

//----------------------------------------------------------------------------------------------

bool DiagramScene::isRelationshipToEntityConnection(
  BasicElement* startElement,
  BasicElement* endElement,
  Relationship** outRelationship,
  Entity** outEntity
) const
{
  if (!startElement || !endElement) {
    return false;
  }

  auto relationship1 = qobject_cast<Relationship*>(startElement);
  auto entity1 = qobject_cast<Entity*>(endElement);

  if (relationship1 && entity1) {
    if (outRelationship) *outRelationship = relationship1;
    if (outEntity) *outEntity = entity1;
    return true;
  }

  auto relationship2 = qobject_cast<Relationship*>(endElement);
  auto entity2 = qobject_cast<Entity*>(startElement);

  if (relationship2 && entity2) {
    if (outRelationship) *outRelationship = relationship2;
    if (outEntity) *outEntity = entity2;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------

void DiagramScene::handleRelationshipDisconnection(
  ConnectionLine* connection
)
{
  auto relationshipConnection = qobject_cast<RelationshipConnectionLine*>(connection);
  if (!relationshipConnection) {
    return;
  }

  auto relationshipEnd = relationshipConnection->relationshipEnd();
  if (!relationshipEnd) {
    return;
  }

  auto startElement = connection->getStartElement();
  auto endElement = connection->getEndElement();

  auto relationship = qobject_cast<Relationship*>(startElement);
  if (!relationship) {
    relationship = qobject_cast<Relationship*>(endElement);
  }

  if (relationship) {
    relationship->removeEnd(relationshipEnd);
    relationshipEnd->deleteLater();
  }
}

// -----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void DiagramScene::handleAttributeDisconnection(
  ConnectionLine* connection
)
{
  if (!connection || !connection->isValid()) {
    return;
  }

  auto startElement = connection->getStartElement();
  auto endElement = connection->getEndElement();

  if (!startElement || !endElement) {
    return;
  }

  auto attribute = qobject_cast<Attribute*>(startElement);

  bool attributeChildIsEndElement = false;
  if (!attribute || attribute->parent() != endElement) {
    attributeChildIsEndElement = true;
    attribute = qobject_cast<Attribute*>(endElement);
  }

  if (attribute) {
    attribute->setParent(nullptr);

    if (attributeChildIsEndElement) {
      startElement->removeAttributeId(attribute->id());
    }
    else {
      endElement->removeAttributeId(attribute->id());
    }
  }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::cancelConnection()
{
  if (!m_isCreatingConnection) return;

  if (m_temporaryConnectionLine) {
    removeItem(m_temporaryConnectionLine);
    delete m_temporaryConnectionLine;
    m_temporaryConnectionLine = nullptr;
  }

  m_isCreatingConnection = false;
  m_connectionStartPoint = nullptr;

  emit connectionCancelled();
}

// -----------------------------------------------------------------------------------------------------

QList<ConnectionLine*> DiagramScene::getAllConnections() const
{
  return m_connections.values();
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::updateTemporaryConnection(
  const QPointF& endPosition
)
{
  if (!m_temporaryConnectionLine || !m_connectionStartPoint) return;

  auto startElement = qobject_cast<BasicElement*>(m_connectionStartPoint->parent());
  if (!startElement) return;

  QPointF startPos = m_connectionStartPoint->absolutePosition(
    startElement->position(),
    startElement->size()
  );

  m_temporaryConnectionLine->setLine(QLineF(startPos, endPosition));
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::updateConnectionControlPoint(
  ConnectionLine* connection,
  ControlPointType type,
  const QPointF& offset
)
{
  if (!connection) {
    return;
  }

  if (type == ControlPointType::Control1) {
    connection->setControl1Offset(offset);
  }
  else {
    connection->setControl2Offset(offset);
  }
}

// -----------------------------------------------------------------------------------------------------

bool DiagramScene::exportToImage(
  const QString& filePath,
  const QString& format,
  qreal scaleFactor
)
{
  if (filePath.isEmpty()) {
    return false;
  }

  QImage image = renderToImage(scaleFactor);

  if (image.isNull()) {
    return false;
  }

  return image.save(filePath, format.toUtf8().constData());
}

// -----------------------------------------------------------------------------------------------------

QImage DiagramScene::renderToImage(
  qreal scaleFactor
)
{
  QRectF contentRect = itemsBoundingRect();

  if (contentRect.isEmpty()) {
    return QImage();
  }

  contentRect.adjust(-20, -20, 20, 20);

  QSize imageSize = (contentRect.size() * scaleFactor).toSize();

  QImage image(imageSize, QImage::Format_ARGB32);
  image.fill(Qt::white);

  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);

  QTransform transform;
  transform.scale(scaleFactor, scaleFactor);
  transform.translate(-contentRect.left(), -contentRect.top());

  render(&painter, QRectF(), contentRect);

  painter.end();

  return image;
}

// -----------------------------------------------------------------------------------------------------