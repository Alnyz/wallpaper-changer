#include <QtTest>
#include "librarymodel.h"

class TestLibraryModel : public QObject {
    Q_OBJECT
private slots:
    void exposesLocalItems() {
        LibraryModel m;
        m.setLocalPaths({ "/a/one.jpg", "/a/two.png" });
        QCOMPARE(m.rowCount(), 2);
        const auto idx = m.index(0, 0);
        QCOMPARE(m.data(idx, LibraryModel::SourcePathRole).toString(), QStringLiteral("/a/one.jpg"));
        QCOMPARE(m.data(idx, LibraryModel::ThumbUrlRole).toString(), QStringLiteral("file:///a/one.jpg"));
        QCOMPARE(m.data(idx, LibraryModel::IsLocalRole).toBool(), true);
    }
    void selectionFlagsOneRow() {
        LibraryModel m;
        m.setLocalPaths({ "/a/one.jpg", "/a/two.png" });
        m.setSelectedIndex(1);
        QCOMPARE(m.data(m.index(0,0), LibraryModel::SelectedRole).toBool(), false);
        QCOMPARE(m.data(m.index(1,0), LibraryModel::SelectedRole).toBool(), true);
    }
};
QTEST_GUILESS_MAIN(TestLibraryModel)
#include "test_librarymodel.moc"
