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

    ui->toolBox->setCurrentIndex(0);
    ui->wcDisplay->setAlgorithm(0);
}

MainWindow::~MainWindow()
{
    delete ui;
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
