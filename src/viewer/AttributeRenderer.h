#ifndef ATTRIBUTERENDERER_H
#define ATTRIBUTERENDERER_H

#include "ElementRenderer.h"
#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtGui/QPen>

// -----------------------------------------------------------------------------------------------------

class AttributeRenderer : public IElementRenderer
{
public:
    AttributeRenderer();

    void render(
        QPainter* painter,
        const BasicElement* element,
        const QRectF& rect
    ) override;

    bool isInElementRect(
        const BasicElement* element,
        const QPointF& point
    ) override;

    QRectF selectionRect(
        const BasicElement* element
    ) override;

    void renderSelection(
        QPainter* painter,
        const BasicElement* element,
        const QRectF& rect
    ) override;

private:
    QPen m_borderPen;
    QPen m_derivedPen;
    QBrush m_fillBrush;
    QBrush m_selectionBrush;
    QFont m_textFont;
    QFont m_primaryKeyFont;
    QPen m_textPen;
    QPen m_selectionPen;

    qreal m_textMargin;

    void setupDefaultStyle();

    void renderByAttributeType(
        QPainter* painter,
        const class Attribute* attribute,
        const QRectF& rect
    );

    void renderNormalAttribute(
        QPainter* painter,
        const QRectF& rect
    );

    void renderKeyAttribute(
        QPainter* painter,
        const QRectF& rect
    );

    void renderDerivedAttribute(
        QPainter* painter,
        const QRectF& rect
    );

    void renderMultivaluedAttribute(
        QPainter* painter,
        const QRectF& rect
    );

    QRectF getTextRect(
        const QRectF& elementRect
    ) const;

    bool isPointInEllipse(
        const QPointF& point,
        const QRectF& ellipseRect
    ) const;

    Qt::PenStyle getPenStyle(
        const BasicElement* element
    ) const;
};

#endif // ATTRIBUTERENDERER_H