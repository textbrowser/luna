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

#include <QLabel>
#include <QScrollArea>

#include "luna-day-widget.h"
#include "luna-event.h"

luna_day_widget::luna_day_widget(QWidget *parent):QWidget(parent)
{
  m_add = nullptr;
  m_day = new QLabel(this);
  m_day->move(10, 5);
  m_events = new QLabel(this);
  m_events_area = new QScrollArea(this);
  m_events_area->move(50, 10);
  m_events_area->setWidget(new QWidget(this));
  m_events_area->setWidgetResizable(true);
  m_events_area->widget()->setLayout(new QVBoxLayout(m_events_area->widget()));
  m_events_area->widget()->setVisible(true);
  m_ui.setupUi(this);
  connect(m_ui.button,
	  &QToolButton::clicked,
	  this,
	  &luna_day_widget::slot_clicked);
  prepare_fonts();
}

luna_day_widget::~luna_day_widget()
{
}

void luna_day_widget::add_event(const QString &title, const qint64 oid)
{
  auto event = new QPushButton(this);

  event->setProperty("oid", oid);
  event->setText(title.isEmpty() ? tr("No Title") : title);
  event->setVisible(true);
  m_events_area->widget()->layout()->addWidget(event);
}

void luna_day_widget::prepare_fonts(void)
{
  auto font(m_day->font());

  font.setBold(true);
  m_day->setFont(font);
  m_day->resize(QFontMetrics(m_day->font()).boundingRect("00").size());
  font = m_events->font();
  font.setBold(true);
  font.setPointSize(10 + font.pointSize());
  m_events->move(50, 5);
  m_events->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_events->setFont(font);
  m_events->resize(m_events->sizeHint());
}

void luna_day_widget::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  m_events_area->resize(-100 + size().width(), -25 + size().height());
}

void luna_day_widget::set_date(const QDate &date)
{
  m_date = date;

  if(m_add == nullptr && m_date.isValid())
    {
      m_add = new QToolButton(this);
      m_add->move(10, 5 + m_day->height());
      m_add->resize(35, 35);
      m_add->setIcon(QIcon(":/32x32/new.svg"));
      m_add->setIconSize(QSize(32, 32));
      m_add->setToolTip(tr("Add Event"));
      connect(m_add,
	      &QToolButton::clicked,
	      this,
	      &luna_day_widget::slot_add);
    }
}

void luna_day_widget::set_day_text(const QString &text)
{
  m_day->setText(text);
}

void luna_day_widget::slot_add(void)
{
  luna_event event(this);

  event.resize(event.sizeHint());
  event.set_date(m_date);

  if(event.exec() == QDialog::Accepted)
    add_event(event.title(), event.oid());
}

void luna_day_widget::slot_clicked(void)
{
  if(QDate::currentDate() == m_date)
    {
      m_ui.button->blockSignals(true);
      m_ui.button->setDown(true);
      m_ui.button->blockSignals(false);
    }
}
