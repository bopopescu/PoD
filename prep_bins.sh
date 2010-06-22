#! /usr/bin/env bash

# This script helps to build a GSI specific version of PoD and
# bins packages for PoD workers

export LD_LIBRARY_PATH=/misc/manafov/Qt/4.4.2_etch32/lib:$LD_LIBRARY_PATH

POD_SRC="/misc/manafov/PoD/BinBuilds"

# prep repo
pushd `pwd`
cd $POD_SRC
rm -rf $POD_SRC/PoD/*
git clone ssh://anar@depc218.gsi.de//home/anar/GitRepository/PROOFonDemand/PoD || exit 1
cd PoD
git submodule update --init --recursive || exit 1
popd

# GSI bin
ssh lxetch32 "$POD_SRC/PoD/bin/prep_gsi_bin.sh $POD_SRC/PoD" || exit 1

# Workers bins
ssh lxetch64 "$POD_SRC/PoD/bin/prep_wrk_bins.sh $POD_SRC/PoD" || exit 1
ssh lxetch32 "$POD_SRC/PoD/bin/prep_wrk_bins.sh $POD_SRC/PoD" || exit 1

exit 0