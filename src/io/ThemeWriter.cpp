#include "ThemeWriter.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

// Local helper, internal to this file
static QString sanitizeName(const QString &name) {
    QString safe = name.trimmed();
    if (safe.isEmpty())
        safe = QStringLiteral("KreateScheme");
    // Remove path separators and reserved characters
    safe.remove(QRegularExpression(R"([/\\:*?"<>|])"));
    if (!safe.endsWith(".colors"))
        safe.append(".colors");
    return safe;
}

bool ThemeWriter::write(
    const QString &name,
    const QMap<QString, QColor> &colors,
    const QColor &windowColor,
    std::function<void(int)> progress
) {
    if (progress)
        progress(5);

    const QString safeName = sanitizeName(name);
    const QString dirPath  = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                           + QStringLiteral("/color-schemes");

    QDir dir(dirPath);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create directory:" << dirPath;
            if (progress) progress(0);
            return false;
        }
    }

    const QString fullPath = dir.filePath(safeName);
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << fullPath << file.errorString();
        if (progress) progress(0);
        return false;
    }

    if (progress)
        progress(25);

    QTextStream out(&file);
    out << "[General]\n";
    out << "Name=" << name << "\n";
    out << "ColorScheme=" << name << "\n\n";

    const auto writeGroup = [&](const QString &group,
                                const QColor &bg,
                                const QColor &fg,
                                const QColor &altBg,
                                const QColor &fgInactive) {
        out << "[Colors:" << group << "]\n";
        out << "BackgroundNormal="    << bg.name(QColor::HexRgb)      << "\n";
        out << "ForegroundNormal="    << fg.name(QColor::HexRgb)      << "\n";
        out << "BackgroundAlternate=" << altBg.name(QColor::HexRgb)   << "\n";
        out << "ForegroundInactive="  << fgInactive.name(QColor::HexRgb) << "\n\n";
    };

    // Base materials (with defaults)
    const QColor base = colors.value("Window",        windowColor);
    const QColor view = colors.value("View",          base.lighter(115));
    const QColor btn  = colors.value("Button",        base.lighter(130));
    const QColor sel  = colors.value("Selection",     base.darker(130));
    const QColor tip  = colors.value("Tooltip",       base.lighter(140));
    const QColor comp = colors.value("Complementary", base.darker(150));

    const auto fgOr = [&](const QString &key, const QColor &fallback) {
        return colors.value(key, fallback);
    };
    const auto fgiOr = [&](const QString &key, const QColor &fallback) {
        return colors.value(key, fallback);
    };

    if (progress)
        progress(55);

    writeGroup("Window",        base, fgOr("FG_Window",        base), base.darker(115), fgiOr("FGI_Window",        base));
    writeGroup("View",          view, fgOr("FG_View",          view), view.darker(115), fgiOr("FGI_View",          view));
    writeGroup("Button",        btn,  fgOr("FG_Button",        btn),  btn.darker(115),  fgiOr("FGI_Button",        btn));
    writeGroup("Selection",     sel,  fgOr("FG_Selection",     sel),  sel.darker(115),  fgiOr("FGI_Selection",     sel));
    writeGroup("Tooltip",       tip,  fgOr("FG_Tooltip",       tip),  tip.darker(115),  fgiOr("FGI_Tooltip",       tip));
    writeGroup("Complementary", comp, fgOr("FG_Complementary", comp), comp.darker(115), fgiOr("FGI_Complementary", comp));

    // Window manager colors (from palette, with simple fallbacks)
    const QColor wmActiveBG   = colors.value("WM_ActiveBG",   base);
    const QColor wmInactiveBG = colors.value("WM_InactiveBG", comp);
    const QColor wmActiveFG   = colors.value("WM_ActiveFG",   fgOr("FG_Window", base));
    const QColor wmInactiveFG = colors.value("WM_InactiveFG", fgOr("FG_Complementary", comp));

    out << "[WM]\n";
    out << "activeBackground="   << wmActiveBG.name(QColor::HexRgb)   << "\n";
    out << "inactiveBackground=" << wmInactiveBG.name(QColor::HexRgb) << "\n";
    out << "activeForeground="   << wmActiveFG.name(QColor::HexRgb)   << "\n";
    out << "inactiveForeground=" << wmInactiveFG.name(QColor::HexRgb) << "\n";

    if (progress)
        progress(90);

    file.close();

    if (progress)
        progress(100);

    qDebug() << "Saved color scheme to:" << fullPath;
    return true;
}
