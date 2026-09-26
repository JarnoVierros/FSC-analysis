

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
#include "TLegend.h"

using namespace std;

//const int n = 23492616;

const bool do_prints = false;

int main() {

    gStyle->SetOptStat(0);

    const bool rm_zeroes = true; //remove zeroes

    const string FSC_detector_names[12] = {"-Up", "-Down", "-BottLeft", "-BottRight", "-TopLeft", "-TopRight", "+Up", "+Down", "+BottLeft", "+BottRight", "+TopLeft", "+TopRight"};

    cout << "starting" << endl;
    TFile* FSC_file(TFile::Open("data/FSC_data_0.root"));
    TFile* ZDC_file(TFile::Open("data/ZDC_data_0.root"));

    TTreeReader FSC_reader("FSCAnalyzerHC/fscdigi", FSC_file);
    TTreeReader ZDC_reader("zdcanalyzer/zdcdigi", ZDC_file);
    TTreeReader ZDC_rechit_reader("zdcanalyzer/zdcrechit", ZDC_file);

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

    vector<TTreeReaderArray<Float_t>> FSC_charges;
    for (int i=0; i<6; i++) {
        string branch = "chargefCTs" + to_string(i);
        FSC_charges.emplace_back(FSC_reader, branch.c_str());
    }

    vector<TTreeReaderArray<Float_t>> ZDC_charges;
    for (int i=0; i<6; i++) {
        string branch = "chargefCTs" + to_string(i);
        ZDC_charges.emplace_back(ZDC_reader, branch.c_str());
    }

    TTreeReaderArray<Float_t> ZDC_energy(ZDC_rechit_reader, "energy");

    const string FSC_names[] = {
        "FSC2m Up",
        "FSC2m Down",
        "FSC3m BottLeft",
        "FSC3m BottRight",
        "FSC3m TopLeft",
        "FSC3m TopRight",
        "FSC2p Up",
        "FSC2p Down",
        "FSC3p BottLeft",
        "FSC3p BottRight",
        "FSC3p TopLeft",
        "FSC3p TopRight",
    };

    const int adc_limit = 200;
    const int adc_bins = 100;

    TH1D* adc_hists[12];
    for (int i=0; i<12; i++) {
        TString name = "FSC_adc_raw_" + to_string(i);
        TString title = FSC_names[i];
        adc_hists[i] = new TH1D(name, title, adc_bins, -0.5, adc_limit+0.5);
    }

    TH1D* selected_adc_hists[12];
    for (int i=0; i<12; i++) {
        TString name = "FSC_adc_selected_" + to_string(i);
        TString title = FSC_names[i];
        selected_adc_hists[i] = new TH1D(name, title, adc_bins, -0.5, adc_limit+0.5);
    }

    const int charge_limit = 200e3;
    const int charge_bins = 20;

    TH1D* charge_hists[12];
    for (int i=0; i<12; i++) {
        TString name = "FSC_charge_raw_" + to_string(i);
        TString title = FSC_names[i];
        charge_hists[i] = new TH1D(name, title, charge_bins, 0, charge_limit);
    }

    TH1D* selected_charge_hists[12];
    for (int i=0; i<12; i++) {
        TString name = "FSC_charge_selected_" + to_string(i);
        TString title = FSC_names[i];
        selected_charge_hists[i] = new TH1D(name, title, charge_bins, 0, charge_limit);
    }

    int count = 0;
    const int max_count = -1;
    const int chosen_ts = 2;
    while(FSC_reader.Next()) {

        int res = ZDC_reader.Next();
        if (res == 0) {
            throw exception();
        }
        res = ZDC_rechit_reader.Next();
        if (res == 0) {
            throw exception();
        }

        for (int i=0; i<12; i++) {
            adc_hists[i]->Fill(FSC_adcs[chosen_ts][i]);
        }

        for (int i=0; i<12; i++) {
            charge_hists[i]->Fill(FSC_charges[chosen_ts][i]);
        }

        bool m_hit = false;
        for (int i=0; i<5; i++) {
            if (ZDC_energy[i] > 2000) {
                m_hit = true;
            }
        }
        for (int i=5; i<9; i++) {
            if (ZDC_energy[i] > 5000) {
                m_hit = true;
            }
        }

        if (m_hit) {
            for (int i=0; i<6; i++) {
                selected_adc_hists[i]->Fill(FSC_adcs[chosen_ts][i]);
            }
        }

        if (m_hit) {
            for (int i=0; i<6; i++) {
                selected_charge_hists[i]->Fill(FSC_charges[chosen_ts][i]);
            }
        }

        bool p_hit = false;
        for (int i=9; i<14; i++) {
            if (ZDC_energy[i] > 2500) {
                p_hit = true;
            }
        }
        for (int i=14; i<18; i++) {
            if (ZDC_energy[i] > 15000) {
                p_hit = true;
            }
        }

        if (p_hit) {
            for (int i=6; i<12; i++) {
                selected_adc_hists[i]->Fill(FSC_adcs[chosen_ts][i]);
            }
        }

        if (p_hit) {
            for (int i=6; i<12; i++) {
                selected_charge_hists[i]->Fill(FSC_charges[chosen_ts][i]);
            }
        }
    }

    const float textsize = 0.03;

    TCanvas* adc_canvases[12*12];
    for (int i=0; i<12; i++) {
        TString canvas_name = "raw_adc_canvas_" + to_string(i);
        adc_canvases[i] = new TCanvas(canvas_name, "", 1000, 800);

        //adc_correlation_canvases[i*10+j]->SetLeftMargin(0.12);
        //adc_correlation_canvases[i*10+j]->SetRightMargin(0.12);

        TString xtitle = FSC_names[i] + " adc";
        adc_hists[i]->GetXaxis()->SetTitle(xtitle);
        adc_hists[i]->GetYaxis()->SetTitle("events");
        gPad->SetLogy();

        adc_hists[i]->Scale(1.0/adc_hists[i]->Integral());
        adc_hists[i]->Draw("HIST SAME");

        selected_adc_hists[i]->Scale(1.0/selected_adc_hists[i]->Integral());
        selected_adc_hists[i]->SetLineColor(kRed);
        selected_adc_hists[i]->Draw("HIST SAME");

        TLegend* legend = new TLegend(0.7, 0.75, 0.9, 0.9);
        legend->AddEntry(adc_hists[i], "raw adc"); 
        legend->AddEntry(selected_adc_hists[i], "ZDC cut adc");
        legend->SetTextSize(textsize);
        legend->Draw();

        TString filename;
        filename = "figures/selection_figures/adc/"+FSC_names[i]+".png";
        adc_canvases[i]->Print(filename);
    }


    TCanvas* charge_canvases[12*12];
    for (int i=0; i<12; i++) {
        TString canvas_name = "raw_charge_canvas_" + to_string(i);
        charge_canvases[i] = new TCanvas(canvas_name, "", 1000, 800);

        //adc_correlation_canvases[i*10+j]->SetLeftMargin(0.12);
        //adc_correlation_canvases[i*10+j]->SetRightMargin(0.12);

        TString xtitle = FSC_names[i] + " charge";
        charge_hists[i]->GetXaxis()->SetTitle(xtitle);
        charge_hists[i]->GetYaxis()->SetTitle("events");
        gPad->SetLogy();

        charge_hists[i]->Scale(1.0/charge_hists[i]->Integral());
        charge_hists[i]->Draw("HIST SAME");

        selected_charge_hists[i]->Scale(1.0/selected_charge_hists[i]->Integral());
        selected_charge_hists[i]->SetLineColor(kRed);
        selected_charge_hists[i]->Draw("HIST SAME");

        TLegend* legend = new TLegend(0.7, 0.75, 0.9, 0.9);
        legend->AddEntry(charge_hists[i], "raw charge"); 
        legend->AddEntry(selected_charge_hists[i], "ZDC cut charge");
        legend->SetTextSize(textsize);
        legend->Draw();

        TString filename;
        filename = "figures/selection_figures/charge/"+FSC_names[i]+".png";
        charge_canvases[i]->Print(filename);
    }



    cout << "finished" << endl;
    return 0;
}
