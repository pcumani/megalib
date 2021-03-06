/* 
 * ResponseManipulator.cxx
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
#include <cctype>
#include <vector>
using namespace std;

// ROOT
#include <TROOT.h>
#include <TEnv.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TList.h>
#include <TSystem.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>

// MEGAlib
#include "MGlobal.h"
#include "MAssert.h"
#include "MStreams.h"
#include "MFileResponse.h"
#include "MResponseMatrix.h"
#include "MResponseMatrixO1.h"
#include "MResponseMatrixO2.h"
#include "MResponseMatrixO3.h"
#include "MResponseMatrixO4.h"
#include "MResponseMatrixO5.h"
#include "MResponseMatrixO6.h"
#include "MResponseMatrixO7.h"
#include "MResponseMatrixO8.h"
#include "MResponseMatrixO9.h"
#include "MResponseMatrixO10.h"
#include "MResponseMatrixO11.h"
#include "MResponseMatrixO12.h"
#include "MResponseMatrixO13.h"
#include "MResponseMatrixO14.h"
#include "MResponseMatrixO15.h"
#include "MResponseMatrixO16.h"
#include "MResponseMatrixO17.h"


/******************************************************************************/

class ResponseManipulator
{
public:
  /// Default constructor
  ResponseManipulator();
  /// Default destructor
  ~ResponseManipulator();
  
  /// Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  /// Analyze what eveer needs to be analyzed...
  bool Analyze();
  /// Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }

protected:
  bool Append();
  bool Show();
  bool Divide();
  bool Statistics();
  bool Ratio();
  bool Probability();
  bool Join();
  bool FindFiles(MString Prefix, MString Type);

private:
  /// True, if the analysis needs to be interrupted
  bool m_Interrupt;

  ///

  /// 
  bool m_Statistics;
  string m_FileName;

  /// 
  bool m_Append;
  vector<string> m_AppendFileNames;

  /// 
  bool m_Divide;
  string m_DividendFileName;
  string m_DivisorFileName;

  /// 
  bool m_Ratio;

  /// 
  bool m_Probability;

  /// 
  bool m_Join;
  string m_Prefix;

  /// Viewing options:
  bool m_Show;
  float m_x1;
  float m_x2;
  float m_x3;
  float m_x4;
  float m_x5;
  float m_x6;
  float m_x7;
  float m_x8;
  float m_x9;
  float m_x10;
  float m_x11;
  float m_x12;
  float m_x13;
  float m_x14;
  float m_x15;
  float m_x16;
  float m_x17;
  bool m_Normalized;

  ///
  bool m_Zero;

  /// 
  int m_NSmooths;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
ResponseManipulator::ResponseManipulator() : m_Interrupt(false)
{
  // Intentionally left blanck

  m_Append = false;
  m_Show = false;
  m_Divide = false;
  m_Ratio = false;
  m_Probability = false;
  m_Join = false;
  m_Normalized = false;
  m_Zero = false;

  m_NSmooths = 0;

  //gStyle->SetPalette(1, 0);

  // banana -> yellow -> orange -> red -> dark red
  const unsigned int Number = 7;
  Double_t Red[Number]   = { 0.2500, 0.5000, 1.0000, 1.0000, 1.0000, 1.0000, 1.0000 };
  Double_t Green[Number] = { 0.0000, 0.0000, 0.0000, 0.5000, 1.0000, 1.0000, 1.0000 };
  Double_t Blue[Number]  = { 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.5000, 0.7500 };
  Double_t Stops[Number] = { 0.0000, 0.1000, 0.3000, 0.5000,  0.700, 0.9000, 1.0000 };

  // Luminiszenz:
  double Min = 0.3*Red[0] + 0.58*Green[0] + 0.11*Blue[0];
  double Max = 0.3*Red[Number-1] + 0.58*Green[Number-1] + 0.11*Blue[Number-1];
  Stops[0] = 0.0;
  Stops[Number-1] = 1.0;
  for (unsigned int i = 1; i < Number-1; ++i) {
    double Value = (0.3*Red[i]+0.58*Green[i]+0.11*Blue[i]-Min)/(Max-Min);
    Stops[i] = Value; 
  }

  gStyle->CreateGradientColorTable(Number, Stops, Red, Green, Blue, 1000);


  gStyle->SetLabelSize(0.04, "X");
  gStyle->SetLabelSize(0.04, "Y");
  gStyle->SetLabelSize(0.04, "Z");
  gStyle->SetTitleSize(0.05, "X");
  gStyle->SetTitleSize(0.05, "Y");
  gStyle->SetTitleSize(0.05, "Z");
  gStyle->SetTitleOffset(1.3, "X");
  gStyle->SetTitleOffset(1.6, "Y");
  //gStyle->SetTitleOffset(1.2*gStyle->GetTitleOffset("Z"), "Z");
  
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadRightMargin(0.14);
  gStyle->SetPadTopMargin(0.15);

  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);

  gStyle->SetOptStat(0);

  gStyle->SetHistFillColor(8);
}


/******************************************************************************
 * Default destructor
 */
ResponseManipulator::~ResponseManipulator()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool ResponseManipulator::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: ResponseManipulator <options>"<<endl;
  Usage<<"    General options:"<<endl;
  // Usage<<"         -f:   <file name>"<<endl;
  Usage<<"         -v:   view <file name> <x1> <x2> <x3> <x4> <x5> <x6> <x7> <x8> <x9> <x10> <x11> <x12> <x13> <x14> <x15> <x16> <x17>"<<endl;
  Usage<<"         -a:   append <file name>"<<endl;
  Usage<<"         -j:   join <file name prefix>"<<endl;
  Usage<<"         -d:   divide <file name> <file name>"<<endl;
  Usage<<"         -r:   ratio <file name> <file name>"<<endl;
  Usage<<"         -p:   probability <good file name> <bad file name>"<<endl;
  Usage<<"         -s:   show statistics <file name>"<<endl;
  Usage<<"         -m:   smooth view <n> times"<<endl;
  Usage<<"         -n:   normalized view"<<endl;
  Usage<<"         -z:   zero is a very small number in the view"<<endl;
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
    if (Option == "-f" || Option == "-a" || Option  == "-j" || Option == "-s") {
			if (!((argc > i+1) && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0))){
				cout<<"Error: Option "<<argv[i][1]<<" needs an argument!"<<endl;
				cout<<Usage.str()<<endl;
				return false;
			}
		} 
		// Two arguments:
		else if (Option == "-d" || Option == "-r") {
			if (!((argc > i+2) && 
            (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) && 
            (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0))){
				cout<<"Error: Option "<<argv[i][1]<<" needs two arguments!"<<endl;
				cout<<Usage.str()<<endl;
				return false;
			}
    }
    // Multiple arguments:
// 		else if (Option == "-v") {
// 			if (!((argc > i+10) && 
//             (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) &&
//             (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0) &&
//             (argv[i+3][0] != '-' || isalpha(argv[i+3][1]) == 0) &&
//             (argv[i+4][0] != '-' || isalpha(argv[i+4][1]) == 0) &&
//             (argv[i+5][0] != '-' || isalpha(argv[i+5][1]) == 0) &&
//             (argv[i+6][0] != '-' || isalpha(argv[i+6][1]) == 0) &&
//             (argv[i+7][0] != '-' || isalpha(argv[i+7][1]) == 0) &&
//             (argv[i+8][0] != '-' || isalpha(argv[i+8][1]) == 0) &&
//             (argv[i+9][0] != '-' || isalpha(argv[i+9][1]) == 0) &&
//             (argv[i+10][0] != '-' || isalpha(argv[i+10][1]) == 0) &&
//             (argv[i+11][0] != '-' || isalpha(argv[i+11][1]) == 0) &&
//             (argv[i+12][0] != '-' || isalpha(argv[i+12][1]) == 0) &&
//             (argv[i+13][0] != '-' || isalpha(argv[i+13][1]) == 0))){
// 				cout<<"Error: Option "<<argv[i][1]<<" needs ten arguments!"<<endl;
// 				cout<<Usage.str()<<endl;
// 				return false;
// 			}
// 		}

		// Then fulfill the options:
    if (Option == "-f") {
      m_FileName = argv[++i];
			cout<<"Accepting file name: "<<m_FileName<<endl;
    } else if (Option == "-a") {
      m_AppendFileNames.push_back(argv[++i]);
      m_Append = true;
			cout<<"Accepting file name for appending: "<<m_AppendFileNames.back()<<endl;
    } else if (Option == "-d") {
      m_DividendFileName = argv[++i];
      m_DivisorFileName = argv[++i];
      m_Divide = true;
			cout<<"Accepting file name for dividing: "
          <<m_DividendFileName<<"/"<<m_DivisorFileName<<endl;
    } else if (Option == "-r") {
      m_DividendFileName = argv[++i];
      m_DivisorFileName = argv[++i];
      m_Ratio = true;
			cout<<"Accepting file name for ratio: "
          <<m_DividendFileName<<"/"<<m_DivisorFileName<<endl;
    } else if (Option == "-p") {
      m_DividendFileName = argv[++i];
      m_DivisorFileName = argv[++i];
      m_Probability = true;
			cout<<"Accepting file name for probability: "
          <<m_DividendFileName<<"/"<<m_DivisorFileName<<endl;
    } else if (Option == "-j") {
      m_Prefix = argv[++i];
      m_Join = true;
			cout<<"Accepting file prefix for join: "
          <<m_Prefix<<endl;
    } else if (Option == "-s") {
      m_FileName = argv[++i];
      m_Statistics = true;
			cout<<"Accepting show statistics for "<<m_FileName<<endl;
    } else if (Option == "-m") {
      m_NSmooths = atoi(argv[++i]);
			cout<<"Accepting to smooth view "<<m_NSmooths<<" times"<<endl;
    } else if (Option == "-n") {
      m_Normalized = true;
			cout<<"Accepting view normalized "<<endl;
    } else if (Option == "-z") {
      m_Zero = true;
			cout<<"Accepting view zeroed "<<endl;
    } else if (Option == "-v") {
      m_FileName = argv[++i];
      string next;
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x1 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x1 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x1 = MResponseMatrix::c_ShowZ;
      else                  m_x1 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x2 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x2 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x2 = MResponseMatrix::c_ShowZ;
      else                  m_x2 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x3 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x3 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x3 = MResponseMatrix::c_ShowZ;
      else                  m_x3 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x4 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x4 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x4 = MResponseMatrix::c_ShowZ;
      else                  m_x4 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x5 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x5 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x5 = MResponseMatrix::c_ShowZ;
      else                  m_x5 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x6 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x6 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x6 = MResponseMatrix::c_ShowZ;
      else                  m_x6 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x7 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x7 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x7 = MResponseMatrix::c_ShowZ;
      else                  m_x7 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x8 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x8 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x8 = MResponseMatrix::c_ShowZ;
      else                  m_x8 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x9 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x9 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x9 = MResponseMatrix::c_ShowZ;
      else                  m_x9 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x10 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x10 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x10 = MResponseMatrix::c_ShowZ;
      else                  m_x10 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x11 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x11 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x11 = MResponseMatrix::c_ShowZ;
      else                  m_x11 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x12 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x12 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x12 = MResponseMatrix::c_ShowZ;
      else                  m_x12 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x13 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x13 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x13 = MResponseMatrix::c_ShowZ;
      else                  m_x13 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x14 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x14 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x14 = MResponseMatrix::c_ShowZ;
      else                  m_x14 = atof(next.c_str());
      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x15 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x15 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x15 = MResponseMatrix::c_ShowZ;
      else                  m_x15 = atof(next.c_str());

      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x16 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x16 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x16 = MResponseMatrix::c_ShowZ;
      else                  m_x16 = atof(next.c_str());

      if (argc > i+1 && (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0)) {
        next = string(argv[i+1]);
        ++i;
      } else {
        next = "0";
      }
      if (next == "x")      m_x17 = MResponseMatrix::c_ShowX;
      else if (next == "y") m_x17 = MResponseMatrix::c_ShowY;
      else if (next == "z") m_x17 = MResponseMatrix::c_ShowZ;
      else                  m_x17 = atof(next.c_str());

      m_Show = true;

      cout<<"Accepting show options: "<<m_x1<<", "<<m_x2<<", "
          <<m_x3<<", "<<m_x4<<", "<<m_x5<<", "<<m_x6<<", "<<m_x7<<", "
          <<m_x8<<", "<<m_x9<<", "<<m_x10<<", "<<m_x11<<", "<<m_x12<<", "
          <<m_x13<<", "<<m_x14<<", "<<m_x15<<", "<<m_x16<<", "<<m_x17<<endl;
		} else {
			cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
			cout<<Usage.str()<<endl;
			return false;
		}
  }

  if (m_Append == true || m_Show == true || m_Statistics == true) {
    if (m_FileName == "") {
      cout<<"Error: No file name given!"<<endl;
      cout<<Usage.str()<<endl;
      return false;  
    }
  }
  if (m_Append == false && m_Show == false && m_Statistics == false && 
      m_Divide == false && m_Ratio == false && m_Join == false) {
    cout<<"Error: You must either append or divide or show a file or the statistics!"<<endl;
    cout<<Usage.str()<<endl;
    return false;
  }

  // Check if files exist:
  if (m_Show == true || m_Append == true || m_Statistics == true) {
    if (MFile::FileExists(m_FileName.c_str()) == false) {
      cout<<"Error: File \""<<m_FileName<<"\" does not exist!"<<endl;
      return false;
    }
  }
  for (unsigned int f = 0; f < m_AppendFileNames.size(); ++f) {
    if (MFile::FileExists(m_AppendFileNames[f].c_str()) == false) {
      cout<<"Error: File \""<<m_AppendFileNames[f]<<"\" does not exist!"<<endl;
      return false;
    }
  }
  if (m_Divide == true || m_Ratio == true || m_Probability == true) {
    if (MFile::FileExists(m_DividendFileName.c_str()) == false) {
      cout<<"Error: File \""<<m_DividendFileName<<"\" does not exist!"<<endl;
      return false;
    }
    if (MFile::FileExists(m_DivisorFileName.c_str()) == false) {
      cout<<"Error: File \""<<m_DivisorFileName<<"\" does not exist!"<<endl;
      return false;
    }    
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool ResponseManipulator::Analyze()
{
  if (m_Interrupt == true) return false;

  if (m_Append == true) Append();
  if (m_Show == true) Show();
  if (m_Statistics == true) Statistics();
  if (m_Divide == true) Divide();
  if (m_Ratio == true) Ratio();
  if (m_Probability == true) Probability();
  if (m_Join == true) Join();

  return true;
}


/******************************************************************************
 * Append a response matrix to this one:
 */
bool ResponseManipulator::Append()
{
  MFileResponse File;
  MResponseMatrix* R = File.Read(m_FileName.c_str());
  if (R == 0) return false;

  for (unsigned int f = 0; f < m_AppendFileNames.size(); ++f) {
    MFileResponse AppendFile;
    MResponseMatrix* RAppend = AppendFile.Read(m_AppendFileNames[f].c_str());
  if (RAppend == 0) return false;

    if (R->GetOrder() != RAppend->GetOrder()) {
      mout<<"Cannot append file, because they are of different order!"<<endl;
    } else {
      if (R->GetOrder() == 1) {
        *dynamic_cast<MResponseMatrixO1*>(R) += 
          *dynamic_cast<MResponseMatrixO1*>(RAppend);
      } else if (R->GetOrder() == 2) {
        *dynamic_cast<MResponseMatrixO2*>(R) += 
          *dynamic_cast<MResponseMatrixO2*>(RAppend);
      } else if (R->GetOrder() == 3) {
        *dynamic_cast<MResponseMatrixO3*>(R) += 
          *dynamic_cast<MResponseMatrixO3*>(RAppend);
      } else if (R->GetOrder() == 4) {
        *dynamic_cast<MResponseMatrixO4*>(R) += 
          *dynamic_cast<MResponseMatrixO4*>(RAppend);
      } else if (R->GetOrder() == 5) {
        *dynamic_cast<MResponseMatrixO5*>(R) += 
          *dynamic_cast<MResponseMatrixO5*>(RAppend);
      } else if (R->GetOrder() == 6) {
        *dynamic_cast<MResponseMatrixO6*>(R) += 
          *dynamic_cast<MResponseMatrixO6*>(RAppend);
      } else if (R->GetOrder() == 7) {
        *dynamic_cast<MResponseMatrixO7*>(R) += 
          *dynamic_cast<MResponseMatrixO7*>(RAppend);
      } else if (R->GetOrder() == 8) {
        *dynamic_cast<MResponseMatrixO8*>(R) += 
          *dynamic_cast<MResponseMatrixO8*>(RAppend);
      } else if (R->GetOrder() == 9) {
        *dynamic_cast<MResponseMatrixO9*>(R) += 
          *dynamic_cast<MResponseMatrixO9*>(RAppend);
      } else if (R->GetOrder() == 10) {
        *dynamic_cast<MResponseMatrixO10*>(R) += 
          *dynamic_cast<MResponseMatrixO10*>(RAppend);
      } else if (R->GetOrder() == 11) {
        *dynamic_cast<MResponseMatrixO11*>(R) += 
          *dynamic_cast<MResponseMatrixO11*>(RAppend);
      } else if (R->GetOrder() == 12) {
        *dynamic_cast<MResponseMatrixO12*>(R) += 
          *dynamic_cast<MResponseMatrixO12*>(RAppend);
      } else if (R->GetOrder() == 13) {
        *dynamic_cast<MResponseMatrixO13*>(R) += 
          *dynamic_cast<MResponseMatrixO13*>(RAppend);
      } else if (R->GetOrder() == 14) {
        *dynamic_cast<MResponseMatrixO14*>(R) += 
          *dynamic_cast<MResponseMatrixO14*>(RAppend);
      } else if (R->GetOrder() == 15) {
        *dynamic_cast<MResponseMatrixO15*>(R) += 
          *dynamic_cast<MResponseMatrixO15*>(RAppend);
      } else if (R->GetOrder() == 16) {
        *dynamic_cast<MResponseMatrixO16*>(R) += 
          *dynamic_cast<MResponseMatrixO16*>(RAppend);
      } else if (R->GetOrder() == 17) {
        *dynamic_cast<MResponseMatrixO17*>(R) += 
          *dynamic_cast<MResponseMatrixO17*>(RAppend);
      } else {
        merr<<"Unsupported matrix order: "<<R->GetOrder()<<endl; 
      }
    }

    delete RAppend;
  }

  R->Write((m_FileName + ".new").c_str(), true);
  delete R;

  return true;
}


/******************************************************************************
 * Find Files:
 */
bool ResponseManipulator::FindFiles(MString Prefix, MString Type)
{
  TSystemDirectory D(".", gSystem->pwd());
  TList* Files = D.GetListOfFiles();
  if (Files == 0) {
    mout<<"Can't get files!"<<endl;
    return false;
  }

  MFileResponse File;
  MResponseMatrix* First = 0;
  MResponseMatrix* Append = 0;
  for (int i = 0; i <= Files->LastIndex(); ++i) {
    MString Name = Files->At(i)->GetName();
    cout<<"Checking "<<Name<<"... ";
    if (Name.BeginsWith(Prefix) == true && Name.EndsWith(Type) == true) {
      cout<<"ok"<<endl;
      if (First == 0) {
        First = File.Read(Name);
        massert(First != 0);
      } else {
        Append = File.Read(Name);
        massert(Append != 0);
        if (First->GetOrder() != Append->GetOrder()) {
          mout<<"Cannot append file, because they are of different order!"<<endl;
        } else {
          if (First->GetOrder() == 1) {
            *dynamic_cast<MResponseMatrixO1*>(First) += 
              *dynamic_cast<MResponseMatrixO1*>(Append);
          } else if (First->GetOrder() == 2) {
            *dynamic_cast<MResponseMatrixO2*>(First) += 
              *dynamic_cast<MResponseMatrixO2*>(Append);
          } else if (First->GetOrder() == 3) {
            *dynamic_cast<MResponseMatrixO3*>(First) += 
              *dynamic_cast<MResponseMatrixO3*>(Append);
          } else if (First->GetOrder() == 4) {
            *dynamic_cast<MResponseMatrixO4*>(First) += 
              *dynamic_cast<MResponseMatrixO4*>(Append);
          } else if (First->GetOrder() == 5) {
            *dynamic_cast<MResponseMatrixO5*>(First) += 
              *dynamic_cast<MResponseMatrixO5*>(Append);
          } else if (First->GetOrder() == 6) {
            *dynamic_cast<MResponseMatrixO6*>(First) += 
              *dynamic_cast<MResponseMatrixO6*>(Append);
          } else if (First->GetOrder() == 7) {
            *dynamic_cast<MResponseMatrixO7*>(First) += 
              *dynamic_cast<MResponseMatrixO7*>(Append);
          } else if (First->GetOrder() == 8) {
            *dynamic_cast<MResponseMatrixO8*>(First) += 
              *dynamic_cast<MResponseMatrixO8*>(Append);
          } else if (First->GetOrder() == 9) {
            *dynamic_cast<MResponseMatrixO9*>(First) += 
              *dynamic_cast<MResponseMatrixO9*>(Append);
          } else if (First->GetOrder() == 10) {
            *dynamic_cast<MResponseMatrixO10*>(First) += 
              *dynamic_cast<MResponseMatrixO10*>(Append);
          } else if (First->GetOrder() == 11) {
            *dynamic_cast<MResponseMatrixO11*>(First) += 
              *dynamic_cast<MResponseMatrixO11*>(Append);
          } else if (First->GetOrder() == 12) {
            *dynamic_cast<MResponseMatrixO12*>(First) += 
              *dynamic_cast<MResponseMatrixO12*>(Append);
          } else if (First->GetOrder() == 13) {
            *dynamic_cast<MResponseMatrixO13*>(First) += 
              *dynamic_cast<MResponseMatrixO13*>(Append);
          } else if (First->GetOrder() == 14) {
            *dynamic_cast<MResponseMatrixO14*>(First) += 
              *dynamic_cast<MResponseMatrixO14*>(Append);
          } else if (First->GetOrder() == 15) {
            *dynamic_cast<MResponseMatrixO15*>(First) += 
              *dynamic_cast<MResponseMatrixO15*>(Append);
          } else if (First->GetOrder() == 16) {
            *dynamic_cast<MResponseMatrixO16*>(First) += 
              *dynamic_cast<MResponseMatrixO16*>(Append);
          } else if (First->GetOrder() == 17) {
            *dynamic_cast<MResponseMatrixO17*>(First) += 
              *dynamic_cast<MResponseMatrixO17*>(Append);
          } else {
            merr<<"Unsupported matrix order: "<<First->GetOrder()<<endl; 
          }
        }
        delete Append;
      }
    } else {
      cout<<"not ok"<<endl;
    }
  }

  if (First != 0) {
    First->Write(Prefix + Type, true);
    delete First;
  } else {
    mout<<"No File has been opened!"<<endl;
  }

  return true;
}


/******************************************************************************
 * Append a response matrix to this one:
 */
bool ResponseManipulator::Join()
{
  FindFiles(m_Prefix, ".tc.goodbad.rsp");
  FindFiles(m_Prefix, ".tc.start.good.rsp");
  FindFiles(m_Prefix, ".tc.start.bad.rsp");
  FindFiles(m_Prefix, ".tc.stop.good.rsp");
  FindFiles(m_Prefix, ".tc.stop.bad.rsp");
  FindFiles(m_Prefix, ".tc.compton.good.rsp");
  FindFiles(m_Prefix, ".tc.compton.bad.rsp");
  FindFiles(m_Prefix, ".tc.comptondistance.good.rsp");
  FindFiles(m_Prefix, ".tc.comptondistance.bad.rsp");
  FindFiles(m_Prefix, ".tc.photodistance.good.rsp");
  FindFiles(m_Prefix, ".tc.photodistance.bad.rsp");

  FindFiles(m_Prefix, ".t.goodbad.rsp");
  FindFiles(m_Prefix, ".t.central.good.rsp");
  FindFiles(m_Prefix, ".t.central.bad.rsp");
  FindFiles(m_Prefix, ".t.start.good.rsp");
  FindFiles(m_Prefix, ".t.start.bad.rsp");
  FindFiles(m_Prefix, ".t.stop.good.rsp");
  FindFiles(m_Prefix, ".t.stop.bad.rsp");
  FindFiles(m_Prefix, ".t.dual.good.rsp");
  FindFiles(m_Prefix, ".t.dual.bad.rsp");
  
  return true;
}


/******************************************************************************
 * Divide two response matrices:
 */
bool ResponseManipulator::Divide()
{
  MFileResponse File;
  MResponseMatrix* Zahler = File.Read(m_DividendFileName.c_str());
  if (Zahler == 0) {
    mout<<"Unable to open Dividend"<<endl;
    return false;
  }
  MResponseMatrix* Nenner = File.Read(m_DivisorFileName.c_str());
  if (Nenner == 0) {
    mout<<"Unable to open Dividor"<<endl;
    return false;
  }
  if (Zahler->GetOrder() != Nenner->GetOrder()) {
    mout<<"Cannot append file, because they are of different order!"<<endl;
  } else {
    if (Zahler->GetOrder() == 1) {
      *dynamic_cast<MResponseMatrixO1*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO1*>(Nenner);
    } else if (Zahler->GetOrder() == 2) {
      *dynamic_cast<MResponseMatrixO2*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO2*>(Nenner);
    } else if (Zahler->GetOrder() == 3) {
      *dynamic_cast<MResponseMatrixO3*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO3*>(Nenner);
    } else if (Zahler->GetOrder() == 4) {
      *dynamic_cast<MResponseMatrixO4*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO4*>(Nenner);
    } else if (Zahler->GetOrder() == 5) {
      *dynamic_cast<MResponseMatrixO5*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO5*>(Nenner);
    } else if (Zahler->GetOrder() == 6) {
      *dynamic_cast<MResponseMatrixO6*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO6*>(Nenner);
    } else if (Zahler->GetOrder() == 7) {
      *dynamic_cast<MResponseMatrixO7*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO7*>(Nenner);
    } else if (Zahler->GetOrder() == 8) {
      *dynamic_cast<MResponseMatrixO8*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO8*>(Nenner);
    } else if (Zahler->GetOrder() == 9) {
      *dynamic_cast<MResponseMatrixO9*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO9*>(Nenner);
    } else if (Zahler->GetOrder() == 10) {
      *dynamic_cast<MResponseMatrixO10*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO10*>(Nenner);
    } else if (Zahler->GetOrder() == 11) {
      *dynamic_cast<MResponseMatrixO11*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO11*>(Nenner);
    } else if (Zahler->GetOrder() == 12) {
      *dynamic_cast<MResponseMatrixO12*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO12*>(Nenner);
    } else if (Zahler->GetOrder() == 13) {
      *dynamic_cast<MResponseMatrixO13*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO13*>(Nenner);
    } else if (Zahler->GetOrder() == 14) {
      *dynamic_cast<MResponseMatrixO14*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO14*>(Nenner);
    } else if (Zahler->GetOrder() == 15) {
      *dynamic_cast<MResponseMatrixO15*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO15*>(Nenner);
    } else if (Zahler->GetOrder() == 16) {
      *dynamic_cast<MResponseMatrixO16*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO16*>(Nenner);
    } else if (Zahler->GetOrder() == 17) {
      *dynamic_cast<MResponseMatrixO17*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO17*>(Nenner);
    } else {
        merr<<"Unsupported matrix order: "<<Zahler->GetOrder()<<endl; 
    }
  }

  Zahler->Write(m_FileName.c_str(), true);
  delete Zahler;
  delete Nenner;

  return true;
}


/******************************************************************************
 * calculate the Bayesian ratio of two response matrices:
 */
bool ResponseManipulator::Ratio()
{
  MFileResponse File;
  MResponseMatrix* Zahler = File.Read(m_DividendFileName.c_str());
  if (Zahler == 0) return false;
  MResponseMatrix* Nenner = File.Read(m_DivisorFileName.c_str());
  if (Nenner == 0) return false;

  float SumZahler = Zahler->GetSum();
  float SumNenner = Nenner->GetSum();

  if (Zahler->GetOrder() != Nenner->GetOrder()) {
    mout<<"Cannot append file, because they are of different order!"<<endl;
  } else {
    if (Zahler->GetOrder() == 1) {
      *dynamic_cast<MResponseMatrixO1*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO1*>(Nenner);
      *dynamic_cast<MResponseMatrixO1*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 2) {
      *dynamic_cast<MResponseMatrixO2*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO2*>(Nenner);
      *dynamic_cast<MResponseMatrixO2*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 3) {
      *dynamic_cast<MResponseMatrixO3*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO3*>(Nenner);
      *dynamic_cast<MResponseMatrixO3*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 4) {
      *dynamic_cast<MResponseMatrixO4*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO4*>(Nenner);
      *dynamic_cast<MResponseMatrixO4*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 5) {
      *dynamic_cast<MResponseMatrixO5*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO5*>(Nenner);
      *dynamic_cast<MResponseMatrixO5*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 6) {
      *dynamic_cast<MResponseMatrixO6*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO6*>(Nenner);
      *dynamic_cast<MResponseMatrixO6*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 7) {
      *dynamic_cast<MResponseMatrixO7*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO7*>(Nenner);
      *dynamic_cast<MResponseMatrixO7*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 8) {
      *dynamic_cast<MResponseMatrixO8*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO8*>(Nenner);
      *dynamic_cast<MResponseMatrixO8*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 9) {
      *dynamic_cast<MResponseMatrixO9*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO9*>(Nenner);
      *dynamic_cast<MResponseMatrixO9*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 10) {
      *dynamic_cast<MResponseMatrixO10*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO10*>(Nenner);
      *dynamic_cast<MResponseMatrixO10*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 11) {
      *dynamic_cast<MResponseMatrixO11*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO11*>(Nenner);
      *dynamic_cast<MResponseMatrixO11*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 12) {
      *dynamic_cast<MResponseMatrixO12*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO12*>(Nenner);
      *dynamic_cast<MResponseMatrixO12*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 13) {
      *dynamic_cast<MResponseMatrixO13*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO13*>(Nenner);
      *dynamic_cast<MResponseMatrixO13*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 14) {
      *dynamic_cast<MResponseMatrixO14*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO14*>(Nenner);
      *dynamic_cast<MResponseMatrixO14*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 15) {
      *dynamic_cast<MResponseMatrixO15*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO15*>(Nenner);
      *dynamic_cast<MResponseMatrixO15*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 16) {
      *dynamic_cast<MResponseMatrixO16*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO16*>(Nenner);
      *dynamic_cast<MResponseMatrixO16*>(Zahler) *= SumNenner/SumZahler;
    } else if (Zahler->GetOrder() == 17) {
      *dynamic_cast<MResponseMatrixO17*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO17*>(Nenner);
      *dynamic_cast<MResponseMatrixO17*>(Zahler) *= SumNenner/SumZahler;
    } else {
        merr<<"Unsupported matrix order: "<<Zahler->GetOrder()<<endl; 
    }
  }

  Zahler->Write(m_FileName.c_str(), true);
  delete Zahler;
  delete Nenner;

  return true;
}




/******************************************************************************
 * Calculate the Probability of two response matrices:
 */
bool ResponseManipulator::Probability()
{
  MFileResponse File;
  MResponseMatrix* Zahler = File.Read(m_DividendFileName.c_str());
  if (Zahler == 0) return false;
  MResponseMatrix* Nenner = File.Read(m_DivisorFileName.c_str());
  if (Nenner == 0) return false;

  if (Zahler->GetOrder() != Nenner->GetOrder()) {
    mout<<"Cannot append file, because they are of different order!"<<endl;
  } else {
    if (Zahler->GetOrder() == 1) {
      *dynamic_cast<MResponseMatrixO1*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO1*>(Zahler);
      *dynamic_cast<MResponseMatrixO1*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO1*>(Nenner);
    } else if (Zahler->GetOrder() == 2) {
      *dynamic_cast<MResponseMatrixO2*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO2*>(Zahler);
      *dynamic_cast<MResponseMatrixO2*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO2*>(Nenner);
    } else if (Zahler->GetOrder() == 3) {
      *dynamic_cast<MResponseMatrixO3*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO3*>(Zahler);
      *dynamic_cast<MResponseMatrixO3*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO3*>(Nenner);
    } else if (Zahler->GetOrder() == 4) {
      *dynamic_cast<MResponseMatrixO4*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO4*>(Zahler);
      *dynamic_cast<MResponseMatrixO4*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO4*>(Nenner);
    } else if (Zahler->GetOrder() == 5) {
      *dynamic_cast<MResponseMatrixO5*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO5*>(Zahler);
      *dynamic_cast<MResponseMatrixO5*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO5*>(Nenner);
    } else if (Zahler->GetOrder() == 6) {
      *dynamic_cast<MResponseMatrixO6*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO6*>(Zahler);
      *dynamic_cast<MResponseMatrixO6*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO6*>(Nenner);
    } else if (Zahler->GetOrder() == 7) {
      *dynamic_cast<MResponseMatrixO7*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO7*>(Zahler);
      *dynamic_cast<MResponseMatrixO7*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO7*>(Nenner);
    } else if (Zahler->GetOrder() == 8) {
      *dynamic_cast<MResponseMatrixO8*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO8*>(Zahler);
      *dynamic_cast<MResponseMatrixO8*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO8*>(Nenner);
    } else if (Zahler->GetOrder() == 9) {
      *dynamic_cast<MResponseMatrixO9*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO9*>(Zahler);
      *dynamic_cast<MResponseMatrixO9*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO9*>(Nenner);
    } else if (Zahler->GetOrder() == 10) {
      *dynamic_cast<MResponseMatrixO10*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO10*>(Zahler);
      *dynamic_cast<MResponseMatrixO10*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO10*>(Nenner);
    } else if (Zahler->GetOrder() == 11) {
      *dynamic_cast<MResponseMatrixO11*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO11*>(Zahler);
      *dynamic_cast<MResponseMatrixO11*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO11*>(Nenner);
    } else if (Zahler->GetOrder() == 12) {
      *dynamic_cast<MResponseMatrixO12*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO12*>(Zahler);
      *dynamic_cast<MResponseMatrixO12*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO12*>(Nenner);
    } else if (Zahler->GetOrder() == 13) {
      *dynamic_cast<MResponseMatrixO13*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO13*>(Zahler);
      *dynamic_cast<MResponseMatrixO13*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO13*>(Nenner);
    } else if (Zahler->GetOrder() == 14) {
      *dynamic_cast<MResponseMatrixO14*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO14*>(Zahler);
      *dynamic_cast<MResponseMatrixO14*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO14*>(Nenner);
    } else if (Zahler->GetOrder() == 15) {
      *dynamic_cast<MResponseMatrixO15*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO15*>(Zahler);
      *dynamic_cast<MResponseMatrixO15*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO15*>(Nenner);
    } else if (Zahler->GetOrder() == 16) {
      *dynamic_cast<MResponseMatrixO16*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO16*>(Zahler);
      *dynamic_cast<MResponseMatrixO16*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO16*>(Nenner);
    } else if (Zahler->GetOrder() == 17) {
      *dynamic_cast<MResponseMatrixO17*>(Nenner) += 
        *dynamic_cast<MResponseMatrixO17*>(Zahler);
      *dynamic_cast<MResponseMatrixO17*>(Zahler) /= 
        *dynamic_cast<MResponseMatrixO17*>(Nenner);
    } else {
        merr<<"Unsupported matrix order: "<<Zahler->GetOrder()<<endl; 
    }
  }

  Zahler->Write(m_FileName.c_str(), true);
  delete Zahler;
  delete Nenner;

  return true;
}


/******************************************************************************
 * Show a response matrix:
 */
bool ResponseManipulator::Statistics()
{
  MFileResponse File;
  MResponseMatrix* R = File.Read(m_FileName.c_str());
 
  cout<<"Statistics of response matrix \""<<m_FileName
      <<"\" of order "<<R->GetOrder()<<":"<<endl;
  cout<<endl;
  cout<<"NBins:   "<<R->GetNBins()<<endl;
  cout<<"Maximum: "<<R->GetMaximum()<<endl;
  cout<<"Minimum: "<<R->GetMinimum()<<endl;
  cout<<"Sum:     "<<R->GetSum()<<endl;
  cout<<"Avg:     "<<R->GetSum()/R->GetNBins()<<endl;
  cout<<endl;
  cout<<"Axis:"<<endl;
  for (unsigned int i = 1; i <= R->GetOrder(); ++i) {
    cout<<"  x"<<i<<":  "<<R->GetAxisName(i)<<" (from "<<R->GetAxisContent(0, i)
        <<" to "<<R->GetAxisContent(R->GetAxisBins(i), i)
        <<" in "<<R->GetAxisBins(i)<<" bins)"<<endl;
  }

  delete R;
  return true;
}


/******************************************************************************
 * Show a response matrix:
 */
bool ResponseManipulator::Show()
{
  string OtherFile(m_FileName);
  if (OtherFile.rfind("bad.rsp") != string::npos) {
    OtherFile.replace(OtherFile.rfind("bad.rsp"), 7, "good.rsp");
  } else if (OtherFile.rfind("good.rsp") != string::npos){
    OtherFile.replace(OtherFile.rfind("good.rsp"), 8, "bad.rsp");
  }

  MFileResponse File;
  MResponseMatrix* R = File.Read(m_FileName.c_str());
  
  if (R->GetOrder() == 1) {
    dynamic_cast<MResponseMatrixO1*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO1*>(R)->Show(m_Normalized);
  } else if (R->GetOrder() == 2) {
    dynamic_cast<MResponseMatrixO2*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO2*>(R)->Show(m_x1, m_x2, m_Normalized);
  } else if (R->GetOrder() == 3) {
    dynamic_cast<MResponseMatrixO3*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO3*>(R)->Show(m_x1, m_x2, m_x3, m_Normalized);
  } else if (R->GetOrder() == 4) {
    //dynamic_cast<MResponseMatrixO4*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO4*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_Normalized);
  } else if (R->GetOrder() == 5) {
    //dynamic_cast<MResponseMatrixO5*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO5*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_Normalized);
  } else if (R->GetOrder() == 6) {
    //dynamic_cast<MResponseMatrixO6*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO6*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_Normalized);
  } else if (R->GetOrder() == 7) {
    //dynamic_cast<MResponseMatrixO7*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO7*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_Normalized);
  } else if (R->GetOrder() == 8) {
    //dynamic_cast<MResponseMatrixO8*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO8*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_Normalized);
  } else if (R->GetOrder() == 9) {
    //dynamic_cast<MResponseMatrixO9*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO9*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_Normalized);
  } else if (R->GetOrder() == 10) {
    //dynamic_cast<MResponseMatrixO10*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO10*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_Normalized);
  } else if (R->GetOrder() == 11) {
    //dynamic_cast<MResponseMatrixO11*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO11*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_Normalized);
  } else if (R->GetOrder() == 12) {
    //dynamic_cast<MResponseMatrixO12*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO12*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_Normalized);
  } else if (R->GetOrder() == 13) {
    //dynamic_cast<MResponseMatrixO13*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO13*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_Normalized);
  } else if (R->GetOrder() == 14) {
    //dynamic_cast<MResponseMatrixO14*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO14*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_x14, m_Normalized);
  } else if (R->GetOrder() == 15) {
    //dynamic_cast<MResponseMatrixO15*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO15*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_x14, m_x15, m_Normalized);
  } else if (R->GetOrder() == 16) {
    //dynamic_cast<MResponseMatrixO16*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO16*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_x14, m_x15, m_x16, m_Normalized);
  } else if (R->GetOrder() == 17) {
    //dynamic_cast<MResponseMatrixO17*>(R)->Smooth(m_NSmooths);
    dynamic_cast<MResponseMatrixO17*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_x14, m_x15, m_x16, m_x17, m_Normalized);
  } else {
    merr<<"Unsupported matrix order: "<<R->GetOrder()<<endl; 
  }
  delete R;

  cout<<"Other:"<<OtherFile<<endl;
  if (OtherFile != m_FileName && MFile::FileExists(OtherFile.c_str())) {
    MFileResponse File;
    MResponseMatrix* R = File.Read(OtherFile.c_str());

    if (R->GetOrder() == 1) {
      dynamic_cast<MResponseMatrixO1*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO1*>(R)->Show(m_Normalized);
    } else if (R->GetOrder() == 2) {
      dynamic_cast<MResponseMatrixO2*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO2*>(R)->Show(m_x1, m_x2, m_Normalized);
    } else if (R->GetOrder() == 3) {
      dynamic_cast<MResponseMatrixO3*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO3*>(R)->Show(m_x1, m_x2, m_x3, m_Normalized);
    } else if (R->GetOrder() == 4) {
      //dynamic_cast<MResponseMatrixO4*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO4*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_Normalized);
    } else if (R->GetOrder() == 5) {
      //dynamic_cast<MResponseMatrixO5*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO5*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_Normalized);
    } else if (R->GetOrder() == 6) {
      //dynamic_cast<MResponseMatrixO6*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO6*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_Normalized);
    } else if (R->GetOrder() == 7) {
      //dynamic_cast<MResponseMatrixO7*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO7*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_Normalized);
    } else if (R->GetOrder() == 8) {
      //dynamic_cast<MResponseMatrixO8*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO8*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_Normalized);
    } else if (R->GetOrder() == 9) {
      //dynamic_cast<MResponseMatrixO9*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO9*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_Normalized);
    } else if (R->GetOrder() == 10) {
      //dynamic_cast<MResponseMatrixO10*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO10*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_Normalized);
    } else if (R->GetOrder() == 11) {
      ///dynamic_cast<MResponseMatrixO11*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO11*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_Normalized);
    } else if (R->GetOrder() == 12) {
      //dynamic_cast<MResponseMatrixO12*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO12*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_Normalized);
    } else if (R->GetOrder() == 13) {
      //dynamic_cast<MResponseMatrixO13*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO13*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_Normalized);
    } else if (R->GetOrder() == 14) {
      //dynamic_cast<MResponseMatrixO14*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO14*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_x14, m_Normalized);
    } else if (R->GetOrder() == 15) {
      //dynamic_cast<MResponseMatrixO15*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO15*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_x14, m_x15, m_Normalized);
    } else if (R->GetOrder() == 16) {
      //dynamic_cast<MResponseMatrixO16*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO16*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_x14, m_x15, m_x16, m_Normalized);
    } else if (R->GetOrder() == 17) {
      //dynamic_cast<MResponseMatrixO17*>(R)->Smooth(m_NSmooths);
      dynamic_cast<MResponseMatrixO17*>(R)->Show(m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8, m_x9, m_x10, m_x11, m_x12, m_x13, m_x14, m_x15, m_x16, m_x17, m_Normalized);
    } else {
      merr<<"Unsupported matrix order: "<<R->GetOrder()<<endl; 
    }
    delete R;
  }



  return true;
}


/******************************************************************************/

ResponseManipulator* g_Prg = 0;

/******************************************************************************/


/******************************************************************************
 * Called when an interrupt signal is flagged
 * All catched signals lead to a well defined exit of the program
 */
void CatchSignal(int a)
{
  cout<<"Catched signal Ctrl-C (ID="<<a<<"):"<<endl;
  
  if (g_Prg != 0) {
    g_Prg->Interrupt();
  }
}


/******************************************************************************
 * Main program
 */
int main(int argc, char** argv)
{
  //void (*handler)(int);
	//handler = CatchSignal;
  //(void) signal(SIGINT, CatchSignal);

  // Initialize global MEGALIB variables, especially mgui, etc.
  MGlobal::Initialize();

  TApplication ResponseManipulatorApp("ResponseManipulatorApp", 0, 0);

  g_Prg = new ResponseManipulator();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  if (gROOT->GetListOfCanvases()->First() != 0) {
    ResponseManipulatorApp.Run();
  }

  cout<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
