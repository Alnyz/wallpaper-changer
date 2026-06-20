#pragma once
#include <QAbstractListModel>
#include <QList>
#include "wallhavenresult.h"

class LibraryModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        ThumbUrlRole = Qt::UserRole + 1,
        SourcePathRole,
        IsLocalRole,
        WidRole,
        FileTypeRole,
        SelectedRole,
    };
    struct Item {
        QString thumbUrl;
        QString sourcePath;
        bool isLocal = true;
        QString wid;
        QString fileType;
    };
    using QAbstractListModel::QAbstractListModel;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setLocalPaths(const QStringList &paths);
    void setRemoteResults(const QList<WallhavenResult> &results);
    void appendRemoteResults(const QList<WallhavenResult> &results);
    void setSelectedIndex(int row);
    const Item &itemAt(int row) const { return m_items.at(row); }
private:
    QList<Item> m_items;
    int m_selected = -1;
};
