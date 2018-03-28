// CVN includes
#include "CVN/art/CaffeNetHandler.h"
#include "CVN/func/TrainingData.h"

// ROOT includes
#include "TChain.h"
#include "TFile.h"
#include "TSystem.h"
#include "Cintex/Cintex.h"

using namespace std;

int main(int argc, char* argv[])
{
  /// Load predefined labels and functions for retrieving data.
  ROOT::Cintex::Cintex::Enable();
  gSystem->Load("libCVNFunc_dict");
  
  /// Name of the data tree
  TChain chain("cvndump/CVNTrainTree");
  
  /// Open files.
  ifstream list_file("/nova/ana/users/slin/temp/cvn_test/test_input.list");
  string ifname;
  while (list_file >> ifname) chain.Add(ifname.c_str());
  
  /// Assign branch for data retrieval
  cvn::TrainingData* data = new cvn::TrainingData();
  chain.SetBranchAddress("train", &data);
  
  /// Container to store timing of hits.
  TFile tinf("shifted_timing.root", "recreate");
  TTree tr("tr", "tree for storing timing information");
  float time;
  tr.Branch("time", &time, "time/F");
  
  /// Event loop.
  unsigned int entries = chain.GetEntries();
  for(unsigned int iEntry = 0; iEntry < entries; ++iEntry)
  {
    chain.GetEntry(iEntry);
    
    const int planes = data->fPMap.fNPlane/2;
    const int cells = data->fPMap.fNCell;
    
    vector<float> tx, ty;
    for (int iPlane = 0; iPlane < planes; ++iPlane) {
      for (int iCell = 0; iCell < cells; ++iCell) {
        if (data->fPMap.fTX.at(iCell + cells*iPlane) > 1e-3)
          tx.push_back(data->fPMap.fTX.at(iCell + cells*iPlane));
        if (data->fPMap.fTY.at(iCell + cells*iPlane) > 1e-3)
          ty.push_back(data->fPMap.fTY.at(iCell + cells*iPlane));
      }
    }
    
    float txmin = *min_element(tx.begin(), tx.end());
    float tymin = *min_element(ty.begin(), ty.end());
    float tmin  = min(txmin, tymin);
    
    for(unsigned int i = 0; i < tx.size(); i++) {
      time = tx[i] - tmin;
      tr.Fill();
    }
    for(unsigned int i = 0; i < ty.size(); i++) {
      time = ty[i] - tmin;
      tr.Fill();
    }
  }
  tr.Write();
  tinf.Close();
}
