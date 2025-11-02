#include "ElementGraphicsItem.h"
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
}

//----------------------------------------------------------------------------------------------

QPainterPath ElementGraphicsItem::shape() const
{
    QPainterPath path;
    if (m_element) {
        QRectF rect(0, 0, m_element->size().width(), m_element->size().height());
        path.addRoundedRect(rect, 5, 5);
    }
    return path;
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::mousePressEvent(
    QGraphicsSceneMouseEvent* event
)
{
    if (event->button() == Qt::LeftButton) {
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
    if (m_isDragging && m_element) {
        QPointF newPos = pos();
        m_element->setPosition(newPos);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::mouseReleaseEvent(
    QGraphicsSceneMouseEvent* event
)
{
    m_isDragging = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::mouseDoubleClickEvent(
    QGraphicsSceneMouseEvent* event
)
{
    if (event->button() == Qt::LeftButton) {
        editElementName();
        event->accept();
        return;
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::hoverEnterEvent(
    QGraphicsSceneHoverEvent* event
)
{
    Q_UNUSED(event)
        m_isHovered = true;
    setCursor(Qt::OpenHandCursor);
    update();
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::hoverLeaveEvent(
    QGraphicsSceneHoverEvent* event
)
{
    Q_UNUSED(event)
        m_isHovered = false;
    setCursor(Qt::ArrowCursor);
    update();
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::setSelected(
    bool selected
)
{
    if (m_isSelected != selected) {
        m_isSelected = selected;
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
        update();
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

        if (pos() != newPosition) {
            setPos(newPosition);
        }
}

//----------------------------------------------------------------------------------------------

void ElementGraphicsItem::editElementName()
{
    if (!m_element) return;

    QGraphicsView* view = nullptr;
    if (scene() && !scene()->views().isEmpty()) {
        view = scene()->views().first();
    }

    bool ok;
    QString currentName = m_element->name();
    QString newName = QInputDialog::getText(
        view,
        "Editar Nome do Elemento",
        "Nome:",
        QLineEdit::Normal,
        currentName,
        &ok
    );

    if (ok && newName != currentName && !newName.trimmed().isEmpty()) {
        m_element->setName(newName.trimmed());
    }
}

//----------------------------------------------------------------------------------------------