#ifndef RELATIONSHIPEND_H
#define RELATIONSHIPEND_H

#include <QtCore/QObject>
#include <QtCore/QString>

// -----------------------------------------------------------------------------------------------------

enum class Cardinality {
    One,          // 1
    Many         // M,N
};

// -----------------------------------------------------------------------------------------------------

class RelationshipEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString entityId READ entityId WRITE setEntityId NOTIFY entityIdChanged)
    Q_PROPERTY(Cardinality cardinality READ cardinality WRITE setCardinality NOTIFY cardinalityChanged)
    Q_PROPERTY(bool isTotalParticipation READ isTotalParticipation WRITE setIsTotalParticipation NOTIFY totalParticipationChanged)

public:
    explicit RelationshipEnd(
        QObject* parent = nullptr
    );

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

    void setEntityId(
        const QString& entityId
    );

    void setCardinality(
        Cardinality cardinality
    );

    void setIsTotalParticipation(
        bool isTotalParticipation
    );

    static QString cardinalityToString(
        Cardinality cardinality
    );

    static Cardinality cardinalityFromString(
        const QString& cardinalityString
    );

signals:
    void entityIdChanged(
        const QString& newEntityId
    );

    void cardinalityChanged(
        Cardinality newCardinality
    );

    void totalParticipationChanged(
        bool isTotalParticipation
    );

private:
    QString m_entityId;
    Cardinality m_cardinality;
    bool m_isTotalParticipation;
};

#endif // RELATIONSHIPEND_H