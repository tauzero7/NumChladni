/**
    @file   HoleListModel.cpp

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

#include "HoleListModel.h"


HoleListModel :: HoleListModel( SystemData* data, QObject *parent)
   : QAbstractListModel(parent),
     mData(data) {
}

int HoleListModel::rowCount(const QModelIndex& ) const {
    return mData->m_holes.count();
}

int HoleListModel::columnCount( const QModelIndex& ) const {
    return 2;
}


bool HoleListModel::insertRows(int position, int rows, const QModelIndex&) {
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        hole_t hp;
        hp.num = 0;
        hp.pos = glm::dvec2(mData->m_border.x + mData->m_border.y,
                            mData->m_border.z + mData->m_border.w);
        mData->m_holes.insert(position, hp);
    }

    endInsertRows();
    return true;
}



bool HoleListModel::removeRows(int position, int rows, const QModelIndex &) {
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row = 0; row < rows; ++row) {
        mData->m_holes.removeAt(position);
    }
    endRemoveRows();
    return true;
}


QVariant HoleListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= mData->m_holes.size()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole: {
            if (index.column()==0) {
                return QString::number(mData->m_holes.at(index.row()).pos.x,'f',FLOAT_PREC);
            }
            else if (index.column()==1) {
                return QString::number(mData->m_holes.at(index.row()).pos.y,'f',FLOAT_PREC);
            }
            break;
        }
        case Qt::EditRole: {
            if (index.column()==0) {
                return QString::number(mData->m_holes.at(index.row()).pos.x,'f',FLOAT_PREC);
            }
            else if (index.column()==1) {
                return QString::number(mData->m_holes.at(index.row()).pos.y,'f',FLOAT_PREC);
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


QVariant HoleListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section==0) {
            return QString("x");
        } else if (section==1) {
            return QString("y");
        }
    } else {
        return QString("%1").arg(section+1);
    }
    return QVariant();
}

Qt::ItemFlags HoleListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    if (index.column()==0 || index.column()==1) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
    return QAbstractItemModel::flags(index);
}

bool HoleListModel::setData( const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid())  {

        hole_t n;
        n.num = mData->m_holes.at(index.row()).num;
        n.pos = glm::dvec2(mData->m_holes.at(index.row()).pos.x,
                           mData->m_holes.at(index.row()).pos.y);
        //fprintf(stderr,"%10.8f %10.8f %d\n",pos.x,pos.y,(int)pos.isFixed);

        switch (role) {           
            case Qt::EditRole: {
                if (index.column()==0) {
                    n.pos.x = value.toDouble();
                } else if (index.column()==1) {
                    n.pos.y = value.toDouble();
                }
                break;
            }
        }
        mData->m_holes.replace(index.row(),n);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

