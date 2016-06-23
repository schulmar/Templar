#include "usedsourcefilemodel.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>

namespace Templar
{

void SourceFileNode::setVisible(bool value)
{
    visible = value;
    std::for_each(children.begin(),children.end(),[=](SourceFileNode *child) { child->setVisible(value);});
}

UsedSourceFileModel::UsedSourceFileModel(const std::vector<QString> &fileNames) {
    size_t fileId = 0;
    for (auto const &path : fileNames) {
        Add(path, fileId++);
    }
}

SourceFileNode * UsedSourceFileModel::AddImpl(const QString &path, size_t fileId)
{
    QStringList components =
        path.split(QDir::separator(), QString::SkipEmptyParts);
    SourceFileNode *currentNode = &root;
    for (int i = 0; i < components.size(); ++i) {
        SourceFileNode **found = std::find_if(
            currentNode->children.begin(), currentNode->children.end(),
            [&](SourceFileNode *node) { return node->name == components[i]; });
        if (found == currentNode->children.end()) {
            currentNode->children.append(
                new SourceFileNode(components[i], currentNode));
            currentNode->children.back()->row = currentNode->children.size();
            currentNode = currentNode->children.back();
        } else {
            currentNode = *found;
        }
    }
    nodeIdMap.insert(fileId, currentNode);
    currentNode->fullPath = path;
    currentNode->id = fileId;
    return currentNode;
}

void UsedSourceFileModel::Add(const QString &path, size_t fileId) {
    AddImpl(path, fileId);
}

size_t UsedSourceFileModel::Add(const QString &newPath)
{
    size_t fileId = nodeIdMap.empty() ? 0 : ((--nodeIdMap.end()).key() + 1);
    AddImpl(newPath, fileId);
    return fileId;
}

QString UsedSourceFileModel::getAbsolutePathOf(size_t fileId) {
  auto iter = nodeIdMap.find(fileId);
  if (iter == nodeIdMap.end()) {
    throw std::runtime_error(
        tr("Could not find file with id %0").arg(fileId).toStdString());
  } else {
    return QDir(relativePathRoot).filePath(iter.value()->fullPath);
  }
}

bool UsedSourceFileModel::checkRelativePathRoot(QString *failingRelativePath) {
  for (auto const &nodeIdPath : nodeIdMap) {
    if (QDir::isRelativePath(nodeIdPath->fullPath)) {
      QFileInfo fileInfo{QDir(relativePathRoot).filePath(nodeIdPath->fullPath)};
      if (!(fileInfo.exists() && fileInfo.isReadable())) {
        if (failingRelativePath) {
          *failingRelativePath = nodeIdPath->fullPath;
        }
        return false;
      }
    }
  }
  return true;
}

QModelIndex UsedSourceFileModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
             return QModelIndex();

    const SourceFileNode  *parentItem;

         if (!parent.isValid())
             parentItem = &root;
         else
             parentItem = static_cast<const SourceFileNode*>(parent.internalPointer());

         if(row < parentItem->children.size())
              return createIndex(row,column,static_cast<void*>(parentItem->children.at(row)));
         else
             return QModelIndex();
}

QModelIndex UsedSourceFileModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    SourceFileNode *childItem = static_cast<SourceFileNode*>(index.internalPointer());
    SourceFileNode *parentItem = childItem->parent;

    if (parentItem == &root)
        return QModelIndex();

    return createIndex(parentItem->row, 0, parentItem);
}

int UsedSourceFileModel::rowCount(const QModelIndex &parent) const
{
    const SourceFileNode *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = &root;
    else
        parentItem = static_cast<SourceFileNode*>(parent.internalPointer());

    return parentItem->children.size();
}

QVariant UsedSourceFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    SourceFileNode *item = static_cast<SourceFileNode*>(index.internalPointer());

    if ( role == Qt::CheckStateRole && index.column() == 0 )
        return static_cast< int >( item->visible ? Qt::Checked : Qt::Unchecked );

    if (role != Qt::DisplayRole)
        return QVariant();

    if(index.column() == 0)
        return item->name;

    return QVariant();
}

bool UsedSourceFileModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if (!index.isValid())
        return false;

    SourceFileNode *item = static_cast<SourceFileNode*>(index.internalPointer());

    if(role == Qt::CheckStateRole)
    {
       item->setVisible(value.toBool());
       dataChanged(index,index);
       if(!item->children.empty())
       {
               QModelIndex childIndex = this->index(item->children.front()->row,0,index);
               QModelIndex childIndex2 = this->index(item->children.back()->row,0,index);
               dataChanged(childIndex,childIndex2);
       }
    }
    return true;
}

Qt::ItemFlags UsedSourceFileModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if ( index.column() == 0 )
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}

/*
bool UsedSourceFileModel::hasChildren(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;


    SourceFileNode *item = static_cast<SourceFileNode*>(index.internalPointer());
    return !item->children.empty();
}*/
}
