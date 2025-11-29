#ifndef QUICKACESSTOOLBAR_H
#define QUICKACESSTOOLBAR_H

#include <QToolBar>
#include <QMenu>
#include <QIcon>
#include <QSize>
#include <QCursor>

class MainWindow;

class QuickAccessToolbar : public QToolBar {
  Q_OBJECT

public:
  explicit QuickAccessToolbar(
    QWidget* parent = nullptr
  );

  void setMainWindow(
    MainWindow* mainWindow
  );

private slots:
  void onNewAction();
  void onSaveAction();
  void onOpenAction();
  void onHelpAction();
  void onAboutAction();

private:
  void setupToolbar();
  void addDefaultActions();
    
  QAction* addActionToToolBar(
    const QIcon& icon,
    const QString& actionName,
    const QString& toolTip,
    const QString& statusTip
  );

  MainWindow* m_mainWindow;
};

#endif // QUICKACESSTOOLBAR_H
