
#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLine.h"

#include "rho_selector.h"

using namespace std;

int main() {

    cout << "starting" << endl;

    TFile* FSC_file(TFile::Open("data/FSC_data_0_cut.root"));
    TFile* ZDC_file(TFile::Open("data/ZDC_data_0_cut.root"));
    TFile* Tracker_file(TFile::Open("data/Tracker_data_0_cut.root"));
    TFile* HF_file(TFile::Open("data/HF_data_0_cut.root"));

    TTreeReader FSC_reader("FSCAnalyzerHC/fscdigi", FSC_file);
    TTreeReader ZDC_reader("zdcanalyzer/zdcdigi", ZDC_file);
    TTreeReader ZDC_rechit_reader("zdcanalyzer/zdcrechit", ZDC_file);
    TTreeReader Tracker_reader("trackAnalyzer/trackTree", Tracker_file);
    TTreeReader HF_reader("rechitanalyzer/hf", HF_file);

    TTreeReaderValue<Float_t> ZDC_sumMinus(ZDC_rechit_reader, "sumMinus");;
    TTreeReaderValue<Float_t> ZDC_sumPlus(ZDC_rechit_reader, "sumPlus");;

    TTreeReaderValue<Int_t> nTrk(Tracker_reader, "nTrk");

    TTreeReaderArray<float> trkEta(Tracker_reader, "trkEta");
    TTreeReaderArray<float> trkPt(Tracker_reader, "trkPt");
    TTreeReaderArray<float> trkPtError(Tracker_reader, "trkPtError");
    TTreeReaderArray<float> trkPhi(Tracker_reader, "trkPhi");
    TTreeReaderArray<char> trkCharge(Tracker_reader, "trkCharge");
    TTreeReaderArray<bool> highPurity(Tracker_reader, "highPurity");

    TTreeReaderArray<float> trkDxy(Tracker_reader, "trkDxyAssociatedVtx");
    TTreeReaderArray<float> trkDxyErr(Tracker_reader, "trkDxyErrAssociatedVtx");
    TTreeReaderArray<float> trkDz(Tracker_reader, "trkDzAssociatedVtx");
    TTreeReaderArray<float> trkDzErr(Tracker_reader, "trkDzErrAssociatedVtx");

    TTreeReaderArray<float> HF_e(HF_reader, "e");
    TTreeReaderArray<float> HF_eta(HF_reader, "eta");

    rho_readers readers(
        &nTrk,
        &trkEta,
        &trkPt,
        &trkPtError,
        &trkPhi,
        &trkCharge,
        &highPurity,
        &trkDxy,
        &trkDxyErr,
        &trkDz,
        &trkDzErr,
        &HF_e,
        &HF_eta
    );

    TH1F* rho_mass_hist = new TH1F("rho_mass_hist", "rho_mass_hist", 100, 0, 2);

    while(FSC_reader.Next()) {

        int res = ZDC_rechit_reader.Next();
        if (res == 0) {
            throw exception();
        }
        res = Tracker_reader.Next();
        if (res == 0) {
            throw exception();
        }
        res = HF_reader.Next();
        if (res == 0) {
            throw exception();
        }

        track rho_track;
        bool is_rho = select_rho(readers, rho_track, false);

        if (!is_rho) {continue;}

        cout << "filling" << endl;
        cout << rho_track.m << endl;
        rho_mass_hist->Fill(rho_track.m);
    }

    TCanvas* rho_mass_canvas = new TCanvas("rho_mass_canvas", "reconstructed rho mass", 1000, 800);

    rho_mass_hist->GetXaxis()->SetTitle("rho mass [GeV]");
    rho_mass_hist->GetYaxis()->SetTitle("events");
    rho_mass_hist->SetTitle("reconstructed rho mass");

    rho_mass_hist->Draw("");

    rho_mass_canvas->Update();

    TLine* low_rho_mass = new TLine(0.65, 0, 0.65, rho_mass_canvas->GetUymax());
    low_rho_mass->SetLineWidth(2);
    low_rho_mass->Draw("SAME");

    TLine* high_rho_mass = new TLine(1.0, 0, 1.0, rho_mass_canvas->GetUymax());
    high_rho_mass->SetLineWidth(2);
    high_rho_mass->Draw("SAME");

    rho_mass_canvas->Print("figures/rho/tho_mass.png");

    cout << "finished" << endl;

    return 0;

}