/* 
 * TMVAAnalyzer.cxx
 *
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */

// Standard
#include <iostream>
#include <string>
#include <sstream>
#include <csignal>
#include <cstdlib>
using namespace std;

// ROOT
#include <TROOT.h>
#include <TEnv.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include "TMVA/Factory.h"

// MEGAlib
#include "MGlobal.h"


////////////////////////////////////////////////////////////////////////////////


//! A standalone program based on MEGAlib and ROOT
class TMVAAnalyzer
{
public:
  //! Default constructor
  TMVAAnalyzer();
  //! Default destructor
  ~TMVAAnalyzer();
  
  //! Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  //! Analyze what eveer needs to be analyzed...
  bool Analyze();
  //! Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }

private:
  //! True, if the analysis needs to be interrupted
  bool m_Interrupt;
  //! The file name
  MString m_FileName;
};


////////////////////////////////////////////////////////////////////////////////


//! Default constructor
TMVAAnalyzer::TMVAAnalyzer() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


////////////////////////////////////////////////////////////////////////////////


//! Default destructor
TMVAAnalyzer::~TMVAAnalyzer()
{
  // Intentionally left blank
}


////////////////////////////////////////////////////////////////////////////////


//! Parse the command line
bool TMVAAnalyzer::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: TMVAAnalyzer <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -f:   file name (e.g. Tree.tmva.seq2.good.root - bad is loaded automatically)"<<endl;
  Usage<<"         -h:   print this help"<<endl;
  Usage<<endl;

  string Option;

  // Check for help
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "-h" || Option == "--help" || Option == "?" || Option == "-?") {
      cout<<Usage.str()<<endl;
      return false;
    }
  }

  // Now parse the command line options:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];

    // First check if each option has sufficient arguments:
    // Single argument
    if (Option == "-f") {
      if (!((argc > i+1) && 
            (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
        cout<<Usage.str()<<endl;
        return false;
      }
    } 
    // Multiple arguments template
    /*
    else if (Option == "-??") {
      if (!((argc > i+2) && 
            (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) && 
            (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs two arguments!"<<endl;
        cout<<Usage.str()<<endl;
        return false;
      }
    }
    */

    // Then fulfill the options:
    if (Option == "-f") {
      m_FileName = argv[++i];
      cout<<"Accepting file name: "<<m_FileName<<endl;
    } else {
      cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
      cout<<Usage.str()<<endl;
      return false;
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


//! Do whatever analysis is necessary
bool TMVAAnalyzer::Analyze()
{
  if (m_Interrupt == true) return false;

  TFile* Results = new TFile("Results.root", "recreate");
  
  TFile* SourceFile = new TFile(m_FileName); 
  TTree* SourceTree = (TTree*) SourceFile->Get("Good");
  if (SourceTree == nullptr) {
    cout<<"Error reading good events tree"<<endl;
    return false;
  }
  
  m_FileName.ReplaceAtEndInPlace(".good.root", ".bad.root");
  TFile* BackgroundFile = new TFile(m_FileName); 
  TTree* BackgroundTree = (TTree*) BackgroundFile->Get("Bad");
  if (BackgroundTree == nullptr) {
    cout<<"Error reading bad events tree"<<endl;
    return false;
  }
      
    
  TMVA::Factory* F = new TMVA::Factory("MVAnalysis", Results, "V");
  F->AddSignalTree(SourceTree, 1.0);
  F->AddBackgroundTree(BackgroundTree, 1.0);
    
  TObjArray* Branches = SourceTree->GetListOfBranches();
  for (int b = 0; b < Branches->GetEntries(); ++b) {
    TBranch* B = dynamic_cast<TBranch*>(Branches->At(b));
    TString Name = B->GetName();
    if (Name != "SimulationIDs") {
      F->AddVariable(Name, 'D');
    }
  }
  
  F->PrepareTrainingAndTestTree("", "SplitMode=Random:V");
  //F->BookMethod(TMVA::Types::kLikelihood, "LikelihoodPCA", 
  //              "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=10:NSmoothBkg[0]=10:NSmooth=5:NAvEvtPerBin=30:VarTransform=PCA");
  //F->BookMethod(TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:NCycles=200:HiddenLayers=N+1,N:TestRate=5");
  
  // BFGS training with bayesian regulators
  //F->BookMethod(TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" );
  
  // Boosted decision tree: Decorrelation + Adaptive Boost
  F->BookMethod(TMVA::Types::kBDT, "BDTD", "!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=1.5:VarTransform=Decorrelate");
  
  //F->BookMethod(TMVA::Types::kPDEFoam, "PDEFoamBoost", "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T" );
  
  // Adaptive Boost
  F->BookMethod(TMVA::Types::kBDT, "BDT", "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );
  
  
  // Support vector machine
  //F->BookMethod(TMVA::Types::kSVM, "SVM - var=norm", "Gamma=0.25:Tol=0.001:VarTransform=Norm" );
  //F->BookMethod(TMVA::Types::kSVM, "SVM - var=pca", "Gamma=0.25:Tol=0.001:VarTransform=PCA" );
  //F->BookMethod(TMVA::Types::kSVM, "SVM - var=decorrelate", "Gamma=0.25:Tol=0.001:VarTransform=Decorrelate" );
  
  
  
  // K-Nearest Neighbour classifier (KNN)
  //F->BookMethod(TMVA::Types::kKNN, "KNN", "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );
    
    
  
  F->TrainAllMethods();
  F->TestAllMethods();
  F->EvaluateAllMethods();
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


TMVAAnalyzer* g_Prg = 0;
int g_NInterruptCatches = 1;


////////////////////////////////////////////////////////////////////////////////


//! Called when an interrupt signal is flagged
//! All catched signals lead to a well defined exit of the program
void CatchSignal(int a)
{
  if (g_Prg != 0 && g_NInterruptCatches-- > 0) {
    cout<<"Catched signal Ctrl-C (ID="<<a<<"):"<<endl;
    g_Prg->Interrupt();
  } else {
    abort();
  }
}


////////////////////////////////////////////////////////////////////////////////


//! Main program
int main(int argc, char** argv)
{
  // Catch a user interupt for graceful shutdown
  // signal(SIGINT, CatchSignal);

  // Initialize global MEGALIB variables, especially mgui, etc.
  MGlobal::Initialize("Standalone", "a standalone example program");

  TApplication TMVAAnalyzerApp("TMVAAnalyzerApp", 0, 0);

  g_Prg = new TMVAAnalyzer();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  //TMVAAnalyzerApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}


////////////////////////////////////////////////////////////////////////////////