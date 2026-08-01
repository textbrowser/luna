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

#include "luna-calendar.h"
#include "luna-day-widget.h"
#include "luna-event.h"

luna_day_widget::luna_day_widget(QWidget *parent):QWidget(parent)
{
  m_day = new QLabel(this);
  m_day->move(10, 10);
  m_day->setAlignment(Qt::AlignCenter);
  m_day->setEnabled(false);
  m_events_area = new QScrollArea(this);
  m_events_area->move(10 + m_day->width(), 0);
  m_events_area->setStyleSheet("QScrollArea {background: transparent;}");
  m_events_area->setWidget(new QWidget(this));
  m_events_area->setWidgetResizable(true);
  m_events_area->widget()->setLayout(new QVBoxLayout(m_events_area->widget()));
  m_events_area->widget()->setVisible(true);
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents);
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
      QString s("");

      if(start.minute() == 0)
	s = start.toString("h AP");
      else
	s = start.toString("h:mm AP");

      if(t.isEmpty())
	title = QString("%1 (%2)").arg(s).arg(tr("No Title"));
      else
	title = QString("%1 (%2)").arg(s).arg(t);
    }
  else if(t.isEmpty())
    title = tr("No Title");
  else
    title = t;

  if(!event)
    {
      event = new QPushButton(this);
      connect(event,
	      &QPushButton::clicked,
	      this,
	      &luna_day_widget::slot_modify);
      event->setProperty("done", properties.value("done"));
      m_events_area->widget()->layout()->addWidget(event);
      m_events_area->widget()->layout()->setAlignment(Qt::AlignTop);
      m_events_area->widget()->layout()->setSpacing(1);
    }

  auto color_background
    (properties.value("color_background").value<QColor> ());
  auto color_text
    (properties.value("color_text").value<QColor> ());

  if(!color_background.isValid())
    color_background = QColor(89, 90, 150, 200);

  if(!color_text.isValid())
    color_text = QColor(Qt::white);

  event->setObjectName(QString::number(oid));
  event->setProperty("color_background", color_background);
  event->setProperty("color_text", color_text);
  event->setProperty("done", properties.value("done"));
  event->setProperty("end", end);
  event->setProperty("oid", oid);
  event->setProperty("start", start);
  event->setProperty("title", t);
  event->setStyleSheet
    (QString("QPushButton {background: %1; color: %2;}").
     arg(color_background.name(QColor::HexArgb)).
     arg(color_text.name(QColor::HexArgb)));
  event->setText(title);
  event->setVisible(true);
  prepare_fonts();
  save ? luna_calendar::save(m_date, properties, t, end, start, oid) : (void) 0;
}

void luna_day_widget::mouseDoubleClickEvent(QMouseEvent *event)
{
  QWidget::mouseDoubleClickEvent(event);
  slot_add();
}

void luna_day_widget::prepare_fonts(void)
{
  for(int i = 0; i < m_events_area->widget()->layout()->count(); i++)
    {
      QFont font;
      auto widget = m_events_area->widget()->layout()->itemAt(i)->widget();

      font = widget->font();
      font.setBold(true);
      font.setStrikeOut(widget->property("done").toBool());
      widget->setFont(font);
    }

  auto font(m_day->font());

  font.setBold(true);
  m_day->setFont(font);
  m_day->resize
    (QFontMetrics(m_day->font()).boundingRect("00").size() + QSize(10, 10));
  m_events_area->move(10 + m_day->width(), 0);
}

void luna_day_widget::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  m_events_area->resize(-50 + size().width(), -15 + size().height());
}

void luna_day_widget::set_date(const QDate &date, const bool enabled)
{
  m_date = date;
  m_day->setEnabled(enabled);

  if(QDate::currentDate() == date)
    m_day->setStyleSheet
      ("QLabel {background: #3969ef; border-radius: 10px; color: white;}");

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
		      "done, "
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
	      properties["done"] = query.value("done").toBool();
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
  event->set_property("color_background", button->property("color_background"));
  event->set_property("color_text", button->property("color_text"));
  event->set_property("done", button->property("done"));
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
  properties["done"] = event->is_done();
  add_event(button,
	    properties,
	    event->title(),
	    event->end(),
	    event->start(),
	    true,
	    event->oid());
}
