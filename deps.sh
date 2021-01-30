#!/bin/bash

#This will install necessary packages on Debian Linux installations.
v_packages=("build-essential" "libsdl2-dev" "libsdl2-net-dev" "libvulkan-dev" "libopengl-dev" "cmake" "liblua5.3-dev" "libluabind-dev" "libglm-dev")

#Run as root
if [ "$EUID" -ne 0 ]
  then echo "Please run as root."
  exit 1
fi

#Check for apt
if ! command -v apt &> /dev/null
then
    echo "apt not found. This script only presently works on Debian and Ubuntu installations using apt."
    exit 1
fi

#Check missing packages 
declare -a v_missing=()
for pkg in ${v_packages[@]}; do
    v_installed=$(dpkg-query --show --showformat='${Status}\n' ${pkg} | grep "install ok installed") 
    if [ "${v_installed}" != "install ok installed" ]; then
        v_missing+=(${pkg})    
    fi
done

#Install 
if [ ${#v_missing[@]} != 0 ]; then  
    echo "VG: Missing: ${#v_missing[@]} packages."    
    #Update package list    
    sudo apt update > /dev/null 2>&1
    #Install each.
    for pkg in ${v_missing[@]}; do
        echo VG: Installing ${pkg}...
        sudo apt install ${pkg}         
    done
fi

