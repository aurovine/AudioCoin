#ifndef MACNOTIFICATIONHANDLER_H
#define MACNOTIFICATIONHANDLER_H

#include <QObject>

/** Macintosh-specific notification handler.
 */
class MacNotificationHandler : public QObject
{
    Q_OBJECT

public:
    void showNotification(const QString &title, const QString &text);
    void execAppleScript(const QString &script);

    bool hasUserNotificationCenterSupport(void);
    static MacNotificationHandler *instance();
};

#endif // MACNOTIFICATIONHANDLER_H
