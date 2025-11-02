#include "DiagramScene.h"
#include "model/Entity.h"

#include <QtCore/QDebug>

#include <QtGui/QKeyEvent>

#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QGraphicsSceneMouseEvent>

// -----------------------------------------------------------------------------------------------------

DiagramScene::DiagramScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_selectionRectActive(false)
    , m_selectionRect(nullptr)
{
    setItemIndexMethod(QGraphicsScene::NoIndex); 
    setSceneRect(-5000, -5000, 10000, 10000);
}

// -----------------------------------------------------------------------------------------------------

DiagramScene::~DiagramScene()
{
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

    ElementGraphicsItem* item = m_elementToItem.value(element, nullptr);
    if (item) {
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

void DiagramScene::deleteSelected()
{
    QList<BasicElement*> selected = getSelectedElements();

    for (BasicElement* element : selected) {
        removeElement(element);
    }

    if (!selected.isEmpty()) {
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
        clearSelection();
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