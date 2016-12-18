/*-----------------------------------------------------------------------------
 * This file is part of Gameroni -
 * http://www.vitorian.com/x1/archives/419
 * https://github.com/hftrader/gameroni
 *
 * Gameroni is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gameroni is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------------------*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileInfo>
#include <QSettings>
#include <QSet>
#include <QDebug>
#include <QMap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( ui->toolBox, SIGNAL(currentChanged(int)),
             ui->wcDisplay, SLOT(setAlgorithm(int)) );
    connect( ui->sbLaplacianRadius, SIGNAL(valueChanged(int)),
             this, SLOT(updateState()) );
    connect( ui->sbLaplacianKernel, SIGNAL(valueChanged(int)),
             this, SLOT(updateState()) );
    connect( ui->sbMotionDuration, SIGNAL(valueChanged(double)),
             this, SLOT(updateState()) );
    connect( ui->sbMotionWidth, SIGNAL(valueChanged(int)),
             this, SLOT(updateState()) );
    connect( ui->cbRigidTransform, SIGNAL(toggled(bool)),
             this, SLOT(updateState()) );
    connect( ui->cbMotionGrayscale, SIGNAL(toggled(bool)),
             this, SLOT(updateState()) );
    connect( ui->cbExecutable, SIGNAL(currentIndexChanged(int)),
             this, SLOT(updateExecutableChoice(int)));

    // Try to remember last settings
    QSettings qs;
    lastKey = qs.value("LastExecutable").toString();
    qDebug() << "Last key:" << lastKey;
    populateProcessList();

    ui->toolBox->setCurrentIndex(0);
    ui->wcDisplay->setAlgorithm(0);
    updateState();

    popTimerId = startTimer( 5000 );
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if ( event->timerId() == popTimerId ) {
        populateProcessList();
    }
}

void MainWindow::populateProcessList()
{
    // Get a list of processes
    std::vector<ProcessInfo> plist;
    getProcessList( plist, true );

    // Create a list of QString items to populate the combo box
    QVariantMap newMap;
    for ( uint32_t j=0; j<plist.size(); ++j ) {
        // Extract values
        QString pname = QString::fromStdWString( plist[j].name );
        QString exename = QString::fromStdWString( plist[j].process );
        QString filename = QFileInfo( exename ).fileName();
        QString key = filename + " | " + pname;

        // Create variant map and update new map
        QVariantMap pmap;
        pmap["ExeName"] = exename;
        pmap["Process"] = pname;
        pmap["hWnd"] = uint64_t(plist[j].hWnd);
        pmap["Filename"] = filename;
        pmap["Key"] = key;
        newMap[key] = pmap;
    }

    // Poor man's diff
    QSet<QString> oldKeys, newKeys;
    for ( QString key : exeMap.keys() ) oldKeys.insert( key );
    for ( QString key : newMap.keys() ) newKeys.insert( key );

    // Added Keys
    QSet<QString> addedKeys  = newKeys - oldKeys;
    for ( QString key : addedKeys ) {
        QVariantMap pmap = newMap.value(key).toMap();
        ui->cbExecutable->addItem( key, pmap );
        qDebug() << "Adding key " << key;
    }

    // Removed keys
    QSet<QString> removedKeys = oldKeys - newKeys;
    for ( QString key : removedKeys ) {
        int idx = ui->cbExecutable->findText( key );
        ui->cbExecutable->removeItem( idx );
        qDebug() << "Removing key " << key;
    }

    // Update keys
    QSet<QString> currentKeys( newKeys );
    currentKeys.intersect( oldKeys );
    for ( QString key: currentKeys ) {
        QVariantMap pmap = newMap.value(key).toMap();
        int idx = ui->cbExecutable->findText( key );
        ui->cbExecutable->setItemData( idx, pmap );
    }

    // Update map
    exeMap = newMap;

    int idx = ui->cbExecutable->findText( lastKey );
    if ( idx>=0 ) {
        ui->cbExecutable->setCurrentIndex( idx );
    }
}

void MainWindow::updateExecutableChoice( int index )
{
    QVariantMap qv = ui->cbExecutable->itemData( index ).toMap();
    HWND hWnd = (HWND)qv["hWnd"].toULongLong();
    ui->wcDisplay->updateWindow( hWnd );
    // Update settings
    lastKey = qv["Key"].toString();
    QSettings qs;
    qs.setValue( "LastExecutable", lastKey );
    qs.sync();
    qDebug() << "Updating process choice to " << lastKey;
}

void MainWindow::updateState()
{
    int index = ui->toolBox->currentIndex();
    QVariantMap vmap;
    if ( index==0 ) { // Laplacian
        vmap["Radius"] = ui->sbLaplacianRadius->value();
        vmap["Kernel"] = ui->sbLaplacianKernel->value();
    }
    else if ( index==1 ) { // Motion
        vmap["Duration"] = ui->sbMotionDuration->value();
        vmap["Width"] = ui->sbMotionWidth->value();
        vmap["RigidTransform"] = ui->cbRigidTransform->isChecked();
        vmap["Grayscale"] = ui->cbMotionGrayscale->isChecked();
    }
    ui->wcDisplay->updateState( vmap );
}
