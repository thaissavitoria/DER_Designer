#ifndef ELEMENTFACTORY_H
#define ELEMENTFACTORY_H

#include <QtCore/QString>

#include "model/BasicElement.h"

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Factory for creating BasicElement instances
 *
 * Centralizes element creation logic, decoupling the View from
 * knowing which concrete subclass to instantiate.
 */
class ElementFactory
{
public:
  static BasicElement* createElement(
    ElementType type,
    QObject* parent = nullptr
  );

  static BasicElement* createElement(
    const QString& typeName,
    QObject* parent = nullptr
  );
};

#endif // ELEMENTFACTORY_H
