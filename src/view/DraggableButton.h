#ifndef DRAGGABLEBUTTON_H
#define DRAGGABLEBUTTON_H

#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QPushButton>

// -----------------------------------------------------------------------------------------------------
/**
 * @brief Draggable button for adding ERD elements to the diagram
 *
 * This class extends QPushButton to provide drag-and-drop functionality for creating
 * new ERD elements in the diagram. It supports:
 * - Drag-and-drop interaction for intuitive element placement
 * - Visual feedback during drag operations
 * - Element type identification (Entity, Attribute, Relationship)
 * - Icon-based visual representation of element types
 * - MIME data encoding for cross-widget drag operations
 * - Click-based element creation as alternative to drag-and-drop
 *
 * The button initiates a drag operation when the user clicks and moves the mouse,
 * allowing elements to be dropped onto the diagram view at specific positions.
 * This provides an intuitive interface similar to drawing/diagramming applications.
 */

class DraggableButton : public QPushButton
{
  Q_OBJECT

public:
  explicit DraggableButton(
    const QString& elementType,
    QWidget* parent = nullptr
  );

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
  QString getDisplayName() const;
};

#endif // DRAGGABLEBUTTON_H