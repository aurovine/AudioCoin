See readme-qt.rst for instructions on building AudioCoin QT, the graphical user interface.

PPC is not supported because it's big-endian.


# Instructions

* Install XCode with all the options checked. The current version is available from http://developer.apple.com

* Clone from Github to get the source code:

`git clone https://github.com/aurovine/AudioCoin.git`

* Download and install MacPorts from http://www.macports.org/

* Install dependencies from MacPorts

`sudo port install boost db48 openssl miniupnpc`

* Optionally install qrencode (and set USE_QRCODE=1)

`sudo port install qrencode`

* Now you should be able to build Audiocoind:

`cd AudioCoin/src`

`make -f makefile.osx`

Run:
  `./audiocoind --help`  # for a list of command-line options.
  
Run
  `./audiocoind -daemon` # to start the Audiocoin daemon.
  
Run
  `./audiocoind help` # When the daemon is running, to get a list of RPC commands
