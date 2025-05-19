#ifndef SUBJECT_H
#define SUBJECT_H

#include <QString>
#include <QColor> // For color representation

struct Subject {
    int id = -1;          // Database ID
    int userId = -1;      // Foreign key to users table
    QString name;
    QString description;
    QColor color;         // Color associated with the subject
    QString tags;         // Optional tags, comma-separated

    bool isValid() const {
        return id != -1 && userId != -1 && !name.isEmpty();
    }

    // Helper to convert QColor to hex string for DB storage
    QString colorToHexString() const {
        return color.isValid() ? color.name(QColor::HexRgb) : QString();
    }

    // Helper to set QColor from hex string from DB
    void setColorFromHexString(const QString& hexColor) {
        QColor c = QColor::fromString(hexColor);
        if (c.isValid()) {
            color = c;
        } else {
            color = QColor(); // Invalid or no color
        }
    }
};

#endif // SUBJECT_H