#include "DiagramScene.h"
#include "model/Entity.h"
#include "viewer/ConnectionGraphicsItem.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <QtGui/QKeyEvent>

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
    for (auto it = m_connectionToItem.begin(); it != m_connectionToItem.end(); ++it) {
        if (ConnectionGraphicsItem* item = it.value()) {
            removeItem(item);
            delete item;
        }
    }

    m_connectionToItem.clear();

    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        delete it.value();
    }
    m_connections.clear();

    for (auto it = m_elementToItem.begin(); it != m_elementToItem.end(); ++it) {
      if (ElementGraphicsItem* item = it.value()) {
        removeItem(item);
        delete item;
      }
    }

    m_elementToItem.clear();

    for (auto it = m_elements.begin(); it != m_elements.end(); ++it) {
      delete it.value();
    }
    m_elements.clear();

    destroySelectionRect();
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

void DiagramScene::removeElement(
    BasicElement* element
)
{
    if (!element || !m_elements.contains(element->id())) {
        return;
    }

    if (ElementGraphicsItem* item = m_elementToItem.value(element, nullptr)) {
        removeItem(item);
        m_elementToItem.remove(element);
        delete item;
    }

    m_elements.remove(element->id());

    emit elementRemoved(element);

    delete element;
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
    bool selected
)
{
    ElementGraphicsItem* item = findGraphicsItem(element);
    if (item) {
        item->setSelected(selected);

        if (selected) {
            emit elementSelected(element);
        }
        else {
            emit elementDeselected(element);
        }

        emit selectionChanged();
    }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::selectElement(
    const QString& elementId,
    bool selected
)
{
    if (BasicElement* element = findElement(elementId)) {
        selectElement(element, selected);
    }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::clearSelection()
{
    QList<BasicElement*> selected = getSelectedElements();

    for (BasicElement* element : selected) {
        selectElement(element, false);
    }

    if (!selected.isEmpty()) {
        emit selectionChanged();
    }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::selectAll()
{
    for (auto it = m_elementToItem.begin(); it != m_elementToItem.end(); ++it) {
        it.value()->setSelected(true);
        emit elementSelected(it.key());
    }

    if (!m_elementToItem.isEmpty()) {
        emit selectionChanged();
    }
}

// -----------------------------------------------------------------------------------------------------

QList<ConnectionLine*> DiagramScene::getSelectedConnections() const
{
    QList<ConnectionLine*> selected;

    for (auto it = m_connectionToItem.begin(); it != m_connectionToItem.end(); ++it) {
        if (it.value()->isSelected()) {
            selected.append(it.key());
        }
    }

    return selected;
}

//----------------------------------------------------------------------------------------------

void DiagramScene::deleteSelected()
{
    QList<BasicElement*> selectedElements = getSelectedElements();
    QList<ConnectionLine*> selectedConnections = getSelectedConnections();

    for (ConnectionLine* connection : selectedConnections) {
        removeConnection(connection);
    }

    for (BasicElement* element : selectedElements) {
        removeElement(element);
    }

    if (!selectedElements.isEmpty() || !selectedConnections.isEmpty()) {
        emit selectionChanged();
    }
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::duplicateSelected()
{
    QList<BasicElement*> selected = getSelectedElements();

    for (BasicElement* element : selected) {
        auto cloned = element->clone();
        if (cloned) {
            QPointF offset(20, 20);
            cloned->setPosition(element->position() + offset);

            addElement(cloned.release());
        }
    }
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

    emit connectionAdded(connection);
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::removeConnection(
    ConnectionLine* connection
)
{
  if (!connection) return;

  if (ConnectionGraphicsItem* item = m_connectionToItem.value(connection, nullptr)) {
    removeItem(item);
    m_connectionToItem.remove(connection);
    delete item;
  }

  m_connections.remove(connection->id());
  delete connection;

  update();
}

// -----------------------------------------------------------------------------------------------------

void DiagramScene::removeConnectionFromContainers(
  ConnectionLine* connection
)
{
  if (!connection) return;

  if (ConnectionGraphicsItem* item = m_connectionToItem.value(connection, nullptr)) {
    removeItem(item);
    m_connectionToItem.remove(connection);
  }

  m_connections.remove(connection->id());
  update();
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
    if (!endPoint || !m_isCreatingConnection || !m_connectionStartPoint) return;

    if (endPoint == m_connectionStartPoint) {
        cancelConnection();
        return;
    }

    auto startElement = qobject_cast<BasicElement*>(m_connectionStartPoint->parent());
    auto endElement = qobject_cast<BasicElement*>(endPoint->parent());

    if (startElement && endElement && startElement == endElement) {
        cancelConnection();
        return;
    }

    auto connection = new ConnectionLine(m_connectionStartPoint, endPoint, this);
    addConnection(connection);

    if (m_temporaryConnectionLine) {
        removeItem(m_temporaryConnectionLine);
        delete m_temporaryConnectionLine;
        m_temporaryConnectionLine = nullptr;
    }

    m_isCreatingConnection = false;
    m_connectionStartPoint = nullptr;

    emit connectionFinished(connection);
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
