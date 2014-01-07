// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Formula;
class MorphWidget;

class QComboBox;



/**
 * @brief
 *
 * @details
 **/
class MainWindow : public QMainWindow
{
Q_OBJECT

public:

	MainWindow ();

private:

	QComboBox * m_saved;

	Formula * m_formula;

	MorphWidget * m_colour_morph;
	MorphWidget * m_image_morph;

private slots:

	void save_formula ();

	void delete_formula ();

	void take_example ();

	void help ();

	void select_formula (const QString &);
};




#endif
