/**
    @file   SegmentListModel.cpp

    Copyright (c) 2013, Universitaet Stuttgart, VISUS, Thomas Mueller

    This file is part of NumChladni.

    NumChladni is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NumChladni is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NumChladni.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SegmentListModel.h"

#include <QSpinBox>

SegBoxDelegate::SegBoxDelegate(SystemData *data, QObject *parent)
    : QItemDelegate(parent),
      mData(data) {
}

QWidget *SegBoxDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    QSpinBox *editor = new QSpinBox(parent);
    editor->setRange(-1,MAX_NUM_CTRL_POINTS);
    return editor;
}

void SegBoxDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const {
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox *edit = static_cast<QSpinBox*>(editor);
    edit->setRange(1,mData->m_vertices.size());
    edit->setValue(value);
}

void SegBoxDelegate::setModelData( QWidget *editor, QAbstractItemModel *model,const QModelIndex &index ) const {
    QSpinBox *edit = static_cast<QSpinBox*>(editor);
    edit->setRange(1,mData->m_vertices.size());
    int value = edit->value();
    model->setData(index, value, Qt::EditRole);
}

void SegBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                             const QModelIndex & ) const {
    editor->setGeometry(option.rect);
}


// --------------------------------------------------------------------------------

SegmentListModel :: SegmentListModel( SystemData* data, QObject *parent)
   : QAbstractListModel(parent) {
    mData = data;
}

int SegmentListModel::rowCount(const QModelIndex& ) const {
    return mData->m_segments.count();
}

int SegmentListModel::columnCount( const QModelIndex& ) const {
    return 2;
}

bool SegmentListModel::insertRows(int position, int rows, const QModelIndex&) {
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        segment_t seg;
        seg.num = 0;
        seg.p1 = 1;
        seg.p2 = 1;
        seg.bmarker = 0;
        mData->m_segments.insert(position,seg);
    }

    endInsertRows();
    return true;
}

bool SegmentListModel::removeRows(int position, int rows, const QModelIndex &) {
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        mData->m_segments.removeAt(position);
    }

    endRemoveRows();
    return true;
}

void SegmentListModel::cleanRows( int position, int rows ) {
    beginRemoveRows(QModelIndex(),position,position+rows-1);
    endRemoveRows();
}

QVariant SegmentListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= mData->m_segments.size()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole: {
            if (index.column()==0) {
                return QString::number(mData->m_segments.at(index.row()).p1);
            }
            else if (index.column()==1) {
                return QString::number(mData->m_segments.at(index.row()).p2);
            }
            if (index.column()==3) {
               return QVariant();
               break;
            }
            break;
        }
        case Qt::EditRole: {
            if (index.column()==0) {
                return QString::number(mData->m_segments.at(index.row()).p1);
            }
            else if (index.column()==1) {
                return QString::number(mData->m_segments.at(index.row()).p2);
            }
            break;
        }
        case Qt::TextAlignmentRole: {
            return Qt::AlignRight + Qt::AlignVCenter;
            break;
        }
    }
    return QVariant();
}

QVariant SegmentListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section==0) {
            return QString("p1");
        } else if (section==1) {
            return QString("p2");
        }
    } else {
        return QString("%1").arg(section+1);
    }
    return QVariant();
}

Qt::ItemFlags SegmentListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    if (index.column()==0 || index.column()==1) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    } else if (index.column()==2) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
    return QAbstractItemModel::flags(index);
}

bool SegmentListModel::setData( const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid())  {

        segment_t  seg;
        seg.num = mData->m_segments.at(index.row()).num;
        seg.p1 = mData->m_segments.at(index.row()).p1;
        seg.p2 = mData->m_segments.at(index.row()).p2;
        seg.bmarker = mData->m_segments.at(index.row()).bmarker;

        switch (role) {
            case Qt::EditRole: {
                if (index.column()==0) {
                    seg.p1 = value.toUInt();
                } else if (index.column()==1) {
                    seg.p2 = value.toUInt();
                }
                break;
            }
        }
        mData->m_segments.replace(index.row(),seg);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

