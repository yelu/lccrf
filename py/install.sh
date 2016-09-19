python Setup.py install --record files.txt && cat files.txt | xargs rm -rf
export CC=/usr/bin/gcc && python Setup.py build
sudo python Setup.py install

