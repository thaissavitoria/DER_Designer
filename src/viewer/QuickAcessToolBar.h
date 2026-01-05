#ifndef QUICKACESSTOOLBAR_H
#define QUICKACESSTOOLBAR_H

#include <QToolBar>
#include <QMenu>
#include <QIcon>
#include <QSize>
#include <QCursor>

class MainWindow;

// -----------------------------------------------------------------------------------------------------
/**
 * @brief Quick access toolbar for some ERD diagram operations
 *
 * This class provides a customizable toolbar with quick access to frequently used
 * actions in the ERD Designer application. It extends QToolBar to provide:
 * - Quick access to common file operations (New, Open, Save)
 * - Help and About actions for user assistance
 * - Integration with MainWindow for action execution
 */

class QuickAccessToolbar : public QToolBar {
  Q_OBJECT

public:
  explicit QuickAccessToolbar(
    QWidget* parent = nullptr
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
