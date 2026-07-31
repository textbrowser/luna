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

#include <QDir>
#include <QLabel>
#include <QScrollArea>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QToolButton>

#include "luna-calendar.h"
#include "luna-day-widget.h"
#include "luna-event.h"

luna_day_widget::luna_day_widget(QWidget *parent):QWidget(parent)
{
  m_add = nullptr;
  m_day = new QLabel(this);
  m_day->move(10, 5);
  m_day->setEnabled(false);
  m_events_area = new QScrollArea(this);
  m_events_area->move(50, 10);
  m_events_area->setWidget(new QWidget(this));
  m_events_area->setWidgetResizable(true);
  m_events_area->widget()->setLayout(new QVBoxLayout(m_events_area->widget()));
  m_events_area->widget()->setVisible(true);
  m_ui.setupUi(this);
  prepare_fonts();
}

luna_day_widget::~luna_day_widget()
{
}

void luna_day_widget::add_event
(QPushButton *event,
 const QHash<QString, QVariant> &properties,
 const QString &t,
 const QTime &end,
 const QTime &start,
 const bool save,
 const qint64 oid)
{
  QString title("");

  if(start.isValid())
    {
      if(t.isEmpty())
	title = QString("%1 (%2)").
	  arg(start.toString("h:mm AP")).
	  arg(tr("No Title"));
      else
	title = QString("%1 (%2)").
	  arg(start.toString("h:mm AP")).
	  arg(t);
    }
  else if(t.isEmpty())
    title = tr("No Title");
  else
    title = t;

  if(!event)
    {
      event = new QPushButton(this);

      auto color_background
	(properties.value("color_background").value<QColor> ());
      auto color_text
	(properties.value("text_background").value<QColor> ());
      auto font(event->font());

      if(!color_background.isValid())
	color_background = QColor(89, 90, 150, 200);

      if(!color_text.isValid())
	color_text = QColor(Qt::white);

      connect(event,
	      &QPushButton::clicked,
	      this,
	      &luna_day_widget::slot_modify);
      font.setBold(true);
      event->setFont(font);
      event->setObjectName(QString::number(oid));
      event->setStyleSheet
	(QString("QPushButton {background: %1; color: %2;}").
	 arg(color_background.name(QColor::HexArgb)).
	 arg(color_text.name(QColor::HexArgb)));
      m_events_area->widget()->layout()->addWidget(event);
      m_events_area->widget()->layout()->setAlignment(Qt::AlignTop);
      m_events_area->widget()->layout()->setSpacing(1);
    }

  event->setProperty("end", end);
  event->setProperty("oid", oid);
  event->setProperty("start", start);
  event->setProperty("title", t);
  event->setText(title);
  event->setVisible(true);
  save ? luna_calendar::save(m_date, properties, t, end, start, oid) : (void) 0;
}

void luna_day_widget::prepare_fonts(void)
{
  auto font(m_day->font());

  font.setBold(true);
  m_day->setFont(font);
  m_day->resize
    (QFontMetrics(m_day->font()).boundingRect("00").size() + QSize(10, 10));
}

void luna_day_widget::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  m_events_area->resize(-100 + size().width(), -25 + size().height());
}

void luna_day_widget::set_date(const QDate &date, const bool add_button)
{
  m_date = date;

  if(add_button && m_add == nullptr && m_date.isValid())
    {
      m_add = new QToolButton(this);
      m_add->move(5, 5 + m_day->height());
      m_add->resize(50, 50);
      m_add->setAutoRaise(true);
      m_add->setDown(QDate::currentDate() == m_date);
      m_add->setIcon(QIcon(":/64x64/new.png"));
      m_add->setIconSize(QSize(32, 32));
      m_add->setToolTip(tr("Add Event"));
      m_day->setEnabled(true);
      connect(m_add,
	      &QToolButton::clicked,
	      this,
	      &luna_day_widget::slot_add);
    }

  QString const connection_name("set_date");

  {
    auto db(QSqlDatabase::addDatabase("QSQLITE", connection_name));

    db.setDatabaseName
      (luna_calendar::home_path() + QDir::separator() + "luna-calendar.db");

    if(db.open())
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);
	query.prepare("SELECT "
		      "color_background, "
		      "color_text, "
		      "identifier, "
		      "time_end, "
		      "time_start, "
		      "title "
		      "FROM event WHERE date = ? "
		      "ORDER BY identifier");
	query.addBindValue(m_date.toString(Qt::ISODate));

	if(query.exec())
	  while(query.next())
	    {
	      QHash<QString, QVariant> properties;
	      auto const end
		(QTime::
		 fromString(query.value("time_end").toString(), Qt::ISODate));
	      auto const oid = query.value("identifier").toLongLong();
	      auto const start
		(QTime::
		 fromString(query.value("time_start").toString(), Qt::ISODate));
	      auto const title(query.value("title").toString().trimmed());

	      properties["color_background"] = QColor
		(query.value("color_background").toString().trimmed());
	      properties["color_text"] = QColor
		(query.value("color_text").toString().trimmed());
	      add_event(nullptr, properties, title, end, start, false, oid);
	    }
      }

    db.close();
  }

  QSqlDatabase::removeDatabase(connection_name);
}

void luna_day_widget::set_day_text(const QString &text)
{
  m_day->setText(text);
  prepare_fonts();
}

void luna_day_widget::slot_add(void)
{
  m_add ? m_add->setDown(QDate::currentDate() == m_date) : (void) 0;

  auto event = new luna_event(this);

  connect(event,
	  &luna_event::save,
	  this,
	  &luna_day_widget::slot_save);
  connect(event,
	  SIGNAL(remove(const qint64)),
	  this,
	  SLOT(slot_remove(const qint64)));
  event->setAttribute(Qt::WA_DeleteOnClose);
  event->resize(event->sizeHint());
  event->setModal(false);
  event->set_date(m_date);
  event->set_oid(luna_calendar::oid());
  event->show();
}

void luna_day_widget::slot_modify(void)
{
  auto button = qobject_cast<QPushButton *> (sender());

  if(!button)
    return;

  auto event = new luna_event(this);

  connect(event,
	  &luna_event::save,
	  this,
	  &luna_day_widget::slot_save);
  connect(event,
	  SIGNAL(remove(const qint64)),
	  this,
	  SLOT(slot_remove(const qint64)));
  event->setAttribute(Qt::WA_DeleteOnClose);
  event->resize(event->sizeHint());
  event->setModal(false);
  event->set_date(m_date);
  event->set_oid(button->property("oid").toLongLong());
  event->set_times
    (button->property("end").toTime(), button->property("start").toTime());
  event->set_title(button->property("title").toString());
  event->show();
}

void luna_day_widget::slot_remove(const qint64 oid)
{
  auto button = m_events_area->findChild<QPushButton *> (QString::number(oid));

  if(button)
    button->deleteLater();

  luna_calendar::remove(oid);
}

void luna_day_widget::slot_save(void)
{
  auto event = qobject_cast<luna_event *> (sender());

  if(!event)
    return;

  QHash<QString, QVariant> properties;
  auto button = m_events_area->findChild<QPushButton *>
    (QString::number(event->oid()));

  properties["color_background"] = event->color_background();
  properties["color_text"] = event->color_text();
  add_event(button,
	    properties,
	    event->title(),
	    event->end(),
	    event->start(),
	    true,
	    event->oid());
}
