/*
 * 
 * This file is to output the training and testing data for wrong sign studies
 * to hdf5 files.
 * 
 */

// Standard includes
#include <fstream>
#include <iostream>

// Boost includes
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

// CVN includes
#include "CVN/art/CaffeNetHandler.h"
#include "CVN/func/TrainingData.h"

// ROOT includes
#include "TChain.h"
#include "TFile.h"
#include "TSystem.h"
#include "Cintex/Cintex.h"

// HDF5 includes
#include "H5Cpp.h"

namespace po = boost::program_options;
using namespace H5;
using namespace std;

/// Time resolution. Obtained by accumulating all times of hits in a run.
/// Using the range across all hits in a run gets zero timing resolution.
/// Offset every times by the earliest hits in an event instead!
//~ const float tmin = 218483.890625;
//~ const float tmax = 228357.421875;
//~ const float tbin = (tmax-tmin)/255;

//------------------------------------------------------------------------------
// To write to HDF5, a contiguous memory block for data is required.
// This class allocates a big linear chunk of memory and records data shape.
template <class T>
class ContiguousArray4D
{
public:
  ContiguousArray4D(int, int, int, int);
  ~ContiguousArray4D();
  
  int fNEntries;
  int fNChannels;
  int fHeight;
  int fWidth;
  T**** fData;
  
private:
  T***  B; ///< B, C, and D are auxiliary arrays.
  T**   C; ///< B, C, and D are auxiliary arrays.
  T*    D; ///< B, C, and D are auxiliary arrays.
};

template <class T>
ContiguousArray4D<T>::ContiguousArray4D(int nentry, int nchannel, int height, int width):
fNEntries(nentry), fNChannels(nchannel), fHeight(height), fWidth(width)
{
  fData  = new T***[fNEntries];
  B = new T**[fNEntries*fNChannels];
  C = new T*[fNEntries*fNChannels*fHeight];
  D = new T[fNEntries*fNChannels*fHeight*fWidth];
  for(int i = 0; i < fNEntries; i++) {
    for(int j = 0; j < fNChannels; j++) {
      for(int k = 0; k < fHeight; k++) {
        C[fHeight*(fNChannels*i+j)+k] = D+(fHeight*(fNChannels*i+j)+k)*fWidth;
      }
      B[fNChannels*i+j] = C+fHeight*(fNChannels*i+j);
    }
    fData[i] = B+fNChannels*i;
  }
}

template <class T>
ContiguousArray4D<T>::~ContiguousArray4D()
{
  delete [] D;
  delete [] C;
  delete [] B;
  delete [] fData;
}

//------------------------------------------------------------------------------
template <class T>
class PixelMapData : public ContiguousArray4D<T>
{
public:
  PixelMapData(int a, int b, int c, int d) : ContiguousArray4D<T>(a, b, c, d) {};
  void FillCharge(cvn::TrainingData*, int, int, int); ///< First integer entry, second cell, third plane.
  void FillTime(cvn::TrainingData*, int, int, int, float); ///< First integer entry, second cell, third plane. Last time offset.
private:
  unsigned char DigitizeTime(float, float);
};

template <class T>
void PixelMapData<T>::FillCharge(cvn::TrainingData* data, int entry, int cell, int plane)
{
  const int ncells = data->fPMap.fNCell;
  /// Check index range.
  if(entry >= this->fNEntries || cell >= this->fHeight || plane >= this->fWidth) return;
  this->fData[entry][0][cell][plane] = cvn::ConvertToChar(data->fPMap.fPEX.at(cell + ncells*plane), true);
  this->fData[entry][2][cell][plane] = cvn::ConvertToChar(data->fPMap.fPEY.at(cell + ncells*plane), true);
}

template <class T>
void PixelMapData<T>::FillTime(cvn::TrainingData* data, int entry, int cell, int plane, float toffset)
{
  const int ncells = data->fPMap.fNCell;
  /// Check index range.
  if(entry >= this->fNEntries || cell >= this->fHeight || plane >= this->fWidth) return;
  this->fData[entry][1][cell][plane] = DigitizeTime(data->fPMap.fTX.at(cell + ncells*plane), toffset);
  this->fData[entry][3][cell][plane] = DigitizeTime(data->fPMap.fTY.at(cell + ncells*plane), toffset);
}

template <class T>
unsigned char PixelMapData<T>::DigitizeTime(float time, float toffset)
{
  /// In an independent timing study, time span is within about [0..350].
  float digtime = ceil(time-toffset);
  if(digtime > 255) return 255;
  if(digtime < 0) return 0;
  return digtime;
}

//------------------------------------------------------------------------------
// Here the assumption is made that the label shape is N x 1 x 1 x 1,
// where N is the number of entries.
template <class T>
class LabelData : public ContiguousArray4D<T>
{
public:
  LabelData(int a) : ContiguousArray4D<T>(a, 1, 1, 1) {};
  void FillLabel(cvn::TrainingData*, int);
};

template <class T>
void LabelData<T>::FillLabel(cvn::TrainingData* data, int entry)
{
  /// Check index range.
  if(entry >= this->fNEntries) return;
  this->fData[entry][0][0][0] = data->fInt;
}

//------------------------------------------------------------------------------
void HDF5Fill(const H5std_string& FILE_NAME, const H5std_string& DATASET_NAME, const DataType& dtype, PixelMapData<unsigned char>& data, LabelData<unsigned char>& label)
{
  try {
    // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    Exception::dontPrint();
    
    // Create a new file using the default property lists.
    H5File file(FILE_NAME, H5F_ACC_TRUNC);
    
    // Create the data space for the dataset.
    hsize_t dims[4];               // dataset dimensions
    dims[0] = data.fNEntries;
    dims[1] = data.fNChannels;
    dims[2] = data.fHeight;
    dims[3] = data.fWidth;
    DataSpace dataspace(4, dims);
    
    // Create the dataset.
    DataSet dataset = file.createDataSet(DATASET_NAME, dtype, dataspace);
    
    // Write the data to the dataset using default memory space, file
    // space, and transfer properties.
    dataset.write(&data.fData[0][0][0][0], dtype);
    
    // Create the data space for the labels.
    hsize_t ldims[4];               // label dimensions
    ldims[0] = label.fNEntries;
    ldims[1] = label.fNChannels;
    ldims[2] = label.fHeight;
    ldims[3] = label.fWidth;
    DataSpace labelspace(4, ldims);
    
    // Create the label dataset.
    DataSet ldataset = file.createDataSet("label", dtype, labelspace);
    ldataset.write(&label.fData[0][0][0][0], dtype);
  }
  catch(FileIException error) { // catch failure caused by the H5File operations
    error.printError();
    return;
  }
}

void fill(po::variables_map& opts)
{
  /// Load predefined labels and functions for retrieving data.
  ROOT::Cintex::Cintex::Enable();
  gSystem->Load("libCVNFunc_dict");
  
  /// Name of the data tree
  TChain chain(opts["TreeName"].as<string>().c_str());
  
  /// Open the input file (list)
  string input = opts["InputFile"].as<string>();
  
  if (boost::ends_with(input,".list")) {
    std::ifstream list_file(input.c_str());
    if (!list_file.is_open()) {
      std::cout << "Could not open " << input << std::endl;
      exit(1);
    }

    std::string ifname;
    while (list_file>>ifname)
      chain.Add(ifname.c_str());
      
  }//end if list file

  else if  (boost::ends_with(input,".root")) {
    chain.Add(input.c_str());
  }//end if root file
  else {
    cerr << "File extension is not recognised." << endl;
    exit(1);
  }
  
  /// Event dump container. Note that the event dump contains all possible
  /// information, including that not used by CVN, ex. time of hits.
  cvn::TrainingData* data = new cvn::TrainingData();
  chain.SetBranchAddress(opts["TrainingDataBranchName"].as<string>().c_str(), &data);
  
  unsigned int entries = chain.GetEntries();
  // Resize the number of entries to run through.
  entries = min<unsigned int>(entries, opts["NEvents"].as<unsigned int>());
  if(entries <= 0){
    std::cout << "Error: Input tree has no entries." << std::endl;
    exit(4);
  }
  
  /// Traning data need to be shuffled. Realizing this by making an ordered list
  /// of consecutive numbers and shuffle them.
  std::srand ( unsigned ( std::time(0) ) );
  std::vector<unsigned int> shuffled;
  for (unsigned int i = 0; i < entries; ++i)
  {
    shuffled.push_back(i);
  }
  std::random_shuffle( shuffled.begin(), shuffled.end() );
  
  /// Figure out the size of the train and test samples
  /// Call a 'block' a particular set of one test and nTrainPerTest train
  int blockSize = opts["NTrainPerTest"].as<int>() + 1;
  /// number of test is the number of blocks, using integer division
  int nTest     = 1 + entries / blockSize;
  /// number of training samples is number of blocks times train per test
  int nTrain    = entries / blockSize * opts["NTrainPerTest"].as<int>();
  /// Add on the entries from the last, potentially partial block, minus test
  if (entries % blockSize) nTrain += entries % blockSize - 1;
  
  /// Create arrays to hold image data. In these case there are 4 channels
  /// (features), namely, energy and time in x and y views.
  /// This data structure assumes that the data shapes are the same for all
  /// data.
  chain.GetEntry(0); // for getting the image dimensions
  const int nChannels = 4;
  const int planes = data->fPMap.fNPlane/2;
  const int cells = data->fPMap.fNCell;
  /// Data containers
  PixelMapData<unsigned char> trainingSample(nTrain, nChannels, cells, planes);
  PixelMapData<unsigned char> testSample(nTest, nChannels, cells, planes);
  LabelData<unsigned char>    trainingLabel(nTrain);
  LabelData<unsigned char>    testLabel(nTrain);
  
  
  /// Start event loop and extract data
  int iTrain = 0;
  int iTest  = 0;
  for(unsigned int iEntry = 0; iEntry < entries; ++iEntry)
  {
    chain.GetEntry(shuffled[iEntry]);
    
    /// In order to offset times of hits by the time of the earliest hit,
    /// 2 passes of loops are needed.
    if(iEntry % (blockSize))
    {
      vector<float> tx, ty; ///< timing containers
      
      for (int iPlane = 0; iPlane < planes; ++iPlane)
        for (int iCell = 0; iCell < cells; ++iCell) {
          trainingSample.FillCharge(data, iTrain, iCell, iPlane);
          if (data->fPMap.fTX.at(iCell + cells*iPlane) > 1e-3)
            tx.push_back(data->fPMap.fTX.at(iCell + cells*iPlane));
          if (data->fPMap.fTY.at(iCell + cells*iPlane) > 1e-3)
            ty.push_back(data->fPMap.fTY.at(iCell + cells*iPlane));
        }
      
      /// Find timing offset.
      float txmin = *min_element(tx.begin(), tx.end());
      float tymin = *min_element(ty.begin(), ty.end());
      float tmin  = min(txmin, tymin);
      /// Second pass...
      for (int iPlane = 0; iPlane < planes; ++iPlane)
        for (int iCell = 0; iCell < cells; ++iCell)
          trainingSample.FillTime(data, iTrain, iCell, iPlane, tmin);
      
      /// Fill event label
      trainingLabel.FillLabel(data, iTrain);
      
      iTrain++;
    }
    else { // Fill test sample.
      vector<float> tx, ty; ///< timing containers
      
      for (int iPlane = 0; iPlane < planes; ++iPlane)
        for (int iCell = 0; iCell < cells; ++iCell) {
          testSample.FillCharge(data, iTest, iCell, iPlane);
          if (data->fPMap.fTX.at(iCell + cells*iPlane) > 1e-3)
            tx.push_back(data->fPMap.fTX.at(iCell + cells*iPlane));
          if (data->fPMap.fTY.at(iCell + cells*iPlane) > 1e-3)
            ty.push_back(data->fPMap.fTY.at(iCell + cells*iPlane));
        }
      
      /// Find timing offset.
      float txmin = *min_element(tx.begin(), tx.end());
      float tymin = *min_element(ty.begin(), ty.end());
      float tmin  = min(txmin, tymin);
      /// Second pass...
      for (int iPlane = 0; iPlane < planes; ++iPlane)
        for (int iCell = 0; iCell < cells; ++iCell)
          testSample.FillTime(data, iTest, iCell, iPlane, tmin);
      
      /// Fill test label
      testLabel.FillLabel(data, iTest);
      
      iTest++;
    }
    
    // Print progress.
    int eprog = iEntry + 1;
    if(!(eprog % 100)) cout << eprog << " events finished." << endl;
  } // Done with all data filling.

  HDF5Fill("training_data.h5", "data", PredType::STD_U8LE, trainingSample, trainingLabel);
  HDF5Fill("test_data.h5", "data", PredType::STD_U8LE, testSample, testLabel);
}

int main(int argc, char* argv[])
{
  try {
    /// Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("NEvents,n", po::value<unsigned int>()->default_value(UINT_MAX), "Number of events to run through")
      ("InputFile,i", po::value<string>()->default_value("/nova/ana/users/slin/temp/cvn_test/test_input.list"), "file name of input file (.root) or list (.list)")
      ("TreeName", po::value<string>()->default_value("cvndump/CVNTrainTree"), "name of the data tree")
      ("TrainingDataBranchName", po::value<string>()->default_value("train"), "name of the training data branch")
      ("NTrainPerTest", po::value<int>()->default_value(4), "Ratio of training to test data. Ex. 4 means 80/20 split.")
    ;
    
    po::variables_map opts;
    po::store(po::parse_command_line(argc, argv, desc), opts);
    po::notify(opts);
    
    if (opts.count("help")) {
      cout << desc << endl;
      return 0;
    }
    
    /// Check file existence.
    if(!boost::filesystem::exists(opts["InputFile"].as<string>())) {
      cerr << "File " << opts["InputFile"].as<string>() << " does not exist." << endl;
      return 1;
    }
    
    fill(opts);
  }
  catch(exception& e) {
    cerr << "error: " << e.what() << endl;
    return 1;
  }
  catch(...) {
    cerr << "Exception of unknown type!" << endl;
  }
  
  return 0;
}
