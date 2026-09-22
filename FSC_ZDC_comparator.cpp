

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

    const string ZDC_detector_names[50] = {
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

    const string FSC_detector_names[12] = {"-Up", "-Down", "-BottLeft", "-BottRight", "-TopLeft", "-TopRight", "+Up", "+Down", "+BottLeft", "+BottRight", "+TopLeft", "+TopRight"};

    cout << "starting" << endl;
    TFile* FSC_file(TFile::Open("data/FSC_data_0.root"));
    TFile* ZDC_file(TFile::Open("data/ZDC_data_0.root"));

    //TTree* tree(file->Get<TTree>("FSCAnalyzerHC/fscdigi"));
    //tree->Print();
    //return 0;

    TTreeReader FSC_reader("FSCAnalyzerHC/fscdigi", FSC_file);
    TTreeReader ZDC_reader("zdcanalyzer/zdcdigi", ZDC_file);

    vector<TTreeReaderArray<Int_t>> FSC_adcs;
    for (int i=0; i<6; i++) {
        string branch = "adcTs" + to_string(i);
        FSC_adcs.emplace_back(FSC_reader, branch.c_str());
    }

    vector<TTreeReaderArray<Int_t>> ZDC_adcs;
    for (int i=0; i<6; i++) {
        string branch = "adcTs" + to_string(i);
        ZDC_adcs.emplace_back(ZDC_reader, branch.c_str());
    }

    const string system_names[10] = {
        "FSCm st2",
        "FSCm st3",
        "FSCp st2",
        "FSCp st3",
        "ZDCm ECAL",
        "ZDCm HCAL",
        "ZDCm RPD",
        "ZDCp ECAL",
        "ZDCp HCAL",
        "ZDCp RPD",
    };

    const int limits[10] = {
        260,
        260,
        260,
        260,
        260,
        260,
        220,
        260,
        260,
        220,
    };

    TH2I* adc_correlations[10*10];
    for (int i=0; i<10; i++) {
        for (int j=0; j<10; j++) {
            if (i==j) {continue;}
            TString name = "FSC_ZDC_correlation_hist_" + to_string(i) + "_" + to_string(j);
            TString title = system_names[i] + " vs " + system_names[j];
            adc_correlations[i*10+j] = new TH2I(name, title, limits[i]+1, -0.5, limits[i]+0.5, limits[j]+1, -0.5, limits[j]+0.5);
        }
    }

    TH1I* hists[10];
    for (int i=0; i<10; i++) {
        TString name = "FSC_ZDC_correlation_hist_" + to_string(i) + "_" + to_string(i);
        TString title = system_names[i];
        hists[i] = new TH1I(name, title, limits[i]+1, -0.5, limits[i]+0.5);
    }

    int count = 0;
    const int max_count = -1;
    const int chosen_ts = 2;
    while(FSC_reader.Next()) {
        int res = ZDC_reader.Next();
        if (res == 0) {
            throw exception();
        }

        int means[10];
        means[0] = (FSC_adcs[chosen_ts][0]+FSC_adcs[chosen_ts][1])/2;
        if (FSC_adcs[chosen_ts][0]*FSC_adcs[chosen_ts][1]==0) {means[0]=0;}
        means[1] = (FSC_adcs[chosen_ts][2]+FSC_adcs[chosen_ts][3]+FSC_adcs[chosen_ts][4]+FSC_adcs[chosen_ts][5])/4;
        if (FSC_adcs[chosen_ts][2]*FSC_adcs[chosen_ts][3]*FSC_adcs[chosen_ts][4]*FSC_adcs[chosen_ts][5]==0) {means[1]=0;}
        means[2] = (FSC_adcs[chosen_ts][6]+FSC_adcs[chosen_ts][7])/2;
        if (FSC_adcs[chosen_ts][6]*FSC_adcs[chosen_ts][7]==0) {means[2]=0;}
        means[3] = (FSC_adcs[chosen_ts][8]+FSC_adcs[chosen_ts][9]+FSC_adcs[chosen_ts][10]+FSC_adcs[chosen_ts][11])/4;
        if (FSC_adcs[chosen_ts][8]*FSC_adcs[chosen_ts][9]*FSC_adcs[chosen_ts][10]*FSC_adcs[chosen_ts][11]==0) {means[3]=0;}
        
        means[4] = 0;
        for (int i=0; i<5; i++) {means[4] += ZDC_adcs[chosen_ts][i];}
        means[4] = means[4]/5;
        float prod = 1;
        for (int i=0; i<5; i++) {prod *= ZDC_adcs[chosen_ts][i];}
        if (prod == 0) {means[4]=0;}

        means[5] = 0;
        for (int i=5; i<9; i++) {means[5] += ZDC_adcs[chosen_ts][i];}
        means[5] = means[5]/4;
        prod = 1;
        for (int i=5; i<9; i++) {prod *= ZDC_adcs[chosen_ts][i];}
        if (prod == 0) {means[5]=0;}

        means[6] = 0;
        for (int i=9; i<25; i++) {means[6] += ZDC_adcs[chosen_ts][i];}
        means[6] = means[6]/16;
        prod = 1;
        for (int i=9; i<25; i++) {prod *= ZDC_adcs[chosen_ts][i];}
        if (prod == 0) {means[6]=0;}

        means[7] = 0;
        for (int i=25; i<30; i++) {means[7] += ZDC_adcs[chosen_ts][i];}
        means[7] = means[7]/5;
        prod = 1;
        for (int i=25; i<30; i++) {prod *= ZDC_adcs[chosen_ts][i];}
        if (prod == 0) {means[7]=0;}

        means[8] = 0;
        for (int i=30; i<34; i++) {means[8] += ZDC_adcs[chosen_ts][i];}
        means[8] = means[8]/4;
        prod = 1;
        for (int i=30; i<34; i++) {prod *= ZDC_adcs[chosen_ts][i];}
        if (prod == 0) {means[8]=0;}

        means[9] = 0;
        for (int i=34; i<50; i++) {means[9] += ZDC_adcs[chosen_ts][i];}
        means[9] = means[9]/16;
        prod = 1;
        for (int i=34; i<50; i++) {prod *= ZDC_adcs[chosen_ts][i];}
        if (prod == 0) {means[9]=0;}


        for (int i=0; i<10; i++) {
            for (int j=0; j<10; j++) {
                if (i==j) {
                    hists[i]->Fill(means[i]);
                } else {
                    adc_correlations[i*10+j]->Fill(means[i], means[j]);
                }
            }
        }
        
        count ++;
        if (count > max_count && max_count != -1) {
            break;
        }
    }

    TCanvas* adc_correlation_canvases[10*10];
    for (int i=0; i<10; i++) {
        for (int j=0; j<10; j++) {

            TString corr_canvas_name = "adc_correlation_canvas_" + to_string(i) + "_" + to_string(j);
            adc_correlation_canvases[i*10+j] = new TCanvas(corr_canvas_name, "", 1000, 1000);

            adc_correlation_canvases[i*10+j]->SetLeftMargin(0.12);
            adc_correlation_canvases[i*10+j]->SetRightMargin(0.12);

            if (i==j) {
                TString xtitle = system_names[i] + " adc";
                hists[i]->GetXaxis()->SetTitle(xtitle);
                hists[i]->GetYaxis()->SetTitle("events");
                gPad->SetLogy();
                hists[i]->Draw("");
            } else {
                TString xtitle = system_names[i] + " adc";
                adc_correlations[i*10+j]->GetXaxis()->SetTitle(xtitle);
                TString ytitle = system_names[j] + " adc";
                adc_correlations[i*10+j]->GetYaxis()->SetTitle(ytitle);
                gPad->SetLogz();
                adc_correlations[i*10+j]->Draw("COLZ");
            }
            TString filename = "figures/FSC_ZDC_correlations/correlation_"+to_string(i)+"_"+to_string(j)+".png";
            adc_correlation_canvases[i*10+j]->Print(filename);
        }
    }

    cout << "finished" << endl;
    return 0;
}
