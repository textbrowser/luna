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
#include <QToolButton>

#include "luna-calendar.h"

luna_calendar::luna_calendar(void):QMainWindow()
{
  m_ui.setupUi(this);
  m_ui.grid->setHorizontalSpacing(0);
  m_ui.grid->setSpacing(0);
  m_ui.grid->setVerticalSpacing(0);
  connect(m_ui.action_Exit,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slot_exit(void)));
  prepare_month();
}

luna_calendar::~luna_calendar()
{
}

void luna_calendar::prepare_month(void)
{
  auto font(m_ui.month->font());

  font.setBold(true);
  font.setPointSize(10 + font.pointSize());
  m_ui.month->setFont(font);

  while(m_ui.grid->itemAt(0))
    {
      delete m_ui.grid->itemAt(0)->widget();
      delete m_ui.grid->takeAt(0);
    }

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
      auto label = new QLabel(days[i], this);

      font = label->font();
      font.setBold(true);
      font.setPointSize(5 + font.pointSize());
      label->setFont(font);
      m_ui.grid->addWidget(label, 0, i, Qt::AlignHCenter | Qt::AlignTop);
    }

  auto const date(QDate::currentDate());
  auto const first = date.daysInMonth() / 7;
  auto const previous(date.addMonths(-1));

  m_ui.month->setText(date.toString("MMMM"));

  for(int day = 0, i = 1, j = 1 - first + previous.daysInMonth(), row = 1;
      i <= 42;
      i++)
    {
      auto tool_button = new QToolButton(this);

      m_ui.grid->addWidget(tool_button, row, (i - 1) % 7);
      tool_button->setAutoRaise(true);
      tool_button->setEnabled(false);
      tool_button->setSizePolicy
	(QSizePolicy::Expanding, QSizePolicy::Expanding);

      if(date.daysInMonth() > day && first <= i)
	{
	  day += 1;
	  tool_button->setDown(date.day() == day);
	  tool_button->setEnabled(true);
	  tool_button->setText(QString::number(day));
	}
      else
	{
	  if(date.daysInMonth() <= day)
	    tool_button->setText(QString::number(1 - day - first + i));
	  else
	    tool_button->setText(QString::number(++j));
	}

      if(i % 7 == 0)
	row += 1;
    }
}

void luna_calendar::slot_exit(void)
{
  close();
}

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
