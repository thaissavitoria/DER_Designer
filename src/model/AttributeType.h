#ifndef ATTRIBUTETYPE_H
#define ATTRIBUTETYPE_H

#include <QtCore/QString>
#include <QtCore/QStringList>

//----------------------------------------------------------------------------------------------

/**
 * @brief Utility class for attribute type management in ERD diagrams
 *
 * This class provides type definitions and conversion utilities for different
 * attribute types in Entity-Relationship diagrams following Peter Chen's notation.
 */

class AttributeType {
public:
  enum Type {
    Normal = 0,
    Derived,
    Multivalued,
    Composite
  };

  static Type attributeTypeFromString(
    const QString& typeString
  );

  static QString attributeTypeToString(
    Type type
  );

  static QStringList getAllAttributeTypeStrings();
};

#endif // ATTRIBUTETYPE_H