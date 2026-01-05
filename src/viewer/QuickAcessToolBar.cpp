#include "QuickAcessToolbar.h"
#include "MainWindow.h"

#include <QStyle>
#include <QAction>
#include <QFile>

//----------------------------------------------------------------------------------------------

QuickAccessToolbar::QuickAccessToolbar(
  QWidget* parent
) :
  QToolBar(parent),
  m_mainWindow(qobject_cast<MainWindow*>(parent))
{
  setupToolbar();
  addDefaultActions();
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::setupToolbar()
{
  setMovable(false);
  setFloatable(false);
  setToolButtonStyle(Qt::ToolButtonIconOnly);
  setIconSize(QSize(16, 16));
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::addDefaultActions()
{
  QAction* newAction = addActionToToolBar(style()->standardIcon(QStyle::SP_FileIcon), "Novo", "Criar novo diagrama", "Criar um novo diagrama DER");
  connect(newAction, &QAction::triggered, this, &QuickAccessToolbar::onNewAction);

  QAction* openAction = addActionToToolBar(style()->standardIcon(QStyle::SP_DirOpenIcon), "Abrir", "Abrir diagrama existente", "Abrir um diagrama DER existente");
  connect(openAction, &QAction::triggered, this, &QuickAccessToolbar::onOpenAction);

  QAction* saveAction = addActionToToolBar(style()->standardIcon(QStyle::SP_DialogSaveButton), "Salvar", "Salvar diagrama atual", "Salvar o diagrama atual");
  connect(saveAction, &QAction::triggered, this, &QuickAccessToolbar::onSaveAction);

  addSeparator();

  QAction* helpAction = addActionToToolBar(style()->standardIcon(QStyle::SP_MessageBoxQuestion), "Ajuda", "Mostrar ajuda", "Mostrar ajuda da aplicação");
  connect(helpAction, &QAction::triggered, this, &QuickAccessToolbar::onHelpAction);

  QAction* aboutAction = addActionToToolBar(style()->standardIcon(QStyle::SP_MessageBoxInformation), "Sobre", "Sobre o DER Designer", "Mostrar informações sobre o DER Designer");
  connect(aboutAction, &QAction::triggered, this, &QuickAccessToolbar::onAboutAction);
}

//----------------------------------------------------------------------------------------------

QAction* QuickAccessToolbar::addActionToToolBar(
  const QIcon& icon,
  const QString& actionName,
  const QString& toolTip,
  const QString& statusTip
)
{
  QAction* newAction = addAction(icon, actionName);
  newAction->setToolTip(toolTip);
  newAction->setStatusTip(statusTip);
  return newAction;
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onNewAction()
{
  if (m_mainWindow) {
    m_mainWindow->newFile();
  }
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onSaveAction()
{
  if (m_mainWindow) {
    m_mainWindow->saveFile();
  }
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onOpenAction()
{
  if (m_mainWindow) {
    m_mainWindow->openFile();
  }
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onHelpAction()
{
  if (m_mainWindow) {
    m_mainWindow->showHelp();
  }
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onAboutAction()
{
  if (m_mainWindow) {
    m_mainWindow->showAbout();
  }
}

//----------------------------------------------------------------------------------------------