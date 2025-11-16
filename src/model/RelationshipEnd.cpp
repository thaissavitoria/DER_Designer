#include "RelationshipEnd.h"

//----------------------------------------------------------------------------------------------

RelationshipEnd::RelationshipEnd(
    QObject* parent
)
    : QObject(parent)
    , m_entityId("")
    , m_cardinality(Cardinality::One)
    , m_isTotalParticipation(false)
{
}

//----------------------------------------------------------------------------------------------

RelationshipEnd::RelationshipEnd(
    const QString& entityId,
    Cardinality cardinality,
    bool isTotalParticipation,
    QObject* parent
)
    : QObject(parent)
    , m_entityId(entityId)
    , m_cardinality(cardinality)
    , m_isTotalParticipation(isTotalParticipation)
{
}

//----------------------------------------------------------------------------------------------

void RelationshipEnd::setEntityId(
    const QString& entityId
)
{
    if (m_entityId != entityId) {
        m_entityId = entityId;
        emit entityIdChanged(m_entityId);
    }
}

//----------------------------------------------------------------------------------------------

void RelationshipEnd::setCardinality(
    Cardinality cardinality
)
{
    if (m_cardinality != cardinality) {
        m_cardinality = cardinality;
        emit cardinalityChanged(m_cardinality);
    }
}

//----------------------------------------------------------------------------------------------

void RelationshipEnd::setIsTotalParticipation(
    bool isTotalParticipation
)
{
    if (m_isTotalParticipation != isTotalParticipation) {
        m_isTotalParticipation = isTotalParticipation;
        emit totalParticipationChanged(m_isTotalParticipation);
    }
}

//----------------------------------------------------------------------------------------------

QString RelationshipEnd::cardinalityToString(
    Cardinality cardinality
)
{
    switch (cardinality) {
        case Cardinality::Many:
            return "M";
        case Cardinality::One:
        default:
            return "1";
    }
}

//----------------------------------------------------------------------------------------------

Cardinality RelationshipEnd::cardinalityFromString(
    const QString& cardinalityString
)
{
    if (cardinalityString == "1") {
        return Cardinality::One;
    }
    else if (cardinalityString == "M" || cardinalityString == "N") {
        return Cardinality::Many;
    }
    
    return Cardinality::One;
}

//----------------------------------------------------------------------------------------------