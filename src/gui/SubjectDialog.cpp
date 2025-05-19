#include "SubjectDialog.h"
#include "ui_SubjectDialog.h"
#include "core/DatabaseManager.h" // For subjectExists check (optional here, can be done before calling DB)

#include <QColorDialog>
#include <QMessageBox> // For potential error messages if not using statusLabel
#include <QDebug>

SubjectDialog::SubjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubjectDialog),
    m_selectedColor(Qt::white), // Default color
    m_isEditMode(false)
{
    ui->setupUi(this);
    setWindowTitle(tr("创建新科目"));
    m_subject.color = m_selectedColor; // Set initial color for the subject object
    updateColorPreview();

    // Connect QDialogButtonBox::accepted to our custom slot for validation
    // Remove the default connection in .ui file if it connects directly to QDialog::accept
    // connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SubjectDialog::on_buttonBox_accepted);
    // The .ui connection will call accept() after this slot if it's also connected there.
    // It's better to handle accept() manually here.
}

SubjectDialog::SubjectDialog(const Subject& subjectToEdit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubjectDialog),
    m_subject(subjectToEdit), // Copy subject data for editing
    m_isEditMode(true)
{
    ui->setupUi(this);
    setWindowTitle(tr("编辑科目 - %1").arg(subjectToEdit.name));
    initDialogWithSubject(subjectToEdit);

    // Connect QDialogButtonBox::accepted to our custom slot for validation
    // connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SubjectDialog::on_buttonBox_accepted);
}


SubjectDialog::~SubjectDialog() {
    delete ui;
}

void SubjectDialog::initDialogWithSubject(const Subject& subject) {
    ui->nameLineEdit->setText(subject.name);
    ui->descriptionTextEdit->setPlainText(subject.description);
    m_selectedColor = subject.color;
    ui->tagsLineEdit->setText(subject.tags);
    updateColorPreview();
}

Subject SubjectDialog::getSubjectData() const {
    // This method might be called after dialog is accepted.
    // m_subject should already be updated by on_buttonBox_accepted()
    return m_subject;
}

void SubjectDialog::on_colorButton_clicked() {
    QColorDialog colorDialog(m_selectedColor, this);
    if (colorDialog.exec() == QDialog::Accepted) {
        m_selectedColor = colorDialog.selectedColor();
        m_subject.color = m_selectedColor; // Update subject's color directly
        updateColorPreview();
    }
}

void SubjectDialog::updateColorPreview() {
    if (m_selectedColor.isValid()) {
        QString styleSheet = QString("background-color: %1; border: 1px solid black;").arg(m_selectedColor.name());
        ui->colorPreviewLabel->setStyleSheet(styleSheet);
        ui->colorPreviewLabel->setText(""); // Clear any text
    } else {
        ui->colorPreviewLabel->setStyleSheet("border: 1px solid black; background-color: white;");
        ui->colorPreviewLabel->setText("无颜色");
    }
}

void SubjectDialog::on_buttonBox_accepted() {
    ui->statusLabel->clear();
    QString name = ui->nameLineEdit->text().trimmed();
    QString description = ui->descriptionTextEdit->toPlainText().trimmed();
    QString tags = ui->tagsLineEdit->text().trimmed();

    if (name.isEmpty()) {
        ui->statusLabel->setText("科目名称不能为空！");
        // To prevent dialog closing with QDialogButtonBox::accepted() if validation fails,
        // and if the .ui file auto-connects accepted() to QDialog::accept(), this won't stop it.
        // We need to ensure this slot is the only one controlling the accept().
        // If on_buttonBox_accepted calls QDialog::accept() itself, it's fine.
        return; // Don't proceed, keep dialog open
    }

    // Populate m_subject with current dialog values
    m_subject.name = name;
    m_subject.description = description;
    m_subject.color = m_selectedColor; // m_selectedColor is updated directly on color change
    m_subject.tags = tags;
    // m_subject.userId and m_subject.id are set externally before calling this dialog (for edit)
    // or userId is set before saving (for new)

    // The actual DB check for subjectExists (name uniqueness for user) should ideally be done
    // *before* calling accept, or the calling code (MainWindow) handles DB interaction and potential errors.
    // For simplicity, this dialog just prepares the data. The caller will handle DB ops.
    
    qDebug() << "SubjectDialog accepted. Name:" << m_subject.name;
    QDialog::accept(); // If all validations pass, accept the dialog.
}