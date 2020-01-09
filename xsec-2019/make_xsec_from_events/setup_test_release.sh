#!/bin/bash

setup_nova -r S19-01-14
pushd .
cd /nova/ana/users/slin/test_releases/NumuCC_Fix_TestRelease/
srt_setup -a
popd