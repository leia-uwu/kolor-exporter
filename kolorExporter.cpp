#include "kolorExporter.h"

#include <QFile>

#include <KColorScheme>
#include <KColorUtils>
#include <KConfig>
#include <KConfigGroup>
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(kolorExporter, "kolorExporter.json")

kolorExporter::kolorExporter(QObject *parent, const QVariantList &)
    : KDEDModule(parent)
    , kdeglobalsConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig()))
    , kdeglobalsConfig(KSharedConfig::openConfig())
{
    connect(kdeglobalsConfigWatcher.data(), &KConfigWatcher::configChanged, this, &kolorExporter::onKdeglobalsSettingsChange);

    setColors();
}

void kolorExporter::setColors() const
{
    const QMap<QString, QColor> colors = getColors();

    writeCssColorsToFile(colors,
                         QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
                             + QStringLiteral("/kde-colors.css"), // ~/.config/kde-colors.css
                         ":root",
                         "--");

    // rofi
    writeCssColorsToFile(colors,
                         QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                             + QStringLiteral("/rofi/themes/kde-colors.rasi"), // ~/.local/share/rofi/themes/kde-colors.rasi
                         "*",
                         "");
}

void kolorExporter::writeCssColorsToFile(QMap<QString, QColor> colors, QString path, QString rootPrefix, QString varPrefix) const
{
    QFile colorsCss(path);

    if (colorsCss.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream colorsCssStream(&colorsCss);

        colorsCssStream << QStringLiteral("%1 {\n").arg(rootPrefix);
        for (auto it = colors.cbegin(); it != colors.cend(); it++) {
            colorsCssStream << QStringLiteral("    %1%2: %3;\n").arg(varPrefix, it.key(), it.value().name());
        }
        colorsCssStream << QStringLiteral("}\n");
    }
}

void kolorExporter::onKdeglobalsSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
{
    if (group.name() == QStringLiteral("General")) {
        if (names.contains(QByteArrayLiteral("ColorScheme")) || names.contains(QByteArrayLiteral("AccentColor"))) {
            setColors();
        }
    }
}

// copied from https://invent.kde.org/plasma/kde-gtk-config/-/blob/master/kded/configvalueprovider.cpp

QMap<QString, QColor> kolorExporter::getColors() const
{
    using KCS = KColorScheme;

    // Color Schemes Collection
    QHash<QString, QHash<QString, KCS>> csc{
        {QStringLiteral("active"),
         {
             {QStringLiteral("view"), KCS(QPalette::Active, KCS::View)},
             {QStringLiteral("window"), KCS(QPalette::Active, KCS::Window)},
             {QStringLiteral("button"), KCS(QPalette::Active, KCS::Button)},
             {QStringLiteral("selection"), KCS(QPalette::Active, KCS::Selection)},
             {QStringLiteral("tooltip"), KCS(QPalette::Active, KCS::Tooltip)},
             {QStringLiteral("complementary"), KCS(QPalette::Active, KCS::Complementary)},
             {QStringLiteral("header"), KCS(QPalette::Active, KCS::Header)},
         }},
        {QStringLiteral("inactive"),
         {
             {QStringLiteral("view"), KCS(QPalette::Inactive, KCS::View)},
             {QStringLiteral("window"), KCS(QPalette::Inactive, KCS::Window)},
             {QStringLiteral("button"), KCS(QPalette::Inactive, KCS::Button)},
             {QStringLiteral("selection"), KCS(QPalette::Inactive, KCS::Selection)},
             {QStringLiteral("tooltip"), KCS(QPalette::Inactive, KCS::Tooltip)},
             {QStringLiteral("complementary"), KCS(QPalette::Inactive, KCS::Complementary)},
             {QStringLiteral("header"), KCS(QPalette::Inactive, KCS::Header)},
         }},
        {QStringLiteral("disabled"),
         {
             {QStringLiteral("view"), KCS(QPalette::Disabled, KCS::View)},
             {QStringLiteral("window"), KCS(QPalette::Disabled, KCS::Window)},
             {QStringLiteral("button"), KCS(QPalette::Disabled, KCS::Button)},
             {QStringLiteral("selection"), KCS(QPalette::Disabled, KCS::Selection)},
             {QStringLiteral("tooltip"), KCS(QPalette::Disabled, KCS::Tooltip)},
             {QStringLiteral("complementary"), KCS(QPalette::Disabled, KCS::Complementary)},
             {QStringLiteral("header"), KCS(QPalette::Disabled, KCS::Header)},
         }},
    };

    // Color mixing
    QColor windowForegroundColor = csc["active"]["window"].foreground(KCS::NormalText).color();
    QColor windowBackgroundColor = csc["active"]["window"].background(KCS::NormalBackground).color();
    QColor bordersColor = KColorUtils::mix(windowBackgroundColor, windowForegroundColor, 0.25);

    QColor inactiveWindowForegroundColor = csc["inactive"]["window"].foreground(KCS::NormalText).color();
    QColor inactiveWindowBackgroundColor = csc["inactive"]["window"].background(KCS::NormalBackground).color();
    QColor inactiveBordersColor = KColorUtils::mix(inactiveWindowBackgroundColor, inactiveWindowForegroundColor, 0.25);

    QColor disabledWindowForegroundColor = csc["disabled"]["window"].foreground(KCS::NormalText).color();
    QColor disabledWindowBackgroundColor = csc["disabled"]["window"].background(KCS::NormalBackground).color();
    QColor disabledBordersColor = KColorUtils::mix(disabledWindowBackgroundColor, disabledWindowForegroundColor, 0.25);

    QColor unfocusedDisabledWindowForegroundColor = csc["disabled"]["window"].foreground(KCS::NormalText).color();
    QColor unfocusedDisabledWindowBackgroundColor = csc["disabled"]["window"].background(KCS::NormalBackground).color();
    QColor unfocusedDisabledBordersColor = KColorUtils::mix(unfocusedDisabledWindowBackgroundColor, unfocusedDisabledWindowForegroundColor, 0.25);

    QColor tooltipForegroundColor = csc["active"]["tooltip"].foreground(KCS::NormalText).color();
    QColor tooltipBackgroundColor = csc["active"]["tooltip"].background(KCS::NormalBackground).color();
    QColor tooltipBorderColor = KColorUtils::mix(tooltipBackgroundColor, tooltipForegroundColor, 0.25);

    KConfigGroup windowManagerConfig = kdeglobalsConfig->group(QStringLiteral("WM"));

    QMap<QString, QColor> result = {
        /*
         * Normal (Non Backdrop, Non Insensitive)
         */

        // General Colors
        {"theme-fg-color-breeze", csc["active"]["window"].foreground(KCS::NormalText).color()},
        {"theme-bg-color-breeze", csc["active"]["window"].background(KCS::NormalBackground).color()},
        {"theme-text-color-breeze", csc["active"]["view"].foreground(KCS::NormalText).color()},
        {"theme-base-color-breeze", csc["active"]["view"].background(KCS::NormalBackground).color()},
        {"theme-view-hover-decoration-color-breeze", csc["active"]["view"].decoration(KCS::HoverColor).color()},
        {"theme-hovering-selected-bg-color-breeze", csc["active"]["selection"].decoration(KCS::HoverColor).color()},
        {"theme-selected-bg-color-breeze", csc["active"]["selection"].background(KCS::NormalBackground).color()},
        {"theme-selected-fg-color-breeze", csc["active"]["selection"].foreground(KCS::NormalText).color()},
        {"theme-view-active-decoration-color-breeze", csc["active"]["view"].decoration(KCS::HoverColor).color()},

        // Button Colors
        {"theme-button-background-normal-breeze", csc["active"]["button"].background(KCS::NormalBackground).color()},
        {"theme-button-decoration-hover-breeze", csc["active"]["button"].decoration(KCS::HoverColor).color()},
        {"theme-button-decoration-focus-breeze", csc["active"]["button"].decoration(KCS::FocusColor).color()},
        {"theme-button-foreground-normal-breeze", csc["active"]["button"].foreground(KCS::NormalText).color()},
        {"theme-button-foreground-active-breeze", csc["active"]["selection"].foreground(KCS::NormalText).color()},

        // Misc Colors
        {"borders-breeze", bordersColor},
        {"warning-color-breeze", csc["active"]["view"].foreground(KCS::NeutralText).color()},
        {"success-color-breeze", csc["active"]["view"].foreground(KCS::PositiveText).color()},
        {"error-color-breeze", csc["active"]["view"].foreground(KCS::NegativeText).color()},

        /*
         * Backdrop (Inactive)
         */

        // General
        {"theme-unfocused-fg-color-breeze", csc["inactive"]["window"].foreground(KCS::NormalText).color()},
        {"theme-unfocused-text-color-breeze", csc["inactive"]["view"].foreground(KCS::NormalText).color()},
        {"theme-unfocused-bg-color-breeze", csc["inactive"]["window"].background(KCS::NormalBackground).color()},
        {"theme-unfocused-base-color-breeze", csc["inactive"]["view"].background(KCS::NormalBackground).color()},
        {"theme-unfocused-selected-bg-color-alt-breeze", csc["inactive"]["selection"].background(KCS::NormalBackground).color()},
        {"theme-unfocused-selected-bg-color-breeze", csc["inactive"]["selection"].background(KCS::NormalBackground).color()},
        {"theme-unfocused-selected-fg-color-breeze", csc["inactive"]["selection"].foreground(KCS::NormalText).color()},

        // Button
        {"theme-button-background-backdrop-breeze", csc["inactive"]["button"].background(KCS::NormalBackground).color()},
        {"theme-button-decoration-hover-backdrop-breeze", csc["inactive"]["button"].decoration(KCS::HoverColor).color()},
        {"theme-button-decoration-focus-backdrop-breeze", csc["inactive"]["button"].decoration(KCS::FocusColor).color()},
        {"theme-button-foreground-backdrop-breeze", csc["inactive"]["button"].foreground(KCS::NormalText).color()},
        {"theme-button-foreground-active-backdrop-breeze", csc["inactive"]["selection"].foreground(KCS::NormalText).color()},

        // Misc Colors
        {"unfocused-borders-breeze", inactiveBordersColor},
        {"warning-color-backdrop-breeze", csc["inactive"]["view"].foreground(KCS::NeutralText).color()},
        {"success-color-backdrop-breeze", csc["inactive"]["view"].foreground(KCS::PositiveText).color()},
        {"error-color-backdrop-breeze", csc["inactive"]["view"].foreground(KCS::NegativeText).color()},

        /*
         * Insensitive (Disabled)
         */

        // General
        {"insensitive-fg-color-breeze", csc["disabled"]["window"].foreground(KCS::NormalText).color()},
        {"insensitive-base-fg-color-breeze", csc["disabled"]["view"].foreground(KCS::NormalText).color()},
        {"insensitive-bg-color-breeze", csc["disabled"]["window"].background(KCS::NormalBackground).color()},
        {"insensitive-base-color-breeze", csc["disabled"]["view"].background(KCS::NormalBackground).color()},
        {"insensitive-selected-bg-color-breeze", csc["disabled"]["selection"].background(KCS::NormalBackground).color()},
        {"insensitive-selected-fg-color-breeze", csc["disabled"]["selection"].foreground(KCS::NormalText).color()},

        // Button
        {"theme-button-background-insensitive-breeze", csc["disabled"]["button"].background(KCS::NormalBackground).color()},
        {"theme-button-decoration-hover-insensitive-breeze", csc["disabled"]["button"].decoration(KCS::HoverColor).color()},
        {"theme-button-decoration-focus-insensitive-breeze", csc["disabled"]["button"].decoration(KCS::FocusColor).color()},
        {"theme-button-foreground-insensitive-breeze", csc["disabled"]["button"].foreground(KCS::NormalText).color()},
        {"theme-button-foreground-active-insensitive-breeze", csc["disabled"]["selection"].foreground(KCS::NormalText).color()},

        // Misc Colors
        {"insensitive-borders-breeze", disabledBordersColor},
        {"warning-color-insensitive-breeze", csc["disabled"]["view"].foreground(KCS::NeutralText).color()},
        {"success-color-insensitive-breeze", csc["disabled"]["view"].foreground(KCS::PositiveText).color()},
        {"error-color-insensitive-breeze", csc["disabled"]["view"].foreground(KCS::NegativeText).color()},

        /*
         * Insensitive Backdrop (Inactive Disabled)
         * These pretty much have the same appearance as regular inactive colors,
         * but they're separate in case we decide to make them different in the
         * future.
         */

        // General
        {"insensitive-unfocused-fg-color-breeze", csc["disabled"]["window"].foreground(KCS::NormalText).color()},
        {"theme-unfocused-view-text-color-breeze", csc["disabled"]["view"].foreground(KCS::NormalText).color()},
        {"insensitive-unfocused-bg-color-breeze", csc["disabled"]["window"].background(KCS::NormalBackground).color()},
        {"theme-unfocused-view-bg-color-breeze", csc["disabled"]["view"].background(KCS::NormalBackground).color()},
        {"insensitive-unfocused-selected-bg-color-breeze", csc["disabled"]["selection"].background(KCS::NormalBackground).color()},
        {"insensitive-unfocused-selected-fg-color-breeze", csc["disabled"]["selection"].foreground(KCS::NormalText).color()},

        // Button
        {"theme-button-background-backdrop-insensitive-breeze", csc["disabled"]["button"].background(KCS::NormalBackground).color()},
        {"theme-button-decoration-hover-backdrop-insensitive-breeze", csc["disabled"]["button"].decoration(KCS::HoverColor).color()},
        {"theme-button-decoration-focus-backdrop-insensitive-breeze", csc["disabled"]["button"].decoration(KCS::FocusColor).color()},
        {"theme-button-foreground-backdrop-insensitive-breeze", csc["disabled"]["button"].foreground(KCS::NormalText).color()},
        {"theme-button-foreground-active-backdrop-insensitive-breeze", csc["disabled"]["selection"].foreground(KCS::NormalText).color()},

        // Misc Colors
        {"unfocused-insensitive-borders-breeze", unfocusedDisabledBordersColor},
        {"warning-color-insensitive-backdrop-breeze", csc["disabled"]["view"].foreground(KCS::NeutralText).color()},
        {"success-color-insensitive-backdrop-breeze", csc["disabled"]["view"].foreground(KCS::PositiveText).color()},
        {"error-color-insensitive-backdrop-breeze", csc["disabled"]["view"].foreground(KCS::NegativeText).color()},

        /*
         * Ignorant Colors (These colors do not care about backdrop or insensitive
         * states)
         */

        {"link-color-breeze", csc["active"]["view"].foreground(KCS::LinkText).color()},
        {"link-visited-color-breeze", csc["active"]["view"].foreground(KCS::VisitedText).color()},

        {"tooltip-text-breeze", tooltipForegroundColor},
        {"tooltip-background-breeze", tooltipBackgroundColor},
        {"tooltip-border-breeze", tooltipBorderColor},

        {"content-view-bg-breeze", csc["active"]["view"].background(KCS::NormalBackground).color()},

    };
    // Handle Headers (menu bars and some of toolbars)
    if (KCS::isColorSetSupported(kdeglobalsConfig, KCS::Header)) {
        // If we have a separate Header color set, use it for both titlebar and
        // header coloring...
        result.insert({{"theme-header-background-breeze", csc["active"]["header"].background().color()},
                       {"theme-header-foreground-breeze", csc["active"]["header"].foreground().color()},
                       {"theme-header-background-light-breeze", csc["active"]["window"].background().color()},
                       {"theme-header-foreground-backdrop-breeze", csc["inactive"]["header"].foreground().color()},
                       {"theme-header-background-backdrop-breeze", csc["inactive"]["header"].background().color()},
                       {"theme-header-foreground-insensitive-breeze", csc["inactive"]["header"].foreground().color()},
                       {"theme-header-foreground-insensitive-backdrop-breeze", csc["inactive"]["header"].foreground().color()},

                       {"theme-titlebar-background-breeze", csc["active"]["header"].background().color()},
                       {"theme-titlebar-foreground-breeze", csc["active"]["header"].foreground().color()},
                       {"theme-titlebar-background-light-breeze", csc["active"]["window"].background().color()},
                       {"theme-titlebar-foreground-backdrop-breeze", csc["inactive"]["header"].foreground().color()},
                       {"theme-titlebar-background-backdrop-breeze", csc["inactive"]["header"].background().color()},
                       {"theme-titlebar-foreground-insensitive-breeze", csc["inactive"]["header"].foreground().color()},
                       {"theme-titlebar-foreground-insensitive-backdrop-breeze", csc["inactive"]["header"].foreground().color()}});
    } else {
        //... if we don't we'll use regular window colors for headerbar and WM group
        // for a titlebar
        result.insert({
            {"theme-header-background-breeze", csc["active"]["window"].background().color()},
            {"theme-header-foreground-breeze", csc["active"]["window"].foreground().color()},
            {"theme-header-background-light-breeze", csc["active"]["window"].background().color()},
            {"theme-header-foreground-backdrop-breeze", csc["inactive"]["window"].foreground().color()},
            {"theme-header-background-backdrop-breeze", csc["inactive"]["window"].background().color()},
            {"theme-header-foreground-insensitive-breeze", csc["inactive"]["window"].foreground().color()},
            {"theme-header-foreground-insensitive-backdrop-breeze", csc["inactive"]["window"].foreground().color()},

            {"theme-titlebar-background-breeze", windowManagerConfig.readEntry("activeBackground", QColor())},
            {"theme-titlebar-foreground-breeze", windowManagerConfig.readEntry("activeForeground", QColor())},
            {"theme-titlebar-background-light-breeze", csc["active"]["window"].background(KCS::NormalBackground).color()},
            {"theme-titlebar-foreground-backdrop-breeze", windowManagerConfig.readEntry("inactiveForeground", QColor())},
            {"theme-titlebar-background-backdrop-breeze", windowManagerConfig.readEntry("inactiveBackground", QColor())},
            {"theme-titlebar-foreground-insensitive-breeze", windowManagerConfig.readEntry("inactiveForeground", QColor())},
            {"theme-titlebar-foreground-insensitive-backdrop-breeze", windowManagerConfig.readEntry("inactiveForeground", QColor())},
        });
    }

    return result;
}

#include "kolorExporter.moc"
