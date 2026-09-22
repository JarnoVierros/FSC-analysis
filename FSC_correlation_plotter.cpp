

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

    const string detector_names[12] = {"-Up", "-Down", "-BottLeft", "-BottRight", "-TopLeft", "-TopRight", "+Up", "+Down", "+BottLeft", "+BottRight", "+TopLeft", "+TopRight"};

    cout << "starting" << endl;
    TFile* file(TFile::Open("data/FSC_data_0.root"));

    //TTree* tree(file->Get<TTree>("FSCAnalyzerHC/fscdigi"));
    //tree->Print();
    //return 0;

    TTreeReader reader("FSCAnalyzerHC/fscdigi", file);
    
    /*
    TTreeReaderArray<Int_t> adc0(reader, "adcTs0");
    TTreeReaderArray<Int_t> adc1(reader, "adcTs1");
    TTreeReaderArray<Int_t> adc2(reader, "adcTs2");
    TTreeReaderArray<Int_t> adc3(reader, "adcTs3");
    TTreeReaderArray<Int_t> adc4(reader, "adcTs4");
    TTreeReaderArray<Int_t> adc5(reader, "adcTs5");
    */

    //TTreeReaderArray<Int_t> zside(reader, "zside");
    //TTreeReaderArray<Int_t> section(reader, "section");
    //TTreeReaderArray<Int_t> channel(reader, "channel");

    vector<TTreeReaderArray<Int_t>> adcs;
    for (int i=0; i<6; i++) {
        string branch = "adcTs" + to_string(i);
        adcs.emplace_back(reader, branch.c_str());
    }

    vector<TTreeReaderArray<Float_t>> charges;
    for (int i=0; i<6; i++) {
        string branch = "chargefCTs" + to_string(i);
        charges.emplace_back(reader, branch.c_str());
    }

    //Float_t* x = new Float_t(n);
    //Float_t* y = new Float_t(n);

    TH2* charge_calib_hist = new TH2F("charge_calib_hist", "charge vs adc for ts 2", 100, 0, 300, 100, 0, 350e3);
    TH2* time_slice_hist = new TH2I("time_slice_hist", "adc vs ts", 6, -0.5, 5.5, 100, 0, 300);

    TH2* adc_correlations[12*12];
    for (int i=0; i<12; i++) {
        for (int j=0; j<12; j++) {
            //cout << i << ":" << j << ":" << i*12+j << endl;;
            TString name = "adc_correlation_hist" + to_string(i) + "_" + to_string(j);
            TString title = detector_names[i] + " vs " + detector_names[j];
            adc_correlations[i*12+j] = new TH2I(name, title, 301, -0.5, 300.5, 301, -0.5, 300.5);
        }
    }

    TH2* charge_correlations[12*12];
    for (int i=0; i<12; i++) {
        for (int j=0; j<12; j++) {
            //cout << i << ":" << j << ":" << i*12+j << endl;;
            TString name = "charge_correlation_hist" + to_string(i) + "_" + to_string(j);
            TString title = detector_names[i] + " vs " + detector_names[j];
            charge_correlations[i*12+j] = new TH2I(name, title, 300, 0, 350e3, 300, 0, 350e3);
        }
    }

    vector<Float_t> x;
    vector<Float_t> y;

    int count = 0;
    const int max_count = -1; 
    while(reader.Next()) {
        for (int j=0; j<12; j++) {
            charge_calib_hist->Fill(adcs[2][j], charges[2][j]);

            time_slice_hist->Fill(Int_t(0), adcs[0][j]);
            time_slice_hist->Fill(Int_t(1), adcs[1][j]);
            time_slice_hist->Fill(Int_t(2), adcs[2][j]);
            time_slice_hist->Fill(Int_t(3), adcs[3][j]);
            time_slice_hist->Fill(Int_t(4), adcs[4][j]);
            time_slice_hist->Fill(Int_t(5), adcs[5][j]);
        }

        const int chosen_ts = 2;
        for (int i=0; i<12; i++) {
            for (int j=0; j<12; j++) {
                adc_correlations[i*12+j]->Fill(adcs[chosen_ts][i], adcs[chosen_ts][j]);
            }
        }

        for (int i=0; i<12; i++) {
            for (int j=0; j<12; j++) {
                charge_correlations[i*12+j]->Fill(charges[chosen_ts][i], charges[chosen_ts][j]);
            }
        }

        count++;
        if (count > max_count && max_count != -1) {
            break;
        }
    }

    cout << "making figures" << endl;

    TCanvas* charge_calib_canvas = new TCanvas("charge_calib_canvas", "", 1000, 600);

    charge_calib_hist->GetXaxis()->SetTitle("adc");
    charge_calib_hist->GetYaxis()->SetTitle("charge");
    charge_calib_hist->Draw();

    charge_calib_canvas->Print("figures/charge_calib.png");

    TCanvas* adc_correlation_canvases[12*12];
    for (int i=0; i<12; i++) {
        for (int j=0; j<12; j++) {
            TString corr_canvas_name = "adc_correlation_canvas_" + to_string(i) + "_" + to_string(j);
            adc_correlation_canvases[i*12+j] = new TCanvas(corr_canvas_name, "", 1000, 1000);

            TString xtitle = detector_names[i] + " adc";
            adc_correlations[i*12+j]->GetXaxis()->SetTitle(xtitle);
            TString ytitle = detector_names[j] + " adc";
            adc_correlations[i*12+j]->GetYaxis()->SetTitle(ytitle);
            gPad->SetLogz();
            adc_correlations[i*12+j]->Draw("COLZ");

            adc_correlation_canvases[i*12+j]->SetLeftMargin(0.12);
            adc_correlation_canvases[i*12+j]->SetRightMargin(0.12);

            TString filename = "figures/adc_correlations/correlation_"+to_string(i)+"_"+to_string(j)+".png";
            adc_correlation_canvases[i*12+j]->Print(filename);
        }
    }


    TCanvas* charge_correlation_canvases[12*12];
    for (int i=0; i<12; i++) {
        for (int j=0; j<12; j++) {
            TString corr_canvas_name = "charge_correlation_canvas_" + to_string(i) + "_" + to_string(j);
            charge_correlation_canvases[i*12+j] = new TCanvas(corr_canvas_name, "", 1000, 1000);

            TString xtitle = detector_names[i] + " charge";
            charge_correlations[i*12+j]->GetXaxis()->SetTitle(xtitle);
            TString ytitle = detector_names[j] + " charge";
            charge_correlations[i*12+j]->GetYaxis()->SetTitle(ytitle);
            gPad->SetLogz();
            charge_correlations[i*12+j]->Draw("COLZ");

            charge_correlation_canvases[i*12+j]->SetLeftMargin(0.12);
            charge_correlation_canvases[i*12+j]->SetRightMargin(0.12);

            TString filename = "figures/charge_correlations/correlation_"+to_string(i)+"_"+to_string(j)+".png";
            charge_correlation_canvases[i*12+j]->Print(filename);
        }
    }


    TCanvas* time_slice_canvas = new TCanvas("time_slice_canvas", "", 1000, 600);
    
    gPad->SetLogz();

    time_slice_hist->GetXaxis()->SetTitle("ts");
    time_slice_hist->GetYaxis()->SetTitle("adc");
    time_slice_hist->Draw();

    time_slice_canvas->Print("figures/time_slice.png");

    cout << "finished" << endl;
    return 0;
}
