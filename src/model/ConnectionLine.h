#ifndef CONNECTIONLINE_H
#define CONNECTIONLINE_H

#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtGui/QColor>

class ConnectionPoint;
class BasicElement;
class DiagramScene;

// -----------------------------------------------------------------------------------------------------

enum class ConnectionLineType {
    Straight = 0,
    Orthogonal,
    Bezier
};

// -----------------------------------------------------------------------------------------------------

class ConnectionLine : public QObject
{
    Q_OBJECT
        Q_PROPERTY(QString id READ id CONSTANT)
        Q_PROPERTY(BasicElement* startElement READ startElement NOTIFY startElementChanged)
        Q_PROPERTY(BasicElement* endElement READ endElement NOTIFY endElementChanged)
        Q_PROPERTY(ConnectionLineType lineType READ lineType WRITE setLineType NOTIFY lineTypeChanged)

public:
    explicit ConnectionLine(
        BasicElement* startElement,
        BasicElement* endElement,
        QObject* parent = nullptr
    );

    virtual ~ConnectionLine();

    QString id() const { return m_id; }
    BasicElement* startElement() const { return m_startElement; }
    BasicElement* endElement() const { return m_endElement; }
    ConnectionLineType lineType() const { return m_lineType; }
    QColor lineColor() const { return m_lineColor; }
    qreal lineWidth() const { return m_lineWidth; }

    void setLineType(
        ConnectionLineType type
    );

    void setLineColor(
        const QColor& color
    );

    void setLineWidth(
        qreal width
    );

    void setStartElement(
        BasicElement* element
    );

    void setEndElement(
        BasicElement* element
    );

    bool isValid() const;

    QPointF getStartPosition() const;
    QPointF getEndPosition() const;

    ConnectionPoint* getStartConnectionPoint() const;
    ConnectionPoint* getEndConnectionPoint() const;

    static QString lineTypeToString(
        ConnectionLineType type
    );

    static ConnectionLineType lineTypeFromString(
        const QString& typeString
    );

signals:
    void startElementChanged(
        BasicElement* newStartElement
    );

    void endElementChanged(
        BasicElement* newEndElement
    );

    void lineTypeChanged(
        ConnectionLineType newType
    );

    void lineColorChanged(
        const QColor& newColor
    );

    void lineWidthChanged(
        qreal newWidth
    );

    void connectionChanged();

private slots:
    void onElementPositionChanged();
    void onElementDestroyed();
    void onConnectionBeingRemoved(
        ConnectionLine* connectionBeingRemoved
    );

private:
    Q_DISABLE_COPY(ConnectionLine)

    QString m_id;
    BasicElement* m_startElement;
    BasicElement* m_endElement;
    ConnectionLineType m_lineType;
    QColor m_lineColor;
    qreal m_lineWidth;

    void connectToElements();
    void disconnectFromElements();
    ConnectionPoint* findBestConnectionPoint(
        BasicElement* fromElement,
        BasicElement* toElement
    ) const;
};

#endif // CONNECTIONLINE_H