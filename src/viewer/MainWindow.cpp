#include "MainWindow.h"
#include "controller/PropertyCommand.h"

#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

#include <QtGui/QAction>
#include <QtGui/QCloseEvent>
#include <QtGui/QIcon>

#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QVBoxLayout>
#include "QStringList"

#include "ElementGraphicsItem.h"
#include "QuickAcessToolbar.h"
#include "model/Attribute.h"
#include "model/AttributeType.h"
#include "model/Entity.h"
#include "model/Relationship.h"

// -----------------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_mainSplitter(nullptr)
    , m_diagramWidget(nullptr)
    , m_diagramLayout(nullptr)
    , m_diagramScene(nullptr)
	  , m_graphicsView(nullptr)
    , m_sideTabWidget(nullptr)
    , m_drawingTab(nullptr)
    , m_propertiesTab(nullptr)
    , m_drawingLayout(nullptr)
    , m_drawingLabel(nullptr)
    , m_propertiesLayout(nullptr)
    , m_propertiesTree(nullptr)
    , m_menuBar(nullptr)
    , m_fileMenu(nullptr)
    , m_helpMenu(nullptr)
    , m_statusBar(nullptr)
    , m_statusLabel(nullptr)
    , m_isModified(false)
{
    setupUI();
    connectSignals();
    updateWindowTitle();
    updateStatusBar("Pronto");

    resize(1200, 800);

    setWindowState(Qt::WindowMaximized);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::setupUI()
{
    createMenuBar();
    createStatusBar();
    createCentralWidget();

    setWindowTitle("DER Designer");
    setMinimumSize(800, 600);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createMenuBar()
{
    m_menuBar = menuBar();

    auto quickAccess = new QuickAccessToolbar(this);

    addToolBar(Qt::TopToolBarArea, quickAccess);

    createFileMenu();
    createHelpMenu();
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createFileMenu()
{
    m_fileMenu = m_menuBar->addMenu("&Arquivo");

    QAction* newAction = createAction("&Novo", QKeySequence::New, "Criar um novo diagrama DER", m_fileMenu);
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);

    QAction* openAction = createAction("&Abrir...", QKeySequence::Open, "Abrir um diagrama DER existente", m_fileMenu);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    m_fileMenu->addSeparator();

    QAction* saveAction = createAction("&Salvar", QKeySequence::Save, "Salvar o diagrama atual", m_fileMenu);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    QAction* saveAsAction = createAction("Salvar &Como...", QKeySequence::SaveAs, "Salvar o diagrama com um novo nome", m_fileMenu);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);

    m_fileMenu->addSeparator();

    QAction* exportAction = createAction("&Exportar...", QKeySequence::UnknownKey, "Exportar diagrama para imagem", m_fileMenu);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportDiagram);

    m_fileMenu->addSeparator();

    QAction* exitAction = createAction("Sai&r", QKeySequence::Quit, "Sair da aplicação", m_fileMenu);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exitApplication);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createHelpMenu()
{
    m_helpMenu = m_menuBar->addMenu("&Ajuda");

    QAction* helpAction = createAction("&Ajuda", QKeySequence::HelpContents, "Mostrar ajuda da aplicação", m_helpMenu);
    connect(helpAction, &QAction::triggered, this, &MainWindow::showHelp);

    m_helpMenu->addSeparator();

    QAction* aboutAction = createAction("&Sobre", QKeySequence::UnknownKey, "Sobre o DER Designer", m_helpMenu);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

// -----------------------------------------------------------------------------------------------------

QAction* MainWindow::createAction(
    const QString& text,
    QKeySequence shortcut,
    const QString& statusTip,
    QMenu* parentMenu
)
{
    auto action = new QAction(text, this);
    action->setShortcut(shortcut);
    action->setStatusTip(statusTip);
    parentMenu->addAction(action);
    return action;
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createStatusBar()
{
    m_statusBar = statusBar();
    m_statusLabel = new QLabel();
    m_statusBar->addWidget(m_statusLabel);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createCentralWidget()
{
    m_centralWidget = new QWidget();
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QHBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);
    m_mainLayout->setSpacing(5);

    m_mainSplitter = new QSplitter(Qt::Horizontal);
    m_mainLayout->addWidget(m_mainSplitter);

    createDiagramArea();
    createSidePanel();

    m_mainSplitter->setStretchFactor(0, 3); // Diagram area
    m_mainSplitter->setStretchFactor(1, 1); // Side panel

    QList<int> initialSizes;
    initialSizes << 900 << 300;
    m_mainSplitter->setSizes(initialSizes);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createSidePanel()
{
    m_sideTabWidget = new QTabWidget();
    m_sideTabWidget->setTabPosition(QTabWidget::North);
    m_sideTabWidget->setMaximumWidth(350);
    m_sideTabWidget->setMinimumWidth(250);

    m_drawingTab = new QWidget();
    m_drawingLayout = new QVBoxLayout(m_drawingTab);
    m_drawingLayout->setContentsMargins(10, 10, 10, 10);
    m_drawingLayout->setSpacing(10);

    auto entityBtn = new DraggableButton("Entity");
    connect(entityBtn, &QPushButton::clicked, this, &MainWindow::onAddEntityClicked);
    m_drawingLayout->addWidget(entityBtn);

    auto attributeBtn = new DraggableButton("Attribute");
    connect(attributeBtn, &QPushButton::clicked, this, &MainWindow::onAddAttributeClicked);
    m_drawingLayout->addWidget(attributeBtn);

    auto relationshipBtn = new DraggableButton("Relationship");
    connect(relationshipBtn, &QPushButton::clicked, this, &MainWindow::onAddRelationshipClicked);
    m_drawingLayout->addWidget(relationshipBtn);

    m_drawingLayout->addStretch();

    m_sideTabWidget->addTab(m_drawingTab, "Desenho");

    m_propertiesTab = new QWidget();
    m_propertiesLayout = new QVBoxLayout(m_propertiesTab);
    m_propertiesLayout->setContentsMargins(10, 10, 10, 10);
    m_propertiesLayout->setSpacing(5);

    auto propertiesLabel = new QLabel("Propriedades:");
    propertiesLabel->setStyleSheet("font-weight: bold;");
    m_propertiesLayout->addWidget(propertiesLabel);

    m_propertiesTree = new QTreeWidget();
    m_propertiesTree->setHeaderLabels(QStringList() << "Propriedade" << "Valor");
    m_propertiesTree->setAlternatingRowColors(true);
    m_propertiesTree->setRootIsDecorated(false);

    m_propertiesLayout->addWidget(m_propertiesTree);

    m_sideTabWidget->addTab(m_propertiesTab, "Propriedades");

    m_mainSplitter->addWidget(m_sideTabWidget);
}

// -----------------------------------------------------------------------------------------------------

QPushButton* MainWindow::createPushButton(
    const QString& text, 
    const QString& iconPath 
)
{
    auto button = new QPushButton(text);
    if (!iconPath.isEmpty()) {
        QIcon icon(iconPath);
        button->setIcon(icon);
    }
    return button;
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_isModified) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this,
            "DER Designer",
            "O diagrama foi modificado.\n"
            "Deseja salvar as alterações?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            saveFile();
            event->accept();
        }
        else if (ret == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }
    event->accept();
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::newFile()
{
    if (m_isModified) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this,
            "DER Designer",
            "O diagrama atual foi modificado.\n"
            "Deseja salvar as alterações?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            saveFile();
        }
        else if (ret == QMessageBox::Cancel) {
            return;
        }
    }

    m_diagramScene->clear();

    m_currentFileName.clear();
    m_isModified = false;
    updateWindowTitle();
    updateStatusBar("Novo diagrama criado");
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Abrir Diagrama DER", "", "Arquivos DER (*.der);;Todos os arquivos (*)");

    if (!fileName.isEmpty()) {
        m_currentFileName = fileName;
        m_isModified = false;
        updateWindowTitle();
        updateStatusBar("Diagrama carregado: " + QFileInfo(fileName).baseName());
    }
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::saveFile()
{
    if (m_currentFileName.isEmpty()) {
        saveAsFile();
        return;
    }

    m_isModified = false;
    updateWindowTitle();
    updateStatusBar("Diagrama salvo: " + QFileInfo(m_currentFileName).baseName());
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Salvar Diagrama DER", "", "Arquivos DER (*.der);;Todos os arquivos (*)");

    if (!fileName.isEmpty()) {
        m_currentFileName = fileName;
        saveFile();
    }
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::exportDiagram()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Exportar Diagrama", "", "PNG (*.png);;JPG (*.jpg);;PDF (*.pdf)");

    if (!fileName.isEmpty()) {
        updateStatusBar("Diagrama exportado: " + QFileInfo(fileName).baseName());
    }
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::exitApplication()
{
    close();
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createDiagramArea()
{
    m_diagramWidget = new QWidget();
    m_diagramLayout = new QVBoxLayout(m_diagramWidget);
    m_diagramLayout->setContentsMargins(0, 0, 0, 0);
    m_diagramLayout->setSpacing(0);

    m_diagramScene = new DiagramScene(this);

    m_graphicsView = new DiagramView(m_diagramScene);  
    m_graphicsView->setBackgroundBrush(QColor(245, 245, 245));

    connect(m_graphicsView, &DiagramView::elementDropped, this, &MainWindow::onElementDropped);

    m_diagramLayout->addWidget(m_graphicsView);
    m_mainSplitter->addWidget(m_diagramWidget);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::onElementDropped(
    const QString& elementType,
    const QPointF& position
)
{
    BasicElement* element = nullptr;

    if (elementType == "Entity") {
        element = new Entity("Entidade");
    }
    else if (elementType == "Attribute") {
        element = new Attribute("Atributo");
    }
    else if (elementType == "Relationship") {
      element = new Relationship("Relacionamento");
    }

    if (element) {
        element->setPosition(position);
        m_diagramScene->addElement(element);

        m_isModified = true;
        updateWindowTitle();
        updateStatusBar(QString("%1 adicionado na posição (%2, %3)")
            .arg(elementType)
            .arg(position.x(), 0, 'f', 1)
            .arg(position.y(), 0, 'f', 1));
    }
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::connectSignals()
{
    connect(m_sideTabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    if (m_diagramScene) {
        connect(m_diagramScene, &DiagramScene::selectionChanged, this, &MainWindow::onSelectionChanged);
    }

    if (m_propertiesTree) {
        connect(m_propertiesTree, &QTreeWidget::itemChanged, this, &MainWindow::onPropertyValueChanged);
    }
}

//----------------------------------------------------------------------------------------------

void MainWindow::onSelectionChanged()
{
    updatePropertiesPanel();
}

//----------------------------------------------------------------------------------------------

void MainWindow::updatePropertiesPanel()
{
  if (!m_propertiesTree || !m_diagramScene) {
    return;
  }

  disconnect(m_propertiesTree, &QTreeWidget::itemChanged, this, &MainWindow::onPropertyValueChanged);

  clearPropertiesPanel();

  QList<ConnectionLine*> selectedConnections = m_diagramScene->getSelectedConnections();
  QList<BasicElement*> selectedElements = m_diagramScene->getSelectedElements();

  if (selectedConnections.size() == 1 && selectedElements.isEmpty()) {
    populatePropertiesForConnection(selectedConnections.first());
  }
  else if (selectedElements.size() == 1 && selectedConnections.isEmpty()) {
    populatePropertiesForElement(selectedElements.first());
  }
  else if (selectedElements.size() > 1 || selectedConnections.size() > 1) {
    QTreeWidgetItem* multiSelectItem = new QTreeWidgetItem(m_propertiesTree);
    multiSelectItem->setText(0, "Seleção Múltipla");
    int totalSelected = selectedElements.size() + selectedConnections.size();
    multiSelectItem->setText(1, QString("%1 itens selecionados").arg(totalSelected));
    multiSelectItem->setFlags(multiSelectItem->flags() & ~Qt::ItemIsEditable);
  }

  connect(m_propertiesTree, &QTreeWidget::itemChanged, this, &MainWindow::onPropertyValueChanged);

  m_propertiesTree->expandAll();
}

//----------------------------------------------------------------------------------------------

void MainWindow::clearPropertiesPanel()
{
    if (m_propertiesTree) {
        m_propertiesTree->clear();
    }
    
    for (auto pair : m_propertyWidgets) {
      pair.second->deleteLater();
    }
    m_propertyWidgets.clear();
}

//----------------------------------------------------------------------------------------------

void MainWindow::populatePropertiesForElement(
  BasicElement* element
)
{
  if (!element || !m_propertiesTree) {
    return;
  }

  QTreeWidgetItem* basicGroup = new QTreeWidgetItem(m_propertiesTree);
  basicGroup->setText(0, "GERAL");
  basicGroup->setExpanded(true);
  basicGroup->setFlags(basicGroup->flags() & ~Qt::ItemIsEditable);

  createPropertyItem(basicGroup, "Nome", element->name(), "name");
  createPropertyItem(basicGroup, "Tipo", element->typeDisplayName(), "type", false);

  auto attribute = qobject_cast<Attribute*>(element);
  if (attribute) {
    populateAttributeProperties(element, nullptr);
  }

  auto entity = qobject_cast<Entity*>(element);
  if (entity) {
    populateEntityProperties(element, nullptr);
  }

  auto geometryGroup = new QTreeWidgetItem(m_propertiesTree);
  geometryGroup->setText(0, "GEOMETRIA");
  geometryGroup->setExpanded(true);
  geometryGroup->setFlags(geometryGroup->flags() & ~Qt::ItemIsEditable);

  createPropertyItem(geometryGroup, "X", QString::number(element->position().x(), 'f', 2), "posX");
  createPropertyItem(geometryGroup, "Y", QString::number(element->position().y(), 'f', 2), "posY");
  createPropertyItem(geometryGroup, "Largura", QString::number(element->size().width(), 'f', 2), "width");
  createPropertyItem(geometryGroup, "Altura", QString::number(element->size().height(), 'f', 2), "height");

  QStringList customProperties = element->propertyKeys();
  if (!customProperties.isEmpty()) {
    auto customGroup = new QTreeWidgetItem(m_propertiesTree);
    customGroup->setText(0, "PROPRIEDADES PERSONALIZADAS");
    customGroup->setExpanded(true);
    customGroup->setFlags(customGroup->flags() & ~Qt::ItemIsEditable);

    for (const QString& key : customProperties) {
      createPropertyItem(customGroup, key, element->getProperty(key).toString(), QString("custom:%1").arg(key));
    }
  }
}

//----------------------------------------------------------------------------------------------

void MainWindow::populateEntityProperties(
  BasicElement* element,
  QTreeWidgetItem* parent
)
{
  auto entity = qobject_cast<Entity*>(element);
  if (!entity) {
    return;
  }

  auto attributesGroup = new QTreeWidgetItem(m_propertiesTree);
  attributesGroup->setText(0, "ATRIBUTOS");
  attributesGroup->setExpanded(true);
  attributesGroup->setFlags(attributesGroup->flags() & ~Qt::ItemIsEditable);

  createButtonPropertyItem(attributesGroup, "Adicionar Atributo", "+", "addAttribute");

  QList<Attribute*> attributes = entity->getAttributes();
  populateAttributeList(attributesGroup, attributes, "attr");
}

//----------------------------------------------------------------------------------------------

QTreeWidgetItem* MainWindow::createButtonPropertyItem(
  QTreeWidgetItem* parent,
  const QString& propertyName,
  const QString& buttonText,
  const QString& propertyKey
)
{
  auto item = new QTreeWidgetItem(parent);
  item->setText(0, propertyName);
  item->setData(0, Qt::UserRole, propertyKey);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  auto button = new QPushButton(buttonText);
  button->setProperty("propertyKey", propertyKey);
  button->setMaximumWidth(30);

  if (propertyKey == "addAttribute") {
    connect(button, &QPushButton::clicked, this, &MainWindow::onAddAttributeToEntityClicked);
  }
  else if (propertyKey.startsWith("removeAttribute_")) {
    connect(button, &QPushButton::clicked, this, &MainWindow::onRemoveAttributeFromEntityClicked);
  }
  else if (propertyKey == "addSubAttribute") {
    connect(button, &QPushButton::clicked, this, &MainWindow::onAddSubAttributeToAttributeClicked);
  }
  else if (propertyKey.startsWith("removeSubAttribute_")) {
    connect(button, &QPushButton::clicked, this, &MainWindow::onRemoveSubAttributeFromAttributeClicked);
  }

  m_propertiesTree->setItemWidget(item, 1, button);
  m_propertyWidgets[propertyKey] = button;

  return item;
}

//----------------------------------------------------------------------------------------------

void MainWindow::onAddSubAttributeToAttributeClicked()
{
  if (!m_diagramScene) {
    return;
  }

  QList<BasicElement*> selectedElements = m_diagramScene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  auto attribute = qobject_cast<Attribute*>(selectedElements.first());
  if (!attribute || !attribute->isCompositeAttribute()) {
    return;
  }

  auto subAttribute = new Attribute(QString("SubAtributo_%1").arg(attribute->getSubAttributes().size() + 1));

  QPointF attributePos = attribute->position();
  QSizeF attributeSize = attribute->size();
  int subAttributeCount = attribute->getSubAttributes().size();

  qreal offsetX = attributeSize.width() + 50;
  qreal offsetY = subAttributeCount * 70;
  subAttribute->setPosition(attributePos.x() + offsetX, attributePos.y() + offsetY);

  attribute->addSubAttribute(subAttribute);
  m_diagramScene->addElement(subAttribute);

  m_isModified = true;
  updateWindowTitle();
  updateStatusBar("Sub-atributo adicionado ao atributo composto");

  updatePropertiesPanel();
}

//----------------------------------------------------------------------------------------------

void MainWindow::onRemoveSubAttributeFromAttributeClicked()
{
  auto button = qobject_cast<QPushButton*>(sender());
  if (!button || !m_diagramScene) {
    return;
  }

  QString propertyKey = button->property("propertyKey").toString();
  QStringList parts = propertyKey.split("_");
  if (parts.size() != 2) {
    return;
  }

  int index = parts[1].toInt();

  QList<BasicElement*> selectedElements = m_diagramScene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  auto attribute = qobject_cast<Attribute*>(selectedElements.first());
  if (!attribute || !attribute->isCompositeAttribute()) {
    return;
  }

  QList<Attribute*> subAttributes = attribute->getSubAttributes();
  if (index >= 0 && index < subAttributes.size()) {
    Attribute* subAttr = subAttributes[index];
    attribute->removeSubAttribute(subAttr);

    m_isModified = true;
    updateWindowTitle();
    updateStatusBar("Sub-atributo removido do atributo composto");

    updatePropertiesPanel();
  }
}

//----------------------------------------------------------------------------------------------

void MainWindow::onAddAttributeToEntityClicked()
{
  if (!m_diagramScene) {
    return;
  }

  QList<BasicElement*> selectedElements = m_diagramScene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  auto entity = qobject_cast<Entity*>(selectedElements.first());
  if (!entity) {
    return;
  }

  auto attribute = new Attribute(QString("Atributo_%1").arg(entity->getAttributes().size() + 1));

  QPointF entityPos = entity->position();
  QSizeF entitySize = entity->size();
  int attributeCount = entity->getAttributes().size();

  qreal offsetX = entitySize.width() + 50;
  qreal offsetY = attributeCount * 80;
  attribute->setPosition(entityPos.x() + offsetX, entityPos.y() + offsetY);

  entity->addAttribute(attribute);
  m_diagramScene->addElement(attribute);

  m_isModified = true;
  updateWindowTitle();
  updateStatusBar("Atributo adicionado à entidade");

  updatePropertiesPanel();
}

//----------------------------------------------------------------------------------------------

void MainWindow::onRemoveAttributeFromEntityClicked()
{
  auto button = qobject_cast<QPushButton*>(sender());
  if (!button || !m_diagramScene) {
    return;
  }

  QString propertyKey = button->property("propertyKey").toString();
  QStringList parts = propertyKey.split("_");
  if (parts.size() != 2) {
    return;
  }

  int index = parts[1].toInt();

  QList<BasicElement*> selectedElements = m_diagramScene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  auto entity = qobject_cast<Entity*>(selectedElements.first());
  if (!entity) {
    return;
  }

  QList<Attribute*> attributes = entity->getAttributes();
  if (index >= 0 && index < attributes.size()) {
    Attribute* attr = attributes[index];
    entity->removeAttribute(attr);

    m_isModified = true;
    updateWindowTitle();
    updateStatusBar("Atributo removido da entidade");

    updatePropertiesPanel();
  }
}

//----------------------------------------------------------------------------------------------

void MainWindow::populateAttributeProperties(
  BasicElement* element,
  QTreeWidgetItem* parent
)
{
  auto attribute = qobject_cast<Attribute*>(element);
  if (!attribute) {
    return;
  }

  auto attributeGroup = new QTreeWidgetItem(m_propertiesTree);
  attributeGroup->setText(0, "ATRIBUTO");
  attributeGroup->setExpanded(true);
  attributeGroup->setFlags(attributeGroup->flags() & ~Qt::ItemIsEditable);

  QStringList attributeTypes = AttributeType::getAllAttributeTypeStrings();
  QString currentType = AttributeType::attributeTypeToString(attribute->attributeType());
  createComboBoxPropertyItem(attributeGroup, "Tipo de Atributo", attributeTypes, currentType, "attributeType");

  if (attribute->isCompositeAttribute()) {
    auto subAttributesGroup = new QTreeWidgetItem(m_propertiesTree);
    subAttributesGroup->setText(0, "SUB-ATRIBUTOS");
    subAttributesGroup->setExpanded(true);
    subAttributesGroup->setFlags(subAttributesGroup->flags() & ~Qt::ItemIsEditable);

    createButtonPropertyItem(subAttributesGroup, "Adicionar Sub-Atributo", "+", "addSubAttribute");

    QList<Attribute*> subAttributes = attribute->getSubAttributes();
    populateAttributeList(subAttributesGroup, subAttributes, "subattr");
  }
}

//----------------------------------------------------------------------------------------------

void MainWindow::populateAttributeList(
  QTreeWidgetItem* parentItem,
  const QList<Attribute*>& attributes,
  const QString& propertyPrefix
)
{
  for (int i = 0; i < attributes.size(); ++i) {
    Attribute* attr = attributes[i];

    auto attrItem = new QTreeWidgetItem(parentItem);

    QString itemLabel;
    if (propertyPrefix == "attr") {
      itemLabel = QString("Atributo %1").arg(i + 1);
    }
    else if (propertyPrefix == "subattr") {
      itemLabel = QString("Sub-Atributo %1").arg(i + 1);
    }

    attrItem->setText(0, itemLabel);
    attrItem->setText(1, attr->name());
    attrItem->setData(0, Qt::UserRole, QString("%1_%2").arg(propertyPrefix).arg(i));
    attrItem->setFlags(attrItem->flags() & ~Qt::ItemIsEditable);

    createPropertyItem(attrItem, "Nome", attr->name(), QString("%1_%2_name").arg(propertyPrefix).arg(i));

    QStringList attributeTypes = AttributeType::getAllAttributeTypeStrings();
    QString currentType = AttributeType::attributeTypeToString(attr->attributeType());
    createComboBoxPropertyItem(attrItem, "Tipo", attributeTypes, currentType, QString("%1_%2_type").arg(propertyPrefix).arg(i));

    QString removeKey;
    if (propertyPrefix == "attr") {
      removeKey = QString("removeAttribute_%1").arg(i);
    }
    else if (propertyPrefix == "subattr") {
      removeKey = QString("removeSubAttribute_%1").arg(i);
    }

    createButtonPropertyItem(attrItem, "Remover", "-", removeKey);
  }
}

//----------------------------------------------------------------------------------------------


QTreeWidgetItem* MainWindow::createPropertyItem(
    QTreeWidgetItem* parent,
    const QString& propertyName,
    const QVariant& value,
    const QString& propertyKey,
    bool editable
)
{
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, propertyName);
    item->setText(1, value.toString());
    item->setData(0, Qt::UserRole, propertyKey);
    
    if (editable) {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    } else {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
    
    return item;
}

//----------------------------------------------------------------------------------------------

QTreeWidgetItem* MainWindow::createComboBoxPropertyItem(
    QTreeWidgetItem* parent,
    const QString& propertyName,
    const QStringList& options,
    const QString& currentValue,
    const QString& propertyKey
)
{
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, propertyName);
    item->setText(1, currentValue);
    item->setData(0, Qt::UserRole, propertyKey);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    auto comboBox = new QComboBox();
    comboBox->addItems(options);
    comboBox->setCurrentText(currentValue);
    comboBox->setProperty("propertyKey", propertyKey);
    
    connect(comboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &MainWindow::onComboBoxChanged);
    
    m_propertiesTree->setItemWidget(item, 1, comboBox);
    m_propertyWidgets[propertyKey] = comboBox;
    
    return item;
}

//----------------------------------------------------------------------------------------------

void MainWindow::populatePropertiesForConnection(
  ConnectionLine* connection
)
{
  if (!connection || !m_propertiesTree) {
    return;
  }

  auto appearanceGroup = new QTreeWidgetItem(m_propertiesTree);
  appearanceGroup->setText(0, "APARÊNCIA");
  appearanceGroup->setExpanded(true);
  appearanceGroup->setFlags(appearanceGroup->flags() & ~Qt::ItemIsEditable);

  QStringList lineTypes;
  lineTypes << "Straight" << "Orthogonal" << "Bezier";
  QString currentType = ConnectionLine::lineTypeToString(connection->lineType());

  QList<QIcon> lineTypeIcons = getConnectionLineTypeIcons();

  createIconComboBoxPropertyItem(
    appearanceGroup,
    "Tipo de Linha",
    lineTypes,
    lineTypeIcons,
    currentType,
    "connectionLineType"
  );

  createPropertyItem(
    appearanceGroup,
    "Largura da Linha",
    QString::number(connection->lineWidth(), 'f', 1),
    "connectionLineWidth"
  );
}

//----------------------------------------------------------------------------------------------

QTreeWidgetItem* MainWindow::createIconComboBoxPropertyItem(
  QTreeWidgetItem* parent,
  const QString& propertyName,
  const QStringList& options,
  const QList<QIcon>& icons,
  const QString& currentValue,
  const QString& propertyKey
)
{
  auto item = new QTreeWidgetItem(parent);
  item->setText(0, propertyName);
  item->setText(1, currentValue);
  item->setData(0, Qt::UserRole, propertyKey);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  auto comboBox = new QComboBox();

  for (int i = 0; i < options.size(); ++i) {
    if (i < icons.size()) {
      comboBox->addItem(icons[i], options[i]);
    }
    else {
      comboBox->addItem(options[i]);
    }
  }

  comboBox->setCurrentText(currentValue);
  comboBox->setProperty("propertyKey", propertyKey);
  comboBox->setIconSize(QSize(48, 24));

  connect(
    comboBox,
    QOverload<const QString&>::of(&QComboBox::currentTextChanged),
    this,
    &MainWindow::onComboBoxChanged
  );

  m_propertiesTree->setItemWidget(item, 1, comboBox);
  m_propertyWidgets[propertyKey] = comboBox;

  return item;
}

//----------------------------------------------------------------------------------------------

QList<QIcon> MainWindow::getConnectionLineTypeIcons()
{
  QList<QIcon> icons;

  QPixmap straightPixmap(48, 24);
  straightPixmap.fill(Qt::transparent);
  QPainter straightPainter(&straightPixmap);
  straightPainter.setRenderHint(QPainter::Antialiasing);
  straightPainter.setPen(QPen(QColor(51, 51, 51), 2));
  straightPainter.drawLine(4, 12, 44, 12);
  icons.append(QIcon(straightPixmap));

  QPixmap orthogonalPixmap(48, 24);
  orthogonalPixmap.fill(Qt::transparent);
  QPainter orthogonalPainter(&orthogonalPixmap);
  orthogonalPainter.setRenderHint(QPainter::Antialiasing);
  orthogonalPainter.setPen(QPen(QColor(51, 51, 51), 2));
  QPainterPath orthogonalPath;
  orthogonalPath.moveTo(4, 12);
  orthogonalPath.lineTo(20, 12);
  orthogonalPath.lineTo(20, 18);
  orthogonalPath.lineTo(44, 18);
  orthogonalPainter.drawPath(orthogonalPath);
  icons.append(QIcon(orthogonalPixmap));

  QPixmap bezierPixmap(48, 24);
  bezierPixmap.fill(Qt::transparent);
  QPainter bezierPainter(&bezierPixmap);
  bezierPainter.setRenderHint(QPainter::Antialiasing);
  bezierPainter.setPen(QPen(QColor(51, 51, 51), 2));
  QPainterPath bezierPath;
  bezierPath.moveTo(4, 12);
  bezierPath.cubicTo(16, 4, 32, 20, 44, 12);
  bezierPainter.drawPath(bezierPath);
  icons.append(QIcon(bezierPixmap));

  return icons;
}

//----------------------------------------------------------------------------------------------

void MainWindow::onPropertyValueChanged(
  QTreeWidgetItem* item,
  int column
)
{
  if (column != 1 || !item || !m_diagramScene) {
    return;
  }

  QString propertyType = item->data(0, Qt::UserRole).toString();

  if (propertyType == "connectionLineWidth") {
    QList<ConnectionLine*> selectedConnections = m_diagramScene->getSelectedConnections();
    if (selectedConnections.size() == 1) {
      ConnectionLine* connection = selectedConnections.first();
      bool ok;
      qreal newWidth = item->text(1).toDouble(&ok);

      if (ok && newWidth > 0.0 && newWidth <= 20.0) {
        connection->setLineWidth(newWidth);

        m_isModified = true;
        updateWindowTitle();
        updateStatusBar("Largura da linha atualizada: " + QString::number(newWidth, 'f', 1));
        return;
      }
      else {
        updateStatusBar("Largura inválida. Use valores entre 0.1 e 20.0");
        updatePropertiesPanel();
        return;
      }
    }
  }

  QList<BasicElement*> selectedElements = m_diagramScene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  BasicElement* element = selectedElements.first();
  if (!element) {
    return;
  }

  QString newValue = item->text(1);

  if (handleAttributePropertyChange(element, propertyType, newValue)) {
    return;
  }

  bool success = PropertyCommand::setElementProperty(element, propertyType, newValue);

  if (success) {
    m_isModified = true;
    updateWindowTitle();
    updateStatusBar("Propriedade atualizada: " + item->text(0));
    updatePropertiesPanel();

    if (ElementGraphicsItem* graphicsItem = m_diagramScene->findGraphicsItem(element)) {
      graphicsItem->update();
    }
  }
  else {
    updateStatusBar("Erro ao atualizar propriedade: " + item->text(0));
    updatePropertiesPanel();
  }
}

//----------------------------------------------------------------------------------------------

bool MainWindow::handleAttributePropertyChange(
  BasicElement* element,
  const QString& propertyType,
  const QString& newValue
)
{
  if (!propertyType.endsWith("_name")) {
    return false;
  }

  QStringList parts = propertyType.split("_");
  if (parts.size() < 3) {
    return false;
  }

  QString prefix = parts[0];
  int index = parts[1].toInt();

  if (prefix == "attr") {
    if (auto entity = qobject_cast<Entity*>(element)) {
      QList<Attribute*> attributes = entity->getAttributes();
      if (index >= 0 && index < attributes.size()) {
        attributes[index]->setName(newValue);
        m_isModified = true;
        updateWindowTitle();
        updateStatusBar("Nome do atributo atualizado");
        updatePropertiesPanel();
        if (ElementGraphicsItem* graphicsItem = m_diagramScene->findGraphicsItem(element)) {
          graphicsItem->update();
        }
        return true;
      }
    }
  }
  else if (prefix == "subattr") {
    auto attribute = qobject_cast<Attribute*>(element);
    if (attribute && attribute->isCompositeAttribute()) {
      QList<Attribute*> subAttributes = attribute->getSubAttributes();
      if (index >= 0 && index < subAttributes.size()) {
        Attribute* subAttr = subAttributes[index];
        subAttr->setName(newValue);
        m_isModified = true;
        updateWindowTitle();
        updateStatusBar("Nome do sub-atributo atualizado");
        updatePropertiesPanel();
        if (ElementGraphicsItem* graphicsItem = m_diagramScene->findGraphicsItem(subAttr)) {
          graphicsItem->update();
        }
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------

void MainWindow::onComboBoxChanged(
  const QString& value
)
{
  auto comboBox = qobject_cast<QComboBox*>(sender());
  if (!comboBox || !m_diagramScene) {
    return;
  }

  QString propertyKey = comboBox->property("propertyKey").toString();

  if (propertyKey == "connectionLineType") {
    QList<ConnectionLine*> selectedConnections = m_diagramScene->getSelectedConnections();
    if (selectedConnections.size() == 1) {
      ConnectionLine* connection = selectedConnections.first();
      ConnectionLineType newType = ConnectionLine::lineTypeFromString(value);
      connection->setLineType(newType);

      m_isModified = true;
      updateWindowTitle();
      updateStatusBar("Tipo de linha atualizado: " + value);
      updatePropertyItemText(propertyKey, value);
      return;
    }
  }

  QList<BasicElement*> selectedElements = m_diagramScene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  BasicElement* element = selectedElements.first();

  if (handleAttributeTypeChange(element, propertyKey, value)) {
    return;
  }

  bool success = PropertyCommand::setElementProperty(element, propertyKey, value);

  if (success) {
    m_isModified = true;
    updateWindowTitle();
    updateStatusBar("Propriedade atualizada: " + propertyKey);

    updatePropertyItemText(propertyKey, value);
    updatePropertiesPanel();

    if (ElementGraphicsItem* graphicsItem = m_diagramScene->findGraphicsItem(element)) {
      graphicsItem->update();
    }
  }
  else {
    updateStatusBar("Erro ao atualizar propriedade: " + propertyKey);
    updatePropertiesPanel();
  }
}

//----------------------------------------------------------------------------------------------

bool MainWindow::handleAttributeTypeChange(
  BasicElement* element,
  const QString& propertyKey,
  const QString& value
)
{
  if (!propertyKey.endsWith("_type")) {
    return false;
  }

  QStringList parts = propertyKey.split("_");
  if (parts.size() < 3) {
    return false;
  }

  QString prefix = parts[0];
  int index = parts[1].toInt();
  AttributeType::Type newType = AttributeType::attributeTypeFromString(value);

  if (prefix == "attr") {
    auto entity = qobject_cast<Entity*>(element);
    if (entity) {
      QList<Attribute*> attributes = entity->getAttributes();
      if (index >= 0 && index < attributes.size()) {
        attributes[index]->setAttributeType(newType);
        m_isModified = true;
        updateWindowTitle();
        updateStatusBar("Tipo do atributo atualizado");
        updatePropertiesPanel();
        if (ElementGraphicsItem* graphicsItem = m_diagramScene->findGraphicsItem(element)) {
          graphicsItem->update();
        }
        return true;
      }
    }
  }
  else if (prefix == "subattr") {
    auto attribute = qobject_cast<Attribute*>(element);
    if (attribute && attribute->isCompositeAttribute()) {
      QList<Attribute*> subAttributes = attribute->getSubAttributes();
      if (index >= 0 && index < subAttributes.size()) {
        Attribute* subAttr = subAttributes[index];
        subAttr->setAttributeType(newType);
        m_isModified = true;
        updateWindowTitle();
        updateStatusBar("Tipo do sub-atributo atualizado");
        updatePropertiesPanel();
        if (ElementGraphicsItem* graphicsItem = m_diagramScene->findGraphicsItem(subAttr)) {
          graphicsItem->update();
        }
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------

void MainWindow::onCheckBoxChanged(
    bool checked
)
{
    auto checkBox = qobject_cast<QCheckBox*>(sender());
    if (!checkBox || !m_diagramScene) {
        return;
    }

    QString propertyKey = checkBox->property("propertyKey").toString();

    QList<BasicElement*> selectedElements = m_diagramScene->getSelectedElements();
    if (selectedElements.size() != 1) {
        return;
    }

    BasicElement* element = selectedElements.first();

    bool success = PropertyCommand::setElementProperty(element, propertyKey, checked);

    if (success) {
        m_isModified = true;
        updateWindowTitle();
        updateStatusBar("Propriedade atualizada: " + propertyKey);
    }
    else {
        updateStatusBar("Erro ao atualizar propriedade: " + propertyKey);
        updatePropertiesPanel();
    }
}

//----------------------------------------------------------------------------------------------

void MainWindow::onLineEditChanged(
    const QString& text
)
{
    auto lineEdit = qobject_cast<QLineEdit*>(sender());
    if (!lineEdit || !m_diagramScene) {
        return;
    }

    QString propertyKey = lineEdit->property("propertyKey").toString();

    QList<BasicElement*> selectedElements = m_diagramScene->getSelectedElements();
    if (selectedElements.size() != 1) {
        return;
    }

    BasicElement* element = selectedElements.first();

    bool success = PropertyCommand::setElementProperty(element, propertyKey, text);

    if (success) {
        m_isModified = true;
        updateWindowTitle();
        updateStatusBar("Propriedade atualizada: " + propertyKey);
    }
    else {
        updateStatusBar("Erro ao atualizar propriedade: " + propertyKey);
        updatePropertiesPanel();
    }
}

//----------------------------------------------------------------------------------------------

void MainWindow::updatePropertyItemText(
    const QString& propertyKey,
    const QString& value
)
{
    for (int i = 0; i < m_propertiesTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* group = m_propertiesTree->topLevelItem(i);
        for (int j = 0; j < group->childCount(); ++j) {
            QTreeWidgetItem* child = group->child(j);
            if (child->data(0, Qt::UserRole).toString() == propertyKey) {
                child->setText(1, value);
                return;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------

void MainWindow::onAddEntityClicked()
{
    if (!m_diagramScene) {
        return;
    }

    QPointF pos =  QPointF(0, 0);

    auto entity = new Entity("Entidade");

	m_diagramScene->addElement(entity);
    if (entity) {
        m_isModified = true;
        updateWindowTitle();
        updateStatusBar("Entidade adicionada");
    } else {
        updateStatusBar("Falha ao adicionar entidade");
    }
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::onAddAttributeClicked()
{
    if (!m_diagramScene) {
        return;
    }

    QPointF pos = QPointF(0, 0);

    auto attribute = new Attribute("Atributo");

    m_diagramScene->addElement(attribute);
    if (attribute) {
        m_isModified = true;
        updateWindowTitle();
        updateStatusBar("Atributo adicionado");
    }
    else {
        updateStatusBar("Falha ao adicionar atributo");
    }
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::onAddRelationshipClicked()
{
  if (!m_diagramScene) {
    return;
  }

  QPointF pos = QPointF(0, 0);

  auto relationship = new Relationship("Relacionamento");

  m_diagramScene->addElement(relationship);
  if (relationship) {
    m_isModified = true;
    updateWindowTitle();
    updateStatusBar("Atributo adicionado");
  }
  else {
    updateStatusBar("Falha ao adicionar atributo");
  }
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::showAbout()
{
    QMessageBox::about(this, "Sobre DER Designer",
        "<h3>DER Designer</h3>"
        "<p>Thaissa Vitoria G.D. de Sousa, Prof.Dr. Andre L. Maravilha Silva</p>"
        "<p>Ferramenta para criação de Diagramas Entidade-Relacionamento "
        "baseados na notação de Peter Chen.</p>"
        "<p>Desenvolvido com Qt e C++.</p>"
        "<p>Versão 1.0</p>");
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::showHelp()
{
    QMessageBox::information(this, "Ajuda",
        "<h3>Como usar o DER Designer</h3>"
        "<p><b>Aba Desenho:</b> Selecione os elementos para adicionar ao diagrama.</p>"
        "<p><b>Aba Propriedades:</b> Visualize e edite propriedades do elemento selecionado.</p>"
        "<p><b>Área de Desenho:</b> Clique e arraste para criar e posicionar elementos.</p>"
        "<p><b>Menu Arquivo:</b> Criar, abrir, salvar e exportar diagramas.</p>");
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::onTabChanged(int index)
{
    QString tabName = m_sideTabWidget->tabText(index);
    updateStatusBar("Aba ativa: " + tabName);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::onTabCloseRequested(int index)
{
    Q_UNUSED(index)
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::updateWindowTitle()
{
    QString title = "DER Designer";
    if (!m_currentFileName.isEmpty()) {
        title += " - " + QFileInfo(m_currentFileName).baseName();
    }
    if (m_isModified) {
        title += " *";
    }
    setWindowTitle(title);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::updateStatusBar(const QString& message)
{
    m_statusLabel->setText(message);
}

// -----------------------------------------------------------------------------------------------------
