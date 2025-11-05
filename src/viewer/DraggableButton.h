#ifndef DRAGGABLEBUTTON_H
#define DRAGGABLEBUTTON_H

#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QPushButton>

class DraggableButton : public QPushButton
{
    Q_OBJECT

public:
    explicit DraggableButton(
        const QString& elementType,
        QWidget* parent = nullptr
    );

    void setElementType(
        const QString& elementType
    );

    QString elementType() const { return m_elementType; }

protected:
    void mousePressEvent(
        QMouseEvent* event
    ) override;

    void mouseMoveEvent(
        QMouseEvent* event
    ) override;

private:
    QString m_elementType;
    QPoint m_dragStartPosition;
    bool m_dragging;

    void setupIcon();
    QString getIconPath() const;
};

#endif // DRAGGABLEBUTTON_H