

#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"

using namespace std;

//const int n = 23492616;

const bool do_prints = false;

int main() {

    gStyle->SetOptStat(0);

    cout << "starting" << endl;
    TFile* file(TFile::Open("data/ZDC_data_0.root"));

    //TTree* tree(file->Get<TTree>("FSCAnalyzerHC/fscdigi"));
    //tree->Print();
    //return 0;

    TTreeReader reader("zdcanalyzer/zdcdigi", file);
    
    
    TTreeReaderArray<Int_t> adc0(reader, "adcTs0");
    TTreeReaderArray<Int_t> adc1(reader, "adcTs1");
    TTreeReaderArray<Int_t> adc2(reader, "adcTs2");
    TTreeReaderArray<Int_t> adc3(reader, "adcTs3");
    TTreeReaderArray<Int_t> adc4(reader, "adcTs4");
    TTreeReaderArray<Int_t> adc5(reader, "adcTs5");
    
    while(reader.Next()) {

        for (int i=6; i<12; i++) {
            cout << adc0[i] << " ";
        }
        cout << endl;

    }

    //TTreeReaderArray<Int_t> zside(reader, "zside");
    //TTreeReaderArray<Int_t> section(reader, "section");
    //TTreeReaderArray<Int_t> channel(reader, "channel");

    cout << "finished" << endl;
    return 0;
}
