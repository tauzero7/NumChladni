/**
    @file   SegmentListModel.h

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

#ifndef  NUMCHLADNI_SEGLIST_MODEL_H
#define  NUMCHLADNI_SEGLIST_MODEL_H


#include <SystemData.h>

#include <QItemDelegate>

/**
 * @brief The SegBoxDelegate class
 */
class SegBoxDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    SegBoxDelegate( SystemData* data, QObject *parent = 0 );

    QWidget *createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    void  setEditorData   ( QWidget *editor, const QModelIndex &index ) const;
    void  setModelData    ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;
    void  updateEditorGeometry ( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

protected:
    SystemData*  mData;
};


/**
 * @brief The SegmentListModel class
 */
class SegmentListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    SegmentListModel( SystemData* data, QObject *parent = 0);

    int  rowCount     ( const QModelIndex &parent = QModelIndex()  ) const;
    int  columnCount  ( const QModelIndex & parent = QModelIndex() ) const;

    bool insertRows   ( int position, int rows, const QModelIndex & = QModelIndex() );
    bool removeRows   ( int position, int rows, const QModelIndex &index = QModelIndex() );

    QVariant data       ( const QModelIndex &index, int role) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags ( const QModelIndex &index) const;

    bool     setData    ( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void cleanRows ( int position, int rows );

private:
    SystemData*  mData;
};

#endif // NUMCHLADNI_SEGLIST_MODEL_H
