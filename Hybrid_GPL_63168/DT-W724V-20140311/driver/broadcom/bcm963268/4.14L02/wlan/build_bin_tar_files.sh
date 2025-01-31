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

echo Tar release on $imp

#############################################################################
# Get epi version and router version

./make_version.sh $imp $2
. $imp/epivers
ver=$EPI_VERSION_STR

. $imp/rver
rver=$ROUTER_VERSION_STR

# Get wl rel brand
brand=$3
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
  --exclude=*.cmd \
  --exclude=*.contrib* \
  --exclude=*.db \
  --exclude=*.keep* \
  --exclude=*.merge \
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

bin_found=no

for x in `find $imp -maxdepth 1 -name *.o_save`; do
  bin_found=yes
done

if [ $bin_found != yes ]; then
  exit
fi

echo "Tarball==========" wlan-bin-all-$ver.tgz

# When cus=1 (manually change this file), 
# there is customized release request: 
# To add directory images and userspace/private/apps/wlan and to the tarball,
# and put directory images to directory bin.
cus=1
comengine=../../../../
worktop=${comengine}../
if  [ $cus == "1" ]; then
  dver=`grep '^BRCM_VERSION=' ${comengine}version.make|cut -d'=' -f2`
  drel=`grep '^BRCM_RELEASE=' ${comengine}version.make|cut -d'=' -f2`
  dext=`grep '^BRCM_EXTRAVERSION=' ${comengine}version.make|cut -d'=' -f2`
  drver="$dver"."$drel"L."$dext"
  dimage=${worktop}linuxout/${drver}
  cp -R ${dimage}/images .
  cp -R ${comengine}userspace/private/apps/wlan .

  # Not keep image when using BRAND profiles
  if [ "$brand" != "" ]; then
    tar -zcvf wlan-bin-all-$ver.tgz makefile.wlan wlan $imp $exclude
  else
    tar -zcvf wlan-bin-all-$ver.tgz makefile.wlan images wlan $imp $exclude
  fi
  rm -rf images
  rm -rf wlan
else
  tar -zcvf wlan-bin-all-$ver.tgz makefile.wlan $imp $exclude
fi

if  [ $imp == "impl6" ]; then
  echo Add Dongle files to $imp release
  gunzip wlan-bin-all-$ver.tgz
  tar --append \
      --file=wlan-bin-all-$ver.tar \
             $imp/usbdev/usbdl/bcmdl \
             $imp/usbdev/libusb-0.1.12/libusb-0.1.12.tgz \
             $imp/dongle/rte/wl/builds/4322-bmac/rom-ag/rtecdc.trx
  gzip wlan-bin-all-$ver.tar
  mv wlan-bin-all-$ver.tar.gz wlan-bin-all-$ver.tgz
fi

#############################################################################
# Put all files in router version directory

if [ ! -d wlan-router.$rver/$ver/bin ]; then
  mkdir -p wlan-router.$rver/$ver/bin
fi

mv *tgz wlan-router.$rver/$ver/bin

if  [ $cus == "1" ]; then
  # Not keep image when using BRAND profiles 
  if [ "$brand" == "" ]; then  
    cp -r ${dimage}/images wlan-router.$rver/$ver/bin/.
  fi
fi
# End of file
