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

#include "luna-event.h"

luna_event::luna_event(QWidget *parent):QDialog(parent)
{
  m_oid = 0;
  m_ui.setupUi(this);
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &luna_event::close);
  connect(m_ui.remove,
	  &QPushButton::clicked,
	  this,
	  &luna_event::slot_remove);
  connect(m_ui.save,
	  &QPushButton::clicked,
	  this,
	  &luna_event::slot_save);
  connect(m_ui.time,
	  SIGNAL(toggled(bool)),
	  m_ui.end,
	  SLOT(setEnabled(bool)));
  connect(m_ui.time,
	  SIGNAL(toggled(bool)),
	  m_ui.start,
	  SLOT(setEnabled(bool)));
}

luna_event::~luna_event()
{
}

QString luna_event::title(void) const
{
  return m_ui.title->text().trimmed();
}

QTime luna_event::end(void) const
{
  return m_ui.time->isChecked() ? m_ui.end->time() : QTime();
}

QTime luna_event::start(void) const
{
  return m_ui.time->isChecked() ? m_ui.start->time() : QTime();
}

qint64 luna_event::oid(void) const
{
  return m_oid;
}

void luna_event::set_date(const QDate &date)
{
  m_date = date;
  m_ui.date->setText(date.toString("dddd, MMMM d"));
}

void luna_event::set_title(const QString &title)
{
  m_ui.title->setText(title);
}

void luna_event::slot_remove(void)
{
  emit remove(m_oid);
  reject();
}

void luna_event::slot_save(void)
{
  emit save();
  accept();
}
