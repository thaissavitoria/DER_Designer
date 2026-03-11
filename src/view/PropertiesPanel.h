#ifndef PROPERTIESPANEL_H
#define PROPERTIESPANEL_H

#include <map>

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

#include "model/BasicElement.h"
#include "model/ConnectionLine.h"

// -----------------------------------------------------------------------------------------------------

class Attribute;
class DiagramScene;

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Panel widget for viewing and editing properties of selected diagram elements.
 *
 * Displays a tree-based property editor that updates based on the current selection
 * in the DiagramScene, supporting elements such as entities, relationships, and attributes,
 * as well as connection lines.
 */

class PropertiesPanel : public QWidget
{
  Q_OBJECT

public:
  explicit PropertiesPanel(
    DiagramScene* scene,
    QWidget* parent = nullptr
  );

  void refresh(
    const QList<BasicElement*>& selectedElements,
    const QSet<ConnectionLine*>& selectedConnections
  );

signals:
  void statusMessageRequested(
    const QString& message
  );

  void diagramModified();

private slots:
  void onPropertyValueChanged(
    QTreeWidgetItem* item,
    int column
  );

  void onComboBoxChanged(
    const QString& value
  );

  void onCheckBoxChanged(
    bool checked
  );

  void onLineEditChanged(
    const QString& text
  );

  void onAddAttributeOnTreeClicked();

  void onRemoveAttributeOnTreeClicked();

private:
  void clearPanel();

  void populateForElement(
    BasicElement* element
  );

  void populateForConnection(
    ConnectionLine* connection
  );

  void populateAttributeProperties(
    Attribute* attribute,
    QTreeWidgetItem* parent
  );

  void populateEntityProperties(
    BasicElement* element
  );

  void populateRelationshipProperties(
    BasicElement* element
  );

  void populateAttributeList(
    QTreeWidgetItem* parentItem,
    const QList<Attribute*>& attributes,
    const QString& propertyPrefix
  );

  QList<Attribute*> getAttributesFromIds(
    const QList<QString>& attributeIds
  );

  QTreeWidgetItem* createPropertyItem(
    QTreeWidgetItem* parent,
    const QString& propertyName,
    const QVariant& value,
    const QString& propertyKey,
    bool editable = true
  );

  QTreeWidgetItem* createComboBoxPropertyItem(
    QTreeWidgetItem* parent,
    const QString& propertyName,
    const QStringList& options,
    const QString& currentValue,
    const QString& propertyKey
  );

  QTreeWidgetItem* createIconComboBoxPropertyItem(
    QTreeWidgetItem* parent,
    const QString& propertyName,
    const QStringList& options,
    const QList<QIcon>& icons,
    const QString& currentValue,
    const QString& propertyKey
  );

  QTreeWidgetItem* createCheckBoxPropertyItem(
    QTreeWidgetItem* parent,
    const QString& propertyName,
    bool currentValue,
    const QString& propertyKey
  );

  QTreeWidgetItem* createLineEditPropertyItem(
    QTreeWidgetItem* parent,
    const QString& propertyName,
    const QString& currentValue,
    const QString& propertyKey
  );

  QTreeWidgetItem* createButtonPropertyItem(
    QTreeWidgetItem* parent,
    const QString& propertyName,
    const QString& buttonText,
    const QString& propertyKey
  );

  void updatePropertyItemText(
    const QString& propertyKey,
    const QString& value
  );

  bool handleAttributePropertyChange(
    BasicElement* element,
    const QString& propertyType,
    const QVariant& newValue
  );

  bool handleRelationshipEndCardinalityChange(
    BasicElement* element,
    const QString& propertyKey,
    const QString& value
  );

  bool handleRelationshipEndParticipationChange(
    BasicElement* element,
    const QString& propertyKey,
    bool value
  );

  void updatesAfterChangingProperty(
    BasicElement* element
  );

  void connectBasicElementsWithConnectionLine(
    BasicElement* startElement,
    BasicElement* endElement
  );

  QList<QIcon> getConnectionLineTypeIcons();

  DiagramScene* m_scene;
  QVBoxLayout* m_layout;
  QTreeWidget* m_tree;
  std::map<QString, QWidget*> m_propertyWidgets;
};

#endif // PROPERTIESPANEL_H
