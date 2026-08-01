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

#ifndef luna_event_h
#define luna_event_h

#include "ui_luna-event.h"

class luna_event: public QDialog
{
  Q_OBJECT

 public:
  luna_event(QWidget *parent);
  ~luna_event();
  QColor color_background(void) const;
  QColor color_text(void) const;
  QString title(void) const;
  QTime end(void) const;
  QTime start(void) const;
  bool is_done(void) const;
  qint64 oid(void) const;
  void set_date(const QDate &date);
  void set_oid(const qint64 oid);
  void set_property(const QString &name, const QVariant &value);
  void set_times(const QTime &end, const QTime &start);
  void set_title(const QString &title);

 private:
  QDate m_date;
  Ui_luna_event m_ui;
  qint64 m_oid;

 private slots:
  void slot_remove(void);
  void slot_save(void);
  void slot_select_color(void);

 signals:
  void remove(const qint64 oid);
  void save(void);
};

#endif
