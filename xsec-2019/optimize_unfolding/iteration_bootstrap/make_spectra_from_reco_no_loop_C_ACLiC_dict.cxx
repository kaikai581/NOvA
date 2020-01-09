// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dInovadIanadIusersdIslindImy_gitdINOvAdInumudIprod4dIxsec_2019dIoptimize_unfoldingdIiteration_bootstrapdImake_spectra_from_reco_no_loop_C_ACLiC_dict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/iteration_bootstrap/./make_spectra_from_reco_no_loop.C"

// Header files passed via #pragma extra_include

namespace {
  void TriggerDictionaryInitialization_make_spectra_from_reco_no_loop_C_ACLiC_dict_Impl() {
    static const char* headers[] = {
"./make_spectra_from_reco_no_loop.C",
0
    };
    static const char* includePaths[] = {
"./include/",
"/cvmfs/nova-development.opensciencegrid.org/novasoft//releases/development/include/",
"/cvmfs/nova.opensciencegrid.org/externals/root/v6_12_06a/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/nutools/v2_28_02/include",
"/cvmfs/nova.opensciencegrid.org/externals/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/boost/v1_66_0a/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/novarwgt/v00.19/inc",
"/cvmfs/nova-development.opensciencegrid.org/novasoft//releases/development/include/OnlineMonitoring",
"/cvmfs/nova-development.opensciencegrid.org/novasoft//releases/development/include/OnlineMonitoring/viewer",
"/cvmfs/nova-development.opensciencegrid.org/novasoft//releases/development/include/EventDisplay",
"/cvmfs/nova-development.opensciencegrid.org/novasoft//releases/development/include",
"/cvmfs/nova.opensciencegrid.org/externals/novarwgt/v00.19/inc",
"/cvmfs/nova.opensciencegrid.org/externals/novabeamlinefragments/v07_00_02/include",
"/cvmfs/nova.opensciencegrid.org/externals/caffe/v1_0k/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/opencv/v3_4_2/Linux64bit+2.6-2.12-e15/include",
"/cvmfs/nova.opensciencegrid.org/externals/hdf5/v1_10_2a/Linux64bit+2.6-2.12-e15/include",
"/cvmfs/nova.opensciencegrid.org/externals/lmdb/v0_9_21/Linux64bit+2.6-2.12/include",
"/cvmfs/nova.opensciencegrid.org/externals/protobuf/v3_5_2/Linux64bit+2.6-2.12-e15/include",
"/cvmfs/nova.opensciencegrid.org/externals/glog/v0_3_5a/Linux64bit+2.6-2.12-e15/include",
"/cvmfs/nova.opensciencegrid.org/externals/gflags/v2_2_1a/Linux64bit+2.6-2.12-e15/include",
"/cvmfs/nova.opensciencegrid.org/externals/snappy/v1_1_7b/Linux64bit+2.6-2.12-e15/include",
"/cvmfs/nova.opensciencegrid.org/externals/leveldb/v1_20b/Linux64bit+2.6-2.12-e15/include",
"/cvmfs/nova.opensciencegrid.org/externals/lapack/v3_8_0a/Linux64bit+2.6-2.12-e15-prof/include",
"/cvmfs/nova.opensciencegrid.org/externals/ppfx/v02_06/include",
"/cvmfs/nova.opensciencegrid.org/externals/art/v2_12_01/include",
"/cvmfs/nova.opensciencegrid.org/externals/fhiclcpp/v4_06_09/include",
"/cvmfs/nova.opensciencegrid.org/externals/cetlib/v3_06_00/slf6.x86_64.e15.debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/boost/v1_66_0a/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/artdaq_core/v3_04_10/include",
"/cvmfs/nova.opensciencegrid.org/externals/artdaq_core/v3_04_10/include",
"/cvmfs/nova.opensciencegrid.org/externals/TRACE/v3_13_11/include",
"/cvmfs/nova.opensciencegrid.org/externals/nutools/v2_28_02/include",
"/cvmfs/nova.opensciencegrid.org/externals/ifdh_art/v2_07_03/slf6.x86_64.e15.s79.debug/inc",
"/cvmfs/nova.opensciencegrid.org/externals/cry/v1_7k/Linux64bit+2.6-2.12-e15-debug/cry_v1.7/include",
"/cvmfs/nova.opensciencegrid.org/externals/dk2nugenie/v01_07_02g/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/include/GENIE",
"/cvmfs/nova.opensciencegrid.org/externals/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/art/v2_12_01/include",
"/cvmfs/nova.opensciencegrid.org/externals/nusimdata/v1_16_01/include",
"/cvmfs/nova.opensciencegrid.org/externals/dk2nudata/v01_07_02/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/canvas_root_io/v1_01_10/include",
"/cvmfs/nova.opensciencegrid.org/externals/xrootd/v4_8_0b/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/mysql_client/v5_5_58a/Linux64bit+2.6-2.12-e15/include",
"/cvmfs/nova.opensciencegrid.org/externals/postgresql/v9_6_6a/Linux64bit+2.6-2.12-p2714b/include",
"/cvmfs/nova.opensciencegrid.org/externals/pythia/v6_4_28k/Linux64bit+2.6-2.12-gcc640-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/gsl/v2_4/Linux64bit+2.6-2.12-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/fftw/v3_3_6_pl2/Linux64bit+2.6-2.12-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/canvas/v3_06_00/include",
"/cvmfs/nova.opensciencegrid.org/externals/range/v3_0_3_0/include",
"/cvmfs/nova.opensciencegrid.org/externals/clhep/v2_3_4_6/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/messagefacility/v2_02_05/include",
"/cvmfs/nova.opensciencegrid.org/externals/hep_concurrency/v1_00_03/slf6.x86_64.e15.debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/tbb/v2018_2a/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/fhiclcpp/v4_06_09/include",
"/cvmfs/nova.opensciencegrid.org/externals/cetlib/v3_06_00/slf6.x86_64.e15.debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/boost/v1_66_0a/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/cetlib_except/v1_02_01/include",
"/cvmfs/nova.opensciencegrid.org/externals/roounfold/v1_1_1_a/NULL_e15_debug/src",
"/cvmfs/nova.opensciencegrid.org/externals/roounfold/v1_1_1_a/NULL_e15_debug/src/../",
"/cvmfs/nova.opensciencegrid.org/externals/root/v6_12_06a/Linux64bit+2.6-2.12-e15-debug/etc",
"/cvmfs/nova.opensciencegrid.org/externals/root/v6_12_06a/Linux64bit+2.6-2.12-e15-debug/etc/cling",
"/cvmfs/nova.opensciencegrid.org/externals/root/v6_12_06a/Linux64bit+2.6-2.12-e15-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/tbb/v2018_2a/Linux64bit+2.6-2.12-e15-debug/include",
"/scratch/workspace/canvas-products/vdevelop/e15/SLF6/debug/build/tbb/v2018_2a/Linux64bit+2.6-2.12-e15-debug/include",
"/usr/include/freetype2",
"Framework/Messenger",
"Types",
"Interfaces",
"GENIE",
"../Types",
"../Interfaces",
"GENIE/Framework/Messenger",
"GENIE/Types",
"GENIE/Interfaces",
"../include",
"../include/GENIE",
"../include/GENIE/Framework/Messenger",
"../include/GENIE/Types",
"../include/GENIE/Interfaces",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/libxml2/v2_9_5/Linux64bit+2.6-2.12-debug/include/libxml2",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/log4cpp/v1_1_3a/Linux64bit+2.6-2.12-e15-debug/include",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/root/v6_12_06a/Linux64bit+2.6-2.12-e15-debug/include",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/Messenger/",
"Framework/Registry",
"GENIE/Framework/Registry",
"../include/GENIE/Framework/Registry",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/Registry/",
"Framework/Algorithm",
"GENIE/Framework/Algorithm",
"../include/GENIE/Framework/Algorithm",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/Algorithm/",
"Framework/Interaction",
"GENIE/Framework/Interaction",
"../include/GENIE/Framework/Interaction",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/Interaction/",
"Framework/GHEP",
"GENIE/Framework/GHEP",
"../include/GENIE/Framework/GHEP",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/GHEP/",
"Framework/Numerical",
"GENIE/Framework/Numerical",
"../include/GENIE/Framework/Numerical",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/Numerical/",
"Framework/Utils",
"GENIE/Framework/Utils",
"../include/GENIE/Framework/Utils",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/Utils/",
"Framework/ParticleData",
"GENIE/Framework/ParticleData",
"../include/GENIE/Framework/ParticleData",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/ParticleData/",
"Framework/EventGen",
"GENIE/Framework/EventGen",
"../include/GENIE/Framework/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/EventGen/",
"Framework/Ntuple",
"GENIE/Framework/Ntuple",
"../include/GENIE/Framework/Ntuple",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Framework/Ntuple/",
"Physics/XSectionIntegration",
"GENIE/Physics/XSectionIntegration",
"../include/GENIE/Physics/XSectionIntegration",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/XSectionIntegration/",
"Physics/PartonDistributions",
"GENIE/Physics/PartonDistributions",
"../include/GENIE/Physics/PartonDistributions",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/PartonDistributions/",
"Physics/NuclearState",
"GENIE/Physics/NuclearState",
"../include/GENIE/Physics/NuclearState",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/NuclearState/",
"Physics/Common",
"GENIE/Physics/Common",
"../include/GENIE/Physics/Common",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Common/",
"Physics/Decay",
"GENIE/Physics/Decay",
"../include/GENIE/Physics/Decay",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Decay/",
"Physics/HadronTransport",
"GENIE/Physics/HadronTransport",
"../include/GENIE/Physics/HadronTransport",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/HadronTransport/",
"Physics/Hadronization",
"GENIE/Physics/Hadronization",
"../include/GENIE/Physics/Hadronization",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Hadronization/",
"Physics/NuclearDeExcitation",
"GENIE/Physics/NuclearDeExcitation",
"../include/GENIE/Physics/NuclearDeExcitation",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/NuclearDeExcitation/",
"Physics/AnomalyMediatedNuGamma/XSection",
"GENIE/Physics/AnomalyMediatedNuGamma/XSection",
"../include/GENIE/Physics/AnomalyMediatedNuGamma/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/AnomalyMediatedNuGamma/XSection/",
"Physics/AnomalyMediatedNuGamma/EventGen",
"GENIE/Physics/AnomalyMediatedNuGamma/EventGen",
"../include/GENIE/Physics/AnomalyMediatedNuGamma/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/AnomalyMediatedNuGamma/EventGen/",
"Physics/Charm/XSection",
"GENIE/Physics/Charm/XSection",
"../include/GENIE/Physics/Charm/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Charm/XSection/",
"Physics/Coherent/XSection",
"GENIE/Physics/Coherent/XSection",
"../include/GENIE/Physics/Coherent/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Coherent/XSection/",
"Physics/Coherent/EventGen",
"GENIE/Physics/Coherent/EventGen",
"../include/GENIE/Physics/Coherent/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Coherent/EventGen/",
"Physics/DeepInelastic/XSection",
"GENIE/Physics/DeepInelastic/XSection",
"../include/GENIE/Physics/DeepInelastic/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/DeepInelastic/XSection/",
"Physics/DeepInelastic/EventGen",
"GENIE/Physics/DeepInelastic/EventGen",
"../include/GENIE/Physics/DeepInelastic/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/DeepInelastic/EventGen/",
"Physics/Diffractive/XSection",
"GENIE/Physics/Diffractive/XSection",
"../include/GENIE/Physics/Diffractive/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Diffractive/XSection/",
"Physics/Diffractive/EventGen",
"GENIE/Physics/Diffractive/EventGen",
"../include/GENIE/Physics/Diffractive/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Diffractive/EventGen/",
"Physics/GlashowResonance/XSection",
"GENIE/Physics/GlashowResonance/XSection",
"../include/GENIE/Physics/GlashowResonance/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/GlashowResonance/XSection/",
"Physics/GlashowResonance/EventGen",
"GENIE/Physics/GlashowResonance/EventGen",
"../include/GENIE/Physics/GlashowResonance/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/GlashowResonance/EventGen/",
"Physics/InverseBetaDecay/XSection",
"GENIE/Physics/InverseBetaDecay/XSection",
"../include/GENIE/Physics/InverseBetaDecay/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/InverseBetaDecay/XSection/",
"Physics/InverseBetaDecay/EventGen",
"GENIE/Physics/InverseBetaDecay/EventGen",
"../include/GENIE/Physics/InverseBetaDecay/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/InverseBetaDecay/EventGen/",
"Physics/Multinucleon/XSection",
"GENIE/Physics/Multinucleon/XSection",
"../include/GENIE/Physics/Multinucleon/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Multinucleon/XSection/",
"Physics/Multinucleon/EventGen",
"GENIE/Physics/Multinucleon/EventGen",
"../include/GENIE/Physics/Multinucleon/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Multinucleon/EventGen/",
"Physics/MuonEnergyLoss",
"GENIE/Physics/MuonEnergyLoss",
"../include/GENIE/Physics/MuonEnergyLoss",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/MuonEnergyLoss/",
"Physics/NuElectron/XSection",
"GENIE/Physics/NuElectron/XSection",
"../include/GENIE/Physics/NuElectron/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/NuElectron/XSection/",
"Physics/NuElectron/EventGen",
"GENIE/Physics/NuElectron/EventGen",
"../include/GENIE/Physics/NuElectron/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/NuElectron/EventGen/",
"Physics/QuasiElastic/XSection",
"GENIE/Physics/QuasiElastic/XSection",
"../include/GENIE/Physics/QuasiElastic/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/QuasiElastic/XSection/",
"Physics/QuasiElastic/EventGen",
"GENIE/Physics/QuasiElastic/EventGen",
"../include/GENIE/Physics/QuasiElastic/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/QuasiElastic/EventGen/",
"Physics/Resonance/XSection",
"GENIE/Physics/Resonance/XSection",
"../include/GENIE/Physics/Resonance/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Resonance/XSection/",
"Physics/Resonance/EventGen",
"GENIE/Physics/Resonance/EventGen",
"../include/GENIE/Physics/Resonance/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Resonance/EventGen/",
"Physics/Strange/XSection",
"GENIE/Physics/Strange/XSection",
"../include/GENIE/Physics/Strange/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Strange/XSection/",
"Physics/Strange/EventGen",
"GENIE/Physics/Strange/EventGen",
"../include/GENIE/Physics/Strange/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/Strange/EventGen/",
"Physics/NucleonDecay",
"GENIE/Physics/NucleonDecay",
"../include/GENIE/Physics/NucleonDecay",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/NucleonDecay/",
"Physics/NNBarOscillation",
"GENIE/Physics/NNBarOscillation",
"../include/GENIE/Physics/NNBarOscillation",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/NNBarOscillation/",
"Physics/BoostedDarkMatter/XSection",
"GENIE/Physics/BoostedDarkMatter/XSection",
"../include/GENIE/Physics/BoostedDarkMatter/XSection",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/BoostedDarkMatter/XSection/",
"Physics/BoostedDarkMatter/EventGen",
"GENIE/Physics/BoostedDarkMatter/EventGen",
"../include/GENIE/Physics/BoostedDarkMatter/EventGen",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Physics/BoostedDarkMatter/EventGen/",
"Tools/Geometry",
"GENIE/Tools/Geometry",
"../include/GENIE/Tools/Geometry",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Tools/Geometry/",
"Tools/Flux",
"GENIE/Tools/Flux",
"../include/GENIE/Tools/Flux",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Generator/src/Tools/Flux/",
"..",
"../tree",
"../include/dk2nu",
"dk2nu",
"../include/tree",
"../include/dk2nu/tree",
"dk2nu/tree",
"/scratch/workspace/build-nulite-matrix/BUILDTYPE/debug/QUAL/s70-e15/label1/swarm/label2/SLF6/build/root/v6_12_06a/Linux64bit+2.6-2.12-e15-debug/include",
"/scratch/workspace/build-nulite-matrix/BUILDTYPE/debug/QUAL/s70-e15/label1/swarm/label2/SLF6/build/dk2nudata/v01_07_02/source/dk2nu/..",
"/scratch/workspace/build-nulite-matrix/BUILDTYPE/debug/QUAL/s70-e15/label1/swarm/label2/SLF6/build/dk2nudata/v01_07_02/source/dk2nu",
"/scratch/workspace/build-nulite-matrix/BUILDTYPE/debug/QUAL/s70-e15/label1/swarm/label2/SLF6/build/dk2nudata/v01_07_02/source/dk2nu/dk2nu",
"/scratch/workspace/build-nulite-matrix/BUILDTYPE/debug/QUAL/s70-e15/label1/swarm/label2/SLF6/build/dk2nudata/v01_07_02/source/dk2nu/tree/-I/scratch/workspace/build-nulite-matrix/BUILDTYPE/debug/QUAL/s70-e15/label1/swarm/label2/SLF6/build/dk2nudata/v01_07_02/source/dk2nu/tree",
"/scratch/workspace/build-nulite-matrix/BUILDTYPE/debug/QUAL/s70-e15/label1/swarm/label2/SLF6/build/dk2nudata/v01_07_02/source/dk2nu/dk2nu/tree",
"/scratch/workspace/build-nulite-matrix/BUILDTYPE/debug/QUAL/s70-e15/label1/swarm/label2/SLF6/build/dk2nudata/v01_07_02/build/Linux64bit+2.6-2.12-e15-debug/tree/",
"RwCalculators",
"GENIE/RwCalculators",
"../include/GENIE/RwCalculators",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Reweight/src/",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Reweight/src/RwCalculators/",
"RwFramework",
"GENIE/RwFramework",
"../include/GENIE/RwFramework",
"/scratch/workspace/nu-release-build/v2_28_02/s79-e15/debug/build/genie/v3_00_04/Linux64bit+2.6-2.12-e15-debug/GENIE-Reweight/src/RwFramework/",
"/scratch/workspace/canvas-products/vdevelop/e15/SLF6/debug/build/fftw/v3_3_6_pl2/Linux64bit+2.6-2.12-debug/include",
"/cvmfs/nova.opensciencegrid.org/externals/root/v6_12_06a/Linux64bit+2.6-2.12-e15-debug/include",
"/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/iteration_bootstrap/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "make_spectra_from_reco_no_loop_C_ACLiC_dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "make_spectra_from_reco_no_loop_C_ACLiC_dict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif
#ifndef __ACLIC__
  #define __ACLIC__ 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "./make_spectra_from_reco_no_loop.C"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"make_spectra_from_reco_no_loop", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("make_spectra_from_reco_no_loop_C_ACLiC_dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_make_spectra_from_reco_no_loop_C_ACLiC_dict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_make_spectra_from_reco_no_loop_C_ACLiC_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_make_spectra_from_reco_no_loop_C_ACLiC_dict() {
  TriggerDictionaryInitialization_make_spectra_from_reco_no_loop_C_ACLiC_dict_Impl();
}
