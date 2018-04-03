/**
    @file   PointListModel.cpp

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

#include "PointListModel.h"
#include "DoubleEdit.h"

#include <QFocusEvent>

DoubleBoxDelegate::DoubleBoxDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *DoubleBoxDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    DoubleEdit *editor = new DoubleEdit(FLOAT_PREC,0.0,0.01,parent);
    editor->setRange(-1000.0,1000.0);
    return editor;
}

void DoubleBoxDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const {
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    DoubleEdit *edit = static_cast<DoubleEdit*>(editor);
    edit->setValue(value);
}

void DoubleBoxDelegate::setModelData( QWidget *editor, QAbstractItemModel *model,const QModelIndex &index ) const {
    DoubleEdit *edit = static_cast<DoubleEdit*>(editor);
    double value = edit->getValue();
    model->setData(index, value, Qt::EditRole);
}

void DoubleBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                             const QModelIndex & ) const {
    editor->setGeometry(option.rect);
}


// --------------------------------------------------------------------------------

PointListModel :: PointListModel( SystemData* data, QObject *parent)
   : QAbstractListModel(parent),
     mData(data) {
}

int PointListModel::rowCount(const QModelIndex& ) const {
    return mData->m_vertices.count();
}

int PointListModel::columnCount( const QModelIndex& ) const {
    return 3;
}

bool PointListModel::insertRows(int position, int rows, const QModelIndex&) {
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        node_t np;
        np.vIdx = 0;
        np.pos = glm::dvec2(mData->m_border.x + mData->m_border.y,
                            mData->m_border.z + mData->m_border.w);
        np.bmarker = 1;
        np.isFixed = false;
        mData->m_vertices.insert(position, np);
    }

    endInsertRows();
    return true;
}

bool PointListModel::removeRows(int position, int rows, const QModelIndex &) {
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        mData->m_vertices.removeAt(position);
    }

    endRemoveRows();
    return true;
}

QVariant PointListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= mData->m_vertices.size()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole: {
            if (index.column()==0) {
                return QString::number(mData->m_vertices.at(index.row()).pos.x,'f',FLOAT_PREC);
            }
            else if (index.column()==1) {
                return QString::number(mData->m_vertices.at(index.row()).pos.y,'f',FLOAT_PREC);
            }
            if (index.column()==3) {
               return QVariant();
               break;
            }
            break;
        }
        case Qt::EditRole: {
            if (index.column()==0) {
                return QString::number(mData->m_vertices.at(index.row()).pos.x,'f',FLOAT_PREC);
            }
            else if (index.column()==1) {
                return QString::number(mData->m_vertices.at(index.row()).pos.y,'f',FLOAT_PREC);
            }
            break;
        }
        case Qt::CheckStateRole: {
            if (index.column()==2) {
                if (mData->m_vertices.at(index.row()).isFixed) {
                    return Qt::Checked;
                } else {
                    return Qt::Unchecked;
                }
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


QVariant PointListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section==0) {
            return QString("x");
        } else if (section==1) {
            return QString("y");
        } else if (section==2) {
            return QString("fix");
        }
    } else {
        return QString("%1").arg(section+1);
    }
    return QVariant();
}

Qt::ItemFlags PointListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    if (index.column()==0 || index.column()==1) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    } else if (index.column()==2) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
    }
    return QAbstractItemModel::flags(index);
}

bool PointListModel::setData( const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid())  {

        node_t n;
        n.vIdx = mData->m_vertices.at(index.row()).vIdx;
        n.pos = glm::dvec2(mData->m_vertices.at(index.row()).pos.x,
                           mData->m_vertices.at(index.row()).pos.y);
        n.bmarker = mData->m_vertices.at(index.row()).bmarker;
        n.isFixed = mData->m_vertices.at(index.row()).isFixed;
        //fprintf(stderr,"%10.8f %10.8f %d\n",pos.x,pos.y,(int)pos.isFixed);

        switch (role) {
            case Qt::CheckStateRole: {
                if (index.column()==2) {
                    if (value==Qt::Checked) {
                        n.bmarker = BOUNDARY_FIXED_MARKER;
                        n.isFixed = true;
                    } else if (value!=Qt::Checked){
                        n.bmarker = 1;
                        n.isFixed = false;
                    }
                }
                break;
            }
            case Qt::EditRole: {
                if (index.column()==0) {
                    n.pos.x = value.toDouble();
                } else if (index.column()==1) {
                    n.pos.y = value.toDouble();
                } else if (index.column()==2) {
                    n.isFixed = value.toBool();
                    if (n.isFixed) {
                        n.bmarker = BOUNDARY_FIXED_MARKER;
                    } else {
                        n.bmarker = 1;
                    }
                }
                break;
            }
        }
        mData->m_vertices.replace(index.row(),n);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}



// --------------------
TableView::TableView( SystemData* data, QWidget *parent ) :
    QTableView(parent),
    mData(data) {
    setFocusPolicy(Qt::StrongFocus);
}

void TableView::focusInEvent( QFocusEvent * event ) {
    //std::cerr << "Focus in: " << event->reason() << std::endl;
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    event->ignore();
}

void TableView::focusOutEvent( QFocusEvent * event ) {
    //std::cerr << "Focus out: " << event->reason() << std::endl;
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->clearSelection();
    mData->m_activePoint = -1;
    event->ignore();
}

