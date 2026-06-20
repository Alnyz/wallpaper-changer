#pragma once
#include <QByteArray>
#include <QList>
#include "wallhavenresult.h"

class WallhavenParser {
public:
    static QList<WallhavenResult> parse(const QByteArray &json, WallhavenMeta *meta = nullptr);
};
