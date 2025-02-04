#!/bin/bash

#############################################################################
# Find the impl from user input.

if [ -z $1 ]; then
  echo "Which impl to build?  Example:" $0 "7"
  exit
else
  imp="impl"$1
fi

if ! [ -d $impl ]; then
  exit
fi

# Get wl rel brand
prefix_config_brand=wlconfig_lx_router
default_config_brand=release_config
brand=$2
if [ $brand != "" ]; then
  if ! [ -d ${imp}-orig ]; then
    cp -rf $imp ${imp}-orig
  fi

  cd $imp
  if [ -e router_rules_${brand}.mk ]; then  
    make -f router_rules_${brand}.mk all
  fi
  cd ..

  cp -f $imp/wl/config/${prefix_config_brand}_$brand $imp/wl/config/${default_config_brand}
fi

echo Tar release on $imp

#############################################################################
# Get epi version and router version

./make_version.sh $imp ""

. $imp/epivers
ver=$EPI_VERSION_STR

. $imp/rver
rver=$ROUTER_VERSION_STR

if [ "$brand" != "" ]; then
  ver=$ver-$brand
  rver=$rver-$brand
fi
#############################################################################
# These files are to be excluded.

exclude=" \
  --wildcards \
  --wildcards-match-slash \
  --exclude=.*.db \
  --exclude=*.a \
  --exclude=*.cmd \
  --exclude=*.contrib* \
  --exclude=*.db \
  --exclude=*.keep* \
  --exclude=*.lib \
  --exclude=*.merge \
  --exclude=*.o \
  --exclude=*.o_save \
  --exclude=*.obj \
  --exclude=*.save \
  --exclude=*.so \
  --exclude=*.tmp \
  --exclude=*build \
  --exclude=tags \
  --exclude=dongle \
  --exclude=$imp/shared/cfe \
  --exclude=$imp/shared/linux \
  --exclude=$imp/shared/nvram \
  --exclude=$imp/shared/zlib \
  --exclude=$imp/usbdev \
  --exclude=$imp/tools \
  --exclude=$imp/router_rules*.mk \
  --exclude=$imp/router-filelist*.txt \
"

#############################################################################
# Some applications need other modules' header files

h_share=" \
  $imp/include/bcmparams.h \
  $imp/include/bcmwpa.h \
  $imp/include/UdpLib.h \
  `find $imp/router/shared -name *.h` \
"

h_crypt=`find $imp/include/bcmcrypto -name *.h`
h_proto=`find $imp/include/proto -name *.h`
h_upnp=`find $imp/router/libupnp -name *.h`
h_eapd=`find $imp/router/eapd -name *.h`
h_nas=`find $imp/router/nas -name *.h`
h_wps=`find $imp/wps/common/include -name *.h`

#############################################################################
# !!!!!!!!!!! Do not use wildcard directly with tar !!!!!!!!!!!!!!!!!!!!!!!!!
#not include wapi code for wlan-src-all tarball

# When cus=1 (manually change this file), 
# there is customized release request: To add userspace/private/apps/wlan to the tarball.

cus=1
comengine=../../../../
if  [ $cus == "1" ]; then
  cp -R ${comengine}userspace/private/apps/wlan .
  tar -zcvf wlan-all-src-$ver.tgz makefile.wlan wlan $imp --exclude=$imp/wapi --exclude=$imp/router/wapi $exclude
  rm -rf wlan
else
  tar -zcvf wlan-all-src-$ver.tgz makefile.wlan $imp --exclude=$imp/wapi --exclude=$imp/router/wapi $exclude
fi

if  [ $imp == "impl6" ]; then
  echo Add Dongle Files to wlan-all-src-$ver.tgz
  gunzip wlan-all-src-$ver.tgz
  tar --append \
      --file=wlan-all-src-$ver.tar \
             $imp/makefile.wlan \
             $imp/usbdev/usbdl/bcmdl \
             $imp/usbdev/libusb-0.1.12/libusb-0.1.12.tgz \
             $imp/dongle/rte/wl/builds/4322-bmac/rom-ag/rtecdc.trx
  gzip wlan-all-src-$ver.tar
  mv wlan-all-src-$ver.tar.gz wlan-all-src-$ver.tgz
fi

if [ -d $imp/router ]; then
  tar -zcvf upnp-src-$ver.tgz \
    $imp/router/libupnp \
    $h_share \
    $h_crypt \
    $h_wps \
    $exclude
  tar -zcvf eapd-src-$ver.tgz \
    $imp/router/eapd \
    $h_share \
    $h_proto \
    $h_nas \
    $exclude
  tar -zcvf lltd-src-$ver.tgz \
    $imp/router/lltd \
    $h_share \
    $exclude
  tar -zcvf nas-src-$ver.tgz \
    $imp/router/nas \
    $h_share \
    $h_crypt \
    $h_proto \
    $h_eapd \
    $exclude
  tar -zcvf wps-src-$ver.tgz \
    $imp/wps \
    $h_share \
    $h_crypt \
    $h_proto \
    $h_eapd \
    $h_upnp \
    $exclude
  tar -zcvf wl-src-$ver.tgz \
    makefile.wlan \
    $imp/dslcpe_wlan_minor_version \
    $imp/Makefile \
    $imp/bcmcrypto \
    $imp/emf/emf \
    $imp/emf/igs \
    $imp/include \
    $imp/router/emf/emf \
    $imp/router/emf/igs \
    $imp/router/shared \
    $imp/wlconf/ \
    $imp/shared/ \
    $imp/wl \
    $exclude
  if [ -d $imp/router/wapi ]; then
    echo Add WAPI Files to wl-wapi-$ver.tgz
    tar -czvf wlan-wapi-$ver.tgz $imp/wapi $imp/router/wapi $exclude
  fi
else
  tar -zcvf lltd-src-$ver.tgz $imp/lltd $exclude
  tar -zcvf nas-src-$ver.tgz  $imp/nas $exclude
  tar -zcvf wsc-src-$ver.tgz  $imp/wsc $exclude
  tar -zcvf wl-src-$ver.tgz \
    makefile.wlan \
    $imp/dslcpe_wlan_minor_version \
    $imp/Makefile \
    $imp/bcmcrypto \
    $imp/include \
    $imp/shared/ \
    $imp/wl \
    $exclude
fi

if  [ $imp == "impl6" ]; then
  echo Add Dongle Files to wl-src-$ver.tgz
  gunzip wl-src-$ver.tgz
  tar --append \
      --file=wl-src-$ver.tar \
             $imp/makefile.wlan \
             $imp/usbdev/usbdl/bcmdl \
             $imp/usbdev/libusb-0.1.12/libusb-0.1.12.tgz \
             $imp/dongle/rte/wl/builds/4322-bmac/rom-ag/rtecdc.trx
  gzip wl-src-$ver.tar
  mv wl-src-$ver.tar.gz wl-src-$ver.tgz
fi

#############################################################################
# Put all files in router version directory

if [ ! -d wlan-router.$rver/$ver/src ]; then
  mkdir -p wlan-router.$rver/$ver/src
fi

mv *tgz wlan-router.$rver/$ver/src

if [ "$brand" != "" ]; then
  # Save mogrify directory
  rm -rf $imp-$brand
  mv $imp $imp-$brand 
  cp -rf ${imp}-orig $imp
fi

# End of file
