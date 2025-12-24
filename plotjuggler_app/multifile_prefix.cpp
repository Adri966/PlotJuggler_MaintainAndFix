/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "multifile_prefix.h"
#include "ui_multifile_prefix.h"

#include <QLabel>
#include <QFormLayout>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QSettings>

DialogMultifilePrefix::DialogMultifilePrefix(QStringList filenames, QWidget* parent)
  : QDialog(parent), ui(new Ui::DialogMultifilePrefix)
{
  ui->setupUi(this);

  QVBoxLayout* vlayout = ui->verticalLayoutFrame;

  QSettings settings;

  QStringList prev_prefixes = settings.value("DialogMultifilePrefix::previous").toStringList();

  for (int i = 0; i < prev_prefixes.size(); i += 2)
  {
    _previous_prefixes.insert({ prev_prefixes[i], prev_prefixes[i + 1] });
  }

  // use a list widget that automatically enables scrolls if the number of files is too high
  QListWidget* listWidget = new QListWidget(this);
  listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  listWidget->setMinimumSize(QSize(550, 250));
  vlayout->addWidget(listWidget);

  for (QString filename : filenames)
  {
    auto item = new QListWidgetItem(listWidget);
    auto rowWidget = new QWidget(listWidget);
    auto rowLayout = new QVBoxLayout(rowWidget);

    auto label_file = new QLabel(filename, this);
    label_file->setTextInteractionFlags(Qt::TextSelectableByMouse);
    rowLayout->addWidget(label_file, 1);

    auto form_layout = new QFormLayout();
    auto label = new QLabel("Prefix: ");
    auto line_edit = new QLineEdit();

    form_layout->addRow(label, line_edit);
    rowLayout->addLayout(form_layout);

    item->setSizeHint(rowWidget->sizeHint());
    listWidget->setItemWidget(item, rowWidget);

    if (_previous_prefixes.count(filename))
    {
      line_edit->setText(_previous_prefixes[filename]);
    }
    else
    {
      line_edit->setText(QFileInfo(filename).baseName());
    }
    _prefixes[filename] = line_edit->text();
    _line_edits.insert({ filename, line_edit });
  }
}

std::map<QString, QString> DialogMultifilePrefix::getPrefixes() const
{
  return _prefixes;
}

DialogMultifilePrefix::~DialogMultifilePrefix()
{
  delete ui;
}

void DialogMultifilePrefix::accept()
{
  QSettings settings;

  QStringList prev_prefixes;

  // merge new with old
  for (const auto& [filename, line_edit] : _line_edits)
  {
    _prefixes[filename] = line_edit->text();
    _previous_prefixes[filename] = line_edit->text();
  }

  for (const auto& [filename, prefix] : _previous_prefixes)
  {
    prev_prefixes.push_back(filename);
    prev_prefixes.push_back(prefix);
  }

  settings.setValue("DialogMultifilePrefix::previous", prev_prefixes);

  QDialog::accept();
}
