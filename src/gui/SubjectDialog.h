#ifndef SUBJECTDIALOG_H
#define SUBJECTDIALOG_H

#include <QDialog>
#include <QColor>
#include "core/Subject.h" // Include Subject struct definition

QT_BEGIN_NAMESPACE
namespace Ui { class SubjectDialog; }
QT_END_NAMESPACE

class SubjectDialog : public QDialog {
    Q_OBJECT

public:
    explicit SubjectDialog(QWidget *parent = nullptr);
    explicit SubjectDialog(const Subject& subjectToEdit, QWidget *parent = nullptr); // Constructor for editing
    ~SubjectDialog();

    Subject getSubjectData() const; // To retrieve data from the dialog

private slots:
    void on_colorButton_clicked();
    void on_buttonBox_accepted(); // Handles validation before accepting

private:
    Ui::SubjectDialog *ui;
    QColor m_selectedColor;
    Subject m_subject; // Holds current subject being created or edited
    bool m_isEditMode;

    void initDialogWithSubject(const Subject& subject); // Helper to populate dialog for editing
    void updateColorPreview();
};

#endif // SUBJECTDIALOG_H