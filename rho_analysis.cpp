
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

int main() {

    cout << "starting" << endl;
    TFile* FSC_file(TFile::Open("data/FSC_data_0.root"));
    TFile* ZDC_file(TFile::Open("data/ZDC_data_0.root"));
    TFile* Tracker_file(TFile::Open("data/Tracker_data_0.root"));
    TFile* HF_file(TFile::Open("data/HF_data_0.root"));

    TTreeReader FSC_reader("FSCAnalyzerHC/fscdigi", FSC_file);
    TTreeReader ZDC_reader("zdcanalyzer/zdcdigi", ZDC_file);
    TTreeReader ZDC_rechit_reader("zdcanalyzer/zdcrechit", ZDC_file);
    TTreeReader Tracker_reader("trackAnalyzer/trackTree", FSC_file);
    TTreeReader HF_reader("rechitanalyzer/hf", FSC_file);

    vector<TTreeReaderArray<Float_t>> FSC_charges;
    for (int i=0; i<6; i++) {
        string branch = "chargefCTs" + to_string(i);
        FSC_charges.emplace_back(FSC_reader, branch.c_str());
    }

    TTreeReaderValue<Float_t> ZDC_sumMinus(ZDC_rechit_reader, "sumMinus");;
    TTreeReaderValue<Float_t> ZDC_sumPlus(ZDC_rechit_reader, "sumPlus");;

    TTreeReaderValue<Int_t> nTrk(Tracker_reader, "nTrk");
    TTreeReaderArray<float> ndofVtx(Tracker_reader, "ndofVtx");
    TTreeReaderArray<float> trkEta(Tracker_reader, "trkEta");
    TTreeReaderArray<float> trkPt(Tracker_reader, "trkPt");
    TTreeReaderArray<Int_t> nTracksVtx(Tracker_reader, "nTracksVtx");



}