/*
  BSD 3-Clause License

  Copyright (c) 2026, Alexis Megas

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this
     list o conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from
     Luna without specific prior written permission.

     LUNA IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
     AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
     PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
     EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
     PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
     OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
     LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
     NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
     OF LUNA, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "luna-calendar.h"
#include "luna-day-widget.h"

QString luna_calendar::s_version = "2026.08.05";

int main(int argc, char *argv[])
{
#if defined(Q_OS_ANDROID)
#else
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);
#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_WINDOWS)
  qputenv("QT_STYLE_OVERRIDE", "Fusion");
#endif

  QApplication application(argc, argv);

  application.setWindowIcon(QIcon(":/luna.png"));

  QDir dir;

  dir.mkdir(luna_calendar::home_path());

  int rc = 0;

  {
    luna_calendar luna;

#ifndef Q_OS_ANDROID
    luna.showFullScreen();
#else
    luna.showMaximized();
#endif
    rc = static_cast<int> (application.exec());
  }

#ifdef Q_OS_ANDROID
  auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());

  activity.callMethod<void> ("finishAndRemoveTask");
#endif
  return rc;
}

luna_calendar::luna_calendar(void):QMainWindow()
{
  m_ui.setupUi(this);
  connect(&m_clock_timer,
	  &QTimer::timeout,
	  this,
	  &luna_calendar::slot_clock_timer_timeout);
  connect(m_ui.action_About,
	  &QAction::triggered,
	  this,
	  &luna_calendar::slot_about);
  connect(m_ui.action_Exit,
	  &QAction::triggered,
	  this,
	  &luna_calendar::slot_exit);
  connect(m_ui.menu_View,
	  &QMenu::aboutToShow,
	  this,
	  &luna_calendar::slot_about_to_show_view_menu);
  connect(m_ui.next_month,
	  &QToolButton::clicked,
	  this,
	  &luna_calendar::slot_select_month);
  connect(m_ui.previous_month,
	  &QToolButton::clicked,
	  this,
	  &luna_calendar::slot_select_month);
  connect(m_ui.today,
	  &QToolButton::clicked,
	  this,
	  &luna_calendar::slot_select_month);
  m_clock_timer.start(1000);
  m_ui.action_About->setIcon(QIcon::fromTheme("help-about"));
  m_ui.action_Exit->setIcon(QIcon::fromTheme("application-exit"));
  m_ui.clock->clear();
  m_ui.grid->setHorizontalSpacing(0);
  m_ui.grid->setSpacing(0);
  m_ui.grid->setVerticalSpacing(0);
  m_ui.next_month->setAutoRaise(true);
  m_ui.previous_month->setAutoRaise(true);
  m_ui.today->setAutoRaise(true);
  prepare_database();
  prepare_fonts();
  prepare_month(m_date = QDate::currentDate());
}

luna_calendar::~luna_calendar()
{
}

qint64 luna_calendar::oid(void)
{
  QString const connection_name("oid");
  qint64 oid = 0;

  {
    auto db(QSqlDatabase::addDatabase("QSQLITE", connection_name));

    db.setDatabaseName(home_path() + QDir::separator() + "luna-calendar.db");

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("CREATE TABLE IF NOT EXISTS oid_sequence "
		   "(value INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT)");
	query.exec("CREATE TRIGGER IF NOT EXISTS oid_sequence_trigger "
		   "BEFORE INSERT ON oid_sequence "
		   "BEGIN "
		   "DELETE FROM oid_sequence; "
		   "END;");

	if(query.exec("INSERT INTO oid_sequence VALUES (NULL)"))
	  oid = query.lastInsertId().toLongLong();
	else if(query.exec("SELECT value FROM oid_sequence") && query.next())
	  oid = query.value(0).toLongLong();
      }

    db.close();
  }

  QSqlDatabase::removeDatabase(connection_name);
  return oid;
}

void luna_calendar::assign_image(QPushButton *button, const QColor &color)
{
  if(!button)
    return;

  QImage image(QSize(16, 16), QImage::Format_ARGB32);
  QPainter painter(&image);

  image.fill(color);
  button->setIcon(QPixmap::fromImage(image));
  button->setProperty("color", color);
}

void luna_calendar::prepare_database(void)
{
  QString const connection_name("prepare_database");

  {
    auto db(QSqlDatabase::addDatabase("QSQLITE", connection_name));

    db.setDatabaseName(home_path() + QDir::separator() + "luna-calendar.db");

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("ALTER TABLE event ADD done INTEGER NOT NULL DEFAULT 0");
      }

    db.close();
  }

  QSqlDatabase::removeDatabase(connection_name);
}

void luna_calendar::prepare_fonts(void)
{
  auto font(m_ui.clock->font());

  font.setPointSize(10 + font.pointSize());
  m_ui.clock->setFont(font);
  m_ui.month_year->setFont(font);
  m_ui.next_month->setFont(font);
  m_ui.previous_month->setFont(font);
  m_ui.today->setFont(font);
}

void luna_calendar::prepare_month(const QDate &date)
{
  for(int i = m_ui.grid->count() - 1; i >= 0; i--)
    {
      delete m_ui.grid->itemAt(i)->widget();
      delete m_ui.grid->takeAt(i);
    }

  m_ui.grid->invalidate();

  QStringList days;

  days << tr("Sunday")
       << tr("Monday")
       << tr("Tuesday")
       << tr("Wednesday")
       << tr("Thursday")
       << tr("Friday")
       << tr("Saturday");

  for(int i = 0; i < days.size(); i++)
    {
      QFont font;
      auto label = new QLabel(days[i], this);

      font = label->font();
      font.setBold(true);
      font.setPointSize(5 + font.pointSize());
      label->setFont(font);
      m_ui.grid->addWidget(label, 0, i, Qt::AlignHCenter | Qt::AlignTop);
    }

  auto const first = QDate(date.year(), date.month(), 1).dayOfWeek() + 1;
  auto const next(date.addMonths(1));
  auto const previous(date.addMonths(-1));
  auto const today(QDate::currentDate());

  m_ui.month_year->setText(date.toString("MMMM, yyyy"));

  for(int day = 0, i = 1, j = 1 - first + previous.daysInMonth(), row = 1;
      i <= 42;
      i++)
    {
      auto widget = new luna_day_widget(this);

      m_ui.grid->addWidget(widget, row, (i - 1) % 7);
      widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      if(date.daysInMonth() > day && first <= i)
	{
	  day += 1;
	  widget->set_date(QDate(date.year(), date.month(), day), true);
	  widget->set_day_text(QString::number(day));
	}
      else
	{
	  if(date.daysInMonth() <= day)
	    {
	      widget->set_date
		(QDate(next.year(), next.month(), 1 - day - first + i), false);
	      widget->set_day_text(QString::number(1 - day - first + i));
	    }
	  else
	    {
	      j += 1;
	      widget->set_date
		(QDate(previous.year(), previous.month(), j), false);
	      widget->set_day_text(QString::number(j));
	    }
	}

      if(i % 7 == 0)
	row += 1;
    }
}

void luna_calendar::remove(const qint64 oid)
{
  QString const connection_name("remove");

  {
    auto db(QSqlDatabase::addDatabase("QSQLITE", connection_name));

    db.setDatabaseName(home_path() + QDir::separator() + "luna-calendar.db");

    if(db.open())
      {
	QSqlQuery query(db);

	query.prepare("DELETE FROM event WHERE identifier = ?");
	query.addBindValue(oid);
	query.exec();
	query.exec
	  ("DELETE FROM date WHERE date NOT IN (SELECT date FROM event)");
      }

    db.close();
  }

  QSqlDatabase::removeDatabase(connection_name);
}

void luna_calendar::save(const QDate &date,
			 const QHash<QString, QVariant> &properties,
			 const QString &title,
			 const QTime &end,
			 const QTime &start,
			 const qint64 oid)
{
  QString const connection_name("save");

  {
    auto db(QSqlDatabase::addDatabase("QSQLITE", connection_name));

    db.setDatabaseName(home_path() + QDir::separator() + "luna-calendar.db");

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("CREATE TABLE IF NOT EXISTS date "
		   "(date TEXT NOT NULL PRIMARY KEY)");
	query.exec("CREATE TABLE IF NOT EXISTS event ("
		   "color_background TEXT NOT NULL, "
		   "color_text TEXT NOT NULL, "
		   "date TEXT NOT NULL, "
		   "done INTEGER NOT NULL DEFAULT 0, "
		   "identifier BIGINT NOT NULL, "
		   "time_end TEXT, "
		   "time_start TEXT, "
		   "title TEXT, "
		   "PRIMARY KEY (date, identifier))");
	query.prepare("INSERT OR REPLACE INTO date (date) VALUES (?)");
	query.addBindValue(date.toString(Qt::ISODate));
	query.exec();
	query.prepare("INSERT OR REPLACE INTO event ("
		      "color_background, "
		      "color_text, "
		      "date, "
		      "done, "
		      "identifier, "
		      "time_end, "
		      "time_start, "
		      "title) "
		      "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	query.addBindValue
	  (properties.value("color_background").value<QColor> ().
	   name(QColor::HexArgb));
	query.addBindValue
	  (properties.value("color_text").value<QColor> ().
	   name(QColor::HexArgb));
	query.addBindValue(date.toString(Qt::ISODate));
	query.addBindValue(properties.value("done").toBool());
	query.addBindValue(oid);
	query.addBindValue(end.toString(Qt::ISODate));
	query.addBindValue(start.toString(Qt::ISODate));
	query.addBindValue(title);
	query.exec();
      }

    db.close();
  }

  QSqlDatabase::removeDatabase(connection_name);
}

void luna_calendar::slot_about(void)
{
  auto about = findChild<QMessageBox *> ("about");

  if(!about)
    about = new QMessageBox(this);

  about->setFont(QApplication::font());
  about->setIconPixmap
    (QPixmap(":/luna.png").scaled(QSize(128, 128),
				  Qt::KeepAspectRatio,
				  Qt::SmoothTransformation));
  about->setObjectName("about");
  about->setStandardButtons(QMessageBox::Close);
  about->setText
    (tr("<html>Luna Version %1<br>"
	"Architecture %2.<br>"
	"Made with love by textbrower.<br>"
	"Software for and from the margins.").
     arg(s_version).
     arg(QSysInfo::currentCpuArchitecture()));
  about->setTextFormat(Qt::RichText);
  about->setWindowIcon(windowIcon());
  about->setWindowModality(Qt::NonModal);
  about->setWindowTitle(tr("Luna: About"));
  about->showNormal();
  about->activateWindow();
  about->button(QMessageBox::Close)->setShortcut(tr("Ctrl+W"));
  about->raise();
}

void luna_calendar::slot_about_to_show_view_menu(void)
{
  m_ui.menu_View->clear();

  if(isFullScreen())
    {
      auto action = m_ui.menu_View->addAction
	(tr("Normal"), this, &luna_calendar::showNormal);

      action->setIcon(QIcon::fromTheme("view-restore"));
    }
  else
    {
      auto action = m_ui.menu_View->addAction
	(tr("Full Screen"), this, &luna_calendar::showFullScreen);

      action->setIcon(QIcon::fromTheme("view-fullscreen"));
    }
}

void luna_calendar::slot_clock_timer_timeout(void)
{
  m_ui.clock->setText(QDateTime::currentDateTime().toString("h:mm:ss AP"));
}

void luna_calendar::slot_exit(void)
{
  close();
}

void luna_calendar::slot_select_month(void)
{
  if(m_ui.next_month == sender())
    prepare_month(m_date = m_date.addMonths(1));
  else if(m_ui.previous_month == sender())
    prepare_month(m_date = m_date.addMonths(-1));
  else if(QDate::currentDate() != m_date)
    prepare_month(m_date = QDate::currentDate());
}
