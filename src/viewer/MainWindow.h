#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "DiagramView.h"
#include "DraggableButton.h"
#include "controller/DiagramScene.h"
#include "model/BasicElement.h"
#include "model/ConnectionLine.h"

// -----------------------------------------------------------------------------------------------------
class Attribute;
class QAction;
class QMenu;
class QMenuBar;
class QPushButton;
class QStatusBar;
// -----------------------------------------------------------------------------------------------------

class PropertiesPanel;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(
    QWidget* parent = nullptr
  );

  ~MainWindow() = default;

protected:
  void closeEvent(
    QCloseEvent* event
  ) override;

public slots:
  void newFile();
  void openFile();
  void saveFile();
  void saveAsFile();
  void exportDiagram();
  void exitApplication();

  void showAbout();
  void showHelp();

private slots:
  void onElementDropped(
    const QString& elementType,
    const QPointF& position
  );

  void onTabChanged(
    int index
  );

  void onTabCloseRequested(
    int index
  );

  void onAddEntityClicked();

  void onAddAttributeClicked();

  void onAddRelationshipClicked();

  void onSelectionChanged();

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

  void onAddAttributeToEntityClicked();
  void onRemoveAttributeFromEntityClicked();

  void onAddSubAttributeToAttributeClicked();
  void onRemoveSubAttributeFromAttributeClicked();

private:
  void setupUI();

  void createMenuBar();

  QAction* createAction(
    const QString& text,
    QKeySequence shortcut,
    const QString& statusTip,
    QMenu* parentMenu
  );

  QPushButton* createPushButton(
    const QString& text,
    const QString& iconPath = ""
  );

  void createHelpMenu();

  void createFileMenu();

  void createStatusBar();

  void createCentralWidget();

  void createDiagramArea();

  void createSidePanel();

  void connectSignals();

  void updateWindowTitle();

  void updateStatusBar(
    const QString& message
  );

  void updatePropertiesPanel();

  void clearPropertiesPanel();

  void populatePropertiesForElement(
    BasicElement* element
  );

  void populatePropertiesForConnection(
    ConnectionLine* connection
  );

  void populateAttributeProperties(
    BasicElement* element,
    QTreeWidgetItem* parent
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

  void populateEntityProperties(
    BasicElement* element,
    QTreeWidgetItem* parent
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

  void populateAttributeList(
    QTreeWidgetItem* parentItem,
    const QList<Attribute*>& attributes,
    const QString& propertyPrefix
  );

  bool handleAttributePropertyChange(
    BasicElement* element,
    const QString& propertyType,
    const QString& newValue
  );

  bool handleAttributeTypeChange(
    BasicElement* element,
    const QString& propertyKey,
    const QString& value
  );

  QList<QIcon> getConnectionLineTypeIcons();

  QWidget* m_centralWidget;
  QHBoxLayout* m_mainLayout;
  QSplitter* m_mainSplitter;

  QWidget* m_diagramWidget;
  QVBoxLayout* m_diagramLayout;
  DiagramScene* m_diagramScene;
  DiagramView* m_graphicsView;

  QTabWidget* m_sideTabWidget;
  QWidget* m_drawingTab;
  QWidget* m_propertiesTab;

  QVBoxLayout* m_drawingLayout;
  QLabel* m_drawingLabel;

  QVBoxLayout* m_propertiesLayout;
  QTreeWidget* m_propertiesTree;

  QMenuBar* m_menuBar;
  QMenu* m_fileMenu;
  QMenu* m_helpMenu;

  QStatusBar* m_statusBar;
  QLabel* m_statusLabel;

  QString m_currentFileName;
  bool m_isModified;

  std::map<QString, QWidget*> m_propertyWidgets;
};

#endif // MAINWINDOW_H