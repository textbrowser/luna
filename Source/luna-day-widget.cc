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

#include "luna-day-widget.h"

luna_day_widget::luna_day_widget(QWidget *parent):QWidget(parent)
{
  m_day = new QLabel(this);
  m_day->move(10, 1);
  m_modify = nullptr;
  m_ui.setupUi(this);
  connect(m_ui.information,
	  &QToolButton::clicked,
	  this,
	  &luna_day_widget::slot_clicked);
  prepare_fonts();
}

luna_day_widget::~luna_day_widget()
{
}

void luna_day_widget::prepare_fonts(void)
{
  auto font(m_day->font());

  font.setBold(true);
  m_day->setFont(font);
}

void luna_day_widget::set_date(const QDate &date)
{
  m_date = date;

  if(m_date.isValid() && m_modify == nullptr)
    {
      m_modify = new QToolButton(this);
      m_modify->move(10, m_day->height());
      m_modify->resize(35, 35);
      m_modify->setAutoRaise(true);
      m_modify->setText("...");
      m_modify->setToolTip(tr("Events"));
      connect(m_modify,
	      &QToolButton::clicked,
	      this,
	      &luna_day_widget::slot_modify);
    }
}

void luna_day_widget::set_day_text(const QString &text)
{
  m_day->setText(text);
}

void luna_day_widget::slot_clicked(void)
{
  if(QDate::currentDate() == m_date)
    {
      m_ui.information->blockSignals(true);
      m_ui.information->setDown(true);
      m_ui.information->blockSignals(false);
    }
}

void luna_day_widget::slot_modify(void)
{
}
