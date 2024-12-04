# ChordSync

## Setting up a Raspberry Pi 5

Flash the Raspberry Pi with Raspberry Pi OS (64-bit)

```sh
sudo apt install cmake
```

```sh
git clone https://github.com/owenpark8/eecs473-VSGuitar.git ~/.local/src/eecs473-ChordSync
cd ~/.local/src/eecs473-ChordSync/rpi-backend
git submodule init
git submodule update
```

```sh
./scripts/setup.sh
```
