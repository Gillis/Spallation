#define FermiAnalyzer_cxx
// The class definition in FermiAnalyzer.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("FermiAnalyzer.C")
// Root > T->Process("FermiAnalyzer.C","some options")
// Root > T->Process("FermiAnalyzer.C+")
//

#include "FermiAnalyzer.h"
#include <TH2.h>
#include <TStyle.h>
#include "Riostream.h"
#include "TMath.h"

void FermiAnalyzer::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

  // Option format:
  // out=output.root:cx=1.2e2:emax=1000
   TString option = GetOption();

   //   TString outputFile
   cout <<"Output file = " << option << endl;
   if(option == "") option = "defaultOutput.root";
   resultsOut = new TFile(option, "RECREATE");

   // Linear spectra:
   neutronEnergyIntegrated = new TH1F("neutronEnergyIntegrated", "Integrated neutron energies", 100, 0.0, 1000.0);
   protonEnergyIntegrated = new TH1F("protonEnergyIntegrated", "Integrated proton energies", 100, 0.0, 1000.0);

   // Initialize double-differential energy spectra:
   //   Float_t emax = 1000.0;
   Float_t emin = 1.0;
   Float_t xbins[99+1];
   DDLinBinWidth = emax/99.0; 
   Float_t fact=(log(emax)-log(emin))/99;
   Int_t index = 99+1;
   for(Int_t i=0;i<99+1;++i) {
     index--;
     xbins[i]=exp(log(emin)+fact*i);
     cout << xbins[i] << endl;
   }

   ddHistBinWidths = new TH1F("ddHistBinWidths","",99,xbins);
   for(Int_t i=0;i<99;++i) {
     ddHistBinWidths->SetBinContent(i,(Double_t)1./(xbins[i+1]-xbins[i]));
   }
   //   de=1.0;

   massnumbers = new TH1F("massnumbers", "Massnumber distribution", 14, 1.0, 14.0);
   carbonIsotopes = new TH1F("carbonIsotopes", "Carbon isotopes", 14, 1.0, 14.0);
   hydrogenIsotopes = new TH1F("hydrogenIsotopes", "Hydrogen isotopes", 14, 1.0, 14.0);
   heliumIsotopes = new TH1F("heliumIsotopes", "Helium isotopes", 14, 1.0, 14.0);

   neutronDD11 = new TH1F("neutronDD11", "Angle 11 deg", 99, xbins);
   neutronDD11Lin = new TH1F("neutronDD11Lin", "Angle 11 deg", 99, 0.0, emax);
   neutronDD50 = new TH1F("neutronDD50", "Angle 50 deg", 99, xbins);
   neutronDD50Lin = new TH1F("neutronDD50Lin", "Angle 50 deg", 99, 0.0, emax);

   protonEnergyIntegratedLogx = new TH1F("protonEnergyIntegratedLogx", "Integrated proton energies", 99, xbins);

   protonDD11 = new TH1F("protonDD11", "Angle 11 deg", 99, xbins);
   protonDD11Lin = new TH1F("protonDD11Lin", "Angle 11 deg", 99, 0.0, emax);
   protonDD50 = new TH1F("protonDD50", "Angle 50 deg", 99, xbins);
   protonDD50Lin = new TH1F("protonDD50Lin", "Angle 50 deg", 99, 0.0, emax);

   //   neutronDD21 = new TH1F("neutronDD11", "Neutrons in angle 11 deg", nbins, energies);
}

void FermiAnalyzer::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t FermiAnalyzer::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either FermiAnalyzer::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.
  fChain->GetEntry(entry);
  //  Int_t numberOfEvents = 100000;
  Float_t dTheta = 2.0;
  Float_t weight = 1.0;
  //  Float_t crossSection = 2.220000e+02;
  if(A == 1 && Z == 0) {
    neutronEnergyIntegrated->Fill(kinE, GetWeight(crossSection, numberOfEvents));
    if(theta > (11.0 - dTheta) && theta < (11.0 + dTheta)) {
      weight = GetDoubleDifferentialWeight(crossSection, numberOfEvents, 11.0, dTheta);
      neutronDD11->Fill(kinE, weight);
      neutronDD11Lin->Fill(kinE, weight/DDLinBinWidth);
    }

    if(theta > (50.0 - dTheta) && theta < (50.0 + dTheta)) {
      weight = GetDoubleDifferentialWeight(crossSection, numberOfEvents, 50.0, dTheta);
      neutronDD50->Fill(kinE, weight);
      neutronDD50Lin->Fill(kinE, weight/DDLinBinWidth);
    }
  }
  if(A == 1 && Z == 1) {
    protonEnergyIntegrated->Fill(kinE, GetWeight(crossSection, numberOfEvents));
    protonEnergyIntegratedLogx->Fill(kinE, GetWeight(crossSection, numberOfEvents));
    if(theta > (11.0 - dTheta) && theta < (11.0 + dTheta)) {
      weight = GetDoubleDifferentialWeight(crossSection, numberOfEvents, 11.0, dTheta);
      protonDD11->Fill(kinE, weight);
      protonDD11Lin->Fill(kinE, weight/DDLinBinWidth);
    }

    if(theta > (50.0 - dTheta) && theta < (50.0 + dTheta)) {
      weight = GetDoubleDifferentialWeight(crossSection, numberOfEvents, 50.0, dTheta);
      protonDD50->Fill(kinE, weight);
      protonDD50Lin->Fill(kinE, weight/DDLinBinWidth);
    }
  }
  //massnumbers->Fill(A);
  massnumbers->Fill(A, GetWeight(crossSection, numberOfEvents));
  if(Z == 6) {
    carbonIsotopes->Fill(A, GetWeight(crossSection, numberOfEvents));
  }
  if(Z == 2) {
    heliumIsotopes->Fill(A, GetWeight(crossSection, numberOfEvents));
  }
  if(Z == 1) {
    hydrogenIsotopes->Fill(A, GetWeight(crossSection, numberOfEvents));
  }
  return kTRUE;
}

Float_t FermiAnalyzer::GetDoubleDifferentialWeight(Float_t crossSection, Int_t numberOfEvents, Float_t theta, Float_t dTheta)
{
  Float_t weight = crossSection/(numberOfEvents * 2.0*TMath::Pi()*(TMath::Cos(TMath::DegToRad()*(theta - dTheta)) - TMath::Cos(TMath::DegToRad()*(theta + dTheta))));
  return weight;
}

Float_t FermiAnalyzer::GetWeight(Float_t crossSection, Int_t numberOfEvents)
{
  return crossSection/double(numberOfEvents);
}

void FermiAnalyzer::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void FermiAnalyzer::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

  protonEnergyIntegratedLogx->Multiply(protonEnergyIntegratedLogx, ddHistBinWidths, 1.0, 1.0);
  neutronDD11->Multiply(neutronDD11, ddHistBinWidths, 1.0, 1.0);
  neutronDD50->Multiply(neutronDD11, ddHistBinWidths, 1.0, 1.0);
  resultsOut->Write();
  resultsOut->Close();
}