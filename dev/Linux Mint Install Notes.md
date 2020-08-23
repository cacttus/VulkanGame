## Dual-Boot installing mint with Windows10
Download latest Mint.
Use the USB tool to install it on a USB.
Plug in and boot up to the USB via the OS boot menu (del/f12..)
Mint will allow you to install it alongside Windows 10 from there.

* Your old windows desktop is still accessible on Devices/vol/users/
* You can change monitor settings under system settings / display.

## Wireless Connection
When you start only the little usb tp-link will work for some reason. Might have to pull it out & plug it back in for Mint to see it.

## NVIDIA drivers -- This explains everything 
https://linuxconfig.org/how-to-install-nvidia-drivers-on-linux-mint

## In short - Install all the video drivers 
Sudo ubuntu-drivers autoinstall 

You must reboot after this. Not sure about the chipset. It would be nice to have the onboard wifi driver though.
* You can change monitor settings under system settings / display.

## Next Install GIT and set it up
sudo apt install git
cd /var/
sudo mkdir git

## Add your user to sudo to allow r/w to /var
sudo usermod -aG sudo **Your_Account** //Makes you a member of sudo
id **Your_Account** //checks groups
su - mario //logs you in
sudo chmod -R ugo+rw /var/git //Makes /var/git/* accessible to everyone, readable, writable.

## Install SDL 
If SDL is already installed check
* apt list --installed | grep libsdl
To see what available packages for SDL are available
* apt list | grep libsdl
If not installed 
* sudo apt-get install libsdl2-dev

## Install Visual Studio Code
VS Code came already installed on Mint.
Download the Ubuntu version of VS code.





