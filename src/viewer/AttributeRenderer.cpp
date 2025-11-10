#include "AttributeRenderer.h"

#include "model/BasicElement.h"
#include "model/Attribute.h"
#include "model/AttributeType.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtCore/QRectF>
#include <QtCore/qmath.h>

// -----------------------------------------------------------------------------------------------------

AttributeRenderer::AttributeRenderer()
    : m_textMargin(6.0)
{
    setupDefaultStyle();
}

// -----------------------------------------------------------------------------------------------------

void AttributeRenderer::setupDefaultStyle()
{
    m_borderPen = QPen(Qt::black, 1.5, Qt::SolidLine);
    m_derivedPen = QPen(Qt::black, 1.5, Qt::DashLine);
    
    m_fillBrush = QBrush(Qt::white);
    
    m_textPen = QPen(Qt::black);
    m_textFont = QFont("Arial", 10);
    m_primaryKeyFont = QFont("Arial", 10);
    m_primaryKeyFont.setUnderline(true);
   
    m_selectionBrush = QBrush(QColor(0, 0, 255, 20)); 
}

// -----------------------------------------------------------------------------------------------------

void AttributeRenderer::render(
    QPainter* painter, 
    const BasicElement* element, 
    const QRectF& rect
)
{
    if (!painter || !element) return;

    auto attribute = qobject_cast<const Attribute*>(element);
    if (!attribute) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    renderByAttributeType(painter, attribute, rect);
    
    painter->setPen(m_textPen);
    
    if (attribute->attributeType() == AttributeType::Key) {
        painter->setFont(m_primaryKeyFont);
    } else {
        painter->setFont(m_textFont);
    }
    
    QRectF textRect = getTextRect(rect);
    painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, element->name());

    painter->restore();
}

// -----------------------------------------------------------------------------------------------------

void AttributeRenderer::renderByAttributeType(
    QPainter* painter,
    const Attribute* attribute,
    const QRectF& rect
)
{
    switch (attribute->attributeType()) {
        case AttributeType::Normal:
            renderNormalAttribute(painter, rect);
            break;
        case AttributeType::Key:
            renderKeyAttribute(painter, rect);
            break;
        case AttributeType::Derived:
            renderDerivedAttribute(painter, rect);
            break;
        case AttributeType::Multivalued:
            renderMultivaluedAttribute(painter, rect);
            break;
        case AttributeType::Composite:
            renderCompositeAttribute(painter, rect);
            break;
    }
}

// -----------------------------------------------------------------------------------------------------

void AttributeRenderer::renderNormalAttribute(
    QPainter* painter,
    const QRectF& rect
)
{
    painter->setPen(m_borderPen);
    painter->setBrush(m_fillBrush);
    painter->drawEllipse(rect);
}

// -----------------------------------------------------------------------------------------------------

void AttributeRenderer::renderKeyAttribute(
    QPainter* painter,
    const QRectF& rect
)
{
    painter->setPen(m_borderPen);
    painter->setBrush(m_fillBrush);
    painter->drawEllipse(rect);
}

// -----------------------------------------------------------------------------------------------------

void AttributeRenderer::renderDerivedAttribute(
    QPainter* painter,
    const QRectF& rect
)
{
    painter->setPen(m_derivedPen);
    painter->setBrush(m_fillBrush);
    painter->drawEllipse(rect);
}

// -----------------------------------------------------------------------------------------------------

void AttributeRenderer::renderMultivaluedAttribute(
    QPainter* painter,
    const QRectF& rect
)
{
    painter->setPen(m_borderPen);
    painter->setBrush(m_fillBrush);
    
    painter->drawEllipse(rect);
    
    qreal margin = 4.0;
    QRectF innerRect = rect.adjusted(margin, margin, -margin, -margin);
    painter->drawEllipse(innerRect);
}

// -----------------------------------------------------------------------------------------------------

void AttributeRenderer::renderCompositeAttribute(
    QPainter* painter,
    const QRectF& rect
)
{
    painter->setPen(m_borderPen);
    painter->setBrush(m_fillBrush);
    painter->drawEllipse(rect);
    
    QPointF center = rect.center();
    qreal halfWidth = rect.width() * 0.3;
    painter->drawLine(
        QPointF(center.x() - halfWidth, center.y()),
        QPointF(center.x() + halfWidth, center.y())
    );
}

// -----------------------------------------------------------------------------------------------------

bool AttributeRenderer::isInElementRect(
    const BasicElement* element, 
    const QPointF& point
)
{
    if (!element) return false;

    QRectF rect = element->boundingRect();
    return isPointInEllipse(point, rect);
}

// -----------------------------------------------------------------------------------------------------

QRectF AttributeRenderer::selectionRect(
    const BasicElement* element
)
{
    if (!element) return QRectF();

    QRectF rect = element->boundingRect();
    qreal margin = 2.0;

    return rect.adjusted(-margin, -margin, margin, margin);
}

// -----------------------------------------------------------------------------------------------------

void AttributeRenderer::renderSelection(
    QPainter* painter, 
    const BasicElement* element, 
    const QRectF& rect
)
{
    if (!painter || !element) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    m_selectionPen = QPen(Qt::blue, 1.5, getPenStyle(element));
    painter->setPen(m_selectionPen);
    painter->setBrush(m_selectionBrush);
    painter->drawEllipse(rect);

    painter->restore();
}

// -----------------------------------------------------------------------------------------------------

Qt::PenStyle AttributeRenderer::getPenStyle(
    const BasicElement* element
) const
{
    auto attribute = qobject_cast<const Attribute*>(element);
    return attribute->isDerivedAttribute() ? Qt::DashLine : Qt::SolidLine;
}

// -----------------------------------------------------------------------------------------------------

QRectF AttributeRenderer::getTextRect(
    const QRectF& elementRect
) const
{
    return elementRect.adjusted(m_textMargin, m_textMargin, -m_textMargin, -m_textMargin);
}

// -----------------------------------------------------------------------------------------------------

bool AttributeRenderer::isPointInEllipse(
    const QPointF& point,
    const QRectF& ellipseRect
) const
{
    if (ellipseRect.isEmpty()) return false;

    QPointF center = ellipseRect.center();

    qreal rx = ellipseRect.width() / 2.0;
    qreal ry = ellipseRect.height() / 2.0;
    
    qreal dx = point.x() - center.x();
    qreal dy = point.y() - center.y();
    
    qreal result = (dx * dx) / (rx * rx) + (dy * dy) / (ry * ry);
    
    return result <= 1.0;
}

// -----------------------------------------------------------------------------------------------------