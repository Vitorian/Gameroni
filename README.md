# Gameroni
Game video postprocessor to provide visual cues for online gaming. 

We provided this game helper as a tool to run some AI studies of my own but it might serve other purposes as well. Be the judge.

This project would not be possible without the sponsorship of the partners at Vitorian LLC - a 100% American Fintech company.

With this project we shipped the binary dependencies inside the folder 3rdParty - only for VS 2015 Win64 at this point. 

If you want to add a new image processor "XXX", the path is:

1. Add a new tab to MainWindow/CentralWidget/toolBox
2. Populate tab with the respective settings
3. Add a corresponding entry for serialization in MainWindow::updateState()
4. Implement a new class ImageProcessorXXX (files imageprocessorXXX.h and imageprocessorXXX.cpp)

Gameroni depends on:
- zlib   http://zlib.net/
- libpng http://libpng.org/pub/png/libpng.html
- openCV http://opencv.org/
- Qt 5+  https://www.qt.io/download/

We provide the 3rd party binaries and libraries and also an installer from our Google Drive:

https://drive.google.com/drive/folders/0BzpG2OedyuVsQmpCcV9GLWt5ZFU?usp=sharing

Please contribute by forking this project and creating new pull requests!

Thanks for participating!

--Henrique Bucher, partner @ Vitorian LLC
--henry@vitorian.com


