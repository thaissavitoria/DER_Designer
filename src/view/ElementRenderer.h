#ifndef ELEMENTRENDERER_H
#define ELEMENTRENDERER_H

#include <QtGui/QPainter>
#include <QtCore/QRectF>
#include <QtCore/QPointF>

class BasicElement;
enum class ElementType;

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Abstract interface for element rendering in ERD diagrams
 *
 * This interface defines the contract for rendering different types of ERD elements
 * following the Strategy pattern. It provides:
 * - Element-specific rendering implementation
 * - Hit detection for user interaction
 * - Selection visualization with custom styling
 * - Bounding rectangle calculation for selection areas
 *
 * Implementations of this interface handle the visual representation of entities,
 * attributes, relationships, and other ERD elements according to Peter Chen's notation,
 * allowing for consistent rendering across different element types while maintaining
 * flexibility for type-specific visual characteristics.
 */

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

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Factory class for creating element renderers
 *
 * This factory class implements the Factory pattern to create appropriate renderer
 * instances based on element type. It provides:
 * - Type-based renderer instantiation
 * - Centralized renderer creation logic
 * - Encapsulation of renderer concrete classes
 * - Easy extensibility for new element types
 *
 * The factory ensures that each element type gets the correct renderer implementation
 * (EntityRenderer, AttributeRenderer, RelationshipRenderer) without requiring
 * client code to know about specific renderer classes.
 */

class ElementRendererFactory
{
public:
  static std::unique_ptr<IElementRenderer> createRenderer(
    ElementType type
  );
};

#endif // ELEMENTRENDERER_H