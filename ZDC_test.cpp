

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
    TFile* file(TFile::Open("data/ZDC_test.root"));

    //TTree* tree(file->Get<TTree>("FSCAnalyzerHC/fscdigi"));
    //tree->Print();
    //return 0;

    TTreeReader reader("zdcanalyzer/zdcrechit", file);
    
    
    TTreeReaderArray<Int_t> zside(reader, "zside");
    TTreeReaderArray<Int_t> section(reader, "section");
    TTreeReaderArray<Int_t> channel(reader, "channel");

    int count = 0;
    const int max_count = -1;
    for (int chosen_ts=0; chosen_ts<6; chosen_ts++) {
        while(reader.Next()) {

            for (int i=0; i<18; i++) {
                cout << zside[i] << " " << section[i] << " " << channel[i] << endl;
            }
            cout << endl;
        }
    }
    return 0;
}
