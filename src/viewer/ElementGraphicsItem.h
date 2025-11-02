#ifndef ELEMENTGRAPHICSITEM_H
#define ELEMENTGRAPHICSITEM_H

#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsSceneMouseEvent>

#include "ElementRenderer.h"
#include "model/BasicElement.h"

#include <memory>

/**
 * @brief Graphic item that represents a BasicElement in the QGraphicsView
 *
 *  Updates the visual representation of BasicElements
 */

class ElementGraphicsItem : public QGraphicsItem, public IElementObserver
{
public:
    explicit ElementGraphicsItem(
        BasicElement* element,
        QGraphicsItem* parent = nullptr
    );

    virtual ~ElementGraphicsItem();

    QRectF boundingRect() const override;

    void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr
    ) override;

    QPainterPath shape() const override;

    void mousePressEvent(
        QGraphicsSceneMouseEvent* event
    ) override;

    void mouseMoveEvent(
        QGraphicsSceneMouseEvent* event
    ) override;

    void mouseReleaseEvent(
        QGraphicsSceneMouseEvent* event
    ) override;

    void mouseDoubleClickEvent(
        QGraphicsSceneMouseEvent* event
    ) override;

    void hoverEnterEvent(
        QGraphicsSceneHoverEvent* event
    ) override;

    void hoverLeaveEvent(
        QGraphicsSceneHoverEvent* event
    ) override;

    void onElementChanged(
        const BasicElement* element
    ) override;

    void onElementPositionChanged(
        const BasicElement* element,
        const QPointF& oldPosition,
        const QPointF& newPosition
    ) override;

    BasicElement* element() const { return m_element; }
    bool isSelected() const { return m_isSelected; }
    bool isHovered() const { return m_isHovered; }

    void setSelected(
        bool selected
    );

protected:
    QVariant itemChange(
        GraphicsItemChange change,
        const QVariant& value
    ) override;

private:
    BasicElement* m_element;
    std::unique_ptr<IElementRenderer> m_renderer;

    bool m_isSelected;
    bool m_isHovered;
    bool m_isDragging;

    QPointF m_dragStartPosition;

    void updateFromElement();
    void setupRenderer();
    void editElementName();
};

#endif // ELEMENTGRAPHICSITEM_H