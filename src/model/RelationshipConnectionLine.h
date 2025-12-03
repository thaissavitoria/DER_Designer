#ifndef RELATIONSHIPCONNECTIONLINE_H
#define RELATIONSHIPCONNECTIONLINE_H

#include "ConnectionLine.h"
#include "RelationshipEnd.h"

#include <QtCore/QPointF>

// -----------------------------------------------------------------------------------------------------

class RelationshipConnectionLine : public ConnectionLine
{
  Q_OBJECT
    Q_PROPERTY(RelationshipEnd* relationshipEnd READ relationshipEnd NOTIFY relationshipEndChanged)
    Q_PROPERTY(QPointF cardinalityOffset READ cardinalityOffset WRITE setCardinalityOffset NOTIFY cardinalityOffsetChanged)

public:
  explicit RelationshipConnectionLine(
    ConnectionPoint* startPoint,
    ConnectionPoint* endPoint,
    RelationshipEnd* relationshipEnd,
    QObject* parent = nullptr
  );

  virtual ~RelationshipConnectionLine();

  RelationshipEnd* relationshipEnd() const { return m_relationshipEnd; }
  QPointF cardinalityOffset() const { return m_cardinalityOffset; }

  void setRelationshipEnd(
    RelationshipEnd* relationshipEnd
  );

  void setCardinalityOffset(
    const QPointF& offset
  );

  QPointF getCardinalityPosition() const;

  QRectF getCardinalityDragBounds() const;

  bool isCardinalityOffsetValid(
    const QPointF& offset
  ) const;

  QString getCardinalityText() const;

  bool isTotalParticipation() const;

  bool isRelationshipAtStart() const;

  QVariantMap serialize() const override;

  bool deserialize(
    const QVariantMap& data
  ) override;

signals:
  void relationshipEndChanged(
    RelationshipEnd* newRelationshipEnd
  );

  void cardinalityOffsetChanged(
    const QPointF& newOffset
  );

private slots:
  void onRelationshipEndChanged();

private:
  RelationshipEnd* m_relationshipEnd;
  QPointF m_cardinalityOffset;

  void connectToRelationshipEnd();
  void disconnectFromRelationshipEnd();
};

#endif // RELATIONSHIPCONNECTIONLINE_H