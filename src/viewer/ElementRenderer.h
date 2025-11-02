#ifndef ELEMENTRENDERER_H
#define ELEMENTRENDERER_H

#include <QtGui/QPainter>
#include <QtCore/QRectF>
#include <QtCore/QPointF>

class BasicElement;
enum class ElementType;

class IElementRenderer
{
public:
    virtual ~IElementRenderer() = default;

    virtual void render(
        QPainter* painter,
        const BasicElement* element,
        const QRectF& rect
    ) = 0;

    virtual bool isInElementRect(
        const BasicElement* element,
        const QPointF& point
    ) = 0;

    virtual QRectF selectionRect(
        const BasicElement* element
    ) = 0;

    virtual void renderSelection(
        QPainter* painter,
        const BasicElement* element,
        const QRectF& rect
    ) = 0;
};


class ElementRendererFactory
{
public:
    static std::unique_ptr<IElementRenderer> createRenderer(
        ElementType type
    );
};

#endif // ELEMENTRENDERER_H