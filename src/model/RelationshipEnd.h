#ifndef RELATIONSHIPEND_H
#define RELATIONSHIPEND_H

#include <QtCore/QObject>
#include <QtCore/QString>

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Enumeration for cardinality types in relationships
 *
 * Represents the cardinality constraints in ERD relationships:
 * - One: Represents a single instance (1)
 * - Many: Represents multiple instances (M, N, ...)
 */
enum class Cardinality {
  One,
  Many
};

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Represents one end of a relationship in an ERD diagram
 *
 * This class manages the connection between a relationship and an entity,
 * encapsulating the constraints and properties of that connection. It provides:
 * - Entity identification through ID reference
 * - Cardinality constraints (One or Many)
 * - Participation constraints (total or partial)
 * - Custom text for cardinality display ("N", "M", ...)
 * - String conversion utilities for serialization
 * - Change notification system through Qt signals
 *
 * Each relationship can have multiple ends, typically two (binary relationship)
 * or more (n-ary relationship), connecting different entities with their
 * respective constraints following Peter Chen's ERD notation.
 */
class RelationshipEnd : public QObject
{
  Q_OBJECT
    Q_PROPERTY(QString entityId READ entityId WRITE setEntityId)
    Q_PROPERTY(Cardinality cardinality READ cardinality WRITE setCardinality NOTIFY cardinalityChanged)
    Q_PROPERTY(bool isTotalParticipation READ isTotalParticipation WRITE setIsTotalParticipation NOTIFY totalParticipationChanged)
    Q_PROPERTY(QString customCardinalityText READ customCardinalityText WRITE setCustomCardinalityText NOTIFY customCardinalityTextChanged)

public:
  explicit RelationshipEnd(
    const QString& entityId,
    Cardinality cardinality,
    bool isTotalParticipation,
    QObject* parent = nullptr
  );

  virtual ~RelationshipEnd() = default;

  QString entityId() const { return m_entityId; }
  Cardinality cardinality() const { return m_cardinality; }
  bool isTotalParticipation() const { return m_isTotalParticipation; }
  QString customCardinalityText() const { return m_customCardinalityText; }

  void setEntityId(
    const QString& entityId
  );

  void setCardinality(
    Cardinality cardinality
  );

  void setIsTotalParticipation(
    bool isTotalParticipation
  );

  void setCustomCardinalityText(
    const QString& text
  );

  QString getDisplayCardinalityText() const;

  static QString cardinalityToString(
    Cardinality cardinality
  );

  static Cardinality cardinalityFromString(
    const QString& cardinalityString
  );

signals:
  void cardinalityChanged(
    Cardinality newCardinality
  );

  void totalParticipationChanged(
    bool isTotalParticipation
  );

  void customCardinalityTextChanged(
    const QString& newText
  );

private:
  QString m_entityId;
  Cardinality m_cardinality;
  bool m_isTotalParticipation;
  QString m_customCardinalityText;
};

#endif // RELATIONSHIPEND_H