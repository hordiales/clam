/*
 * Copyright (c) 2001-2003 MUSIC TECHNOLOGY GROUP (MTG)
 *                         UNIVERSITAT POMPEU FABRA
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "Qt_ProcessingConfigPresentation.hxx"
#include "ActionButton.hxx"
#include <qlabel.h>
#include <qgroupbox.h>  
#include <qlayout.h>
#include <qhbox.h>
#include <qsizepolicy.h>

#include "ProcessingConfig.hxx"

namespace NetworkGUI
{

Qt_ProcessingConfigPresentation::Qt_ProcessingConfigPresentation( QWidget *parent , const char *name )
	: QDialog(parent, name)
{
	if ( !name )
		setName( "ProcessingConfigDesigner" );

	setCaption( tr( "Processing Config" ) );

	QVBoxLayout * top =new QVBoxLayout(this);

	mAttributeContainer = new QVBox(this,"AttributeContainer");
	top->addWidget(mAttributeContainer);

	top->addStretch(10);
	top->setSpacing(22);

	QHBox * buttonBox = new QHBox(this,"buttonBox" );
	top->addWidget(buttonBox);

	buttonBox->setSpacing(20);
	buttonBox->setMargin(10);
	
	mOkButton = new ActionButton( buttonBox, "mOkButton" );

	mCancelButton = new ActionButton( buttonBox, "mCancelButton" );
	
	mOkButton->setText( tr( "Ok" ) );
	mCancelButton->setText( tr( "Cancel" ) );

	SlotOk.Wrap( this, &Qt_ProcessingConfigPresentation::Ok );
	SlotCancel.Wrap( this, &Qt_ProcessingConfigPresentation::Cancel );

	mOkButton->Pressed.Connect( SlotOk );
	mCancelButton->Pressed.Connect( SlotCancel );

}
	
void Qt_ProcessingConfigPresentation::Ok( bool )
{
	ConfigureProcessing();
	close();
}

void Qt_ProcessingConfigPresentation::Cancel( bool )
{
	close();
}

void Qt_ProcessingConfigPresentation::Show()
{
	show();
}

void Qt_ProcessingConfigPresentation::Hide()
{
	hide();
}


} // namespace NetworkGUI
