#pragma once
#include <QString>

struct WallhavenResult {
    QString id;
    QString path;        // full-res image URL
    QString thumbSmall;  // grid thumbnail
    QString thumbLarge;  // hover preview
    QString resolution;  // "WxH"
    QString fileType;    // MIME, e.g. "image/png"
};

struct WallhavenMeta {
    int currentPage = 0;
    int lastPage = 0;
    int total = 0;
};
