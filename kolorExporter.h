#pragma once

#include <KConfigWatcher>
#include <KDEDModule>

class Q_DECL_EXPORT kolorExporter : public KDEDModule
{
    Q_OBJECT
public:
    kolorExporter(QObject *parent, const QVariantList &args);

public Q_SLOTS:
    void onKdeglobalsSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;

private:
    void setColors() const;
    void writeCssColorsToFile(QMap<QString, QColor> colors, QString path, QString rootPrefix, QString varPrefix) const;
    KConfigWatcher::Ptr kdeglobalsConfigWatcher;
    QMap<QString, QColor> getColors() const;
    KSharedConfigPtr kdeglobalsConfig;
};
