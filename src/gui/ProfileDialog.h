#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>

class ProfileDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ProfileDialog(QWidget *parent = nullptr);
	~ProfileDialog();
};

#endif