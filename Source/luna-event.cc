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

#include <QColorDialog>

#include "luna-calendar.h"
#include "luna-event.h"

luna_event::luna_event(QWidget *parent):QDialog(parent)
{
  m_oid = 0;
  m_ui.setupUi(this);
  connect(m_ui.close,
	  &QToolButton::clicked,
	  this,
	  &luna_event::close);
  connect(m_ui.color_background,
	  &QPushButton::clicked,
	  this,
	  &luna_event::slot_select_color);
  connect(m_ui.color_text,
	  &QPushButton::clicked,
	  this,
	  &luna_event::slot_select_color);
  connect(m_ui.remove,
	  &QToolButton::clicked,
	  this,
	  &luna_event::slot_remove);
  connect(m_ui.save,
	  &QPushButton::clicked,
	  this,
	  &luna_event::slot_save);
  connect(m_ui.set_notes,
	  SIGNAL(toggled(bool)),
	  m_ui.notes,
	  SLOT(setVisible(bool)));
  connect(m_ui.time,
	  SIGNAL(toggled(bool)),
	  m_ui.time_box,
	  SLOT(setVisible(bool)));
  connect(m_ui.title,
	  &QLineEdit::returnPressed,
	  this,
	  &luna_event::slot_save);
  luna_calendar::assign_image(m_ui.color_background, QColor(89, 90, 150, 200));
  luna_calendar::assign_image(m_ui.color_text, QColor(Qt::white));
  m_ui.close->setShortcut(tr("Ctrl+W"));
  m_ui.notes->setVisible(false);
  m_ui.save->setAutoDefault(true);
  m_ui.save->setIcon(QIcon::fromTheme("document-save"));
  m_ui.time->setIcon(QIcon::fromTheme("appointment-new"));
  m_ui.time_box->setVisible(false);
  m_ui.title->setFocus();
}

luna_event::~luna_event()
{
}

QColor luna_event::color_background(void) const
{
  return m_ui.color_background->property("color").value<QColor> ();
}

QColor luna_event::color_text(void) const
{
  return m_ui.color_text->property("color").value<QColor> ();
}

QString luna_event::notes(void) const
{
  return m_ui.notes->toPlainText().trimmed();
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

bool luna_event::is_done(void) const
{
  return m_ui.done->isChecked();
}

qint64 luna_event::oid(void) const
{
  return m_oid;
}

void luna_event::set_date(const QDate &date)
{
  m_date = date;
  m_ui.date->setText(date.toString("dddd, MMMM d yyyy"));
}

void luna_event::set_oid(const qint64 oid)
{
  m_oid = oid;
}

void luna_event::set_property(const QString &name, const QVariant &value)
{
  if(name == "color_background")
    luna_calendar::assign_image(m_ui.color_background, value.value<QColor> ());
  else if(name == "color_text")
    luna_calendar::assign_image(m_ui.color_text, value.value<QColor> ());
  else if(name == "done")
    m_ui.done->setChecked(value.toBool());
  else if(name == "notes")
    {
      m_ui.notes->setPlainText(value.toString().trimmed());
      m_ui.set_notes->setChecked
	(!m_ui.notes->toPlainText().trimmed().isEmpty());
    }
}

void luna_event::set_times(const QTime &end, const QTime &start)
{
  m_ui.end->setTime(end);
  m_ui.start->setTime(start);
  m_ui.time->setChecked(end.isValid() && start.isValid());
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

void luna_event::slot_select_color(void)
{
  auto button = qobject_cast<QPushButton *> (sender());

  if(!button)
    return;

  QColorDialog dialog(this);
  auto const color(button->property("color").value<QColor> ());

  dialog.setCurrentColor(color);
  dialog.setOption(QColorDialog::ShowAlphaChannel);

  if(dialog.exec() == QDialog::Accepted)
    luna_calendar::assign_image(button, dialog.selectedColor());
}
