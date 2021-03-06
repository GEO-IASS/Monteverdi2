/*=========================================================================

  Program:   Monteverdi
  Language:  C++


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See Copyright.txt for details.

  Monteverdi is distributed under the CeCILL licence version 2. See
  Licence_CeCILL_V2-en.txt or
  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt for more details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mvdTreeWidget.h"


/*****************************************************************************/
/* INCLUDE SECTION                                                           */

//
// Qt includes (sorted by alphabetic order)
//// Must be included before system/custom includes.

//
// System includes (sorted by alphabetic order)

//
// ITK includes (sorted by alphabetic order)

//
// OTB includes (sorted by alphabetic order)

//
// Monteverdi includes (sorted by alphabetic order)
#include "Core/mvdAlgorithm.h"
#include "Core/mvdDataStream.h"
#include "Gui/mvdTreeWidgetItem.h"

namespace mvd
{

/*
  TRANSLATOR mvd::TreeWidget

  Necessary for lupdate to be aware of C++ namespaces.

  Context comment for translator.
*/

/*****************************************************************************/
/* CONSTANTS                                                                 */

const char*
TreeWidget
::ITEM_MIME_TYPE = "application/x-qtreewidgetitemptrlist";

/*****************************************************************************/
/* STATIC IMPLEMENTATION SECTION                                             */

#if USE_CUSTOM_MIME_DATA

class StaticInitializer
{
public:
  StaticInitializer() :
    m_QTreeWidgetItemPtrMetaTypeId( -1 )
  {
    Initialize();
  }

  ~StaticInitializer()
  {
    Finalize();
  }

private:
  inline
  void
  Initialize()
  {
    //
    // Call qRegisterMetaType<>() to make type available in
    // non-template signatures and serialization.
    m_QTreeWidgetItemPtrMetaTypeId =
      qRegisterMetaType< QTreeWidgetItem* >( "QTreeWidgetItem*" );

    //
    // Register serialization operators for custom meta-types.
#if QT_VERSION >= QT_VERSION_CHECK( 4, 7, 0 )
    qRegisterMetaTypeStreamOperators< QTreeWidgetItem* >();
#else // QT_VERSION >= QT_VERSION_CHECK( 4, 7, 0 )
    qRegisterMetaTypeStreamOperators< QTreeWidgetItem* >(
      QMetaType::typeName( m_QTreeWidgetItemPtrMetaTypeId )
    );
#endif // QT_VERSION >= QT_VERSION_CHECK( 4, 7, 0 )
  }

  inline
  void
  Finalize()
  {
  }

  int m_QTreeWidgetItemPtrMetaTypeId;
};

namespace
{
static const StaticInitializer STATIC_INITIALIZER;
}

#endif // USE_CUSTOM_MIME_DATA

/*****************************************************************************/
QMimeData*
EncodeMimeData( QMimeData* mimeData, const QList< QTreeWidgetItem* >& items )
{
#if USE_CUSTOM_MIME_DATA

  assert( mimeData!=NULL );

  typedef QList< QTreeWidgetItem* > QTreeWidgetItemList;

  QByteArray byteArray;
  QDataStream stream( &byteArray, QIODevice::WriteOnly );

  for( QTreeWidgetItemList::const_iterator it( items.begin() );
       it!=items.end();
       ++it )
    {
    /*
    qDebug()
      << "QTreeWidgetItem::parent()==" << ( *it )->parent();
    qDebug()
      << "Pointer:" << static_cast< void* >( *it );
    qDebug()
      << "Variant:" << QVariant::fromValue< QTreeWidgetItem* >( *it );
    */

#if 1
    QTreeWidgetItem * item = *it;

    qDebug()
      << "Item (encoded):"
      << item << "\n"
      << "text[ 0 ]:" << item->text( 0 ) << "\n"
      << "text[ 1 ]:" << item->text( 1 ) << "\n"
      << "text[ 2 ]:" << item->text( 2 );

    if( item->parent()!=NULL )
      {
      qDebug()
        << "parent:" << item->parent() << "\n"
        << "text[ 0 ]:" << item->parent()->text( 0 ) << "\n"
        << "text[ 1 ]:" << item->parent()->text( 1 );
      }
#endif

    // http://www.qtfr.org/viewtopic.php?id=9630
    // stream << *it;
    stream << QVariant::fromValue< QTreeWidgetItem* >( *it );
    }

  mimeData->setData( TreeWidget::ITEM_MIME_TYPE, byteArray );

  /*
  qDebug() << mimeData->formats();

  for( QTreeWidgetItemList::const_iterator it( items.begin() );
       it!=items.end();
       ++it )
    {
    QTreeWidgetItem* item = *it;

    qDebug()
      << item->type() << item->text( 0 ) << item->text( 1 ) << item->text( 2 );
    }
  */

#endif // USE_CUSTOM_MIME_DATA

  return mimeData;
}

/*****************************************************************************/
int
DecodeMimeData( QList< QTreeWidgetItem* >& items, const QMimeData* mimeData )
{
  assert( mimeData!=NULL );

  int count = 0;

#if USE_CUSTOM_MIME_DATA

  if( !mimeData->hasFormat( TreeWidget::ITEM_MIME_TYPE ) )
    return 0;

  QByteArray byteArray(
    mimeData->data( TreeWidget::ITEM_MIME_TYPE )
  );

  QDataStream stream( &byteArray, QIODevice::ReadOnly );

  //
  // http://www.qtcentre.org/threads/8756-QTreeWidgetItem-mime-type

  QTreeWidgetItem* item = NULL;

  while( !stream.atEnd() )
    {
    QVariant variant;

    stream >> variant;

    // qDebug() << "Variant:" << variant;

    // http://www.qtfr.org/viewtopic.php?id=9630

    item = variant.value< QTreeWidgetItem* >();
    assert( item!=NULL );

    items.push_back( item );

#if 1
    qDebug()
      << "Item (decoded):"
      << item << "\n"
      << "text[ 0 ]:" << item->text( 0 ) << "\n"
      << "text[ 1 ]:" << item->text( 1 ) << "\n"
      << "text[ 2 ]:" << item->text( 2 );

      if( item->parent()!=NULL )
        {
        qDebug()
          << "parent:" << item->parent() << "\n"
          << "text[ 0 ]:" << item->parent()->text( 0 ) << "\n"
          << "text[ 1 ]:" << item->parent()->text( 1 );
        }
#endif

    ++ count;
    }

  // qDebug() << count2 << "items.";

#else // USE_CUSTOM_MIME_DATA

  /*
  if( !mimeData->hasFormat( "application/x-qabstractitemmodeldatalist" ) )
    return 0;

  QByteArray byteArray(
    mimeData->data( "application/x-qabstractitemmodeldatalist" )
  );

  QDataStream stream( &byteArray, QIODevice::ReadOnly );

  //
  // http://www.qtcentre.org/threads/8756-QTreeWidgetItem-mime-type

  QTreeWidgetItem* item = NULL;

  while( !stream.atEnd() )
    {
    ++ count;
    }
  */

#endif // USE_CUSTOM_MIME_DATA

  return count;
}

/*****************************************************************************/
/* CLASS IMPLEMENTATION SECTION                                              */

/*******************************************************************************/
TreeWidget
::TreeWidget( QWidget* parent  ):
  QTreeWidget( parent )
{
  setSelectionBehavior( QAbstractItemView::SelectRows );

  // MANTIS-929: Leave default behaviour
  //
  // setDefaultDropAction( Qt::MoveAction );
  // setDefaultDropAction( Qt::CopyAction );
  // setDefaultDropAction( Qt::LinkAction );

  // MANTIS-929: Setting inspired from QGis; don't understand why
  // there is one call with InternalMode and another with DragDrop but
  // think it might change setting drag-enabled and accept-drop
  // behaviour.
  //
  setDragDropMode( QAbstractItemView::InternalMove );
  setDragEnabled( true );
  setAcceptDrops( true );
  setDragDropMode( QAbstractItemView::DragDrop );
}

/*******************************************************************************/
TreeWidget
::~TreeWidget()
{
}

/*******************************************************************************/
QStringList
TreeWidget
::mimeTypes() const
{
  // qDebug() << this << "::mimeTypes()";

  QStringList mimeTypes( QTreeWidget::mimeTypes() );

#if USE_CUSTOM_MIME_DATA

  mimeTypes << TreeWidget::ITEM_MIME_TYPE;

#endif // USE_CUSTOM_MIME_DATA

  return mimeTypes;
}

/*******************************************************************************/
QMimeData*
TreeWidget
::mimeData( const QList< QTreeWidgetItem* > items ) const
{
  // qDebug() << this << "::mimeData(" << items << ")";

  return EncodeMimeData( QTreeWidget::mimeData( items ), items );
}

/*******************************************************************************/
Qt::DropActions
TreeWidget
::supportedDropActions() const
{
  // This method is overloaded only to spy/debug Qt behavior.

  Qt::DropActions actions = QTreeWidget::supportedDropActions();

  qDebug() << this << "::supportedDropActions():" << actions;

  return actions;
}

/*******************************************************************************/
void
TreeWidget
::startDrag( Qt::DropActions supportedActions )
{
  // This method is overloaded only to spy/debug Qt behavior.

  qDebug() << this << "::startDrag(" << supportedActions << ")";

  QTreeWidget::startDrag( supportedActions );
}

/*******************************************************************************/
void
TreeWidget
::dragEnterEvent( QDragEnterEvent* event )
{
  qDebug() << this << "::dragEnterEvent(" << event << ")";

  // event->acceptProposedAction();

  QTreeWidget::dragEnterEvent( event );
}

/*******************************************************************************/
void 
TreeWidget
::dragMoveEvent( QDragMoveEvent* event )
{
  assert( event!=NULL );

  qDebug() << this << "::dragMoveEvent(" << event << ")";
  qDebug() << this << itemAt( event->pos() );

  QTreeWidget::dragMoveEvent( event );

  QTreeWidgetItem* item = itemAt( event->pos() );
  // const QMimeData * mimeData = event->mimeData();

#if USE_CUSTOM_MIME_DATA
  if( event->mimeData()->hasFormat( TreeWidget::ITEM_MIME_TYPE ) &&
      item!=NULL )
#else // USE_CUSTOM_MIME_DATA
  if( item!=NULL )
#endif // USE_CUSTOM_MIME_DATA
    {
    qDebug() << "ACCEPT";
    event->accept();
    }
  else
    {
    qDebug() << "IGNORE";
    event->ignore();
    }

  /*
  if( event->source()==this )
    {
    event->setDropAction( Qt::MoveAction );
    event->accept();
    }
  */

  /*
  if( itemAt( event->pos() )==NULL )
    event->ignore();
  else
    event->accept();
  */
}

/*******************************************************************************/
void
TreeWidget
::dragLeaveEvent( QDragLeaveEvent* event )
{
  qDebug() << this << "::dragLeaveEvent(" << event << ")";

  QTreeWidget::dragLeaveEvent( event );
}

/*******************************************************************************/
void 
TreeWidget
::dropEvent( QDropEvent* event )
{
  assert( event!=NULL );

  // qDebug() << this << "::dropEvent(" << event << ")";

  typedef QList< QTreeWidgetItem* > QTreeWidgetItemList;

  QTreeWidgetItemList items;

  DecodeMimeData( items, event->mimeData() );

  if( event->source()==this )
    {
    event->setDropAction( Qt::MoveAction );
    // event->accept();
    }

  qDebug() << "dropAction:" << event->dropAction();

  QTreeWidget::dropEvent( event );

  qDebug() << "dropAction:" << event->dropAction();

  /*
  QTreeWidgetItem * item = itemAt( event->pos() );

  while( !item->flags().testFlag( Qt::ItemIsDropEnabled ) )
    {
    item = item->parent();
    assert( item!=NULL ); 

#if 1
    qDebug()
      << "Item (target):"
      << item << "\n"
      << "text[ 0 ]:" << item->text( 0 ) << "\n"
      << "text[ 1 ]:" << item->text( 1 ) << "\n"
      << "text[ 2 ]:" << item->text( 2 ) << "\n"
      << "parent:" << item->parent() << "\n"
      << "text[ 0 ]:" << item->parent()->text( 0 ) << "\n"
      << "text[ 1 ]:" << item->parent()->text( 1 );
#endif
    }
  */

  /*
  for( QTreeWidgetItemList::const_iterator it = items.begin();
       it!=items.end();
       ++it )
    {
    switch( event->dropAction() )
      {
      case Qt::MoveAction:
        emit ItemMoved( *it, itemAt( event->pos() ) );
        break;

      default:
        break;
      }
    }
  */

  QTreeWidgetItem * target = itemAt( event->pos() );

  qDebug() << "itemAt(" << event->pos() << "):" << target;

  if( event->source()==this )
    for( QTreeWidgetItemList::const_iterator it = items.begin();
         it!=items.end();
         ++it )
      emit ItemMoved( *it, target );
}

/*******************************************************************************/
/* SLOTS                                                                       */
/*******************************************************************************/

} // end namespace 'mvd'

/*****************************************************************************/
/* GLOBAL FUNCTIONS IMPLEMENTATION SECTION                                   */

#if USE_CUSTOM_MIME_DATA

#if TREE_WIDGET_ITEM_USE_STREAM_OPERATORS

/*****************************************************************************/
QDataStream&
operator << ( QDataStream& out, QTreeWidgetItem const * item )
{
  /*
  qDebug() <<
    "QDataStream& operator << ( QDataStream&, QTreeWidgetItem const * & );";
  */

#if DATA_STREAM_USE_TEMPLATE_OPERATORS
  return operator << < QTreeWidgetItem >( out, item );

#else // DATA_STREAM_USE_TEMPLATE_OPERATORS
  DATA_STREAM_OUT( out, QTreeWidgetItem, item );

  return out;

#endif // DATA_STREAM_USE_TEMPLATE_OPERATORS
}

/*****************************************************************************/
QDataStream&
operator >>( QDataStream& in, QTreeWidgetItem * & item )
{
  /*
  qDebug() <<
    "QDataStream& operator >> ( QDataStream&, QTreeWidgetItem * & );";
  */

#if DATA_STREAM_USE_TEMPLATE_OPERATORS
  return operator >> < QTreeWidgetItem >( in, item );

#else // DATA_STREAM_USE_TEMPLATE_OPERATORS
  DATA_STREAM_IN( in, QTreeWidgetItem, item );

  return in;

#endif // DATA_STREAM_USE_TEMPLATE_OPERATORS

}

#endif // TREE_WIDGET_ITEM_USE_STREAM_OPERATORS

#endif // USE_CUSTOM_MIME_DATA
