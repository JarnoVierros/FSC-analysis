

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
    TFile* file(TFile::Open("data/Tracker_data_0.root"));

    //TTree* tree(file->Get<TTree>("trackAnalyzer/trackTree"));
    //tree->Print();
    //return 0;


    TTreeReader reader("trackAnalyzer/trackTree", file);
    
    
    TTreeReaderValue<Int_t> nEv(reader, "nEv");
    TTreeReaderValue<Int_t> nTrk(reader, "nTrk");
    TTreeReaderArray<float> ndofVtx(reader, "ndofVtx");
    TTreeReaderArray<float> trkEta(reader, "trkEta");
    TTreeReaderArray<float> trkPt(reader, "trkPt");
    TTreeReaderArray<Int_t> nTracksVtx(reader, "nTracksVtx");

    int count = 0;
    const int max_count = -1;

    while(reader.Next()) {
        cout << count << ": " << *nEv << ", " << *nTrk << endl;
        for (int i=0; i<*nTrk; i++) {
            cout << trkEta[i] << " ";
        }
        cout << endl << endl;
        count ++;
    }

    return 0;
}
