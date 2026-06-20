#include "librarymodel.h"
#include <QUrl>

int LibraryModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_items.size()) return {};
    const Item &it = m_items.at(index.row());
    switch (role) {
    case ThumbUrlRole:   return it.thumbUrl;
    case SourcePathRole: return it.sourcePath;
    case IsLocalRole:    return it.isLocal;
    case WidRole:        return it.wid;
    case FileTypeRole:   return it.fileType;
    case SelectedRole:   return index.row() == m_selected;
    default:             return {};
    }
}

QHash<int, QByteArray> LibraryModel::roleNames() const {
    return {
        { ThumbUrlRole,   "thumbUrl" },
        { SourcePathRole, "sourcePath" },
        { IsLocalRole,    "isLocal" },
        { WidRole,        "wid" },
        { FileTypeRole,   "fileType" },
        { SelectedRole,   "selected" },
    };
}

void LibraryModel::setLocalPaths(const QStringList &paths) {
    beginResetModel();
    m_items.clear();
    m_selected = -1;
    for (const QString &p : paths) {
        Item it;
        it.thumbUrl = QUrl::fromLocalFile(p).toString();
        it.sourcePath = p;
        it.isLocal = true;
        m_items.append(it);
    }
    endResetModel();
}

static LibraryModel::Item fromResult(const WallhavenResult &r) {
    LibraryModel::Item it;
    it.thumbUrl = r.thumbSmall;
    it.sourcePath = r.path;
    it.isLocal = false;
    it.wid = r.id;
    it.fileType = r.fileType;
    return it;
}

void LibraryModel::setRemoteResults(const QList<WallhavenResult> &results) {
    beginResetModel();
    m_items.clear();
    m_selected = -1;
    for (const auto &r : results) m_items.append(fromResult(r));
    endResetModel();
}

void LibraryModel::appendRemoteResults(const QList<WallhavenResult> &results) {
    if (results.isEmpty()) return;
    beginInsertRows({}, m_items.size(), m_items.size() + results.size() - 1);
    for (const auto &r : results) m_items.append(fromResult(r));
    endInsertRows();
}

void LibraryModel::setSelectedIndex(int row) {
    const int prev = m_selected;
    m_selected = row;
    if (prev >= 0 && prev < m_items.size())
        emit dataChanged(index(prev, 0), index(prev, 0), { SelectedRole });
    if (row >= 0 && row < m_items.size())
        emit dataChanged(index(row, 0), index(row, 0), { SelectedRole });
}
