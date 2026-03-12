#include "PropertiesPanel.h"

#include "view/DiagramScene.h"
#include "view/ElementGraphicsItem.h"
#include "model/Attribute.h"
#include "model/AttributeType.h"
#include "model/ElementFactory.h"
#include "model/Entity.h"
#include "model/PropertyCommand.h"
#include "model/Relationship.h"
#include "model/RelationshipEnd.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QTreeWidgetItem>

//----------------------------------------------------------------------------------------------

PropertiesPanel::PropertiesPanel(
  DiagramScene* scene,
  QWidget* parent
)
  : QWidget(parent)
  , m_scene(scene)
  , m_layout(new QVBoxLayout(this))
  , m_tree(new QTreeWidget())
{
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->setSpacing(0);

  m_tree->setHeaderLabels(QStringList() << "Propriedade" << "Valor");
  m_tree->setAlternatingRowColors(true);
  m_tree->setRootIsDecorated(false);
  m_layout->addWidget(m_tree);

  connect(
    m_tree,
    &QTreeWidget::itemChanged,
    this,
    &PropertiesPanel::onPropertyValueChanged
  );
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::refresh(
  const QList<BasicElement*>& selectedElements,
  const QSet<ConnectionLine*>& selectedConnections
)
{
  disconnect(m_tree, &QTreeWidget::itemChanged, this, &PropertiesPanel::onPropertyValueChanged);

  clearPanel();

  if (selectedConnections.size() == 1 && selectedElements.isEmpty()) {
    populateForConnection(*selectedConnections.begin());
  }
  else if (selectedElements.size() == 1 && selectedConnections.isEmpty()) {
    populateForElement(selectedElements.first());
  }
  else if (selectedElements.size() > 1 || selectedConnections.size() > 1) {
    auto multiSelectItem = new QTreeWidgetItem(m_tree);
    multiSelectItem->setText(0, "Seleção Múltipla");
    int totalSelected = selectedElements.size() + selectedConnections.size();
    multiSelectItem->setText(1, QString("%1 itens selecionados").arg(totalSelected));
    multiSelectItem->setFlags(multiSelectItem->flags() & ~Qt::ItemIsEditable);
  }

  connect(
    m_tree,
    &QTreeWidget::itemChanged,
    this,
    &PropertiesPanel::onPropertyValueChanged
  );

  m_tree->expandAll();
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::clearPanel()
{
  m_tree->clear();

  for (auto& pair : m_propertyWidgets) {
    pair.second->deleteLater();
  }
  m_propertyWidgets.clear();
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::populateForElement(
  BasicElement* element
)
{
  if (!element) {
    return;
  }

  auto basicGroup = new QTreeWidgetItem(m_tree);
  basicGroup->setText(0, "GERAL");
  basicGroup->setExpanded(true);
  basicGroup->setFlags(basicGroup->flags() & ~Qt::ItemIsEditable);

  createPropertyItem(basicGroup, "Nome", element->name(), "name");
  createPropertyItem(basicGroup, "Tipo", element->typeDisplayName(), "type", false);

  if (qobject_cast<Attribute*>(element)) {
    populateAttributeProperties(qobject_cast<Attribute*>(element), nullptr);
  }

  if (qobject_cast<Entity*>(element)) {
    populateEntityProperties(element);
  }

  if (qobject_cast<Relationship*>(element)) {
    populateRelationshipProperties(element);
  }

  auto geometryGroup = new QTreeWidgetItem(m_tree);
  geometryGroup->setText(0, "GEOMETRIA");
  geometryGroup->setExpanded(true);
  geometryGroup->setFlags(geometryGroup->flags() & ~Qt::ItemIsEditable);

  createPropertyItem(geometryGroup, "X", QString::number(element->position().x(), 'f', 2), "posX");
  createPropertyItem(geometryGroup, "Y", QString::number(element->position().y(), 'f', 2), "posY");
  createPropertyItem(geometryGroup, "Largura", QString::number(element->size().width(), 'f', 2), "width");
  createPropertyItem(geometryGroup, "Altura", QString::number(element->size().height(), 'f', 2), "height");
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::populateForConnection(
  ConnectionLine* connection
)
{
  if (!connection) {
    return;
  }

  auto appearanceGroup = new QTreeWidgetItem(m_tree);
  appearanceGroup->setText(0, "APARÊNCIA");
  appearanceGroup->setExpanded(true);
  appearanceGroup->setFlags(appearanceGroup->flags() & ~Qt::ItemIsEditable);

  QStringList lineTypes;
  lineTypes << "Straight" << "Orthogonal" << "Bezier";
  QString currentType = ConnectionLine::lineTypeToString(connection->lineType());

  createIconComboBoxPropertyItem(
    appearanceGroup,
    "Tipo de Linha",
    lineTypes,
    getConnectionLineTypeIcons(),
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

void PropertiesPanel::populateEntityProperties(
  BasicElement* element
)
{
  auto entity = qobject_cast<Entity*>(element);
  if (!entity) {
    return;
  }

  auto attributesGroup = new QTreeWidgetItem(m_tree);
  attributesGroup->setText(0, "ATRIBUTOS");
  attributesGroup->setExpanded(true);
  attributesGroup->setFlags(attributesGroup->flags() & ~Qt::ItemIsEditable);

  createButtonPropertyItem(attributesGroup, "Adicionar Atributo", "+", "addAttribute");

  QList<Attribute*> attributes = getAttributesFromIds(entity->getAttributeIds());
  populateAttributeList(attributesGroup, attributes, "attribute");
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::populateAttributeProperties(
  Attribute* attribute,
  QTreeWidgetItem* parent
)
{
  if (!attribute) {
    return;
  }

  QTreeWidgetItem* attributeGroup;
  if (parent) {
    attributeGroup = parent;
  }
  else {
    attributeGroup = new QTreeWidgetItem(m_tree);
    attributeGroup->setText(0, "ATRIBUTO");
    attributeGroup->setExpanded(true);
    attributeGroup->setFlags(attributeGroup->flags() & ~Qt::ItemIsEditable);
  }

  QStringList attributeTypes = AttributeType::getAllAttributeTypeStrings();
  QString currentType = AttributeType::attributeTypeToString(attribute->attributeType());

  QString typePropertyKey = parent ?
    QString("%1_attributeType").arg(parent->data(0, Qt::UserRole).toString()) :
    "attributeType";

  createComboBoxPropertyItem(attributeGroup, "Tipo de Atributo", attributeTypes, currentType, typePropertyKey);

  if (attribute->isNormalAttribute() || attribute->isCompositeAttribute()) {
    QString keyPropertyKey = parent ?
      QString("%1_isPrimaryKey").arg(parent->data(0, Qt::UserRole).toString()) :
      "isPrimaryKey";

    createCheckBoxPropertyItem(attributeGroup, "Chave Primária", attribute->isPrimaryKey(), keyPropertyKey);
  }

  if (attribute->isCompositeAttribute() && !parent) {
    auto subAttributesGroup = new QTreeWidgetItem(attributeGroup);
    subAttributesGroup->setText(0, "SUB-ATRIBUTOS");
    subAttributesGroup->setExpanded(true);
    subAttributesGroup->setFlags(subAttributesGroup->flags() & ~Qt::ItemIsEditable);

    createButtonPropertyItem(subAttributesGroup, "Adicionar Sub-Atributo", "+", "addAttribute");

    QList<Attribute*> subAttributes = getAttributesFromIds(attribute->getAttributeIds());
    populateAttributeList(subAttributesGroup, subAttributes, "attribute");
  }
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::populateRelationshipProperties(
  BasicElement* element
)
{
  auto relationship = qobject_cast<Relationship*>(element);
  if (!relationship) {
    return;
  }

  auto relationshipGroup = new QTreeWidgetItem(m_tree);
  relationshipGroup->setText(0, "RELACIONAMENTO");
  relationshipGroup->setExpanded(true);
  relationshipGroup->setFlags(relationshipGroup->flags() & ~Qt::ItemIsEditable);

  QList<RelationshipEnd*> ends = relationship->ends();

  if (ends.isEmpty()) {
    auto noConnectionsItem = new QTreeWidgetItem(relationshipGroup);
    noConnectionsItem->setText(0, "Nenhuma conexão");
    noConnectionsItem->setText(1, "Conecte entidades ao relacionamento");
    noConnectionsItem->setFlags(noConnectionsItem->flags() & ~Qt::ItemIsEditable);
  }
  else {
    for (int i = 0; i < ends.size(); ++i) {
      RelationshipEnd* end = ends[i];

      auto endItem = new QTreeWidgetItem(relationshipGroup);

      BasicElement* entity = m_scene->findElement(end->entityId());
      QString entityName = entity ? entity->name() : "";

      endItem->setText(0, QString("Conexão %1").arg(i + 1));
      endItem->setText(1, entityName);
      endItem->setData(0, Qt::UserRole, QString("relend_%1").arg(i));
      endItem->setFlags(endItem->flags() & ~Qt::ItemIsEditable);

      QStringList cardinalityOptions;
      cardinalityOptions << "Um" << "Muitos";

      createComboBoxPropertyItem(
        endItem,
        "Cardinalidade",
        cardinalityOptions,
        RelationshipEnd::cardinalityToString(end->cardinality()),
        QString("relend_%1_cardinality").arg(i)
      );

      if (end->cardinality() == Cardinality::Many) {
        QString customText = end->customCardinalityText().isEmpty() ? "M" : end->customCardinalityText();
        createLineEditPropertyItem(
          endItem,
          "Texto Cardinalidade",
          customText,
          QString("relend_%1_cardinalitytext").arg(i)
        );
      }

      createCheckBoxPropertyItem(
        endItem,
        "Participação Total",
        end->isTotalParticipation(),
        QString("relend_%1_participation").arg(i)
      );
    }
  }

  auto attributesGroup = new QTreeWidgetItem(m_tree);
  attributesGroup->setText(0, "ATRIBUTOS");
  attributesGroup->setExpanded(true);
  attributesGroup->setFlags(attributesGroup->flags() & ~Qt::ItemIsEditable);

  createButtonPropertyItem(attributesGroup, "Adicionar Atributo", "+", "addAttribute");

  QList<Attribute*> attributes = getAttributesFromIds(relationship->getAttributeIds());
  populateAttributeList(attributesGroup, attributes, "attribute");
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::populateAttributeList(
  QTreeWidgetItem* parentItem,
  const QList<Attribute*>& attributes,
  const QString& propertyPrefix
)
{
  for (int i = 0; i < attributes.size(); ++i) {
    Attribute* attr = attributes[i];

    auto attrItem = new QTreeWidgetItem(parentItem);
    attrItem->setText(0, QString("Atributo %1").arg(i + 1));
    attrItem->setText(1, attr->name());
    attrItem->setData(0, Qt::UserRole, QString("%1_%2").arg(propertyPrefix).arg(i));
    attrItem->setFlags(attrItem->flags() & ~Qt::ItemIsEditable);

    createPropertyItem(attrItem, "Nome", attr->name(), QString("%1_%2_name").arg(propertyPrefix).arg(i));

    populateAttributeProperties(attr, attrItem);

    createButtonPropertyItem(attrItem, "Remover", "-", QString("removeAttribute_%1").arg(i));
  }
}

//----------------------------------------------------------------------------------------------

QList<Attribute*> PropertiesPanel::getAttributesFromIds(
  const QList<QString>& attributeIds
)
{
  QList<Attribute*> attributes;
  for (const QString& attrId : attributeIds) {
    if (BasicElement* elem = m_scene->findElement(attrId)) {
      if (auto attr = qobject_cast<Attribute*>(elem)) {
        attributes.append(attr);
      }
    }
  }
  return attributes;
}

//----------------------------------------------------------------------------------------------

QTreeWidgetItem* PropertiesPanel::createPropertyItem(
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
  }
  else {
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  }

  return item;
}

//----------------------------------------------------------------------------------------------

QTreeWidgetItem* PropertiesPanel::createComboBoxPropertyItem(
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

  connect(
    comboBox,
    QOverload<const QString&>::of(&QComboBox::currentTextChanged),
    this,
    &PropertiesPanel::onComboBoxChanged
  );

  m_tree->setItemWidget(item, 1, comboBox);
  m_propertyWidgets[propertyKey] = comboBox;

  return item;
}

//----------------------------------------------------------------------------------------------

QTreeWidgetItem* PropertiesPanel::createIconComboBoxPropertyItem(
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
    &PropertiesPanel::onComboBoxChanged
  );

  m_tree->setItemWidget(item, 1, comboBox);
  m_propertyWidgets[propertyKey] = comboBox;

  return item;
}

//----------------------------------------------------------------------------------------------

QTreeWidgetItem* PropertiesPanel::createCheckBoxPropertyItem(
  QTreeWidgetItem* parent,
  const QString& propertyName,
  bool currentValue,
  const QString& propertyKey
)
{
  auto item = new QTreeWidgetItem(parent);
  item->setText(0, propertyName);
  item->setText(1, currentValue ? "Sim" : "Não");
  item->setData(0, Qt::UserRole, propertyKey);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  auto checkBox = new QCheckBox();
  checkBox->setChecked(currentValue);
  checkBox->setProperty("propertyKey", propertyKey);

  connect(
    checkBox,
    &QCheckBox::toggled,
    this,
    &PropertiesPanel::onCheckBoxChanged
  );

  m_tree->setItemWidget(item, 1, checkBox);
  m_propertyWidgets[propertyKey] = checkBox;

  return item;
}

//----------------------------------------------------------------------------------------------

QTreeWidgetItem* PropertiesPanel::createLineEditPropertyItem(
  QTreeWidgetItem* parent,
  const QString& propertyName,
  const QString& currentValue,
  const QString& propertyKey
)
{
  auto item = new QTreeWidgetItem(parent);
  item->setText(0, propertyName);
  item->setText(1, currentValue);
  item->setData(0, Qt::UserRole, propertyKey);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  auto lineEdit = new QLineEdit();
  lineEdit->setText(currentValue);
  lineEdit->setProperty("propertyKey", propertyKey);
  lineEdit->setMaxLength(1);

  connect(
    lineEdit,
    &QLineEdit::textChanged,
    this,
    &PropertiesPanel::onLineEditChanged
  );

  m_tree->setItemWidget(item, 1, lineEdit);
  m_propertyWidgets[propertyKey] = lineEdit;

  return item;
}

//----------------------------------------------------------------------------------------------

QTreeWidgetItem* PropertiesPanel::createButtonPropertyItem(
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
    connect(button, &QPushButton::clicked, this, &PropertiesPanel::onAddAttributeOnTreeClicked);
  }
  else if (propertyKey.startsWith("removeAttribute_")) {
    connect(button, &QPushButton::clicked, this, &PropertiesPanel::onRemoveAttributeOnTreeClicked);
  }

  m_tree->setItemWidget(item, 1, button);
  m_propertyWidgets[propertyKey] = button;

  return item;
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::updatePropertyItemText(
  const QString& propertyKey,
  const QString& value
)
{
  for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
    QTreeWidgetItem* group = m_tree->topLevelItem(i);
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

void PropertiesPanel::updatesAfterChangingProperty(
  BasicElement* element
)
{
  emit diagramModified();

  if (ElementGraphicsItem* graphicsItem = m_scene->findGraphicsItem(element)) {
    graphicsItem->update();
  }

  QList<BasicElement*> selectedElements = m_scene->getSelectedElements();
  QSet<ConnectionLine*> selectedConnections = m_scene->getSelectedConnections();
  refresh(selectedElements, selectedConnections);
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::onPropertyValueChanged(
  QTreeWidgetItem* item,
  int column
)
{
  if (column != 1 || !item || !m_scene) {
    return;
  }

  QString propertyType = item->data(0, Qt::UserRole).toString();

  if (propertyType == "connectionLineWidth") {
    QSet<ConnectionLine*> selectedConnections = m_scene->getSelectedConnections();
    if (selectedConnections.size() == 1) {
      ConnectionLine* connection = *selectedConnections.begin();
      bool ok;
      qreal newWidth = item->text(1).toDouble(&ok);

      if (ok && newWidth > 0.0 && newWidth <= 20.0) {
        connection->setLineWidth(newWidth);
        emit diagramModified();
        emit statusMessageRequested("Largura da linha atualizada: " + QString::number(newWidth, 'f', 1));
      }
      else {
        emit statusMessageRequested("Largura inválida. Use valores entre 0.1 e 20.0");
        QList<BasicElement*> sel = m_scene->getSelectedElements();
        refresh(sel, selectedConnections);
      }
    }
    return;
  }

  QList<BasicElement*> selectedElements = m_scene->getSelectedElements();
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
    emit statusMessageRequested("Propriedade atualizada: " + item->text(0));
    updatesAfterChangingProperty(element);
  }
  else {
    emit statusMessageRequested("Erro ao atualizar propriedade: " + item->text(0));
    QSet<ConnectionLine*> selConn = m_scene->getSelectedConnections();
    refresh(selectedElements, selConn);
  }
}

//----------------------------------------------------------------------------------------------

bool PropertiesPanel::handleAttributePropertyChange(
  BasicElement* element,
  const QString& propertyType,
  const QVariant& newValue
)
{
  QStringList parts = propertyType.split("_");
  if (parts.size() < 3) {
    return false;
  }

  int index = parts[1].toInt();

  QList<QString> attributeIds = element->getAttributeIds();
  if (index >= 0 && index < attributeIds.size()) {
    if (BasicElement* attrElem = m_scene->findElement(attributeIds[index])) {
      auto attribute = qobject_cast<Attribute*>(attrElem);
      PropertyCommand::setElementProperty(attribute, parts[2], newValue);
      updatesAfterChangingProperty(attribute);
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::onComboBoxChanged(
  const QString& value
)
{
  auto comboBox = qobject_cast<QComboBox*>(sender());
  if (!comboBox || !m_scene) {
    return;
  }

  QString propertyKey = comboBox->property("propertyKey").toString();

  if (propertyKey == "connectionLineType") {
    QSet<ConnectionLine*> selectedConnections = m_scene->getSelectedConnections();
    if (selectedConnections.size() == 1) {
      ConnectionLine* connection = *selectedConnections.begin();
      connection->setLineType(ConnectionLine::lineTypeFromString(value));
      emit diagramModified();
      emit statusMessageRequested("Tipo de linha atualizado: " + value);
      updatePropertyItemText(propertyKey, value);
    }
    return;
  }

  QList<BasicElement*> selectedElements = m_scene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  BasicElement* element = selectedElements.first();

  if (handleAttributePropertyChange(element, propertyKey, value)) {
    return;
  }

  if (handleRelationshipEndCardinalityChange(element, propertyKey, value)) {
    return;
  }

  bool success = PropertyCommand::setElementProperty(element, propertyKey, value);

  if (success) {
    emit statusMessageRequested("Propriedade atualizada: " + propertyKey);
    updatePropertyItemText(propertyKey, value);
    updatesAfterChangingProperty(element);
  }
  else {
    emit statusMessageRequested("Erro ao atualizar propriedade: " + propertyKey);
    QSet<ConnectionLine*> selConn = m_scene->getSelectedConnections();
    refresh(selectedElements, selConn);
  }
}

//----------------------------------------------------------------------------------------------

bool PropertiesPanel::handleRelationshipEndCardinalityChange(
  BasicElement* element,
  const QString& propertyKey,
  const QString& value
)
{
  if (!propertyKey.contains("relend_") || !propertyKey.endsWith("_cardinality")) {
    return false;
  }

  auto relationship = qobject_cast<Relationship*>(element);
  if (!relationship) {
    return false;
  }

  QStringList parts = propertyKey.split("_");
  if (parts.size() < 3) {
    return false;
  }

  int index = parts[1].toInt();
  QList<RelationshipEnd*> ends = relationship->ends();

  if (index >= 0 && index < ends.size()) {
    RelationshipEnd* end = ends[index];
    end->setCardinality(RelationshipEnd::cardinalityFromString(value));
    emit statusMessageRequested("Cardinalidade atualizada: " + value);
    updatesAfterChangingProperty(relationship);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::onCheckBoxChanged(
  bool checked
)
{
  auto checkBox = qobject_cast<QCheckBox*>(sender());
  if (!checkBox || !m_scene) {
    return;
  }

  QString propertyKey = checkBox->property("propertyKey").toString();

  QList<BasicElement*> selectedElements = m_scene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  BasicElement* element = selectedElements.first();

  if (handleAttributePropertyChange(element, propertyKey, checked)) {
    return;
  }

  if (propertyKey == "isPrimaryKey") {
    if (auto attribute = qobject_cast<Attribute*>(element)) {
      attribute->setPrimaryKey(checked);
      emit statusMessageRequested(QString("Chave primária: %1").arg(checked ? "Sim" : "Não"));
      updatesAfterChangingProperty(element);
      return;
    }
  }

  if (handleRelationshipEndParticipationChange(element, propertyKey, checked)) {
    return;
  }

  bool success = PropertyCommand::setElementProperty(element, propertyKey, checked);

  if (success) {
    emit statusMessageRequested("Propriedade atualizada: " + propertyKey);
    updatesAfterChangingProperty(element);
  }
  else {
    emit statusMessageRequested("Erro ao atualizar propriedade: " + propertyKey);
    QSet<ConnectionLine*> selConn = m_scene->getSelectedConnections();
    refresh(selectedElements, selConn);
  }
}

//----------------------------------------------------------------------------------------------

bool PropertiesPanel::handleRelationshipEndParticipationChange(
  BasicElement* element,
  const QString& propertyKey,
  bool value
)
{
  if (!propertyKey.contains("relend_") || !propertyKey.endsWith("_participation")) {
    return false;
  }

  auto relationship = qobject_cast<Relationship*>(element);
  if (!relationship) {
    return false;
  }

  QStringList parts = propertyKey.split("_");
  if (parts.size() < 3) {
    return false;
  }

  int index = parts[1].toInt();
  QList<RelationshipEnd*> ends = relationship->ends();

  if (index >= 0 && index < ends.size()) {
    RelationshipEnd* end = ends[index];
    end->setIsTotalParticipation(value);
    emit statusMessageRequested(
      QString("Participação %1: %2").arg(value ? "total" : "parcial").arg(end->entityId())
    );
    updatesAfterChangingProperty(relationship);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::onLineEditChanged(
  const QString& text
)
{
  auto lineEdit = qobject_cast<QLineEdit*>(sender());
  if (!lineEdit || !m_scene) {
    return;
  }

  QString propertyKey = lineEdit->property("propertyKey").toString();

  if (propertyKey.contains("relend_") && propertyKey.endsWith("_cardinalitytext")) {
    QList<BasicElement*> selectedElements = m_scene->getSelectedElements();
    if (selectedElements.size() != 1) {
      return;
    }

    auto relationship = qobject_cast<Relationship*>(selectedElements.first());
    if (!relationship) {
      return;
    }

    QStringList parts = propertyKey.split("_");
    if (parts.size() < 3) {
      return;
    }

    int index = parts[1].toInt();
    QList<RelationshipEnd*> ends = relationship->ends();

    if (index >= 0 && index < ends.size()) {
      RelationshipEnd* end = ends[index];
      end->setCustomCardinalityText(text);
      emit statusMessageRequested("Texto da cardinalidade atualizado: " + end->customCardinalityText());
      updatesAfterChangingProperty(relationship);
    }
    return;
  }

  QList<BasicElement*> selectedElements = m_scene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  BasicElement* element = selectedElements.first();

  bool success = PropertyCommand::setElementProperty(element, propertyKey, text);

  if (success) {
    emit diagramModified();
    emit statusMessageRequested("Propriedade atualizada: " + propertyKey);
  }
  else {
    emit statusMessageRequested("Erro ao atualizar propriedade: " + propertyKey);
    QSet<ConnectionLine*> selConn = m_scene->getSelectedConnections();
    refresh(selectedElements, selConn);
  }
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::onAddAttributeOnTreeClicked()
{
  if (!m_scene) {
    return;
  }

  QList<BasicElement*> selectedElements = m_scene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  BasicElement* element = selectedElements.first();
  if (!element) {
    return;
  }

  auto newAttribute = qobject_cast<Attribute*>(
    ElementFactory::createElement(ElementType::Attribute, element)
  );
  newAttribute->setName(QString("Atributo_%1").arg(element->getAttributeIds().size() + 1));

  QPointF attributePos = element->position();
  QSizeF attributeSize = element->size();
  int attributeCount = element->getAttributeIds().size();

  newAttribute->setPosition(
    attributePos.x() + attributeSize.width() + 50,
    attributePos.y() + attributeCount * 70
  );

  m_scene->addElement(newAttribute);
  element->addAttributeId(newAttribute->id());
  connectBasicElementsWithConnectionLine(element, newAttribute);

  emit diagramModified();
  emit statusMessageRequested("Atributo adicionado ao elemento pai.");

  QSet<ConnectionLine*> selConn = m_scene->getSelectedConnections();
  refresh(selectedElements, selConn);
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::onRemoveAttributeOnTreeClicked()
{
  auto button = qobject_cast<QPushButton*>(sender());
  if (!button || !m_scene) {
    return;
  }

  QString propertyKey = button->property("propertyKey").toString();
  QStringList parts = propertyKey.split("_");
  if (parts.size() != 2) {
    return;
  }

  int index = parts[1].toInt();

  QList<BasicElement*> selectedElements = m_scene->getSelectedElements();
  if (selectedElements.size() != 1) {
    return;
  }

  BasicElement* element = selectedElements.first();
  if (!element) {
    return;
  }

  QList<QString> attributeIds = element->getAttributeIds();
  if (index >= 0 && index < attributeIds.size()) {
    QString attrId = attributeIds[index];
    element->removeAttributeId(attrId);

    if (BasicElement* attr = m_scene->findElement(attrId)) {
      m_scene->removeElement(attr);
    }

    emit diagramModified();
    emit statusMessageRequested("Atributo removido de seu elemento pai.");

    QSet<ConnectionLine*> selConn = m_scene->getSelectedConnections();
    refresh(selectedElements, selConn);
  }
}

//----------------------------------------------------------------------------------------------

void PropertiesPanel::connectBasicElementsWithConnectionLine(
  BasicElement* startElement,
  BasicElement* endElement
)
{
  const bool isRelationshipEntityConnection =
    (startElement->type() == ElementType::Relationship && endElement->type() == ElementType::Entity) ||
    (startElement->type() == ElementType::Entity && endElement->type() == ElementType::Relationship);

  if (isRelationshipEntityConnection) {
    return;
  }

  auto startPoint = startElement->connectionPoints()[2];
  auto endPoint = endElement->connectionPoints()[3];

  auto connection = new ConnectionLine(startPoint, endPoint, this);
  m_scene->addConnection(connection);
}

//----------------------------------------------------------------------------------------------

QList<QIcon> PropertiesPanel::getConnectionLineTypeIcons()
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
