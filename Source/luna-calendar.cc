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

#include "luna-calendar.h"
#include "luna-day-widget.h"

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
  connect(m_ui.action_Exit,
	  &QAction::triggered,
	  this,
	  &luna_calendar::slot_exit);
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
  m_ui.clock->clear();
  m_ui.grid->setHorizontalSpacing(0);
  m_ui.grid->setSpacing(0);
  m_ui.grid->setVerticalSpacing(0);
  m_ui.next_month->setAutoRaise(true);
  m_ui.previous_month->setAutoRaise(true);
  m_ui.today->setAutoRaise(true);
  prepare_fonts();
  prepare_month(m_date = QDate::currentDate());
}

luna_calendar::~luna_calendar()
{
}

void luna_calendar::prepare_fonts(void)
{
  auto font(m_ui.clock->font());

  font.setBold(true);
  font.setPointSize(10 + font.pointSize());
  m_ui.clock->setFont(font);
  m_ui.month_year->setFont(font);
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
  auto const previous(date.addMonths(-1));
  auto const today(QDate::currentDate());

  m_ui.month_year->setText(date.toString("MMMM, yyyy"));

  for(int day = 0, i = 1, j = 1 - first + previous.daysInMonth(), row = 1;
      i <= 42;
      i++)
    {
      auto widget = new luna_day_widget(this);

      m_ui.grid->addWidget(widget, row, (i - 1) % 7);
      widget->setEnabled(false);
      widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      if(date.daysInMonth() > day && first <= i)
	{
	  day += 1;
	  widget->setEnabled(true);
	  widget->set_date(QDate(date.year(), date.month(), day));
	  widget->set_day_text(QString::number(day));
	  widget->set_down(date.day() == day && today == date);
	}
      else
	{
	  if(date.daysInMonth() <= day)
	    widget->set_day_text(QString::number(1 - day - first + i));
	  else
	    widget->set_day_text(QString::number(++j));
	}

      if(i % 7 == 0)
	row += 1;
    }
}

void luna_calendar::slot_about(void)
{
}

void luna_calendar::slot_clock_timer_timeout(void)
{
  m_ui.clock->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
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
  else
    prepare_month(m_date = QDate::currentDate());
}
