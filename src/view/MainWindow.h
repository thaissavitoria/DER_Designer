#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

#include "DiagramView.h"
#include "DraggableButton.h"
#include "PropertiesPanel.h"
#include "view/DiagramScene.h"
#include "model/DiagramFileManager.h"
#include "model/AutoSaveManager.h"
#include "model/BasicElement.h"
#include "model/ConnectionLine.h"

// -----------------------------------------------------------------------------------------------------
class QAction;
class QMenu;
class QMenuBar;
class QPushButton;
class QStatusBar;
// -----------------------------------------------------------------------------------------------------

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

  void onAddElementClicked(
    ElementType elementType
  );

  void onSelectionChanged();

  void onAutoSaveTriggered();

private:
  void setupUI();

  void createMenuBar();

  QAction* createAction(
    const QString& text,
    QKeySequence shortcut,
    const QString& statusTip,
    QMenu* parentMenu
  );

  DraggableButton* createDrawingButton(
    const QString& elementTypeName,
    ElementType elementType
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
  PropertiesPanel* m_propertiesPanel;

  QMenuBar* m_menuBar;
  QMenu* m_fileMenu;
  QMenu* m_helpMenu;

  QStatusBar* m_statusBar;
  QLabel* m_statusLabel;

  bool m_isModified;

  AutoSaveManager* m_autoSaveManager;
  DiagramFileManager* m_fileManager;

  void setupAutoSave();
};

#endif // MAINWINDOW_H