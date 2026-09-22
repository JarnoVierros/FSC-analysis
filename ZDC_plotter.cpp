

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

    const string detector_names[50] = {
        "ZDCm EM 1",
        "ZDCm EM 2",
        "ZDCm EM 3",
        "ZDCm EM 4",
        "ZDCm EM 5",
        "ZDCm HAD 1",
        "ZDCm HAD 2",
        "ZDCm HAD 3",
        "ZDCm HAD 4",
        "ZDCp EM 1",
        "ZDCp EM 2",
        "ZDCp EM 3",
        "ZDCp EM 4",
        "ZDCp EM 5",
        "ZDCp HAD 1",
        "ZDCp HAD 2",
        "ZDCp HAD 3",
        "ZDCp HAD 4",
        "ZDCm RPD 1",
        "ZDCm RPD 2",
        "ZDCm RPD 3",
        "ZDCm RPD 4",
        "ZDCm RPD 5",
        "ZDCm RPD 6",
        "ZDCm RPD 7",
        "ZDCm RPD 8",
        "ZDCm RPD 9",
        "ZDCm RPD 10",
        "ZDCm RPD 11",
        "ZDCm RPD 12",
        "ZDCm RPD 13",
        "ZDCm RPD 14",
        "ZDCm RPD 15",
        "ZDCm RPD 16",
        "ZDCp RPD 1",
        "ZDCp RPD 2",
        "ZDCp RPD 3",
        "ZDCp RPD 4",
        "ZDCp RPD 5",
        "ZDCp RPD 6",
        "ZDCp RPD 7",
        "ZDCp RPD 8",
        "ZDCp RPD 9",
        "ZDCp RPD 10",
        "ZDCp RPD 11",
        "ZDCp RPD 12",
        "ZDCp RPD 13",
        "ZDCp RPD 14",
        "ZDCp RPD 15",
        "ZDCp RPD 16",
    };

    const int limits[50] = {
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        260,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
        220,
    };

    cout << "starting" << endl;
    TFile* file(TFile::Open("data/ZDC_data_0.root"));

    //TTree* tree(file->Get<TTree>("FSCAnalyzerHC/fscdigi"));
    //tree->Print();
    //return 0;

    TTreeReader reader("zdcanalyzer/zdcdigi", file);
    
    
    TTreeReaderArray<Int_t> zside(reader, "zside");
    TTreeReaderArray<Int_t> section(reader, "section");
    TTreeReaderArray<Int_t> channel(reader, "channel");

    vector<TTreeReaderArray<Int_t>> adcs;
    for (int i=0; i<6; i++) {
        string branch = "adcTs" + to_string(i);
        adcs.emplace_back(reader, branch.c_str());
    }

    TH1I* adc_hists[50];
    for (int i=0; i<50; i++) {
        TString name = "adc_" + to_string(i) ;
        TString title = detector_names[i];
        adc_hists[i] = new TH1I(name, title, limits[i]+1, -0.5, limits[i]+0.5);
    }

    int count = 0;
    const int max_count = -1;
    for (int chosen_ts=0; chosen_ts<6; chosen_ts++) {
        while(reader.Next()) {

            for (int i=0; i<50; i++) {
                adc_hists[i]->Fill(adcs[chosen_ts][i]);
            }
            
            count ++;
            if (count > max_count && max_count != -1) {
                break;
            }
        }
        reader.Restart();

        TCanvas* adc_canvases[50];
        for (int i=0; i<50; i++) {
            TString adc_canvas_name = "adc_canvas_" + to_string(i) + "_ts" + to_string(chosen_ts);
            adc_canvases[i] = new TCanvas(adc_canvas_name, "", 1000, 800);

            adc_hists[i]->GetXaxis()->SetTitle("adc");
            adc_hists[i]->GetYaxis()->SetTitle("events");
            gPad->SetLogy();
            adc_hists[i]->Draw("");

            //adc_correlation_canvases[i*12+j]->SetLeftMargin(0.12);
            //adc_correlation_canvases[i*12+j]->SetRightMargin(0.12);

            TString filename = "figures/ZDC/adc_hists/ts"+to_string(chosen_ts)+"/adc_"+to_string(i) + "_ts" + to_string(chosen_ts) + ".png";
            adc_canvases[i]->Print(filename);

            adc_hists[i]->Reset();
        }
    }

    cout << "finished" << endl;
    return 0;
}
