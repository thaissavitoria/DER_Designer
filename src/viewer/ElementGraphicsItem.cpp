#include "ElementGraphicsItem.h"
#include "controller/DiagramScene.h"
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QGraphicsView>

//----------------------------------------------------------------------------------------------

ElementGraphicsItem::ElementGraphicsItem(
    BasicElement* element,
    QGraphicsItem* parent
)
    : QGraphicsItem(parent)
    , m_element(element)
    , m_isSelected(false)
    , m_isHovered(false)
    , m_isDragging(false)
    , m_connectionPointsVisible(false)
    , m_hoveredConnectionPoint(nullptr)
{
    if (m_element) {
        m_element->addObserver(this);
        setupRenderer();
        updateFromElement();

        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
        setAcceptHoverEvents(true);
    }
}

//----------------------------------------------------------------------------------------------

ElementGraphicsItem::~ElementGraphicsItem()
{
    if (m_element) {
        m_element->removeObserver(this);
    }
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::setupRenderer()
{
    if (m_element) {
        m_renderer = ElementRendererFactory::createRenderer(m_element->type());
    }
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::updateFromElement()
{
    if (!m_element) return;

    prepareGeometryChange();
    setPos(m_element->position());
    update();
}

//----------------------------------------------------------------------------------------------

QRectF ElementGraphicsItem::boundingRect() const
{
    if (!m_element) return QRectF();

    QRectF rect(0, 0, m_element->size().width(), m_element->size().height());

    if (m_isSelected) {
        rect = rect.adjusted(-3, -3, 3, 3);
    }

    if (m_connectionPointsVisible) {
        rect = rect.adjusted(-6, -6, 6, 6);
    }

    return rect;
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget
)
{
    Q_UNUSED(option)
        Q_UNUSED(widget)

    if (!m_element || !m_renderer) return;

    QRectF rect(0, 0, m_element->size().width(), m_element->size().height());

    m_renderer->render(painter, m_element, rect);

    if (m_isSelected) {
        m_renderer->renderSelection(painter, m_element, rect);
    }

    if (m_connectionPointsVisible) {
        renderConnectionPoints(painter);
    }
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::renderConnectionPoints(
    QPainter* painter
)
{
    if (!m_element) return;

    painter->save();

    QList<ConnectionPoint*> connectionPoints = m_element->connectionPoints();
    
    for (ConnectionPoint* point : connectionPoints) {
        if (!point) continue;

        QPointF absolutePos = point->absolutePosition(QPointF(0, 0), m_element->size());
        QRectF pointRect = getConnectionPointRect(point);

        if (point == m_hoveredConnectionPoint) {
            painter->setPen(QPen(QColor(0, 100, 0), 2));
            painter->setBrush(QBrush(QColor(34, 139, 34, 200)));
        } else if (point->isConnected()) {
            //  conectado
            painter->setPen(QPen(QColor(0, 0, 0), 2));
            painter->setBrush(QBrush(QColor(0, 0, 0, 150)));
        } else {
            // normal
            painter->setPen(QPen(QColor(34, 139, 34), 1));
            painter->setBrush(QBrush(QColor(34, 139, 34,100)));
        }

        painter->drawEllipse(pointRect);
    }

    painter->restore();
}

//----------------------------------------------------------------------------------------------

ConnectionPoint* ElementGraphicsItem::findConnectionPointAt(
    const QPointF& localPosition
)
{
    if (!m_element) return nullptr;

    QList<ConnectionPoint*> connectionPoints = m_element->connectionPoints();
    
    for (ConnectionPoint* point : connectionPoints) {
        if (!point) continue;

        QRectF pointRect = getConnectionPointRect(point);
        if (pointRect.contains(localPosition)) {
            return point;
        }
    }

    return nullptr;
}

//----------------------------------------------------------------------------------------------

QRectF ElementGraphicsItem::getConnectionPointRect(
    ConnectionPoint* connectionPoint
) const
{
    if (!connectionPoint || !m_element) return QRectF();

    QPointF absolutePos = connectionPoint->absolutePosition(QPointF(0, 0), m_element->size());
    qreal size = (connectionPoint == m_hoveredConnectionPoint) ? 10.0 : 8.0;
    
    return QRectF(absolutePos.x() - size/2, absolutePos.y() - size/2, size, size);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::updateHoveredConnectionPoint(
    const QPointF& localPosition
)
{
    ConnectionPoint* newHovered = findConnectionPointAt(localPosition);
    
    if (m_hoveredConnectionPoint != newHovered) {
        m_hoveredConnectionPoint = newHovered;
        update(); 
        
        if (m_hoveredConnectionPoint) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::setConnectionPointsVisible(
    bool visible
)
{
    if (m_connectionPointsVisible != visible) {
        prepareGeometryChange();
        m_connectionPointsVisible = visible;
        
        if (!visible) {
            m_hoveredConnectionPoint = nullptr;
            setCursor(Qt::ArrowCursor);
        }
        
        update();
    }
}

//----------------------------------------------------------------------------------------------

QPainterPath ElementGraphicsItem::shape() const
{
    QPainterPath path;
    if (m_element) {
        QRectF rect(0, 0, m_element->size().width(), m_element->size().height());
        path.addRoundedRect(rect, 5, 5);
        
        if (m_connectionPointsVisible) {
            QList<ConnectionPoint*> connectionPoints = m_element->connectionPoints();
            for (ConnectionPoint* point : connectionPoints) {
                if (point) {
                    QRectF pointRect = getConnectionPointRect(point);
                    path.addEllipse(pointRect);
                }
            }
        }
    }
    return path;
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::hoverEnterEvent(
    QGraphicsSceneHoverEvent* event
)
{
    m_isHovered = true;
    setConnectionPointsVisible(true);
    updateHoveredConnectionPoint(event->pos());
    update();
    
    QGraphicsItem::hoverEnterEvent(event);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::hoverLeaveEvent(
    QGraphicsSceneHoverEvent* event
)
{
    m_isHovered = false;
    setConnectionPointsVisible(false);
    update();
    
    QGraphicsItem::hoverLeaveEvent(event);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::hoverMoveEvent(
    QGraphicsSceneHoverEvent* event
)
{
    if (m_connectionPointsVisible) {
        updateHoveredConnectionPoint(event->pos());
    }
    
    QGraphicsItem::hoverMoveEvent(event);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::mousePressEvent(
    QGraphicsSceneMouseEvent* event
)
{
    if (event->button() == Qt::LeftButton) {
        ConnectionPoint* clickedPoint = findConnectionPointAt(event->pos());
        if (clickedPoint) {
            auto diagramScene = qobject_cast<DiagramScene*>(scene());
            if (diagramScene) {
                if (diagramScene->isCreatingConnection()) {
                    diagramScene->finishConnection(clickedPoint);
                } else {
                    diagramScene->startConnection(clickedPoint);
                }
            }
            event->accept();
            return;
        }
        
        m_isDragging = true;
        m_dragStartPosition = event->pos();
    }

    QGraphicsItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::mouseMoveEvent(
    QGraphicsSceneMouseEvent* event
)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        QPointF newPos = event->scenePos() - m_dragStartPosition;
        setPos(newPos);
        
        if (m_element) {
            m_element->setPosition(newPos);
        }
    }

    QGraphicsItem::mouseMoveEvent(event);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::mouseReleaseEvent(
    QGraphicsSceneMouseEvent* event
)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::mouseDoubleClickEvent(
    QGraphicsSceneMouseEvent* event
)
{
    Q_UNUSED(event)
    editElementName();
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::editElementName()
{
    if (!m_element) return;

    bool ok;
    QString newName = QInputDialog::getText(
        nullptr,
        "Editar Nome",
        "Nome do elemento:",
        QLineEdit::Normal,
        m_element->name(),
        &ok
    );

    if (ok && !newName.isEmpty()) {
        m_element->setName(newName);
    }
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::setSelected(
    bool selected
)
{
    if (m_isSelected != selected) {
        m_isSelected = selected;
        prepareGeometryChange();
        update();
    }
}

//----------------------------------------------------------------------------------------------

QVariant ElementGraphicsItem::itemChange(
    GraphicsItemChange change,
    const QVariant& value
)
{
    if (change == ItemPositionChange && m_element) {
        QPointF newPos = value.toPointF();
        m_element->setPosition(newPos);
    }

    return QGraphicsItem::itemChange(change, value);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::onElementChanged(
    const BasicElement* element
)
{
    Q_UNUSED(element)
    updateFromElement();
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::onElementPositionChanged(
    const BasicElement* element,
    const QPointF& oldPosition,
    const QPointF& newPosition
)
{
    Q_UNUSED(element)
    Q_UNUSED(oldPosition)
    Q_UNUSED(newPosition)
    updateFromElement();
}