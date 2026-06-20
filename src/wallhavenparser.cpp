#include "wallhavenparser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QList<WallhavenResult> WallhavenParser::parse(const QByteArray &json, WallhavenMeta *meta) {
    QList<WallhavenResult> out;
    const QJsonDocument doc = QJsonDocument::fromJson(json);
    if (!doc.isObject()) return out;
    const QJsonObject root = doc.object();
    for (const QJsonValue &v : root.value("data").toArray()) {
        const QJsonObject o = v.toObject();
        WallhavenResult r;
        r.id         = o.value("id").toString();
        r.path       = o.value("path").toString();
        r.resolution = o.value("resolution").toString();
        r.fileType   = o.value("file_type").toString();
        const QJsonObject t = o.value("thumbs").toObject();
        r.thumbSmall = t.value("small").toString();
        r.thumbLarge = t.value("large").toString();
        if (!r.id.isEmpty()) out.append(r);
    }
    if (meta) {
        const QJsonObject m = root.value("meta").toObject();
        meta->currentPage = m.value("current_page").toInt();
        meta->lastPage    = m.value("last_page").toInt();
        meta->total       = m.value("total").toInt();
    }
    return out;
}
