#include "EntityRenderer.h"

#include "model/BasicElement.h"
#include "model/Entity.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtCore/QRectF>

// -----------------------------------------------------------------------------------------------------

EntityRenderer::EntityRenderer()
:
  m_textMargin(8.0)
{
  setupDefaultStyle();
}

// -----------------------------------------------------------------------------------------------------

void EntityRenderer::setupDefaultStyle()
{
    m_borderPen = QPen(Qt::black, 1.5, Qt::SolidLine);
    m_fillBrush = QBrush(Qt::white);
    m_textPen = QPen(Qt::black);
    m_textFont = QFont("Arial", 12);
    m_selectionPen = QPen(Qt::blue, 1.5, Qt::SolidLine);
    m_selectionBrush = QBrush(QColor(0, 0, 225, 20));
}

// -----------------------------------------------------------------------------------------------------

void EntityRenderer::render(
    QPainter* painter, 
    const BasicElement* element, 
    const QRectF& rect
)
{
    if (!painter || !element) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(m_borderPen);
    painter->setBrush(m_fillBrush);

    painter->drawRect(rect);

    painter->setPen(m_textPen);
    painter->setFont(m_textFont);

    QRectF textRect = getTextRect(rect);
    painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, element->name());

    painter->restore();
}

// -----------------------------------------------------------------------------------------------------

bool EntityRenderer::isInElementRect(
    const BasicElement* element, 
    const QPointF& point
)
{
    if (!element) return false;

    QRectF rect = element->boundingRect();
    return rect.contains(point);
}

// -----------------------------------------------------------------------------------------------------

QRectF EntityRenderer::selectionRect(
    const BasicElement* element
)
{
    if (!element) return QRectF();

    QRectF rect = element->boundingRect();
    qreal margin = 2.0;

    return rect.adjusted(-margin, -margin, margin, margin);
}

// -----------------------------------------------------------------------------------------------------

void EntityRenderer::renderSelection(
    QPainter* painter, 
    const BasicElement* element, 
    const QRectF& rect
)
{
    if (!painter || !element) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setPen(m_selectionPen);
    painter->setBrush(m_selectionBrush);
    painter->drawRect(rect);

    painter->restore();
}

// -----------------------------------------------------------------------------------------------------

QRectF EntityRenderer::getTextRect(const QRectF& elementRect) const
{
    return elementRect.adjusted(m_textMargin, m_textMargin, -m_textMargin, -m_textMargin);
}

// -----------------------------------------------------------------------------------------------------