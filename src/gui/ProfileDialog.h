#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include "core/User.h" // Make sure this path is correct

QT_BEGIN_NAMESPACE
namespace Ui { class ProfileDialog; }
QT_END_NAMESPACE

class ProfileDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProfileDialog(const User& currentUser, QWidget *parent = nullptr);
    ~ProfileDialog();

    User getUpdatedUser() const;

private slots:
    void on_changeAvatarButton_clicked();
    void on_buttonBox_accepted(); // Connected to QDialogButtonBox's "Save" button (or OK)

private:
    Ui::ProfileDialog *ui;
    User m_currentUser;     // Stores the initial user data passed to the dialog
    User m_updatedUser;     // Stores changes, will be returned
    QString m_newAvatarInternalPath; // Relative path within data/avatars for the new avatar

    void loadUserData();
    void displayAvatar(const QString& avatarPath);
};

#endif // PROFILEDIALOG_H