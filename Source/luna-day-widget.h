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

#ifndef luna_day_widget_h
#define luna_day_widget_h

#include <QDate>

#include "ui_luna-day-widget.h"

class QLabel;
class QPushButton;
class QScrollArea;
class QToolButton;

class luna_day_widget: public QWidget
{
  Q_OBJECT

 public:
  luna_day_widget(QWidget *parent);
  ~luna_day_widget();
  void set_date(const QDate &date);
  void set_day_text(const QString &text);

 private:
  QDate m_date;
  QLabel *m_day;
  QScrollArea *m_events_area;
  QToolButton *m_add;
  Ui_luna_day_widget m_ui;
  void add_event(QPushButton *button,
		 const QString &t,
		 const QTime &time,
		 const qint64 oid);
  void prepare_fonts(void);
  void resizeEvent(QResizeEvent *event);

 private slots:
  void slot_add(void);
  void slot_modify(void);
};

#endif
