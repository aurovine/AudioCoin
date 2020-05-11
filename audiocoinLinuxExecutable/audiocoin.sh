echo "Audiocoin Installation started . . . ."

echo "Essential dependency installing .."

sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git
sudo apt-get install build-essential
sudo apt-get install libssl1.0-dev
sudo apt-get install libdb++-dev
sudo apt-get install libboost-all-dev
sudo apt-get install libqrencode-dev
sudo apt-get install qt5-default
sudo apt-get install qt5-qmake
sudo apt-get install qtbase5-dev-tools
sudo apt-get install qttools5-dev-tools
sudo apt-get install libboost-dev
sudo apt-get install libboost-system-dev
sudo apt-get install libboost-filesystem-dev
sudo apt-get install libboost-program-options-dev
sudo apt-get install libboost-thread-dev
sudo apt-get install libminiupnpc-dev

echo "Dependency Installation finished"

echo "\t Cloning Audiocoin Repository \t"

git clone https://github.com/aurovine/AudioCoin.git

cp audiocoin.conf AudioCoin

cd AudioCoin/

qmake

make


